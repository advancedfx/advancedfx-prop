#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-09-29 dominik.matrixstorm.com
//
// First changes
// 2010-03-23 dominik.matrixstorm.com

#include "EasySampler.h"

#include <assert.h>
#include <math.h>

//#define DEBUG_EASYSAMPLER

#ifdef DEBUG_EASYSAMPLER
#include <hlsdk.h>
#include <hooks/HookHw.h>
#endif


void EasySamplerIntegrator_Fn(ISampleFns *fns, void const * sampleA, void const *sampleB, float timeA, float timeB, float subTimeA, float subTimeB)
{
	float weightA;
	float weightB;

	{
		// Calculate weigths:

		float dAB = timeB -timeA;
		float dSAB = subTimeB -subTimeA;
		float w = dAB ? subTimeA +subTimeB -2 * timeA / dAB : 0.0f;
		weightA = dSAB -w / 2.0f;
		weightB = dSAB * w / 2.0f;
	}

	//
	// optimize / combine the function:

	if(0 == weightA)
	{
		weightA = weightB;
		weightB = 0;
		sampleA = sampleB;
		sampleB = 0;
	}

	if(0 == weightA)
	{
		// zero weights.

		return;  // done;
	}

	assert(0 != weightA);

	if(0 == sampleA)
	{
		// switch to 1 point sampling.

		sampleA = sampleB;
		sampleB = 0;

		weightA = weightA + weightB;
		weightB = 0;
	}

	if(0 == sampleA)
	{
		// 0 point sampling.

		return; // done.
	}

	assert(0 != weightA);
	assert(0 != sampleA);

	if(0 == sampleB || 0 == weightB)
	{
		if(1 == weightA)
		{
			fns->Fn_1(sampleA);
		}
		else
		{
			fns->Fn_2(sampleA, weightA);
		}
	}
	else
	{
		// 2 points.

		assert(0 != weightB && 0 != sampleB);

		if(weightA == weightB)
		{
			if(1 == weightA)
			{
				fns->Fn_1(sampleA);
				fns->Fn_1(sampleB);
			}
			else
			{
				fns->Fn_4(sampleA, sampleB, weightA);
			}
		}
		else
		{
			if(1 == weightB)
			{
				void const *tS = sampleA;
				float tW = weightA;

				sampleA = sampleB;
				sampleB = tS;

				weightA = weightB;
				weightB = tW;
			}

			if(1 == weightA)
			{
				fns->Fn_1(sampleA);
				fns->Fn_2(sampleB, weightB);
			}
			else
			{
				fns->Fn_2(sampleA, weightA);
				fns->Fn_2(sampleB, weightB);
			}
		}
	}
}


// EasyByteSampler /////////////////////////////////////////////////////////////

EasyByteSampler::EasyByteSampler(
	int width,
	int height,
	int pitch,
	Method method,
	IFramePrinter * framePrinter,
	float frameDuration,
	float startTime,
	float exposure,
	float frameStrength
)
: EasySampler(frameDuration, startTime, exposure)
{
	assert(0 <= width);
	assert(0 <= height);
	assert(width <= pitch);

	m_DeltaPitch = pitch -width;
	m_Frame = new Frame(height * width);
	m_FramePrinter = framePrinter;
	m_FrameStrength = frameStrength;
	m_HasLastSample = false;
	m_Height = height;
	m_LastSample = ESM_Trapezoid == method ? new unsigned char[height * pitch] : 0;
	m_PrintMem = new unsigned char[height * pitch];
	m_Width =  width;
}


EasyByteSampler::~EasyByteSampler()
{
	// ? // PrintFrame();

	delete m_PrintMem;
	delete m_LastSample;
	delete m_Frame;
}

bool EasyByteSampler::CanSkipConstant(float time, float durationPerSample)
{
	return EasySampler::CanSkipConstant(time, durationPerSample, m_LastSample ? 1 : 0);
}


void EasyByteSampler::ClearFrame(float frameStrength)
{
	float w = m_Frame->WhitePoint * (1.0f -frameStrength);

	if(w) w = 1 / w;

	ScaleFrame(w);
}


void EasyByteSampler::Fn_1(void const * sample)
{
	int height = m_Height;
	int width = m_Width;
	int deltaPitch = m_DeltaPitch;
	float *fdata = m_Frame->Data;
	unsigned char const * cdata = (unsigned char const *)sample;

	for( int iy=0; iy < height; iy++ )
	{
		for( int ix=0; ix < width; ix++ ) 
		{
			*fdata = *fdata + *cdata;
			
			fdata++;
			cdata++;
		}
		cdata += deltaPitch;
	}

	m_Frame->WhitePoint += 255.0f;
}

void EasyByteSampler::Fn_2(void const * sample, float w)
{
	int height = m_Height;
	int width = m_Width;
	int deltaPitch = m_DeltaPitch;
	float *fdata = m_Frame->Data;
	unsigned char const * cdata = (unsigned char const *)sample;

	for( int iy=0; iy < height; iy++ )
	{
		for( int ix=0; ix < width; ix++ ) 
		{
			*fdata = *fdata + w * *cdata;
			
			fdata++;
			cdata++;
		}
		cdata += deltaPitch;
	}

	m_Frame->WhitePoint += w * 255.0f;
}

