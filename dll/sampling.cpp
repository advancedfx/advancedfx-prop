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

const float C_Values_Norm_Gauss_0_4_129 [129] =
{
0.398942280401, 0.398747531916, 0.398163856687, 0.397192962614,
0.395837686945, 0.394101982457, 0.391990898253, 0.389510555288,
0.386668116803, 0.383471753857, 0.379930606199, 0.376054738732,
0.371855093870, 0.367343440092, 0.362532317040, 0.357434977520,
0.352065326764, 0.346437859376, 0.340567594320, 0.334470008388,
0.328160968550, 0.321656663586, 0.314973535427, 0.308128210596,
0.301137432155, 0.294017992524, 0.286786667566, 0.279460152271,
0.272054998379, 0.264587554262, 0.257073907347, 0.249529829342,
0.241970724519, 0.234411581246, 0.226866926969, 0.219350786791,
0.211876645776, 0.204457415069, 0.197105401919, 0.189832283616,
0.182649085389, 0.175566162213, 0.168593184518, 0.161739127716,
0.155012265458, 0.148420166525, 0.141969695205, 0.135667015026,
0.129517595666, 0.123526222878, 0.117697011224, 0.112033419427,
0.106538268131, 0.101213759852, 0.096061500905, 0.091082525076,
0.086277318827, 0.081645847804, 0.077187584440, 0.072901536421,
0.068786275827, 0.064839968725, 0.061060405041, 0.057445028505,
0.053990966513, 0.050695059725, 0.047553891261, 0.044563815342,
0.041720985256, 0.039021380526, 0.036460833176, 0.034035053014,
0.031739651836, 0.029570166499, 0.027522080803, 0.025590846131,
0.023771900830, 0.022060688298, 0.020452673773, 0.018943359814,
0.017528300494, 0.016203114300, 0.014963495786, 0.013805225978,
0.012724181597, 0.011716343115, 0.010777801700, 0.009904765100,
0.009093562502, 0.008340648445, 0.007642605819, 0.006996148019,
0.006398120311, 0.005845500468, 0.005335398732, 0.004865057159,
0.004431848412, 0.004033274046, 0.003666962346, 0.003330665769,
0.003022258035, 0.002739730929, 0.002481190836, 0.002244855075,
0.002029048057, 0.001832197314, 0.001652829422, 0.001489565869,
0.001341118873, 0.001206287202, 0.001083951999, 0.000973072648,
0.000872682695, 0.000781885839, 0.000699852011, 0.000625813556,
0.000559061522, 0.000498942068, 0.000444853004, 0.000396240458,
0.000352595682, 0.000313452002, 0.000278381897, 0.000246994234,
0.000218931638, 0.000193867996, 0.000171506111, 0.000151575484,
0.000133830226
};

// the function clamps x to the normalized range [-1,1]
float FrameWeight_Norm_Gauss_0_4_129_InterpLin(float x)
{

	if( x<+0 ) x = -x; // gauss(-x) = gauss(x)
	if( x>+1 ) x = +1; // clamp

	x = x*128;
	float f_x_lo = floor(x);
	float f_x_hi = ceil(x);
	
	float f_x_diff = f_x_hi - f_x_lo;
	if( 0==f_x_diff ) x = 0;
	else x = (x-f_x_lo)/f_x_diff;

	float f_y_lo = C_Values_Norm_Gauss_0_4_129[ (unsigned char)(f_x_lo) ];

	return f_y_lo + x*( C_Values_Norm_Gauss_0_4_129[ (unsigned char)(f_x_hi) ] - f_y_lo );
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

	if( (SM_INT_TRAPEZOID == m_setting.sampleMethod) && ( 1 == pframe->layers ) )
	{
		// accumulate a single lingering sample that has not been interpolated
		// yet due to lack of a previous smaple
		Accum(pframe,false,pframe->oldtime,pframe->oldsample);
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
		fFrameWeight = FrameWeight_Norm_Gauss_0_4_129_InterpLin( 2*frame_time -1 );
		if( bTwoPoint ) fFrameWeight2 = FrameWeight_Norm_Gauss_0_4_129_InterpLin( 2*frame_time_2 -1 );
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
					pfdata[0] += fcw*255;//pData[0];
					pfdata[1] += fcw*255;//pData[1];
					pfdata[2] += fcw*255;//pData[2];
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
