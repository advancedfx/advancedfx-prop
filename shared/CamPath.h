#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-08-15 dominik.matrixstorm.com
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

	double Fov;

	CamPathValue();

	CamPathValue(double x, double y, double z, double pitch, double yaw, double roll, double fov);
};

class CamPathValuePiggyBack
{
public:
	CamPathValuePiggyBack()
	: Selected(false)
	{
		++m_InstanceCount;
	}

	CamPathValuePiggyBack(bool selected)
	: Selected(selected)
	{
		++m_InstanceCount;
	}

	CamPathValuePiggyBack(const CamPathValuePiggyBack * value)
	: Selected(value->Selected)
	{
		++m_InstanceCount;
	}

	~CamPathValuePiggyBack()
	{
		--m_InstanceCount;
	}

	bool Selected;

	static int GetInstanceCount() { return m_InstanceCount; }

private:
	static int m_InstanceCount;
};

struct CamPathIterator
{
public:
	COSPoints::const_iterator wrapped;

	CamPathIterator(COSPoints::const_iterator & it);

	double GetTime();

	CamPathValue GetValue();

	bool IsSelected();

	CamPathIterator& operator ++ ();

	bool operator == (CamPathIterator const &it) const;

	bool operator != (CamPathIterator const &it) const;

};

class CamPath;

class ICamPathChanged abstract
{
public:
	virtual void CamPathChanged(CamPath * obj) = 0;
};

class CamPath
: public ICosObjectSplineValueRemoved
{
public:
	CamPath();
	CamPath(ICamPathChanged * onChanged);
	
	~CamPath();

	bool Enable(bool enable);
	bool IsEnabled();

	void Add(double time, CamPathValue value, bool selected = false);

	/// <remarks>value.pUser will be overwritten.</remarks>
	void Add(double time, COSValue value, bool selected = false);
	
	void Remove(double time);
	void Clear();

	size_t GetSize();
	CamPathIterator GetBegin();
	CamPathIterator GetEnd();
	double GetDuration();

	/// <remarks>Must not be called if GetSize is less than 1!</remarks>
	double GetLowerBound();

	/// <remarks>Must not be called if GetSize is less than 1!</remarks>
	double GetUpperBound();

	/// <remarks>Must not be called if GetSize is less than 4!</remarks>
	CamPathValue Eval(double t);

	bool Save(wchar_t const * fileName);
	bool Load(wchar_t const * fileName);
	
	/// <remarks>In the current implementation if points happen to fall on the same time value, then the last point's value will be used (no interpolation).</remarks>
	void SetStart(double t);
	
	/// <remarks>In the current implementation if points happen to fall on the same time value, then the last point's value will be used (no interpolation).
	/// Setting duration for a path with less than 2 points will do nothing.</remarks>
	void SetDuration(double t);

	void SetPosition(double x, double y, double z);

	void SetAngles(double yPitch, double zYaw, double xRoll);

	void SetFov(double fov);

	void Rotate(double yPitch, double zYaw, double xRoll);

	size_t SelectAll();

	void SelectNone();

	size_t SelectInvert();

	/// <summary>Adds a range of key frames to the selection.</param>
	/// <param name="min">Index of first keyframe to add to selection.</param>
	/// <param name="max">Index of last keyframe to add to selection.</param>
	/// <returns>Number of selected keyframes.</returns>
	size_t SelectAdd(size_t min, size_t max);

	/// <summary>Adds a range of key frames to the selection.</param>
	/// <param name="min">Lower bound to start adding selection at.</param>
	/// <param name="count">Number of keyframes to select.</param>
	/// <returns>Number of selected keyframes.</returns>
	size_t SelectAdd(double min, size_t count);

	/// <summary>Adds a range of key frames to the selection.</param>
	/// <param name="min">Lower bound to start adding selection at.</param>
	/// <param name="count">Upper bound to end adding selection at.</param>
	/// <returns>Number of selected keyframes.</returns>
	size_t SelectAdd(double min, double max);

	void OnChanged_set(ICamPathChanged * value);

	virtual void CosObjectSplineValueRemoved(CubicObjectSpline * cos, COSValue & value);

private:
	bool m_Enabled;
	ICamPathChanged * m_OnChanged;
	CubicObjectSpline m_Spline;

	void Changed();
	void CopyCOS(CubicObjectSpline & dst, CubicObjectSpline & src);
	bool DoEnable(bool enable);

	bool GetSelected(const COSValue & value);
	CamPathValuePiggyBack const * GetPiggy( const COSValue & value);
};
