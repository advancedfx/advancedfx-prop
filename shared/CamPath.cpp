#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-08-15 dominik.matrixstorm.com
//
// First changes:
// 2014-11-03 dominik.matrixstorm.com

#include "CamPath.h"

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include <iterator>
#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>

CamPathValue::CamPathValue()
: X(0.0), Y(0.0), Z(0.0), Pitch(0.0), Yaw(0.0), Roll(0.0), Fov(90.0)
{
}

CamPathValue::CamPathValue(double x, double y, double z, double pitch, double yaw, double roll, double fov)
: X(x), Y(y), Z(z), Pitch(pitch), Yaw(yaw), Roll(roll), Fov(fov)
{
}

CamPathIterator::CamPathIterator(COSPoints::const_iterator & it) : wrapped(it)
{
}

double CamPathIterator::GetTime()
{
	return wrapped->first;
}

CamPathValue CamPathIterator::GetValue()
{
	Quaternion Q = wrapped->second.R;
	QEulerAngles angles = Q.ToQREulerAngles().ToQEulerAngles();

	CamPathValue result(
		wrapped->second.T.X,
		wrapped->second.T.Y,
		wrapped->second.T.Z,
		angles.Pitch,
		angles.Yaw,
		angles.Roll,
		wrapped->second.Fov
	);

	return result;
}

bool CamPathIterator::IsSelected()
{
	return ((CamPathValuePiggyBack *)(wrapped->second.pUser))->Selected;
}

CamPathIterator& CamPathIterator::operator ++ ()
{
	wrapped++;
	return *this;
}

bool CamPathIterator::operator == (CamPathIterator const &it) const
{
	return wrapped == it.wrapped;
}

bool CamPathIterator::operator != (CamPathIterator const &it) const
{
	return !(*this == it);
}


CamPath::CamPath()
: m_OnChanged(0)
, m_Enabled(false)
{
	m_Spline.OnValueRemoved_set(this);

	Changed();
}

CamPath::CamPath(ICamPathChanged * onChanged)
: m_OnChanged(onChanged)
, m_Enabled(false)
{
	m_Spline.OnValueRemoved_set(this);

	Changed();
}

CamPath::~CamPath()
{
	m_Spline.Clear();
	m_Spline.OnValueRemoved_set(0);
}

bool CamPath::DoEnable(bool enable)
{
	m_Enabled = enable && 4 <= GetSize();

	return m_Enabled;
}

bool CamPath::Enable(bool enable)
{
	bool result = DoEnable(enable);

	if(result != enable)
		Changed();

	return result;
}

bool CamPath::IsEnabled()
{
	return m_Enabled;
}

void CamPath::Add(double time, CamPathValue value, bool selected)
{
	COSValue val;

	val.R = Quaternion::FromQREulerAngles(QREulerAngles::FromQEulerAngles(QEulerAngles(
		value.Pitch,
		value.Yaw,
		value.Roll
	)));

	val.T.X = value.X;
	val.T.Y = value.Y;
	val.T.Z = value.Z;

	val.Fov = value.Fov;

	Add(time, val, selected);

	Changed();
}

void CamPath::Add(double time, COSValue value, bool selected)
{
	value.pUser = new CamPathValuePiggyBack(selected);

	m_Spline.Add(time, value);

	Changed();
}

void CamPath::Changed()
{
	if(m_OnChanged) m_OnChanged->CamPathChanged(this);
}


void CamPath::Remove(double time)
{
	m_Spline.Remove(time);

	m_Enabled = m_Enabled && 4 <= GetSize();

	Changed();
}

void CamPath::Clear()
{
	bool selectAll = true;

	// TODO: optimize this somehow, this can take up to log(N)*N, which is rather slow for such a shitty operation:

	COSPoints::const_iterator last = m_Spline.GetEnd();
	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd();)
	{
		if(GetSelected(it->second))
		{
			selectAll = false;
			m_Spline.Remove(it->first);

			if(last != m_Spline.GetEnd())
			{
				it = last;
				last = m_Spline.GetEnd();
			}
			else
				it = m_Spline.GetBegin();
		}
		else
		{
			last = it;
			++it;
		}
	}

	if(selectAll) m_Spline.Clear();

	m_Enabled = m_Enabled && 4 <= GetSize();

	Changed();
}

size_t CamPath::GetSize()
{
	return m_Spline.GetSize();
}

CamPathIterator CamPath::GetBegin()
{
	return CamPathIterator(m_Spline.GetBegin());
}

CamPathIterator CamPath::GetEnd()
{
	return CamPathIterator(m_Spline.GetEnd());
}

