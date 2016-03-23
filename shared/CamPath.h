#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-03-22 dominik.matrixstorm.com
//
// First changes:
// 2014-11-03 dominik.matrixstorm.com

#include "RefCounted.h"
#include "AfxMath.h"

using namespace Afx;
using namespace Afx::Math;

struct CamPathValue
{
	double X;
	double Y;
	double Z;

	Quaternion R;

	double Fov;

	bool Selected;

	CamPathValue();

	CamPathValue(double x, double y, double z, double pitch, double yaw, double roll, double fov);

};

struct CamPathIterator
{
public:
	CInterpolationMap<CamPathValue>::const_iterator wrapped;

	CamPathIterator(CInterpolationMap<CamPathValue>::const_iterator & it);

	double GetTime();

	CamPathValue GetValue();

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
{
public:
	CamPath();
	
	~CamPath();

	void Enabled_set(bool enable);
	bool Enabled_get(void);

	void Add(double time, CamPathValue value);

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

	bool CanEval(void);

	/// <remarks>
	/// Must not be called if CanEval() returns false!<br />
	/// </remarks>
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

private:
	static double XSelector(CamPathValue const & value)
	{
		return value.X;
	}

	static double YSelector(CamPathValue const & value)
	{
		return value.Y;
	}

	static double ZSelector(CamPathValue const & value)
	{
		return value.Z;
	}

	static Quaternion RSelector(CamPathValue const & value)
	{
		return value.R;
	}

	static double FovSelector(CamPathValue const & value)
	{
		return value.Fov;
	}

	static bool SelectedSelector(CamPathValue const & value)
	{
		return value.Selected;
	}

	bool m_Enabled;
	ICamPathChanged * m_OnChanged;
	
	CInterpolationMap<CamPathValue> m_Map;

	CInterpolationMapView<CamPathValue, double> m_XView;
	CInterpolationMapView<CamPathValue, double> m_YView;
	CInterpolationMapView<CamPathValue, double> m_ZView;
	CInterpolationMapView<CamPathValue, Quaternion> m_RView;
	CInterpolationMapView<CamPathValue, double> m_FovView;
	CInterpolationMapView<CamPathValue, bool> m_SelectedView;

	CInterpolation<double> * m_XInterp;
	CInterpolation<double> * m_YInterp;
	CInterpolation<double> * m_ZInterp;
	CInterpolation<Quaternion> * m_RInterp;
	CInterpolation<double> * m_FovInterp;
	CInterpolation<bool> * m_SelectedInterp;

	void Changed();
	void CopyMap(CInterpolationMap<CamPathValue> & dst, CInterpolationMap<CamPathValue> & src);

	void DoInterpolationMapChangedAll(void);
};
