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


// EasyByteSampler //////////////////////////////////////////////////////////////

float EasyByteSampler::GaussWeighter(float t, float deltaT)
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

float EasyByteSampler::RectangleWeighter(float t, float deltaT)
{
	return deltaT;
}

EasyByteSampler::EasyByteSampler(
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
		)
{
	assert(0 <= width);
	assert(0 <= height);
	assert(width <= pitch);

	m_DeltaT = 0;
	m_FacLeakage = facLeakage;
	m_FacFrame = facFrame;
	m_FacSample = facSample;
	m_Frame = new Frame(height * width);
	m_FrameDuration = frameDuration;
	m_FramePrinter = framePrinter;
	m_Height = height;
	m_Pitch = pitch;
	m_OldSample = ESM_Trapezoid == method ? new unsigned char[height * pitch] : 0; 
	m_PrintMem = new unsigned char[height * pitch];
	m_TwoPoint = false;
	m_Width =  width;
	m_Weighter = weighter;
}

EasyByteSampler::~EasyByteSampler()
{
	FinishFrame();

	delete m_PrintMem;
	delete m_OldSample;
	delete m_Frame;
}


void EasyByteSampler::FinishFrame()
{
	float w = m_Frame->WhitePoint;
	float ff = m_FacFrame;

	unsigned char * data = m_PrintMem;

	if(0 == w)
	{
		memset(data, 0, m_Height * m_Pitch);
	}
	else
	{
		float * fdata = m_Frame->Data;

		int ymax = m_Height;
		int xmax = m_Width;

		if(0 == ff)
		{
			for( int iy=0; iy < ymax; iy++ )
			{
				for( int ix=0; ix < xmax; ix++ )
				{
					*data = (unsigned char)(255.0f * (*fdata / w));
					*fdata = 0;

					fdata++;
					data++;
				}

				data += m_Pitch -m_Width;
			}
		}
		else if(1 == ff)
		{
			for( int iy=0; iy < ymax; iy++ )
			{
				for( int ix=0; ix < xmax; ix++ )
				{
					*data = (unsigned char)(255.0f * (*fdata / w));

					fdata++;
					data++;
				}

				data += m_Pitch -m_Width;
			}
		}
		else
		{
			for( int iy=0; iy < ymax; iy++ )
			{
				for( int ix=0; ix < xmax; ix++ )
				{
					float f = *fdata;
					*data = (unsigned char)(255.0f * (f / w));
					*fdata = ff * f;

					fdata++;
					data++;
				}

				data += m_Pitch -m_Width;
			}
		}
	}

	m_FramePrinter->Print(m_PrintMem);

	m_Frame->WhitePoint = ff * w;
}

void EasyByteSampler::Sample(unsigned char const * data, float sampleDuration)
{
	while(0 < sampleDuration)
	{
		float frameRemaining = m_FrameDuration +m_Frame->Offset;
		float deltaT = sampleDuration;
		if(deltaT > frameRemaining) deltaT = frameRemaining;

		float t = -m_Frame->Offset / m_FrameDuration;
		float w = m_FacSample * m_Weighter(t, deltaT);
		float fl = m_DeltaT * m_FacLeakage;

		//
		// update image data:

		float * fdata = m_Frame->Data;

		int ymax = m_Height;
		int xmax = m_Width;

		if(!m_TwoPoint)
		{
			unsigned char const * data1 = data;

			if(0 == fl)
			{
				for( int iy=0; iy < ymax; iy++ )
				{
					for( int ix=0; ix < xmax; ix++ )
					{
						*fdata = *fdata + w * (*data1);

						fdata++;
						data1++;
					}

					data1 += m_Pitch -m_Width;
				}
			}
			else if(m_Frame->WhitePoint <= fl)
			{
				for( int iy=0; iy < ymax; iy++ )
				{
					for( int ix=0; ix < xmax; ix++ )
					{
						*fdata = w * (*data1);

						fdata++;
						data1++;
					}

					data1 += m_Pitch -m_Width;
				}
			}
			else
			{
				for( int iy=0; iy < ymax; iy++ )
				{
					for( int ix=0; ix < xmax; ix++ )
					{
						float f = (*fdata) -fl;
						if(f < 0) f = 0;
						*fdata = f + w * (*data1);

						fdata++;
						data1++;
					}

					data1 += m_Pitch -m_Width;
				}
			}
		}
		else
		{
			float w2 = 0.5*w;

			unsigned char const * data1 = data;
			unsigned char const * data2 = m_OldSample;

			if(0 == fl)
			{
				for( int iy=0; iy < ymax; iy++ )
				{
					for( int ix=0; ix < xmax; ix++ )
					{
						*fdata = *fdata + w2 * ((int)(*data1) +(int)(*data2));

						fdata++;
						data1++;
						data2++;
					}

					data1 += m_Pitch -m_Width;
					data2 += m_Pitch -m_Width;
				}
			}
			else if(m_Frame->WhitePoint <= fl)
			{
				for( int iy=0; iy < ymax; iy++ )
				{
					for( int ix=0; ix < xmax; ix++ )
					{
						*fdata = w2 * ((int)(*data1) +(int)(*data2));

						fdata++;
						data1++;
						data2++;
					}

					data1 += m_Pitch -m_Width;
					data2 += m_Pitch -m_Width;
				}
			}
			else
			{
				for( int iy=0; iy < ymax; iy++ )
				{
					for( int ix=0; ix < xmax; ix++ )
					{
						float f = (*fdata) -fl;
						if(f < 0) f = 0;
						*fdata = f + w2 * ((int)(*data1) +(int)(*data2));

						fdata++;
						data1++;
						data2++;
					}

					data1 += m_Pitch -m_Width;
					data2 += m_Pitch -m_Width;
				}
			}
		}

		m_Frame->WhitePoint = m_Frame->WhitePoint -fl;
		if(m_Frame->WhitePoint < 0) m_Frame->WhitePoint = 0;
		m_Frame->WhitePoint += w * 255.0f;

		// update frame time:
		m_Frame->Offset -= deltaT;

		// check if frame needs to be printed:
		if(m_Frame->Offset + m_FrameDuration <= 0)
		{
			m_Frame->Offset += m_FrameDuration;
			FinishFrame();
		}

		m_DeltaT = deltaT;
		sampleDuration -= deltaT;
	}

	if(m_OldSample)
	{
		memcpy(m_OldSample, data, m_Height * m_Pitch);	
		m_TwoPoint = true;
	}
}