double CamPath::GetLowerBound()
{
	return m_Spline.GetLowerBound();
}

bool CamPath::GetSelected(const COSValue & value)
{
	return GetPiggy(value)->Selected;
}

CamPathValuePiggyBack const * CamPath::GetPiggy( const COSValue & value)
{
	return (CamPathValuePiggyBack *)value.pUser;
}

double CamPath::GetUpperBound()
{
	return m_Spline.GetUpperBound();
}

CamPathValue CamPath::Eval(double t)
{
	COSValue val = m_Spline.Eval(t);

	QEulerAngles angles = val.R.ToQREulerAngles().ToQEulerAngles();

	return CamPathValue(
		val.T.X,
		val.T.Y,
		val.T.Z,
		angles.Pitch,
		angles.Yaw,
		angles.Roll,
		val.Fov
	);
}

void CamPath::OnChanged_set(ICamPathChanged * value)
{
	m_OnChanged = value;
}

char * double2xml(rapidxml::xml_document<> & doc, double value)
{
	char szTmp[196];
	_snprintf_s(szTmp, _TRUNCATE,"%f", value);
	return doc.allocate_string(szTmp);
}

bool CamPath::Save(wchar_t const * fileName)
{
	rapidxml::xml_document<> doc;

	rapidxml::xml_node<> * decl = doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(doc.allocate_attribute("encoding", "utf-8"));
	doc.append_node(decl);

	rapidxml::xml_node<> * cam = doc.allocate_node(rapidxml::node_element, "campath");
	doc.append_node(cam);

	rapidxml::xml_node<> * pts = doc.allocate_node(rapidxml::node_element, "points");
	cam->append_node(pts);

	rapidxml::xml_node<> * cmt = doc.allocate_node(rapidxml::node_comment,0,
		"Points are in Quake coordinates, meaning x=forward, y=left, z=up and rotation order is first rx, then ry and lastly rz.\n"
		"Rotation direction follows the right-hand grip rule.\n"
		"rx (roll), ry (pitch), rz(yaw) are the Euler angles in degrees.\n"
		"qw, qx, qy, qz are the quaternion values.\n"
		"When read it is sufficient that either rx, ry, rz OR qw, qx, qy, qz are present.\n"
		"If both are pesent then qw, qx, qy, qz take precedence."
	);
	pts->append_node(cmt);

	for(CamPathIterator it = GetBegin(); it != GetEnd(); ++it)
	{
		double time = it.GetTime();
		CamPathValue val = it.GetValue();

		rapidxml::xml_node<> * pt = doc.allocate_node(rapidxml::node_element, "p");
		pt->append_attribute(doc.allocate_attribute("t", double2xml(doc,time)));
		pt->append_attribute(doc.allocate_attribute("x", double2xml(doc,val.X)));
		pt->append_attribute(doc.allocate_attribute("y", double2xml(doc,val.Y)));
		pt->append_attribute(doc.allocate_attribute("z", double2xml(doc,val.Z)));
		pt->append_attribute(doc.allocate_attribute("fov", double2xml(doc,val.Fov)));
		pt->append_attribute(doc.allocate_attribute("rx", double2xml(doc,val.Roll)));
		pt->append_attribute(doc.allocate_attribute("ry", double2xml(doc,val.Pitch)));
		pt->append_attribute(doc.allocate_attribute("rz", double2xml(doc,val.Yaw)));
		pt->append_attribute(doc.allocate_attribute("qw", double2xml(doc,it.wrapped->second.R.W)));
		pt->append_attribute(doc.allocate_attribute("qx", double2xml(doc,it.wrapped->second.R.X)));
		pt->append_attribute(doc.allocate_attribute("qy", double2xml(doc,it.wrapped->second.R.Y)));
		pt->append_attribute(doc.allocate_attribute("qz", double2xml(doc,it.wrapped->second.R.Z)));

		if(GetSelected(it.wrapped->second))
			pt->append_attribute(doc.allocate_attribute("selected"));

		pts->append_node(pt);
	}

	std::string xmlString;
	rapidxml::print(std::back_inserter(xmlString), doc);

	FILE * pFile = 0;
	_wfopen_s(&pFile, fileName, L"wb");

	if(0 != pFile)
	{
		fputs(xmlString.c_str(), pFile);
		fclose(pFile);
		return true;
	}
	
	return false;
}

