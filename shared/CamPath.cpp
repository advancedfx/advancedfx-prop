#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-03-22 dominik.matrixstorm.com
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

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

CamPathValue::CamPathValue()
: X(0.0), Y(0.0), Z(0.0), R(), Fov(90.0), Selected(false)
{
}

CamPathValue::CamPathValue(double x, double y, double z, double pitch, double yaw, double roll, double fov)
: X(x)
, Y(y)
, Z(z)
, R(Quaternion::FromQREulerAngles(QREulerAngles::FromQEulerAngles(QEulerAngles(pitch,yaw,roll))))
, Fov(fov)
, Selected(false)
{
}

CamPathIterator::CamPathIterator(CInterpolationMap<CamPathValue>::const_iterator & it) : wrapped(it)
{
}

double CamPathIterator::GetTime()
{
	return wrapped->first;
}

CamPathValue CamPathIterator::GetValue()
{
	return wrapped->second;
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
, m_XView(&m_Map, XSelector)
, m_YView(&m_Map, YSelector)
, m_ZView(&m_Map, ZSelector)
, m_RView(&m_Map, RSelector)
, m_FovView(&m_Map, FovSelector)
, m_SelectedView(&m_Map, SelectedSelector)
{
	m_XInterp = new CCubicDoubleInterpolation<CamPathValue>(&m_XView);
	m_YInterp = new CCubicDoubleInterpolation<CamPathValue>(&m_YView);
	m_ZInterp = new CCubicDoubleInterpolation<CamPathValue>(&m_ZView);
	m_RInterp = new CSCubicQuaternionInterpolation<CamPathValue>(&m_RView);
	m_FovInterp = new CCubicDoubleInterpolation<CamPathValue>(&m_FovView);
	m_SelectedInterp = new CBoolAndInterpolation<CamPathValue>(&m_SelectedView);

	Changed();
}

CamPath::~CamPath()
{
	m_Map.clear();

	Changed();

	delete m_SelectedInterp;
	delete m_FovInterp;
	delete m_RInterp;
	delete m_ZInterp;
	delete m_YInterp;
	delete m_XInterp;
}

void CamPath::DoInterpolationMapChangedAll(void)
{
	m_XInterp->InterpolationMapChanged();
	m_YInterp->InterpolationMapChanged();
	m_ZInterp->InterpolationMapChanged();
	m_RInterp->InterpolationMapChanged();
	m_FovInterp->InterpolationMapChanged();
	m_SelectedInterp->InterpolationMapChanged();
}

void CamPath::Enabled_set(bool enable)
{
	m_Enabled = enable;
}

bool CamPath::Enabled_get(void)
{
	return m_Enabled;
}

void CamPath::Add(double time, CamPathValue value)
{
	m_Map[time] = value;
	DoInterpolationMapChangedAll();
	Changed();
}

void CamPath::Changed()
{
	if(m_OnChanged) m_OnChanged->CamPathChanged(this);
}

void CamPath::Remove(double time)
{
	m_Map.erase(time);
	DoInterpolationMapChangedAll();
	Changed();
}

void CamPath::Clear()
{
	bool selectAll = true;

	CInterpolationMap<CamPathValue>::iterator last = m_Map.end();
	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end();)
	{
		CInterpolationMap<CamPathValue>::iterator itNext = it;
		++itNext;

		if(it->second.Selected)
		{
			selectAll = false;
			m_Map.erase(it);
		}

		it = itNext;
	}

	if(selectAll) m_Map.clear();

	DoInterpolationMapChangedAll();
	Changed();
}

size_t CamPath::GetSize()
{
	return m_Map.size();
}

CamPathIterator CamPath::GetBegin()
{
	return CamPathIterator(m_Map.begin());
}

CamPathIterator CamPath::GetEnd()
{
	return CamPathIterator(m_Map.end());
}

double CamPath::GetLowerBound()
{
	return m_Map.begin()->first;
}

double CamPath::GetUpperBound()
{
	return (--m_Map.end())->first;
}

bool CamPath::CanEval(void)
{
	return
		m_XInterp->CanEval()
		&& m_YInterp->CanEval()
		&& m_ZInterp->CanEval()
		&& m_RInterp->CanEval()
		&& m_FovInterp->CanEval()
		&& m_SelectedInterp->CanEval();
}

