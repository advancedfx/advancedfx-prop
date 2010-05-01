#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-24 dominik.matrixstorm.com
//
// First changes
// 2010-03-23 dominik.matrixstorm.com

#include "EasySampler.h"

#include <math.h>

const float C_Values_Direct_Gauss_0_3_129 [129] =
{
1.000000000000f, 0.999725379512f, 0.998901970464f, 0.997531128860f,
0.995615110574f, 0.993157065160f, 0.990161027230f, 0.986631905413f,
0.982575468958f, 0.977998332017f, 0.972907935674f, 0.967312527792f,
0.961221140740f, 0.954643567107f, 0.947590333473f, 0.940072672343f,
0.932102492360f, 0.923692346882f, 0.914855401070f, 0.905605397578f,
0.895956620993f, 0.885923861140f, 0.875522375390f, 0.864767850104f,
0.853676361345f, 0.842264335000f, 0.830548506450f, 0.818545879924f,
0.806273687668f, 0.793749349083f, 0.780990429942f, 0.768014601840f,
0.754839601989f, 0.741483193501f, 0.727963126258f, 0.714297098514f,
0.700502719315f, 0.686597471861f, 0.672598677914f, 0.658523463330f,
0.644388724825f, 0.630211098049f, 0.616006927043f, 0.601792235150f,
0.587582697452f, 0.573393614771f, 0.559239889298f, 0.545136001887f,
0.531095991035f, 0.517133433601f, 0.503261427251f, 0.489492574676f,
0.475838969551f, 0.462312184267f, 0.448923259405f, 0.435682694952f,
0.422600443223f, 0.409685903480f, 0.396947918201f, 0.384394770969f,
0.372034185937f, 0.359873328824f, 0.347918809378f, 0.336176685273f,
0.324652467358f, 0.313351126216f, 0.302277099945f, 0.291434303119f,
0.280826136839f, 0.270455499815f, 0.260324800397f, 0.250435969489f,
0.240790474272f, 0.231389332653f, 0.222233128371f, 0.213322026693f,
0.204655790611f, 0.196233797482f, 0.188055056028f, 0.180118223638f,
0.172421623894f, 0.164963264262f, 0.157740853880f, 0.150751821380f,
0.143993332689f, 0.137462308758f, 0.131155443147f, 0.125069219437f,
0.119199928403f, 0.113543684919f, 0.108096444544f, 0.102854019745f,
0.097812095742f, 0.092966245918f, 0.088311946788f, 0.083844592479f,
0.079559508718f, 0.075451966295f, 0.071517193990f, 0.067750390945f,
0.064146738483f, 0.060701411342f, 0.057409588343f, 0.054266462469f,
0.051267250371f, 0.048407201285f, 0.045681605375f, 0.043085801505f,
0.040615184442f, 0.038265211506f, 0.036031408671f, 0.033909376134f,
0.031894793362f, 0.029983423630f, 0.028171118069f, 0.026453819242f,
0.024827564252f, 0.023288487422f, 0.021832822542f, 0.020456904718f,
0.019157171837f, 0.017930165663f, 0.016772532593f, 0.015681024078f,
0.014652496752f, 0.013683912259f, 0.012772336819f, 0.011914940547f,
0.011108996538f
};


// EasyBgrSampler //////////////////////////////////////////////////////////////

float EasyBgrSampler::GaussWeighter(float t, float deltaT)
{
	float x = 2.0f *(t +deltaT / 2.0f) -1.0f;

	if( x<+0 ) x = -x; // gauss(-x) = gauss(x)
	if( x>+1 ) x = +1; // clamp

	x = x*128;
	float f_x_lo = floor(x);
	float f_x_hi = ceil(x);
	
	float f_x_diff = f_x_hi - f_x_lo;
	if( 0==f_x_diff ) x = 0;
	else x = (x-f_x_lo)/f_x_diff;

	float f_y_lo = C_Values_Direct_Gauss_0_3_129[ (unsigned char)(f_x_lo) ];

	return deltaT*(f_y_lo + x*( C_Values_Direct_Gauss_0_3_129[ (unsigned char)(f_x_hi) ] - f_y_lo ));
}

float EasyBgrSampler::RectangleWeighter(float t, float deltaT)
{
	return deltaT;
}

EasyBgrSampler::EasyBgrSampler(
		int width,
		int height,
		size_t rowAlignment, // >= 1
		Method method, 
		Weighter weighter, // cannot be 0
		float leftOffset,
		float rightOffset,
		FramePrinter framePrinter,
		float frameDuration)
{
	m_FrameCount = 0;
	m_FrameDuration = frameDuration;
	m_FramePrinter = framePrinter;
	m_Height = height;
	m_LeftOffset = leftOffset;
	m_PeakFrameCount = 0;
	m_PrintedCount = 0;
	m_RightOffset = rightOffset;
	m_RowSkip = (rowAlignment -(3*width % rowAlignment)) % rowAlignment;
	m_TwoPoint = false;
	m_Width =  width;
	m_Weighter = weighter;

	m_FrameFactory = new FrameFactory(m_Height*3*m_Width);
	m_OldSample = ESM_Trapezoid == method ? new unsigned char[m_Height*(3*m_Width +m_RowSkip)] : 0; 
	m_PrintMem = new unsigned char[m_Height*(3*m_Width +m_RowSkip)];

	m_FrameStore = new FrequentStore(m_FrameFactory);
}