bool CamPath::Load(wchar_t const * fileName)
{
	bool bOk = false;

	FILE * pFile = 0;

	_wfopen_s(&pFile, fileName, L"rb");

	if(!pFile)
		return false;
	
	fseek(pFile, 0, SEEK_END);
	size_t fileSize = ftell(pFile);
	rewind(pFile);

	char * pData = new char[fileSize+1];
	pData[fileSize] = 0;

	size_t readSize = fread(pData, sizeof(char), fileSize, pFile);
	bOk = readSize == fileSize;
	if(bOk)
	{
		try
		{
			do
			{
				rapidxml::xml_document<> doc;
				doc.parse<0>(pData);

				rapidxml::xml_node<> * cur_node = doc.first_node("campath");
				if(!cur_node) break;

				cur_node = cur_node->first_node("points");
				if(!cur_node) break;

				// Clear current Campath:
				Clear();

				for(cur_node = cur_node->first_node("p"); cur_node; cur_node = cur_node->next_sibling("p"))
				{
					rapidxml::xml_attribute<> * timeAttr = cur_node->first_attribute("t");
					if(!timeAttr) continue;

					rapidxml::xml_attribute<> * xA = cur_node->first_attribute("x");
					rapidxml::xml_attribute<> * yA = cur_node->first_attribute("y");
					rapidxml::xml_attribute<> * zA = cur_node->first_attribute("z");
					rapidxml::xml_attribute<> * fovA = cur_node->first_attribute("fov");
					rapidxml::xml_attribute<> * rxA = cur_node->first_attribute("rx");
					rapidxml::xml_attribute<> * ryA = cur_node->first_attribute("ry");
					rapidxml::xml_attribute<> * rzA = cur_node->first_attribute("rz");
					rapidxml::xml_attribute<> * qwA = cur_node->first_attribute("qw");
					rapidxml::xml_attribute<> * qxA = cur_node->first_attribute("qx");
					rapidxml::xml_attribute<> * qyA = cur_node->first_attribute("qy");
					rapidxml::xml_attribute<> * qzA = cur_node->first_attribute("qz");
					rapidxml::xml_attribute<> * selectedA = cur_node->first_attribute("selected");

					double dT = atof(timeAttr->value());
					double dX = xA ? atof(xA->value()) : 0.0;
					double dY = yA ? atof(yA->value()) : 0.0;
					double dZ = zA ? atof(zA->value()) : 0.0;
					double dFov = fovA ? atof(fovA->value()) : 90.0;

					if(qwA && qxA && qyA && qzA)
					{
						COSValue r;
						r.T.X = dX;
						r.T.Y = dY;
						r.T.Z = dZ;
						r.R.W = atof(qwA->value());
						r.R.X = atof(qxA->value());
						r.R.Y = atof(qyA->value());
						r.R.Z = atof(qzA->value());
						r.Fov = dFov;

						// Add point:
						Add(dT, r, 0 != selectedA);
					}
					else
					{
						double dRXroll = rxA ? atof(rxA->value()) : 0.0;
						double dRYpitch = ryA ? atof(ryA->value()) : 0.0;
						double dRZyaw = rzA ? atof(rzA->value()) : 0.0;

						// Add point:
						Add(dT, CamPathValue(
							dX, dY, dZ,
							dRYpitch, dRZyaw, dRXroll,
							dFov),
							0 != selectedA
						);
					}
				}
			}
			while (false);
		}
		catch(rapidxml::parse_error &)
		{
			bOk=false;
		}
	}

	delete pData;

	fclose(pFile);

	Changed();

	return bOk;
}

size_t CamPath::SelectAll()
{
	SelectNone();

	size_t max = m_Spline.GetSize();
	if(0 < max) --max;

	return SelectAdd((size_t)0, max);
}

void CamPath::SelectNone()
{
	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		CamPathValuePiggyBack * newPig = new CamPathValuePiggyBack(GetPiggy(it->second));
		newPig->Selected = false;

		delete GetPiggy(it->second);

		m_Spline.SetUser(it->first, newPig);
	}
}

size_t CamPath::SelectInvert()
{
	size_t selected = 0;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		CamPathValuePiggyBack * newPig = new CamPathValuePiggyBack(GetPiggy(it->second));
		newPig->Selected = !newPig->Selected;

		if(newPig->Selected) ++selected;

		delete GetPiggy(it->second);

		m_Spline.SetUser(it->first, newPig);
	}

	return selected;
}

size_t CamPath::SelectAdd(size_t min, size_t max)
{
	size_t i = 0;
	size_t selected = 0;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		CamPathValuePiggyBack * newPig = new CamPathValuePiggyBack(GetPiggy(it->second));
		newPig->Selected = newPig->Selected || min <= i && i <= max;

		if(newPig->Selected) ++selected;

		delete GetPiggy(it->second);

		m_Spline.SetUser(it->first, newPig);

		++i;
	}

	return selected;
}