CamPathValue CamPath::Eval(double t)
{
	CamPathValue val;
	
	val.X = m_XInterp->Eval(t);
	val.Y = m_YInterp->Eval(t);
	val.Z = m_ZInterp->Eval(t);
	val.R = m_RInterp->Eval(t);
	val.Fov = m_FovInterp->Eval(t);
	val.Selected = m_SelectedInterp->Eval(t);

	return val;
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
		QEulerAngles ang = val.R.ToQREulerAngles().ToQEulerAngles();

		rapidxml::xml_node<> * pt = doc.allocate_node(rapidxml::node_element, "p");
		pt->append_attribute(doc.allocate_attribute("t", double2xml(doc,time)));
		pt->append_attribute(doc.allocate_attribute("x", double2xml(doc,val.X)));
		pt->append_attribute(doc.allocate_attribute("y", double2xml(doc,val.Y)));
		pt->append_attribute(doc.allocate_attribute("z", double2xml(doc,val.Z)));
		pt->append_attribute(doc.allocate_attribute("fov", double2xml(doc,val.Fov)));
		pt->append_attribute(doc.allocate_attribute("rx", double2xml(doc,ang.Roll)));
		pt->append_attribute(doc.allocate_attribute("ry", double2xml(doc,ang.Pitch)));
		pt->append_attribute(doc.allocate_attribute("rz", double2xml(doc,ang.Yaw)));
		pt->append_attribute(doc.allocate_attribute("qw", double2xml(doc,it.wrapped->second.R.W)));
		pt->append_attribute(doc.allocate_attribute("qx", double2xml(doc,it.wrapped->second.R.X)));
		pt->append_attribute(doc.allocate_attribute("qy", double2xml(doc,it.wrapped->second.R.Y)));
		pt->append_attribute(doc.allocate_attribute("qz", double2xml(doc,it.wrapped->second.R.Z)));

		if(val.Selected)
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
				SelectNone();
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
						CamPathValue r;
						r.X = dX;
						r.Y = dY;
						r.Z = dZ;
						r.R.W = atof(qwA->value());
						r.R.X = atof(qxA->value());
						r.R.Y = atof(qyA->value());
						r.R.Z = atof(qzA->value());
						r.Fov = dFov;
						r.Selected = 0 != selectedA;

						// Add point:
						Add(dT, r);
					}
					else
					{
						double dRXroll = rxA ? atof(rxA->value()) : 0.0;
						double dRYpitch = ryA ? atof(ryA->value()) : 0.0;
						double dRZyaw = rzA ? atof(rzA->value()) : 0.0;

						CamPathValue r;
						r.X = dX;
						r.Y = dY;
						r.Z = dZ;
						r.R = Quaternion::FromQREulerAngles(QREulerAngles::FromQEulerAngles(QEulerAngles(dRYpitch, dRZyaw, dRXroll)));
						r.Fov = dFov;
						r.Selected = 0 != selectedA;

						// Add point:
						Add(dT, r);
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

	DoInterpolationMapChangedAll();
	Changed();

	return bOk;
}

size_t CamPath::SelectAll()
{
	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		it->second.Selected = true;
	}

	m_SelectedInterp->InterpolationMapChanged();
	Changed();

	return m_Map.size();
}

void CamPath::SelectNone()
{
	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		it->second.Selected = false;
	}

	m_SelectedInterp->InterpolationMapChanged();
	Changed();
}

size_t CamPath::SelectInvert()
{
	size_t selected = 0;

	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		it->second.Selected = !it->second.Selected;

		if(it->second.Selected) ++selected;
	}

	m_SelectedInterp->InterpolationMapChanged();
	Changed();

	return selected;
}

size_t CamPath::SelectAdd(size_t min, size_t max)
{
	size_t i = 0;
	size_t selected = 0;

	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		it->second.Selected = it->second.Selected || min <= i && i <= max;

		if(it->second.Selected) ++selected;

		++i;
	}

	m_SelectedInterp->InterpolationMapChanged();
	Changed();

	return selected;
}

size_t CamPath::SelectAdd(double min, size_t count)
{
	size_t selected = 0;

	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		it->second.Selected = it->second.Selected || min <= it->first && selected < count;

		if(it->second.Selected) ++selected;
	}

	m_SelectedInterp->InterpolationMapChanged();
	Changed();

	return selected;
}

