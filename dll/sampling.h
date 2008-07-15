#pragma once

// Project :  Half-Life Advanced Effects
// File    :  dll/sampling.h

// Authors : last change / first change / name

// 2008-07-12 / 2008-07-10 / Dominik Tugend

// Comment: sampling related classes

////////////////////////////////////////////////////////////////////////////////

#include <list>

namespace hlae
{

namespace sampler
{

typedef void * FrameId_t;
typedef void * SampleId_t;

class ISampleMaster
{
public:
	virtual void * I_GetSampleData( SampleId_t sampleId ) = 0;
	virtual void   I_ReleaseSample( SampleId_t sampleId ) = 0;
};

class IFrameMaster
{
public:
	virtual void I_SamplerConnect( ISampleMaster* interfaceSampleMaster )=0;

	virtual bool I_CreateFrame(FrameId_t &getFrameId) = 0;
	virtual bool I_NewSample(FrameId_t frameid, float frame_time, SampleId_t sampleId) = 0; 
	virtual bool I_FinishFrame(FrameId_t frameid) = 0;
};

class CSampleMaster : public ISampleMaster
{
public:
	enum SamplerError_e
	{
		SE_NONE,
		SE_ERROR,
		SE_STATE_ERROR,
		SE_TIME_ERROR,
		SE_CREATEFRAME_FAILED
	};

public:
	CSampleMaster();
	~CSampleMaster();

	// Prepares the class for sampling data, any previous settings are discarded
	bool BeginSampling(
		IFrameMaster *interfaceFrameMaster,
		float time_start,
		float frames_per_second, // >0
		float offset_window_begin, // <= 0
		float offset_window_end // >= 0
	);

	//	Ends sampling, discards any current data or settings and frees memory
	bool EndSampling( float time_end );

	//	Supplies data to be sampled for the current frame.
	//	The data has to be compatible with the SampleFormat set in BeginSampling
	//	WARNING: no range / buffer checking etc.
	bool Sample( float time, const void *pData, unsigned long cbDataSize );

	SamplerError_e HadError();

public:
	// ISampleMaster:
	(void *) I_GetSampleData( SampleId_t sampleId );
	void     I_ReleaseSample( SampleId_t sampleId );

private:
	enum SamplerState_e
	{
		SS_IDLE,
		SS_SAMPLING,
	};

	struct Frame_s
	{
		FrameId_t frameId;
		struct
		{
			float base_time;
			float window_min;
			float window_max;
			float window_delta;
		} time;
	};

	struct Sample_s
	{
		unsigned long ul_RefCount;
		void *pData;
	};

private:
	SamplerState_e m_SamplerState;
	SamplerError_e m_SamplerError;

	struct
	{
		IFrameMaster *interfaceFrameMaster;
		float time_start;
		float frames_per_second;
		float offset_window_begin;
		float offset_window_end;
	} m_setting;

	std::list<Frame_s *> frames;

	struct
	{
		float time_min;
		unsigned long frame_count;
	} m_time;

private:
	void RaiseError();
	void RaiseError( SamplerError_e errorcode );

	bool Finish_Frames_OutOfScope(float time);
	bool Create_Frames_NewInScope(float time);


};


}	// namespace sampler

}	// namespace hlae


/*
mirv_sample factor
  1, 2, 3, 4
  samples per frame

mirv_sample colorhandling
rgb | avg_rgb | luma_srgb

mirv_sample method
int_const | int_linear


1. Sample weight function: const, avg, srgb_luma
2. Frame weight function:
   - defined from [0,1]
   - values [0,1]
   - stretch factor (determines frame overlapping)
   - functions: rectangle, gauss approx
*/