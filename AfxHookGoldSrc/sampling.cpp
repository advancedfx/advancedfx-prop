#include "stdafx.h"

// Project :  Half-Life Advanced Effects
// File    :  dll/sampling.cpp

// Authors : last change / first change / name

// 2008-07-10 / 2008-07-10 / Dominik Tugend

#include "sampling.h"

#include <math.h> // ceil, floor
#include <list>

using namespace hlae;
using namespace hlae::sampler;


////////////////////////////////////////////////////////////////////////////////
//
//	Frame weight functions:
//

const float C_Values_Direct_Gauss_0_3_129 [129] =
{
1.000000000000, 0.999725379512, 0.998901970464, 0.997531128860,
0.995615110574, 0.993157065160, 0.990161027230, 0.986631905413,
0.982575468958, 0.977998332017, 0.972907935674, 0.967312527792,
0.961221140740, 0.954643567107, 0.947590333473, 0.940072672343,
0.932102492360, 0.923692346882, 0.914855401070, 0.905605397578,
0.895956620993, 0.885923861140, 0.875522375390, 0.864767850104,
0.853676361345, 0.842264335000, 0.830548506450, 0.818545879924,
0.806273687668, 0.793749349083, 0.780990429942, 0.768014601840,
0.754839601989, 0.741483193501, 0.727963126258, 0.714297098514,
0.700502719315, 0.686597471861, 0.672598677914, 0.658523463330,
0.644388724825, 0.630211098049, 0.616006927043, 0.601792235150,
0.587582697452, 0.573393614771, 0.559239889298, 0.545136001887,
0.531095991035, 0.517133433601, 0.503261427251, 0.489492574676,
0.475838969551, 0.462312184267, 0.448923259405, 0.435682694952,
0.422600443223, 0.409685903480, 0.396947918201, 0.384394770969,
0.372034185937, 0.359873328824, 0.347918809378, 0.336176685273,
0.324652467358, 0.313351126216, 0.302277099945, 0.291434303119,
0.280826136839, 0.270455499815, 0.260324800397, 0.250435969489,
0.240790474272, 0.231389332653, 0.222233128371, 0.213322026693,
0.204655790611, 0.196233797482, 0.188055056028, 0.180118223638,
0.172421623894, 0.164963264262, 0.157740853880, 0.150751821380,
0.143993332689, 0.137462308758, 0.131155443147, 0.125069219437,
0.119199928403, 0.113543684919, 0.108096444544, 0.102854019745,
0.097812095742, 0.092966245918, 0.088311946788, 0.083844592479,
0.079559508718, 0.075451966295, 0.071517193990, 0.067750390945,
0.064146738483, 0.060701411342, 0.057409588343, 0.054266462469,
0.051267250371, 0.048407201285, 0.045681605375, 0.043085801505,
0.040615184442, 0.038265211506, 0.036031408671, 0.033909376134,
0.031894793362, 0.029983423630, 0.028171118069, 0.026453819242,
0.024827564252, 0.023288487422, 0.021832822542, 0.020456904718,
0.019157171837, 0.017930165663, 0.016772532593, 0.015681024078,
0.014652496752, 0.013683912259, 0.012772336819, 0.011914940547,
0.011108996538
};

// the function clamps x to the normalized range [-1,1]
float FrameWeight_Direct_Gauss_0_3_129_InterpLin(float x)
{

	if( x<+0 ) x = -x; // gauss(-x) = gauss(x)
	if( x>+1 ) x = +1; // clamp

	x = x*128;
	float f_x_lo = floor(x);
	float f_x_hi = ceil(x);
	
	float f_x_diff = f_x_hi - f_x_lo;
	if( 0==f_x_diff ) x = 0;
	else x = (x-f_x_lo)/f_x_diff;

	float f_y_lo = C_Values_Direct_Gauss_0_3_129[ (unsigned char)(f_x_lo) ];

	return f_y_lo + x*( C_Values_Direct_Gauss_0_3_129[ (unsigned char)(f_x_hi) ] - f_y_lo );
}



