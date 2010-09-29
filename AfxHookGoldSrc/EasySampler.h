#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-09-29 dominik.matrixstorm.com
//
// First changes
// 2010-03-23 dominik.matrixstorm.com

#include "Store.h"

#include <list>

using namespace std;


class __declspec(novtable) IFramePrinter abstract
{
public:
	virtual void Print(unsigned char const * data) abstract = 0;
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


/// <summary>
///	  Auto 2 (trapezium) / 1 (rectangle) / 0 point sampling by integration.<br />
///   Selects, optimizes and combines the integration using a given set of base functions.
/// </summary>
/// <param name="sampleA">can be 0</param>
/// <param name="sampleB">can be 0</param>
void EasySamplerIntegrator_Fn(ISampleFns *fns, void const *sampleA, void const *sampleB, float timeA, float timeB, float subTimeA, float subTimeB);


/// <summary>
///		Base class, not intended to be used directly.
///		This class supplies implementation of fundamental sampling logic.
/// </summary>
class EasySampler abstract
{
public:

protected:
	/// <param name="exposure">time the shutter is kept open measured in number of frames</param>
	EasySampler(
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

private:
	float m_FrameDuration;
	float m_LastFrameTime;
	float m_LastSampleTime;
	bool m_ShutterOpen;
	float m_ShutterOpenDuration;
	float m_ShutterTime;
};


class EasyByteSampler : public EasySampler,
	private ISampleFns
{
public:
	enum Method
	{
		ESM_Rectangle,
		ESM_Trapezoid
	};

	/// <param name="pitch">bytes of memory to skip for a row</param>
	/// <param name="exposure">time the shutter is kept open measured in number of frames</param>
	EasyByteSampler(
		int width, 
		int height,
		int pitch,
		Method method,
		IFramePrinter * framePrinter,
		float frameDuration,
		float startTime,
		float exposure,
		float frameStrength
		);

	~EasyByteSampler();

	bool CanSkipConstant(float time, float durationPerSample);

	///	<param name="data">NULLPTR is interpreted as the ideal shutter being closed for the deltaTime that passed.</param>
	/// <remarks>A closed shutter and a weight of 0 are not the same, because the integral can be different (depends on the method).</remarks>
	void Sample(unsigned char const * data, float time);

protected:
	virtual void EasySampler::MakeFrame()
	{
		PrintFrame();
		ClearFrame(m_FrameStrength);
	}

	virtual void EasySampler::SubSample(
		float timeA,
		float timeB,
		float subTimeA,
		float subTimeB)
	{
		EasySamplerIntegrator_Fn(this,
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
	int m_DeltaPitch;
	Frame * m_Frame;
	float m_FrameStrength;
	IFramePrinter * m_FramePrinter;
	bool m_HasLastSample;
	int m_Height;
	unsigned char * m_LastSample;
	unsigned char * m_PrintMem;
	int m_Width;

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
