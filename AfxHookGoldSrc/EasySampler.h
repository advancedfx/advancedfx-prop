#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2011-07-12 dominik.matrixstorm.com
//
// First changes
// 2010-03-23 dominik.matrixstorm.com

// TODO:
//
// In the average case where the full range of a sample is
// used (withn a frame), CPU power is wasted, because the same multiplication
// is done twice when interpolating from the next sample.
// (First it's the new one, then the old one and factors
// are about the same, this is a waste).

#include "Store.h"

#include <list>

using namespace std;


class __declspec(novtable) IFramePrinter abstract
{
public:
	virtual void Print(unsigned char const * data) abstract = 0;
};

class __declspec(novtable) IFloatFramePrinter abstract
{
public:
	virtual void Print(float const * data) abstract = 0;
};

class __declspec(novtable) IXAlphaFramePrinter abstract
{
public:
	virtual void Print(unsigned char const * data, unsigned char const * alpha) abstract = 0;
};


class __declspec(novtable) ISampleFns abstract
{
public:
	/// <summary>frame += sample</summary>
	virtual void Fn_1(void const *sample) abstract = 0;

	/// <summary>frame += w * sample </summary>
	virtual void Fn_2(void const *sample, float w) abstract = 0;

	/// <summary>frame += w * (sampleA + sampleB)</summary>
	virtual void Fn_4(void const *sampleA, void const *sampleB, float w) abstract = 0;
};


// EasySamplerBase /////////////////////////////////////////////////////////////

/// <summary>
///   Base class, not intended to be used directly.
///   This class supplies implementation of fundamental sampling logic.
/// </summary>
class EasySamplerBase abstract
{
public:

protected:
	/// <param name="exposure">time the shutter is kept open measured in number of frames</param>
	EasySamplerBase(
		float frameDuration,
		float startTime,
		float exposure);
	
	bool CanSkipConstant(float time, float durationPerSample, int numPreviousSamplesRequired);

	/// <summary>
	///   Triggers the sampling logic for an example. If there is anything to be sampled
	///   SubSample will be called one or more times.
	/// </summary>
	void Sample(float time);

	/// <summary>
	///   Can be called mutliple times by Sample(float).<br />
	///   The calle is supposed to approximate a sub-integral
	///   between two samples, using the information supplied.
	/// </summary>
	/// <param name="timeA">Time of last sample.</param>
	/// <param name="timeB">Time of current sample.</param>
	/// <param name="subTimeA">lower integral boundary</param>
	/// <param name="subTimeB">upper integral boundary</param>
	/// <remarks>timeA &lt;= subTimeA &lt; subTimeB &lt;=timeB</remarks>
	virtual void SubSample(
		float timeA,
		float timeB,
		float subTimeA,
		float subTimeB) abstract = 0;

	/// <summary>
	///   Can be called mutliple times by Sample(float).<br />
	///   The calle is supposed to finish (print + clear)
	///   the current frame.
	/// </summary>
	virtual void MakeFrame() abstract = 0;

protected:
	/// <summary>
	///	  Auto 2 (trapezium) / 1 (rectangle) / 0 point sampling by integration.<br />
	///   Selects, optimizes and combines the integration using a given set of base functions.
	/// </summary>
	/// <param name="sampleA">can be 0</param>
	/// <param name="sampleB">can be 0</param>
	static void Integrator_Fn(ISampleFns *fns, void const *sampleA, void const *sampleB, float timeA, float timeB, float subTimeA, float subTimeB);

private:
	float m_FrameDuration;
	float m_LastFrameTime;
	float m_LastSampleTime;
	bool m_ShutterOpen;
	float m_ShutterOpenDuration;
	float m_ShutterTime;
};


// EasySamplerSettings /////////////////////////////////////////////////////////

/// <summary>
///   Encapsulates common sampler settings.
/// </summary>
class EasySamplerSettings
{
public:
	enum Method
	{
		ESM_Rectangle,
		ESM_Trapezoid
	};

	EasySamplerSettings(
		int width, 
		int height,
		Method method,
		float frameDuration,
		float startTime,
		float exposure,
		float frameStrength
	);

	EasySamplerSettings(EasySamplerSettings const & settings);

	float Exposure_get() const;
	float FrameDuration_get() const;
	float FrameStrength_get() const;
	int Height_get() const;
	Method Method_get() const;
	float StartTime_get() const;
	int Width_get() const;

private:
	float m_Exposure;
	float m_FrameDuration;
	float m_FrameStrength;
	int m_Height;
	Method m_Method;
	float m_StartTime;
	int m_Width;
};


// EasyByteSampler /////////////////////////////////////////////////////////////

