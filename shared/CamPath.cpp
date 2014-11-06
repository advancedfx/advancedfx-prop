#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-11-03 dominik.matrixstorm.com
//
// First changes:
// 2014-11-03 dominik.matrixstorm.com

#include "CamPath.h"

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include <iterator>
#include <stdio.h>

CamPathValue::CamPathValue()
{
}

CamPathValue::CamPathValue(double x, double y, double z, double pitch, double yaw, double roll)
: X(x), Y(y), Z(z), Pitch(pitch), Yaw(yaw), Roll(roll)
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
		angles.Roll
	);

	return result;
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
: m_Enabled(false)
{
}

bool CamPath::Enable(bool enable)
{
	m_Enabled = enable && 4 <= GetSize();

	return m_Enabled;
}

bool CamPath::IsEnabled()
{
	return m_Enabled;
}

void CamPath::Add(double time, CamPathValue value)
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

	m_Spline.Add(time, val);
}

void CamPath::Remove(double time)
{
	m_Spline.Remove(time);

	m_Enabled = m_Enabled && 4 <= GetSize();
}

void CamPath::Clear()
{
	m_Spline.Clear();

	m_Enabled = m_Enabled && 4 <= GetSize();
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
		angles.Roll
	);
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

	for(CamPathIterator it = GetBegin(); it != GetEnd(); ++it)
	{
		double time = it.GetTime();
		CamPathValue val = it.GetValue();

		rapidxml::xml_node<> * pt = doc.allocate_node(rapidxml::node_element, "p");
		pt->append_attribute(doc.allocate_attribute("t", double2xml(doc,time)));
		pt->append_attribute(doc.allocate_attribute("x", double2xml(doc,val.X)));
		pt->append_attribute(doc.allocate_attribute("y", double2xml(doc,val.Y)));
		pt->append_attribute(doc.allocate_attribute("z", double2xml(doc,val.Z)));
		pt->append_attribute(doc.allocate_attribute("rx", double2xml(doc,val.Roll)));
		pt->append_attribute(doc.allocate_attribute("ry", double2xml(doc,val.Pitch)));
		pt->append_attribute(doc.allocate_attribute("rz", double2xml(doc,val.Yaw)));
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
					rapidxml::xml_attribute<> * rxA = cur_node->first_attribute("rx");
					rapidxml::xml_attribute<> * ryA = cur_node->first_attribute("ry");
					rapidxml::xml_attribute<> * rzA = cur_node->first_attribute("rz");

					double dT = atof(timeAttr->value());
					double dX = xA ? atof(xA->value()) : 0.0;
					double dY = yA ? atof(yA->value()) : 0.0;
					double dZ = zA ? atof(zA->value()) : 0.0;
					double dRXroll = rxA ? atof(rxA->value()) : 0.0;
					double dRYpitch = ryA ? atof(ryA->value()) : 0.0;
					double dRZyaw = rzA ? atof(rzA->value()) : 0.0;

					// Add point:
					Add(dT, CamPathValue(
						dX, dY, dZ,
						dRYpitch, dRZyaw, dRXroll
					));
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

	return bOk;
}