size_t CamPath::SelectAdd(double min, size_t count)
{
	size_t selected = 0;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		CamPathValuePiggyBack * newPig = new CamPathValuePiggyBack(GetPiggy(it->second));
		newPig->Selected = newPig->Selected || min <= it->first && selected < count;

		if(newPig->Selected) ++selected;

		delete GetPiggy(it->second);

		m_Spline.SetUser(it->first, newPig);
	}

	return selected;
}

size_t CamPath::SelectAdd(double min, double max)
{
	size_t selected = 0;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		CamPathValuePiggyBack * newPig = new CamPathValuePiggyBack(GetPiggy(it->second));
		newPig->Selected = newPig->Selected || min <= it->first && it->first <= max;

		if(newPig->Selected) ++selected;

		delete GetPiggy(it->second);

		m_Spline.SetUser(it->first, newPig);
	}

	return selected;
}

void CamPath::SetStart(double t)
{
	if(m_Spline.GetSize()<1) return;

	CubicObjectSpline tempSpline;
	tempSpline.OnValueRemoved_set(this);

	bool selectAll = true;
	double first = 0;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		if(GetSelected(it->second))
		{
			if(selectAll)
			{
				selectAll = false;
				first = it->first;
				break;
			}
		}
	}

	double deltaT = selectAll ? t -m_Spline.GetBegin()->first : t -first;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		double curT = it->first;
		COSValue curValue = it->second;

		curValue.pUser = new CamPathValuePiggyBack(GetPiggy(curValue));

		if(selectAll || GetSelected(curValue))
		{
			tempSpline.Add(deltaT+curT, curValue);
		}
		else
		{
			tempSpline.Add(curT, curValue);
		}

	}

	CopyCOS(m_Spline, tempSpline);

	DoEnable(m_Enabled);

	Changed();
}
	
void CamPath::SetDuration(double t)
{
	if(m_Spline.GetSize()<2) return;

	CubicObjectSpline tempSpline;
	tempSpline.OnValueRemoved_set(this);

	CopyCOS(tempSpline, m_Spline);

	bool selectAll = true;
	double first = 0, last = 0;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		if(GetSelected(it->second))
		{
			if(selectAll)
			{
				selectAll = false;
				first = it->first;
				last = first;
			}
			else
			{
				last = it->first;
			}
		}
	}

	double oldDuration = selectAll ? GetDuration() : last -first;

	m_Spline.Clear();

	double scale = oldDuration ? t / oldDuration : 0.0;
	bool isFirst = true;
	double firstT = 0;

	for(COSPoints::const_iterator it = tempSpline.GetBegin(); it != tempSpline.GetEnd(); ++it)
	{
		double curT = it->first;
		COSValue curValue = it->second;

		curValue.pUser = new CamPathValuePiggyBack(GetPiggy(curValue));

		if(selectAll || GetSelected(curValue))
		{
			if(isFirst)
			{
				m_Spline.Add(curT, curValue);
				firstT = curT;
				isFirst = false;
			}
			else
				m_Spline.Add(firstT+scale*(curT-firstT), curValue);
		}
		else
			m_Spline.Add(curT, curValue);
	}

	DoEnable(m_Enabled);

	Changed();
}

void CamPath::SetPosition(double x, double y, double z)
{
	if(m_Spline.GetSize()<1) return;

	bool selectAll = true;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		if(GetSelected(it->second))
		{
			if(selectAll)
			{
				selectAll = false;
				break;
			}
		}
	}

	bool first = true;
	COSValue firstValue;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		double curT = it->first;
		COSValue curValue = it->second;
		curValue.pUser = new CamPathValuePiggyBack(GetPiggy(curValue));

		if(selectAll || GetSelected(curValue))
		{
			if(first)
			{
				first = false;
				firstValue = curValue;

				curValue.T.X = x;
				curValue.T.Y = y;
				curValue.T.Z = z;
			}
			else
			{
				curValue.T.X = x +(-firstValue.T.X +curValue.T.X);
				curValue.T.Y = y +(-firstValue.T.Y +curValue.T.Y);
				curValue.T.Z = z +(-firstValue.T.Z +curValue.T.Z);
			}

			m_Spline.Add(curT, curValue);
		}

	}

	DoEnable(m_Enabled);

	Changed();
}