EasyBgrSampler::~EasyBgrSampler()
{
	for(list<IStoreItem *>::iterator it = m_Frames.begin(); it != m_Frames.end(); it++)
		FinishFrame(*it);

	delete m_FrameStore;

	delete m_PrintMem;
	delete m_OldSample;
	delete m_FrameFactory;
}

IStoreItem * EasyBgrSampler::BeginFrame(float offset)
{
	IStoreItem * i = m_FrameStore->Aquire();
	m_FrameCount++;

	Frame * f = (Frame *)i->GetValue();

	memset(f->Data, 0, sizeof(float)*m_Height*3*m_Width);
	f->Offset = offset;
	f->WhitePoint = 0;

	if(m_PeakFrameCount < m_FrameCount)
		m_PeakFrameCount = m_FrameCount;

	return i;
}

void EasyBgrSampler::FinishFrame(IStoreItem * item)
{
	Frame * f = (Frame *)item->GetValue();
	float w = f->WhitePoint;

	unsigned char * data = m_PrintMem;

	if(0 == w)
	{
		memset(data, 0, m_Height*(3*m_Width+m_RowSkip));
	}
	else
	{
		float * fdata = f->Data;

		int ymax = m_Height;
		int xmax = 3*m_Width;

		for( int iy=0; iy < ymax; iy++ )
		{
			for( int ix=0; ix < xmax; ix++ )
			{
				*data = (unsigned char)(255.0f * (*fdata / w));

				fdata++;
				data++;
			}

			data += m_RowSkip;
		}
	}

	m_FramePrinter(m_PrintMem, m_PrintedCount);
	m_PrintedCount++;

	m_FrameCount--;
	item->Release();

	return;
}

int EasyBgrSampler::GetPeakFrameCount()
{
	return m_PeakFrameCount;
}


void EasyBgrSampler::Sample(unsigned char const * data, float sampleDuration)
{
	float frameBoundLo = m_LeftOffset;
	float frameBoundHi = m_FrameDuration + m_RightOffset;
	list<IStoreItem *>::iterator it;
	
	// insert new frames:
	float maxFrameBound = m_Frames.empty() ? 0 : ((Frame *)m_Frames.back()->GetValue())->Offset +m_FrameDuration;
	while(maxFrameBound +frameBoundLo < sampleDuration)
	{
		IStoreItem * i = BeginFrame(maxFrameBound);
		m_Frames.push_back(i);

		maxFrameBound += m_FrameDuration;
	}

	// sample (and update frames):
	for(it = m_Frames.begin(); it != m_Frames.end(); it++)
		SampleFrame(*it, data, sampleDuration);

	// finish frames:
	it = m_Frames.begin();
	while(it != m_Frames.end())
	{
		IStoreItem * i = *it;
		Frame * f = (Frame *)i->GetValue();

		if(f->Offset +frameBoundHi <= 0)
		{
			FinishFrame(i);
			m_Frames.erase(it);
			it = m_Frames.begin();
		}
		else
			it++;
	}

	if(m_OldSample)
	{
		memcpy(m_OldSample, data, m_Height*(3*m_Width +m_RowSkip));	
		m_TwoPoint = true;
	}
}

void EasyBgrSampler::SampleFrame(IStoreItem * item, unsigned char const * data, float sampleDuration)
{
	Frame * f = (Frame *)item->GetValue();

	float frameLo = m_LeftOffset +f->Offset;
	float frameHi = m_FrameDuration +m_RightOffset +f->Offset;
	float frameDelta = frameHi -frameLo;

	// calculate and normalize intersection with frame:
	float sampT ,sampDelta;

	if(0 != frameDelta)
	{
		sampT = -frameLo / frameDelta;
		sampDelta = sampleDuration * m_FrameDuration  / frameDelta;
	}
	else
	{
		sampT = 0;
		sampDelta = 0;
	}

	// get frame weight:
	float w = m_Weighter(sampT, sampDelta);

	//
	// update image data:

	f->WhitePoint += w*255.0f;
	float * fdata = f->Data;

	int ymax = m_Height;
	int xmax = 3*m_Width;

	if(!m_TwoPoint)
	{
		for( int iy=0; iy < ymax; iy++ )
		{
			for( int ix=0; ix < xmax; ix++ )
			{
				*fdata += w * (float)(*data);

				fdata++;
				data++;
			}

			data += m_RowSkip;
		}
	}
	else
	{
		w *= 0.5f;

		unsigned char const * data2 = m_OldSample;
		for( int iy=0; iy < ymax; iy++ )
		{
			for( int ix=0; ix < xmax; ix++ )
			{
				*fdata += w * ((int)(*data) +(int)(*data2));

				fdata++;
				data++;
				data2++;
			}

			data += m_RowSkip;
			data2 += m_RowSkip;
		}
	}

	// update frame time:
	f->Offset -= sampDelta;
}