size_t CamPath::SelectAdd(double min, double max)
{
	size_t selected = 0;

	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		it->second.Selected = it->second.Selected || min <= it->first && it->first <= max;

		if(it->second.Selected) ++selected;
	}

	m_SelectedInterp->InterpolationMapChanged();
	Changed();

	return selected;
}

void CamPath::SetStart(double t)
{
	if(m_Map.size()<1) return;

	CInterpolationMap<CamPathValue> tempMap;

	bool selectAll = true;
	double first = 0;

	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		if(it->second.Selected)
		{
			if(selectAll)
			{
				selectAll = false;
				first = it->first;
				break;
			}
		}
	}

	double deltaT = selectAll ? t -m_Map.begin()->first : t -first;

	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		double curT = it->first;
		CamPathValue curValue = it->second;

		if(selectAll || curValue.Selected)
		{
			tempMap[deltaT+curT] = curValue;
		}
		else
		{
			tempMap[curT] = curValue;
		}
	}

	CopyMap(m_Map, tempMap);

	DoInterpolationMapChangedAll();

	Changed();
}
	
void CamPath::SetDuration(double t)
{
	if(m_Map.size()<2) return;

	CInterpolationMap<CamPathValue> tempMap;

	CopyMap(tempMap, m_Map);

	bool selectAll = true;
	double first = 0, last = 0;

	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		if(it->second.Selected)
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

	m_Map.clear();

	double scale = oldDuration ? t / oldDuration : 0.0;
	bool isFirst = true;
	double firstT = 0;

	for(CInterpolationMap<CamPathValue>::const_iterator it = tempMap.begin(); it != tempMap.end(); ++it)
	{
		double curT = it->first;
		CamPathValue curValue = it->second;

		if(selectAll || curValue.Selected)
		{
			if(isFirst)
			{
				m_Map[curT] = curValue;
				firstT = curT;
				isFirst = false;
			}
			else
				m_Map[firstT+scale*(curT-firstT)] = curValue;
		}
		else
			m_Map[curT] = curValue;
	}

	DoInterpolationMapChangedAll();

	Changed();
}

void CamPath::SetPosition(double x, double y, double z)
{
	if(m_Map.size()<1) return;

	bool selectAll = true;

	for(CInterpolationMap<CamPathValue>::const_iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		if(it->second.Selected)
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

	for(CInterpolationMap<CamPathValue>::const_iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		if(selectAll || it->second.Selected)
		{
			CamPathValue curValue = it->second;

			if(first)
			{
				minX = curValue.X;
				minY = curValue.Y;
				minZ = curValue.Z;
				maxX = curValue.X;
				maxY = curValue.Y;
				maxZ = curValue.Z;
				first = false;
			}
			else
			{
				minX = std::min(minX, curValue.X);
				minY = std::min(minY, curValue.Y);
				minZ = std::min(minZ, curValue.Z);
				maxX = std::max(maxX, curValue.X);
				maxY = std::max(maxY, curValue.Y);
				maxZ = std::max(maxZ, curValue.Z);
			}
		}
	}

	double x0 = (maxX +minX) / 2;
	double y0 = (maxY +minY) / 2;
	double z0 = (maxZ +minZ) / 2;

	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		double curT = it->first;
		CamPathValue curValue = it->second;

		if(selectAll || curValue.Selected)
		{
			curValue.X = x +(curValue.X -x0);
			curValue.Y = y +(curValue.Y -y0);
			curValue.Z = z +(curValue.Z -z0);

			it->second = curValue;
		}
	}

	m_XInterp->InterpolationMapChanged();
	m_YInterp->InterpolationMapChanged();
	m_ZInterp->InterpolationMapChanged();

	Changed();
}

void CamPath::SetAngles(double yPitch, double zYaw, double xRoll)
{
	if(m_Map.size()<1) return;

	bool selectAll = true;

	for(CInterpolationMap<CamPathValue>::const_iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		if(it->second.Selected)
		{
			if(selectAll)
			{
				selectAll = false;
				break;
			}
		}
	}

	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		double curT = it->first;
		CamPathValue curValue = it->second;

		if(selectAll || curValue.Selected)
		{
			curValue.R = Quaternion::FromQREulerAngles(QREulerAngles::FromQEulerAngles(QEulerAngles(yPitch, zYaw, xRoll)));

			it->second = curValue;
		}

	}

	m_RInterp->InterpolationMapChanged();

	Changed();
}