////////////////////////////////////////////////////////////////////////////////
//
//	C_FrameSampler

CSampleMaster::CSampleMaster()
{
	m_SamplerState = SS_IDLE;
	m_SamplerError = SE_NONE;

	m_setting.time_start = 0;
	m_time.time_min = 0;

	m_tracker = 0;
}

CSampleMaster::~CSampleMaster()
{
	EndSampling( m_time.time_min );
}

bool CSampleMaster::BeginSampling(
	IFrameMaster *interfaceFrameMaster,
	float time_start,
	float frames_per_second, // >0
	float offset_window_begin, // <= 0
	float offset_window_end // >= 0
)
{
	if( SS_IDLE != m_SamplerState )
	{
		RaiseError( SE_STATE_ERROR );
		return false;
	}

	m_setting.interfaceFrameMaster = interfaceFrameMaster;
	m_setting.time_start           = time_start;
	m_setting.frames_per_second    = frames_per_second;
	m_setting.offset_window_begin  = offset_window_begin;
	m_setting.offset_window_end    = offset_window_end;

	m_time.time_min    = time_start;
	m_time.frame_count = 0;

	m_SamplerState = SS_SAMPLING;

	interfaceFrameMaster->I_SamplerConnect( this );

	return true;
}

bool CSampleMaster::Sample( float time, const void *pData, unsigned long cbDataSize )
{
	if (SS_SAMPLING != m_SamplerState)
	{
		RaiseError( SE_STATE_ERROR );
		return false;
	}
	if( time < m_time.time_min )
	{
		RaiseError( SE_TIME_ERROR );
		return false;
	}

	m_time.time_min = time;

	if( !Finish_Frames_OutOfScope( time ) )
		return false;

	if ( !Create_Frames_NewInScope( time ) )
		return false;

	Sample_s *psample = new Sample_s;
	psample->ul_RefCount = 0;
	psample->pData = malloc(cbDataSize);
	memcpy(psample->pData,pData,cbDataSize);
	m_tracker++;

	psample->ul_RefCount++; // lock sample during operation on the frame list

	bool bOk = true;
	std::list<Frame_s *>::iterator it_end = frames.end();
	for( std::list<Frame_s *>::iterator it = frames.begin(); it != it_end; it++ )
	{
		psample->ul_RefCount++; // lock for the frame
		if( !(m_setting.interfaceFrameMaster->I_NewSample(
			(*it)->frameId,
			(time - (*it)->time.window_min) / (*it)->time.window_delta,
			(SampleId_t)psample
		)) )
		{
			psample->ul_RefCount--; // frame has failed to sample, unlock again
			bOk=false;
		}
	}

	I_ReleaseSample( (SampleId_t)psample ); // unlock again and free if not used anymore

	if( !bOk )
		RaiseError();

	return bOk;
}

bool CSampleMaster::EndSampling( float time_end )
{
	if (SS_IDLE == m_SamplerState)
		return true;

	bool bOk = true;
	while(!frames.empty())
	{
		Frame_s *frame = frames.front();
		if( !(m_setting.interfaceFrameMaster->I_FinishFrame(
			frame->frameId,
			time_end
		)) )
			bOk = false;
		delete frame;
		frames.pop_front();
	}

	m_SamplerState = SS_IDLE;

	if( !bOk )
	{
		RaiseError();
		return false;
	}
	
	return true;
}

CSampleMaster::SamplerError_e CSampleMaster::HadError()
{
	SamplerError_e retError=m_SamplerError;
	m_SamplerError = SE_NONE;
	return retError;
}


unsigned long CSampleMaster::GetTracker()
{
	return m_tracker;
}


void * CSampleMaster::I_GetSampleData( SampleId_t sampleId )
{
	return ((Sample_s *)sampleId)->pData;
}

void CSampleMaster::I_ReleaseSample( SampleId_t sampleId )
{
	Sample_s *psample = (Sample_s*)sampleId;

	psample->ul_RefCount--;

	if(!(psample->ul_RefCount))
	{
		free(psample->pData);
		delete psample;
		m_tracker--;
	}
}

