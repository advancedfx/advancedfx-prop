#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-24 dominik.matrixstorm.com
//
// First changes
// 2010-03-23 dominik.matrixstorm.com

#include "Store.h"

#include <list>

using namespace std;


class EasyBgrSampler
{
public:
	enum Method
	{
		ESM_Rectangle,
		ESM_Trapezoid
	};

	typedef float (* Weighter)(float t, float deltaT);
	typedef void (* FramePrinter)(unsigned char * data, int number);

	static float GaussWeighter(float t, float deltaT);
	static float RectangleWeighter(float t, float deltaT);

	EasyBgrSampler(
		int width,
		int height,
		size_t rowAlignment, // >= 1
		Method method,  // non-functional atm, always trapezoid
		Weighter weighter, // cannot be 0, the class provides default weighters, see bellow
		float leftOffset,
		float rightOffset,
		FramePrinter framePrinter,
		float frameDuration);

	~EasyBgrSampler();

	int GetPeakFrameCount();

	void Sample(unsigned char const * data, float sampleDuration);

private:
	struct Frame
	{
		float * Data;
		float Offset;
		float WhitePoint;
	};

	class FrameFactory :
		public IStoreFactory
	{
	public:
		FrameFactory(int dataCount)
		{
			m_DataCount = dataCount;
		}

		virtual StoreValue ConstructValue() override
		{
			Frame * frame = new Frame();
			frame->Data = new float[m_DataCount];

			return (StoreValue)frame;
		}

		virtual void DestructValue(StoreValue value) override
		{
			Frame * frame = (Frame *)value;

			delete frame->Data;
			delete frame;
		}

	private:

		int m_DataCount;
	};

	int m_FrameCount;
	float m_FrameDuration;
	FrameFactory  * m_FrameFactory;
	IStore * m_FrameStore;
	list<IStoreItem *> m_Frames;
	FramePrinter m_FramePrinter;
	int m_Height;
	float m_LeftOffset;
	unsigned char * m_OldSample;
	int m_PeakFrameCount;
	unsigned char * m_PrintMem;
	int m_PrintedCount;
	float m_RightOffset;
	int m_RowSkip;
	bool m_TwoPoint;
	int m_Width;
	Weighter m_Weighter;

	IStoreItem *  BeginFrame(float offset);
	
	void FinishFrame(IStoreItem * item);

	void SampleFrame(IStoreItem * item, unsigned char const * data, float sampleDuration);
};
