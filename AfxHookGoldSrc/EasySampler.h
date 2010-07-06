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

	typedef float (* Weighter)(float t, float deltaT);

	static float GaussWeighter(float t, float deltaT);
	static float RectangleWeighter(float t, float deltaT);

	/// <param name="pitch">number of bytes in a row</param>
	/// <param name="method">non-functional atm, always trapezoid</param>
	/// <param name="weighter">cannot be 0, the class provides default weighters</param>
	EasyByteSampler(
		int width,
		int height,
		int pitch,
		Method method,
		Weighter weighter,
		IFramePrinter * framePrinter,
		float frameDuration,
		float facFrame,
		float facLeakage,
		float facSample
		);

	~EasyByteSampler();

	void Sample(unsigned char const * data, float sampleDuration);

private:
	class Frame
	{
	public:
		Frame(size_t length)
		{
			Data = new float[length];
			Offset = 0;
			WhitePoint = 0;

			memset(Data, 0, sizeof(float) * length);
		}

		~Frame()
		{
			delete Data;
		}

		float * Data;
		float Offset;
		float WhitePoint;
	};

	float m_DeltaT;
	float m_FacLeakage;
	float m_FacFrame;
	float m_FacSample;
	Frame * m_Frame;
	float m_FrameDuration;
	IFramePrinter * m_FramePrinter;
	int m_Height;
	unsigned char * m_OldSample;
	int m_Pitch;
	unsigned char * m_PrintMem;
	bool m_TwoPoint;
	int m_Width;
	Weighter m_Weighter;

	void FinishFrame();
};