void CSampleMaster::RaiseError()
{
	RaiseError( SE_ERROR );
}

void CSampleMaster::RaiseError( SamplerError_e errorcode )
{
	if( SE_NONE == m_SamplerError)
		m_SamplerError = errorcode;
}

bool CSampleMaster::Finish_Frames_OutOfScope(float time)
{
	bool bDone = false;
	bool bOk = true;

	while(!frames.empty() && !bDone)
	{
		Frame_s *frame = frames.front();
		if( frame->time.window_max < time )
		{
			if( !(m_setting.interfaceFrameMaster->I_FinishFrame(
				frame->frameId,
				time
			)) )
				bOk = false;
			delete frame;
			frames.pop_front();
		}
		else bDone = true;
	}

	if( !bOk)
		RaiseError();

	return bOk;
}

bool CSampleMaster::Create_Frames_NewInScope(float time)
{
	bool bDone = false;
	bool bOk = true;

	while( !bDone )
	{
		float time_covered = m_setting.time_start
			+ m_time.frame_count / m_setting.frames_per_second
			+ m_setting.offset_window_begin / m_setting.frames_per_second;

		if ( time_covered > time ) 
			bDone = true;
		else
		{
			Frame_s *frame = new Frame_s;

			if( !(m_setting.interfaceFrameMaster->I_CreateFrame(
				frame->frameId
			)) )
			{
				delete frame;
				bDone = true;
				bOk = false;
				RaiseError( SE_CREATEFRAME_FAILED );
			} else {
				frame->time.base_time = m_setting.time_start + m_time.frame_count / m_setting.frames_per_second;
				frame->time.window_min = frame->time.base_time + m_setting.offset_window_begin / m_setting.frames_per_second;
				frame->time.window_max = m_setting.time_start + (m_time.frame_count+1) / m_setting.frames_per_second + m_setting.offset_window_end / m_setting.frames_per_second;
				frame->time.window_delta = frame->time.window_max - frame->time.window_min;
				if( frame->time.window_delta <= 0 )
					frame->time.window_delta = 0.000000000001;

				frames.push_back( frame );

				m_time.frame_count++;
			}
		}
	}

	return bOk;
}


////////////////////////////////////////////////////////////////////////////////
//
//	BGRSampler

BGRSampler::BGRSampler(
	int iWidth,
	int iHeight,
	SampleMethod_e sampleMethod,
	ColorMethod_e colorMethod,
	FrameFunction_e frameFunction,
	OnPrintFrame_t OnPrintFrame
)
{
	m_setting.iWidth            = iWidth;
	m_setting.iHeight           = iHeight;
	m_setting.sampleMethod      = sampleMethod;
	m_setting.colorMethod       = colorMethod;
	m_setting.frameFunction     = frameFunction;
	m_setting.OnPrintFrame      = OnPrintFrame;

	m_frames = 0;
	m_interfaceSampleMaster = 0;

	m_tracker = 0;
}

BGRSampler::~BGRSampler()
{

}

unsigned long BGRSampler::GetTracker()
{
	return m_tracker;
}

void BGRSampler::I_SamplerConnect( ISampleMaster* interfaceSampleMaster )
{
	m_interfaceSampleMaster = interfaceSampleMaster;
}

bool BGRSampler::I_CreateFrame(FrameId_t &getFrameId)
{
	Frame_s *pframe = new Frame_s;

	pframe->id = m_frames++;
	pframe->oldsample = 0;
	pframe->layers = 0;
	pframe->fWhitePoint[0]=0;
	pframe->fWhitePoint[1]=0;
	pframe->fWhitePoint[2]=0;

	int imax = 3* m_setting.iWidth * m_setting.iHeight;
	pframe->pfdata = new float[imax];
	for(int i=0;i<imax;i++)pframe->pfdata[i]=0;

	getFrameId = (FrameId_t)pframe;

	m_tracker++;

	return true;
}

