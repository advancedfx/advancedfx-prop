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
	if (f_x_diff=0) x = 0;
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

	bool bOk = true;
	std::list<Frame_s *>::iterator it_end = frames.end();
	for( std::list<Frame_s *>::iterator it = frames.begin(); it != it_end; it++ )
	{
		if( m_setting.interfaceFrameMaster->I_NewSample(
			(*it)->frameId,
			(time - (*it)->time.window_min) / (*it)->time.window_delta,
			(SampleId_t)psample
		) )
			psample->ul_RefCount++;
		else
			bOk=false;
	}

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
			frame->frameId
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

void * CSampleMaster::I_GetSampleData( SampleId_t sampleId )
{
	return ((Sample_s *)sampleId)->pData;
}

void CSampleMaster::I_ReleaseSample( SampleId_t sampleId )
{
	Sample_s *psample = (Sample_s*)sampleId;

	psample->ul_RefCount--;

	if(!(psample->ul_RefCount))
		free(psample);
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
				frame->frameId
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
			+(m_time.frame_count / m_setting.frames_per_second)
			+ m_setting.offset_window_begin;

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
				frame->time.base_time = m_setting.time_start+(m_time.frame_count+1 / m_setting.frames_per_second);
				frame->time.window_min = frame->time.base_time + m_setting.offset_window_begin;
				frame->time.window_max = m_setting.time_start+(m_time.frame_count+2 / m_setting.frames_per_second) + m_setting.offset_window_end;
				frame->time.window_delta = frame->time.window_max - frame->time.window_min;
				if( frame->time.window_delta <= 0 )
					frame->time.window_delta = 0.000000000001;

				frames.push_back( frame );
			}
		}
	}

	return bOk;
}
