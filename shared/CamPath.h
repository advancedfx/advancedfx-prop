#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-11-06 dominik.matrixstorm.com
//
// First changes:
// 2014-11-03 dominik.matrixstorm.com

#include "AfxMath.h"

using namespace Afx::Math;

struct CamPathValue
{
	double X;
	double Y;
	double Z;

	double Pitch;
	double Yaw;
	double Roll;

	CamPathValue();

	CamPathValue(double x, double y, double z, double pitch, double yaw, double roll);
};

struct CamPathIterator
{
public:
	COSPoints::const_iterator wrapped;

	CamPathIterator(COSPoints::const_iterator & it);

	double GetTime();

	CamPathValue GetValue();

	CamPathIterator& operator ++ ();

	bool operator == (CamPathIterator const &it) const;

	bool operator != (CamPathIterator const &it) const;

};

class CamPath
{
public:
	CamPath();

	bool Enable(bool enable);
	bool IsEnabled();

	void Add(double time, CamPathValue value);
	void Add(double time, COSValue value);
	void Remove(double time);
	void Clear();

	size_t GetSize();
	CamPathIterator GetBegin();
	CamPathIterator GetEnd();
	double GetDuration();

	/// <remarks>Must not be called if IsEnabled is false!</remarks>
	double GetLowerBound();

	/// <remarks>Must not be called if IsEnabled is false!</remarks>
	double GetUpperBound();

	/// <remarks>Must not be called if IsEnabled is false!</remarks>
	CamPathValue Eval(double t);

	bool Save(wchar_t const * fileName);
	bool Load(wchar_t const * fileName);
	
	/// <remarks>In the current implementation if points happen to fall on the same time value, then the last point's value will be used (no interpolation).</remarks>
	void SetStart(double t);
	
	/// <remarks>In the current implementation if points happen to fall on the same time value, then the last point's value will be used (no interpolation).
	/// Setting duration for a path with less than 2 points will do nothing.</remarks>
	void SetDuration(double t);

private:
	bool m_Enabled;
	CubicObjectSpline m_Spline;

	void CopyCOS(CubicObjectSpline & dst, CubicObjectSpline & src);

};