void CamPath::SetAngles(double yPitch, double zYaw, double xRoll)
{
	if(m_Spline.GetSize()<1) return;

	bool selectAll = true;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		if(GetSelected(it->second))
		{
			if(selectAll)
			{
				selectAll = false;
				break;
			}
		}
	}

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		double curT = it->first;
		COSValue curValue = it->second;
		curValue.pUser = new CamPathValuePiggyBack(GetPiggy(curValue));

		if(selectAll || GetSelected(curValue))
		{
			curValue.R = Quaternion::FromQREulerAngles(QREulerAngles::FromQEulerAngles(QEulerAngles(yPitch, zYaw, xRoll)));

			m_Spline.Add(curT, curValue);
		}

	}

	DoEnable(m_Enabled);

	Changed();
}

void CamPath::SetFov(double fov)
{
	if(m_Spline.GetSize()<1) return;

	bool selectAll = true;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		if(GetSelected(it->second))
		{
			if(selectAll)
			{
				selectAll = false;
				break;
			}
		}
	}

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		double curT = it->first;
		COSValue curValue = it->second;
		curValue.pUser = new CamPathValuePiggyBack(GetPiggy(curValue));

		if(selectAll || GetSelected(curValue))
		{
			curValue.Fov = fov;

			m_Spline.Add(curT, curValue);
		}

	}

	DoEnable(m_Enabled);

	Changed();
}

void CamPath::Rotate(double yPitch, double zYaw, double xRoll)
{
	zYaw = -zYaw;
	
	if(m_Spline.GetSize()<1) return;

	bool selectAll = true;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		if(GetSelected(it->second))
		{
			if(selectAll)
			{
				selectAll = false;
				break;
			}
		}
	}

	// calcualte mid:

	double minX = 0, maxX = 0, minY = 0, maxY = 0, minZ = 0, maxZ = 0;
	bool first = true;

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		if(selectAll || GetSelected(it->second))
		{
			COSValue curValue = it->second;

			if(first)
			{
				minX = curValue.T.X;
				minY = curValue.T.Y;
				minZ = curValue.T.Z;
				maxX = curValue.T.X;
				maxY = curValue.T.Y;
				maxZ = curValue.T.Z;
				first = false;
			}
			else
			{
				minX = std::min(minX, curValue.T.X);
				minY = std::min(minY, curValue.T.Y);
				minZ = std::min(minZ, curValue.T.Z);
				maxX = std::max(maxX, curValue.T.X);
				maxY = std::max(maxY, curValue.T.Y);
				maxZ = std::max(maxZ, curValue.T.Z);
			}
		}
	}

	double x0 = (maxX +minX) / 2;
	double y0 = (maxY +minY) / 2;
	double z0 = (maxZ +minZ) / 2;

	// rotate:

	for(COSPoints::const_iterator it = m_Spline.GetBegin(); it != m_Spline.GetEnd(); ++it)
	{
		double curT = it->first;
		COSValue curValue = it->second;
		curValue.pUser = new CamPathValuePiggyBack(GetPiggy(curValue));

		if(selectAll || GetSelected(curValue))
		{
			// translate into origin:
			double x = curValue.T.X -x0;
			double y = curValue.T.Y -y0;
			double z = curValue.T.Z -z0;

			// rotate:
			double forward[3];
			double right[3];
			double up[3];
			MakeVectors(xRoll, yPitch, zYaw, forward, right, up);


			double rx = x*forward[0] +y*forward[1] +z*forward[2];
			double ry = -x*right[0] -y*right[1] -z*right[2];
			double rz = x*up[0] +y*up[1] +z*up[2];

			// translate back:
			curValue.T.X = rx +x0;
			curValue.T.Y = ry +y0;
			curValue.T.Z = rz +z0;

			// update:
			m_Spline.Add(curT, curValue);
		}

	}

	DoEnable(m_Enabled);

	Changed();
}


void CamPath::CopyCOS(CubicObjectSpline & dst, CubicObjectSpline & src)
{
	dst.Clear();

	for(COSPoints::const_iterator it = src.GetBegin(); it != src.GetEnd(); ++it)
	{
		COSValue val = it->second;
		val.pUser = new CamPathValuePiggyBack(GetPiggy(it->second));

		dst.Add(it->first, val);
	}
}

double CamPath::GetDuration()
{
	if(m_Spline.GetSize()<2) return 0.0;

	return (--m_Spline.GetEnd())->first - m_Spline.GetBegin()->first;
}

void CamPath::CosObjectSplineValueRemoved(CubicObjectSpline * cos, COSValue & value)
{
	delete GetPiggy(value);
}
