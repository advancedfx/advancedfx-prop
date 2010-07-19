#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-07-04 dominik.matrixstorm.com
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


class EasyByteSampler
{
public:
	enum Method
	{
		ESM_Rectangle,
		ESM_Trapezoid
	};

	struct SkipHint
	{
		float Duration;
		int Samples;
	};

	/// <param name="pitch">bytes of memory to skip for a row</param>
	/// <param name="frameSpan">time span per frame</param>
	/// <param name="exposure">time the shutter is kept open measured in number of frames</param>
	/// <param name="shutterStrength">closing strength of the shutter (none = 0.0 - 1.0 = total)</param>
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

	bool CanSkipConstant(float time, float durationPerSample)
	{
		// skipping is only allowed when enough samples are guaranteed to be captured to allow interpolation before the shutter is opened.

		int numSamples = m_LastSample ? 1 : 0;

		float prepareTime = numSamples * durationPerSample;
		float timeLeft = m_FrameDuration -(time -m_LastFrameTime);

		return
			!m_ShutterOpen && prepareTime < timeLeft
		;
	}

	///	<param name="data">NULLPTR is interpreted as the ideal shutter being closed for the deltaTime that passed.</param>
	/// <remarks>A closed shutter and a weight of 0 are not the same, because the integral can be different (depends on the method).</remarks>
	void Sample(unsigned char const * data, float time);


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

	int m_DeltaPitch;
	Frame * m_Frame;
	float m_FrameDuration;
	float m_FrameStrength;
	IFramePrinter * m_FramePrinter;
	bool m_HasLastSample;
	int m_Height;
	float m_LastFrameTime;
	unsigned char * m_LastSample;
	float m_LastSampleTime;
	unsigned char * m_PrintMem;
	bool m_ShutterOpen;
	float m_ShutterOpenDuration;
	float m_ShutterTime;
	int m_Width;

	void ClearFrame(float frameStrength);

	void PrintFrame();

	/// <summary>buffer += weight * sample</summary>
	void SampleFn(
		unsigned char const * sample,
		float weight);

	/// <summary>buffer += weight * (sampleA +sampleB)</summary>
	void SampleFn(
		unsigned char const * sampleA,
		unsigned char const * sampleB,
		float weight);

	/// <summary>buffer += weightA * sampleA +weightB * sampleB</summary>
	void SampleFn(
		unsigned char const * sampleA,
		unsigned char const * sampleB,
		float weightA,
		float weightB);

	void ScaleFrame(float factor);


	void SubSample(
		unsigned char const * sampleA,
		unsigned char const * sampleB,
		float timeA,
		float timeB,
		float subTimeA,
		float subTimeB);
};