void CamPath::SetFov(double fov)
{
	if(m_Map.size()<1) return;

	bool selectAll = true;

	for(CInterpolationMap<CamPathValue>::const_iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		if(it->second.Selected)
		{
			if(selectAll)
			{
				selectAll = false;
				break;
			}
		}
	}

	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		double curT = it->first;
		CamPathValue curValue = it->second;

		if(selectAll || curValue.Selected)
		{
			curValue.Fov = fov;

			it->second = curValue;
		}

	}

	m_FovInterp->InterpolationMapChanged();

	Changed();
}

void CamPath::Rotate(double yPitch, double zYaw, double xRoll)
{
	if(m_Map.size()<1) return;

	bool selectAll = true;

	for(CInterpolationMap<CamPathValue>::const_iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		if(it->second.Selected)
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

	for(CInterpolationMap<CamPathValue>::const_iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		if(selectAll || it->second.Selected)
		{
			CamPathValue curValue = it->second;

			if(first)
			{
				minX = curValue.X;
				minY = curValue.Y;
				minZ = curValue.Z;
				maxX = curValue.X;
				maxY = curValue.Y;
				maxZ = curValue.Z;
				first = false;
			}
			else
			{
				minX = std::min(minX, curValue.X);
				minY = std::min(minY, curValue.Y);
				minZ = std::min(minZ, curValue.Z);
				maxX = std::max(maxX, curValue.X);
				maxY = std::max(maxY, curValue.Y);
				maxZ = std::max(maxZ, curValue.Z);
			}
		}
	}

	double x0 = (maxX +minX) / 2;
	double y0 = (maxY +minY) / 2;
	double z0 = (maxZ +minZ) / 2;

	// build rotation matrix:
	double R[3][3];
	{
		double angle;
		double sr, sp, sy, cr, cp, cy;

		angle = zYaw * (M_PI*2 / 360);
		sy = sin(angle);
		cy = cos(angle);
		angle = yPitch * (M_PI*2 / 360);
		sp = sin(angle);
		cp = cos(angle);
		angle = xRoll * (M_PI*2 / 360);
		sr = sin(angle);
		cr = cos(angle);

		// R = YAW * (PITCH * ROLL)
		R[0][0] = cy*cp;
		R[0][1] = cy*sp*sr -sy*cr;
		R[0][2] = cy*sp*cr +sy*sr;
		R[1][0] = sy*cp;
		R[1][1] = sy*sp*sr +cy*cr;
		R[1][2] = sy*sp*cr +cy*-sr;
		R[2][0] = -sp;
		R[2][1] = cp*sr;
		R[2][2] = cp*cr;
	}
	Quaternion quatR = Quaternion::FromQREulerAngles(QREulerAngles::FromQEulerAngles(QEulerAngles(yPitch, zYaw, xRoll)));

	// rotate:

	for(CInterpolationMap<CamPathValue>::iterator it = m_Map.begin(); it != m_Map.end(); ++it)
	{
		double curT = it->first;
		CamPathValue curValue = it->second;

		if(selectAll || curValue.Selected)
		{
			// update position:
			{
				// translate into origin:
				double x = curValue.X -x0;
				double y = curValue.Y -y0;
				double z = curValue.Z -z0;

				// rotate:
				double Rx = R[0][0]*x +R[0][1]*y +R[0][2]*z;
				double Ry = R[1][0]*x +R[1][1]*y +R[1][2]*z;
				double Rz = R[2][0]*x +R[2][1]*y +R[2][2]*z;

				// translate back:
				curValue.X = Rx +x0;
				curValue.Y = Ry +y0;
				curValue.Z = Rz +z0;
			}

			// update rotation:
			{
				Quaternion quatQ = curValue.R;

				curValue.R = quatR * quatQ;
			}

			// update:
			it->second = curValue;
		}

	}

	m_XInterp->InterpolationMapChanged();
	m_YInterp->InterpolationMapChanged();
	m_ZInterp->InterpolationMapChanged();
	m_RInterp->InterpolationMapChanged();

	Changed();
}


void CamPath::CopyMap(CInterpolationMap<CamPathValue> & dst, CInterpolationMap<CamPathValue> & src)
{
	dst.clear();

	for(CInterpolationMap<CamPathValue>::const_iterator it = src.begin(); it != src.end(); ++it)
	{
		dst[it->first] = it->second;
	}
}

double CamPath::GetDuration()
{
	if(m_Map.size()<2) return 0.0;

	return (--m_Map.end())->first - m_Map.begin()->first;
}
