#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-07-04 dominik.matrixstorm.com
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

// fdata and sample are modified.
#define SAMPLE_FN(fdata, sample, width, height, deltaPitch, add) \
{ \
	for( int iy=0; iy < (height); iy++ ) \
	{ \
		for( int ix=0; ix < (width); ix++ ) \
		{ \
			*(fdata) = *(fdata) + (add); \
			\
			(fdata)++; \
			(sample)++; \
		} \
		\
		(sample) += (deltaPitch); \
	} \
}

// fdata and sampleA and SampleB are modified.
#define SAMPLE_FN_2(fdata, sampleA, sampleB, width, height, deltaPitch, add) \
{ \
	for( int iy=0; iy < (height); iy++ ) \
	{ \
		for( int ix=0; ix < (width); ix++ ) \
		{ \
			*(fdata) = *(fdata) + (add); \
			\
			(fdata)++; \
			(sampleA)++; \
			(sampleB)++; \
		} \
		\
		(sampleA) += (deltaPitch); \
		(sampleB) += (deltaPitch); \
	} \
}


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
{
	assert(0 <= width);
	assert(0 <= height);
	assert(width <= pitch);

	m_DeltaPitch = pitch -width;
	m_Frame = new Frame(height * width);
	m_FrameDuration = frameDuration;
	m_FramePrinter = framePrinter;
	m_FrameStrength = frameStrength;
	m_HasLastSample = false;
	m_Height = height;
	m_LastFrameTime = startTime;
	m_LastSample = ESM_Trapezoid == method ? new unsigned char[height * pitch] : 0;
	m_LastSampleTime = startTime;
	m_PrintMem = new unsigned char[height * pitch];
	m_ShutterOpen = 0.0f < exposure;
	m_ShutterOpenDuration = frameDuration * min(max(exposure, 0.0f), 1.0f);
	m_ShutterTime = m_LastFrameTime;
	m_Width =  width;
}


EasyByteSampler::~EasyByteSampler()
{
	// ? // PrintFrame();

	delete m_PrintMem;
	delete m_LastSample;
	delete m_Frame;
}


void EasyByteSampler::ClearFrame(float frameStrength)
{
	float w = m_Frame->WhitePoint * (1.0f -frameStrength);

	if(w) w = 1 / w;

	ScaleFrame(w);
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
#ifdef DEBUG_EASYSAMPLER
			if(!m_HasLastSample ||!data)
				pEngfuncs->Con_Printf("WARNING: missing sample(s), possible quality loss, verify code, only at start past samples may be missing.\n");
#endif

			SubSample(
				m_HasLastSample ? m_LastSample : 0, data,
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

			PrintFrame();
			ClearFrame(m_FrameStrength);
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
	if(m_LastSample && data)
	{
		memcpy(m_LastSample, data, m_Height * (m_Width +m_DeltaPitch)* sizeof(unsigned char));
		m_HasLastSample = true;
	}
	else
		m_HasLastSample = false;
}


void EasyByteSampler::SampleFn(
	unsigned char const * sample,
	float weight)
{
	if(0 == weight) return;
	if(0 == sample) return;

	float * fdata = m_Frame->Data;
	int xmax = m_Width;
	int ymax = m_Height;

	m_Frame->WhitePoint += weight * (unsigned char)255;

	if(1 == weight)
	{
		SAMPLE_FN(fdata, sample, xmax, ymax, m_DeltaPitch,
			*sample
		);
	}
	else
	{
		SAMPLE_FN(fdata, sample, xmax, ymax, m_DeltaPitch,
			weight * *sample
		);
	}

}


void EasyByteSampler::SampleFn(
	unsigned char const * sampleA,
	unsigned char const * sampleB,
	float weight)
{
	if(0 == weight) return;
	if(0 == sampleA)
	{
		SampleFn(sampleB, weight);
		return;
	}
	if(0 == sampleB)
	{
		SampleFn(sampleA, weight);
		return;
	}

	int ymax = m_Height;
	int xmax = m_Width;
	float * fdata = m_Frame->Data;

	m_Frame->WhitePoint += weight * (unsigned char)255;

	if(1 == weight)
	{
		SAMPLE_FN_2(fdata, sampleA, sampleB, xmax, ymax, m_DeltaPitch,
			(unsigned short)*sampleA +*sampleB
		);
	}
	else
	{
		SAMPLE_FN_2(fdata, sampleA, sampleB, xmax, ymax, m_DeltaPitch,
			weight * ((unsigned short)*sampleA +*sampleB)
		);
	}
}


void EasyByteSampler::SampleFn(
	unsigned char const * sampleA,
	unsigned char const * sampleB,
	float weightA,
	float weightB)
{
	if(0 == weightA)
	{
		SampleFn(sampleB, weightB);
		return;
	}
	if(0 == weightB)
	{
		SampleFn(sampleA, weightA);
		return;
	}
	if(weightA == weightB)
	{
		SampleFn(sampleA, sampleB, weightA);
		return;
	}
	if(0 == sampleA)
	{
		SampleFn(sampleB, weightA +weightB);
		return;
	}
	if(0 == sampleB)
	{
		SampleFn(sampleA, weightA +weightB);
		return;
	}

	// 0 != weightA != weightB != 0
	// , 0!= SampleA, 0 != SampleB.

	float * fdata = m_Frame->Data;
	int xmax = m_Width;
	int ymax = m_Height;

	m_Frame->WhitePoint += (weightA +weightB) * (unsigned char)255;

	if(1 == weightA && 1 == weightB)
	{
		SAMPLE_FN_2(fdata, sampleA, sampleB, xmax, ymax, m_DeltaPitch,
			(unsigned short)*sampleA + *sampleB
		);
	}
	else if(1 == weightA || 1 == weightB)
	{
		SAMPLE_FN_2(fdata, sampleA, sampleB, xmax, ymax, m_DeltaPitch,
			weightA * *sampleA + *sampleB
		);
	}
	else
	{
		SAMPLE_FN_2(fdata, sampleA, sampleB, xmax, ymax, m_DeltaPitch,
			weightA * *sampleA + weightB * *sampleB
		);
	}
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


void EasyByteSampler::SubSample(
	unsigned char const * sampleA,
	unsigned char const * sampleB,
	float timeA,
	float timeB,
	float subTimeA,
	float subTimeB)
{
	float dAB = timeB -timeA;
	float dSAB = subTimeB -subTimeA;
	float w = dAB ? subTimeA +subTimeB -2 * timeA / dAB : 0.0f;
	float wA = dSAB -w / 2.0f;
	float wB = dSAB * w / 2.0f;

	SampleFn(sampleA, sampleB, wA, wB);
}