class EasyByteSampler : public EasySamplerBase,
	private ISampleFns
{
public:


	/// <param name="pitch">bytes of memory to skip for a row</param>
	EasyByteSampler(
		EasySamplerSettings const & settings,
		int pitch,
		IFramePrinter * framePrinter
	);

	~EasyByteSampler();

	bool CanSkipConstant(float time, float durationPerSample);

	///	<param name="data">NULLPTR is interpreted as the ideal shutter being closed for the deltaTime that passed.</param>
	/// <remarks>A closed shutter and a weight of 0 are not the same, because the integral can be different (depends on the method).</remarks>
	void Sample(unsigned char const * data, float time);

protected:
	virtual void EasySamplerBase::MakeFrame()
	{
		PrintFrame();
		ClearFrame(m_Settings.FrameStrength_get());
	}

	virtual void EasySamplerBase::SubSample(
		float timeA,
		float timeB,
		float subTimeA,
		float subTimeB)
	{
		Integrator_Fn(this,
			m_HasLastSample ? m_LastSample : 0, m_CurSample,
			timeA, timeB, subTimeA, subTimeB
		);
	}

private:

	class Frame
	{
	public:
		Frame(size_t length)
		{
			Data = new float[length];
			WhitePoint = 0;

			memset(Data, 0, sizeof(float) * length);
		}

		~Frame()
		{
			delete Data;
		}

		float * Data;
		float WhitePoint;
	};

	unsigned char const * m_CurSample;
	EasySamplerSettings m_Settings;
	Frame * m_Frame;
	IFramePrinter * m_FramePrinter;
	bool m_HasLastSample;
	unsigned char * m_LastSample;
	int m_Pitch;
	unsigned char * m_PrintMem;

	void ClearFrame(float frameStrength);

	/// <summary>Implements ISampleFns.</summary>
	virtual void Fn_1(void const * sample);

	/// <summary>Implements ISampleFns.</summary>
	virtual void Fn_2(void const * sample, float w);

	/// <summary>Implements ISampleFns.</summary>
	virtual void Fn_4(void const * sampleA, void const * sampleB, float w);

	void PrintFrame();

	void ScaleFrame(float factor);
};


// EasyFloatSampler ////////////////////////////////////////////////////////////

class EasyFloatSampler : public EasySamplerBase,
	private ISampleFns
{
public:
	EasyFloatSampler(
		EasySamplerSettings const & settings,
		IFloatFramePrinter * framePrinter
	);

	~EasyFloatSampler();

	bool CanSkipConstant(float time, float durationPerSample);

	///	<param name="data">NULLPTR is interpreted as the ideal shutter being closed for the deltaTime that passed.</param>
	/// <remarks>A closed shutter and a weight of 0 are not the same, because the integral can be different (depends on the method).</remarks>
	void Sample(float const * data, float time);

protected:
	virtual void EasySamplerBase::MakeFrame()
	{
		PrintFrame();
		ClearFrame(m_Settings.FrameStrength_get());
	}

	virtual void EasySamplerBase::SubSample(
		float timeA,
		float timeB,
		float subTimeA,
		float subTimeB)
	{
		Integrator_Fn(this,
			m_HasLastSample ? m_LastSample : 0, m_CurSample,
			timeA, timeB, subTimeA, subTimeB
		);
	}

private:
	float const * m_CurSample;
	float * m_FrameData;
	IFloatFramePrinter * m_FramePrinter;
	float m_FrameWhitePoint;
	float * m_PrintMem;
	EasySamplerSettings m_Settings;
	bool m_HasLastSample;
	float * m_LastSample;

	void ClearFrame(float frameStrength);

	/// <summary>Implements ISampleFns.</summary>
	virtual void Fn_1(void const * sample);

	/// <summary>Implements ISampleFns.</summary>
	virtual void Fn_2(void const * sample, float w);

	/// <summary>Implements ISampleFns.</summary>
	virtual void Fn_4(void const * sampleA, void const * sampleB, float w);

	void PrintFrame();

	void ScaleFrame(float factor);
};


/*
// EasyXAlphaSampler ///////////////////////////////////////////////////////////

class EasyXAlphaSampler : public EasySamplerBase,
	private ISampleFns
{
public:


	/// <param name="dataComponents">number of bytes per alpha bytes</param>
	/// <param name="pitch">data bytes of memory to skip for a row</param>
	/// <param name="pitch">alpha bytes of memory to skip for a row</param>
	EasyXAlphaSampler(
		EasySamplerSettings const & settings,
		int dataComponents,
		int dataPitch,
		int alphaPitch,
		IXAlphaFramePrinter * framePrinter
	);

	~EasyByteSampler();

	bool CanSkipConstant(float time, float durationPerSample);

	///	<param name="data">NULLPTR is interpreted as the ideal shutter being closed for the deltaTime that passed.</param>
	/// <remarks>A closed shutter and a weight of 0 are not the same, because the integral can be different (depends on the method).</remarks>
	void Sample(unsigned char const * data, float time);

protected:
	virtual void EasySamplerBase::MakeFrame()
	{
		PrintFrame();
		ClearFrame(m_Settings.FrameStrength_get());
	}

	virtual void EasySamplerBase::SubSample(
		float timeA,
		float timeB,
		float subTimeA,
		float subTimeB)
	{
		Integrator_Fn(this,
			m_HasLastSample ? m_LastSample : 0, m_CurSample,
			timeA, timeB, subTimeA, subTimeB
		);
	}

private:
	int m_AlphaPitch;
	unsigned char const * m_CurSample;
	int m_DataComponents;
	int m_DataPitch;
	float * m_FrameAlpha;
	float * m_FrameData;
	float m_FrameWhitePoint;
	EasySamplerSettings m_Settings;
	IXAlphaFramePrinter * m_FramePrinter;
	bool m_HasLastSample;
	unsigned char * m_LastSample;
	int m_Pitch;
	unsigned char * m_PrintMem;

	void ClearFrame(float frameStrength);

	/// <summary>Implements ISampleFns.</summary>
	virtual void Fn_1(void const * sample);

	/// <summary>Implements ISampleFns.</summary>
	virtual void Fn_2(void const * sample, float w);

	/// <summary>Implements ISampleFns.</summary>
	virtual void Fn_4(void const * sampleA, void const * sampleB, float w);

	void PrintFrame();

	void ScaleFrame(float factor);
};
*/