void EasyByteSampler::Fn_4(void const * sampleA, void const * sampleB, float w)
{
	int height = m_Height;
	int width = m_Width;
	int deltaPitch = m_DeltaPitch;
	float *fdata = m_Frame->Data;
	unsigned char const * cdataA = (unsigned char const *)sampleA;
	unsigned char const * cdataB = (unsigned char const *)sampleB;

	for( int iy=0; iy < height; iy++ )
	{
		for( int ix=0; ix < width; ix++ ) 
		{
			*fdata = *fdata + w * ((unsigned int)*cdataA +(unsigned int)*cdataB);
			
			fdata++;
			cdataA++;
			cdataB++;
		}
		cdataA += deltaPitch;
		cdataB += deltaPitch;
	}

	m_Frame->WhitePoint += w * 2.0f * 255.0f;
}


void EasyByteSampler::PrintFrame()
{
	float w = m_Frame->WhitePoint;

	unsigned char * data = m_PrintMem;

	if(0 == w)
	{
		memset(data, 0, m_Height * (m_Width +m_DeltaPitch)*sizeof(unsigned char));
	}
	else
	{
		float * fdata = m_Frame->Data;

		int ymax = m_Height;
		int xmax = m_Width;

		w = 255.0f / w;

		for( int iy=0; iy < ymax; iy++ )
		{
			for( int ix=0; ix < xmax; ix++ )
			{
				*data = (unsigned char)(w * *fdata);

				fdata++;
				data++;
			}

			data += m_DeltaPitch;
		}
	}

	m_FramePrinter->Print(m_PrintMem);
}


void EasyByteSampler::Sample(unsigned char const * data, float time)
{
	m_CurSample = data;

	EasySampler::Sample(time);

	if(m_LastSample && data)
	{
		memcpy(m_LastSample, data, m_Height * (m_Width +m_DeltaPitch)* sizeof(unsigned char));
		m_HasLastSample = true;
	}
	else
		m_HasLastSample = false;
}


void EasyByteSampler::ScaleFrame(float factor)
{
	float w = m_Frame->WhitePoint;

	if(w * factor == w)
	{
		// No change.
		return;
	}
	
	if(0 == w * factor)
	{
		// Zero.
		m_Frame->WhitePoint = 0;
		memset(m_Frame->Data, 0, m_Height * m_Width * sizeof(float));
		return;
	}
	
	float * fdata = m_Frame->Data;

	int ymax = m_Height;
	int xmax = m_Width;

	m_Frame->WhitePoint *= factor;

	for( int iy=0; iy < ymax; iy++ )
	{
		for( int ix=0; ix < xmax; ix++ )
		{
			*fdata = factor * *fdata;
			fdata++;
		}
	}
}



// EasySampler /////////////////////////////////////////////////////////////////


EasySampler::EasySampler(
	float frameDuration,
	float startTime,
	float exposure)
{
	m_FrameDuration = frameDuration;
	m_LastFrameTime = startTime;
	m_LastSampleTime = startTime;
	m_ShutterOpen = 0.0f < exposure;
	m_ShutterOpenDuration = frameDuration * min(max(exposure, 0.0f), 1.0f);
	m_ShutterTime = m_LastFrameTime;
}


bool EasySampler::CanSkipConstant(float time, float durationPerSample, int numPreviousSamplesRequired)
{
	// skipping is only allowed when enough samples are guaranteed to be captured to allow interpolation before the shutter is opened.

	float prepareTime = numPreviousSamplesRequired * durationPerSample;
	float timeLeft = m_FrameDuration -(time -m_LastFrameTime);

	return
		!m_ShutterOpen && prepareTime < timeLeft
	;
}


void EasySampler::Sample(float time)
{	
	float subMin = m_LastSampleTime;

#ifdef DEBUG_EASYSAMPLER
	pEngfuncs->Con_Printf("Sample: [%f, %f]\n", m_LastSampleTime, time);
#endif

	while(subMin < time)
	{		
		float subMax = time;

		float shutterEvent = m_ShutterTime+ (m_ShutterOpen ? m_ShutterOpenDuration : m_FrameDuration);
		float frameEnd = m_LastFrameTime +m_FrameDuration;

		// apply restrictions:

		if(subMin < frameEnd && frameEnd <= subMax)
		{
			subMax = frameEnd;
		}

		if(subMin < shutterEvent && shutterEvent <= subMax)
		{
			subMax = shutterEvent;
		}

		// sub sample restricted interval:

#ifdef DEBUG_EASYSAMPLER
		pEngfuncs->Con_Printf("\tSub interval: [%f, %f] shutter_%s", subMin, subMax, m_ShutterOpen ? "open" : "closed");
#endif

		if(m_ShutterOpen)
		{
			SubSample(
				m_LastSampleTime, time,
				subMin, subMax
			);
		}

		// process active restrictions:

		if(subMin < frameEnd && frameEnd <= subMax)
		{
#ifdef DEBUG_EASYSAMPLER
			pEngfuncs->Con_Printf(" frame_end");
#endif

			MakeFrame();
			m_LastFrameTime = subMax;
		}

		if(subMin < shutterEvent && shutterEvent <= subMax)
		{
#ifdef DEBUG_EASYSAMPLER
			pEngfuncs->Con_Printf(" shutter_event");
#endif
			if(0.0f < m_ShutterOpenDuration && m_ShutterOpenDuration < m_FrameDuration)
			{
				m_ShutterOpen = !m_ShutterOpen;

				if(m_ShutterOpen)
					m_ShutterTime = subMax;
			}
		}

#ifdef DEBUG_EASYSAMPLER
		pEngfuncs->Con_Printf("\n");
#endif

		subMin = subMax;
	}

	m_LastSampleTime = time;
}