bool BGRSampler::I_NewSample(FrameId_t frameid, float frame_time, SampleId_t sampleId)
{
	Frame_s* pframe=((Frame_s *)frameid);
	pframe->layers++;

	if( SM_INT_TRAPEZOID == m_setting.sampleMethod)
	{
		// 2 point integral interpolation

		if(1 < pframe->layers)
		{
			// 2 points present now, we can accumalte
			Accum(pframe,true,frame_time,sampleId);
			m_interfaceSampleMaster->I_ReleaseSample(pframe->oldsample);
		}

		// save new point
		pframe->oldsample = sampleId;
		pframe->oldtime = frame_time;
	} else {
		// 1 point integral interpolation

		// we can accumulate instantly
		Accum(pframe,false,frame_time,sampleId);
		m_interfaceSampleMaster->I_ReleaseSample(sampleId);
	}
	
	return true;
}

bool BGRSampler::I_FinishFrame(FrameId_t frameid, float time_end)
{
	Frame_s* pframe=((Frame_s *)frameid);

	if( (SM_INT_TRAPEZOID == m_setting.sampleMethod) && ( 1 <= pframe->layers ) )
	{
		if( 1 == pframe->layers )
		{
			// accumulate a single lingering sample that has not been interpolated
			// yet due to lack of a previous smaple
			Accum(pframe,false,pframe->oldtime,pframe->oldsample);
		}

		// Free sample memory:
		m_interfaceSampleMaster->I_ReleaseSample(pframe->oldsample);
	}

	PrintAccu(pframe, time_end);

	delete pframe->pfdata;
	delete pframe;

	m_tracker--;

	return true;
}

void BGRSampler::Accum( Frame_s *pframe, bool bTwoPoint, float frame_time, SampleId_t sampleId)
{
	float *pfdata = pframe->pfdata;
	unsigned char *pData  = (unsigned char *)(m_interfaceSampleMaster->I_GetSampleData(sampleId));

	float frame_time_2=0;
	unsigned char *pData_2=0;


	if(bTwoPoint)
	{
		pData_2 = (unsigned char *)(m_interfaceSampleMaster->I_GetSampleData(pframe->oldsample));
		frame_time_2 = pframe->oldtime;
	}

	float fFrameWeight=0;
	float fFrameWeight2=0;

	switch( m_setting.frameFunction )
	{
	case FF_GAUSS:
		fFrameWeight = FrameWeight_Direct_Gauss_0_3_129_InterpLin( 2*frame_time -1 );
		if( bTwoPoint ) fFrameWeight2 = FrameWeight_Direct_Gauss_0_3_129_InterpLin( 2*frame_time_2 -1 );
		break;
	default:
	case FF_RECTANGLE:
		fFrameWeight = 1;
		if( bTwoPoint ) fFrameWeight2 = 1;
	}

	float fColorWeight[3];

	switch( m_setting.colorMethod )
	{
	default:
	case CM_RGB:
	case CM_AVERAGE:
		fColorWeight[2] = (float)1/3;
		fColorWeight[1] = (float)1/3;
		fColorWeight[0] = (float)1/3;

		break;
	case CM_SRGB_LUMA:
		fColorWeight[2] = 0.2126; // Red
		fColorWeight[1] = 0.7152; // Green
		fColorWeight[0] = 0.0722; // Blue
		break;
	}

	float fcc = 1;

	if( bTwoPoint)
	{
		fcc = (frame_time-frame_time_2)/2;
		pframe->fWhitePoint[2]+=fcc*255*(fFrameWeight+fFrameWeight2);
		pframe->fWhitePoint[1]=pframe->fWhitePoint[2];
		pframe->fWhitePoint[0]=pframe->fWhitePoint[2];
	} else {
		pframe->fWhitePoint[2]+=fFrameWeight*255;
		pframe->fWhitePoint[1]=pframe->fWhitePoint[2];
		pframe->fWhitePoint[0]=pframe->fWhitePoint[2];
	}

	for( int iy=0; iy < m_setting.iHeight; iy++ )
		for( int ix=0; ix < m_setting.iWidth; ix++ )
		{
			if(bTwoPoint)
			{
				if( CM_RGB == m_setting.colorMethod )
				{
					pfdata[0] += fcc*(fFrameWeight*pData[0]+fFrameWeight2*pData_2[0]);
					pfdata[1] += fcc*(fFrameWeight*pData[1]+fFrameWeight2*pData_2[1]);
					pfdata[2] += fcc*(fFrameWeight*pData[2]+fFrameWeight2*pData_2[2]);
				} else {
					float fcw = fFrameWeight*((fColorWeight[0]*pData[0]+fColorWeight[1]*pData[1]+fColorWeight[2]*pData[2])/255);
					float fcw2 = fFrameWeight2*((fColorWeight[0]*pData_2[0]+fColorWeight[1]*pData_2[1]+fColorWeight[2]*pData_2[2])/255);
					pfdata[0] += fcc*(fcw*pData[0]+fcw2*pData_2[0]);
					pfdata[1] += fcc*(fcw*pData[1]+fcw2*pData_2[1]);
					pfdata[2] += fcc*(fcw*pData[2]+fcw2*pData_2[2]);
				}
			} else {
				if( CM_RGB == m_setting.colorMethod )
				{
					pfdata[0] += fFrameWeight*pData[0];
					pfdata[1] += fFrameWeight*pData[1];
					pfdata[2] += fFrameWeight*pData[2];
				} else {
					float fcw = fFrameWeight*((fColorWeight[0]*pData[0]+fColorWeight[1]*pData[1]+fColorWeight[2]*pData[2])/255);
					pfdata[0] += fcw*pData[0];
					pfdata[1] += fcw*pData[1];
					pfdata[2] += fcw*pData[2];
				}
			}

			pfdata  += 3;
			pData   += 3;
			pData_2 += 3;
		}
}

