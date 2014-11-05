#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-11-03 dominik.matrixstorm.com
//
// First changes:
// 2014-11-03 dominik.matrixstorm.com

#include "CamPath.h"


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