void BGRSampler::PrintAccu( Frame_s *pframe, float time_end )
{
	float *pfdata = pframe->pfdata;
	
	float fWhitePoint[3];
	fWhitePoint[0]=pframe->fWhitePoint[0];
	fWhitePoint[1]=pframe->fWhitePoint[1];
	fWhitePoint[2]=pframe->fWhitePoint[2];

	bool bWhitePoint[3]={false,false,false};
	if(0!=fWhitePoint[0]) bWhitePoint[0]=true;
	if(0!=fWhitePoint[1]) bWhitePoint[1]=true;
	if(0!=fWhitePoint[2]) bWhitePoint[2]=true;

	unsigned char *pprint = new unsigned char[3*m_setting.iWidth*m_setting.iHeight];
	unsigned char *pdata=pprint;

	for(int i=0; i<m_setting.iWidth*m_setting.iHeight; i++)
	{
		// read and apply whitepoint:
		float ftmp[3];
		ftmp[0] = bWhitePoint[0] ? 255*(pfdata[0] / fWhitePoint[0]) : pfdata[0];
		ftmp[1] = bWhitePoint[1] ? 255*(pfdata[1] / fWhitePoint[1]) : pfdata[1];
		ftmp[2] = bWhitePoint[2] ? 255*(pfdata[2] / fWhitePoint[2]) : pfdata[2];

		// clamp:
		if(ftmp[0]<0) ftmp[0]=0;
		if(ftmp[1]<0) ftmp[1]=0;
		if(ftmp[2]<0) ftmp[2]=0;
		if(ftmp[0]>255) ftmp[0]=255;
		if(ftmp[1]>255) ftmp[1]=255;
		if(ftmp[2]>255) ftmp[2]=255;

		// store:
		pdata[0] = (unsigned char)(ftmp[0]);
		pdata[1] = (unsigned char)(ftmp[1]);
		pdata[2] = (unsigned char)(ftmp[2]);

		// advance pointers:
		pdata+=3;
		pfdata+=3;
	}

	// output:
	m_setting.OnPrintFrame(pframe->id, pprint, m_setting.iWidth, m_setting.iHeight );

	delete pprint;
}
