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
	virtual bool I_FinishFrame(FrameId_t frameid, float time_end) = 0;
};

////////////////////////////////////////////////////////////////////////////////
//
//  CSampleMaster
//
//
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

	unsigned long GetTracker();

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
		float overlap;
	} m_setting;

	std::list<Frame_s *> frames;

	struct
	{
		float time_min;
		unsigned long frame_count;
	} m_time;

	unsigned long m_tracker;

private:
	void RaiseError();
	void RaiseError( SamplerError_e errorcode );

	bool Finish_Frames_OutOfScope(float time);
	bool Create_Frames_NewInScope(float time);


};

////////////////////////////////////////////////////////////////////////////////
//
//  BGRSampler
//
//  Asumes 8 bit BGR data without any alignment.
//
//
class BGRSampler : public IFrameMaster
{
public:
	enum SampleMethod_e
	{
		SM_INT_RECTANGLE,
		SM_INT_TRAPEZOID
	};

	enum ColorMethod_e
	{
		CM_RGB,
		CM_AVERAGE,
		CM_SRGB_LUMA
	};

	enum FrameFunction_e
	{
		FF_RECTANGLE,
		FF_GAUSS
	};

	typedef bool (*OnPrintFrame_t)(unsigned long id, void *prgbdata, int iWidht, int iHeight);

	BGRSampler(
		int iWidth,
		int iHeight,
		SampleMethod_e sampleMethod,
		ColorMethod_e colorMethod,
		FrameFunction_e frameFunction,
		OnPrintFrame_t OnPrintFrame
	);
	~BGRSampler();

	unsigned long GetTracker();

	// IFrameMaster:
	void I_SamplerConnect( ISampleMaster* interfaceSampleMaster );

	bool I_CreateFrame(FrameId_t &getFrameId);
	bool I_NewSample(FrameId_t frameid, float frame_time, SampleId_t sampleId); 
	bool I_FinishFrame(FrameId_t frameid, float time_end);

private:
	struct Frame_s
	{
		float fWhitePoint[3];
		unsigned long id;
		float *pfdata;
		SampleId_t	oldsample;
		float oldtime;
		unsigned long layers;
	};

	struct Setting_s
	{
		int iWidth;
		int iHeight;
		SampleMethod_e sampleMethod;
		ColorMethod_e colorMethod;
		FrameFunction_e frameFunction;
		OnPrintFrame_t OnPrintFrame;
		void * OnPrintFrame_this;
	} m_setting;

	ISampleMaster* m_interfaceSampleMaster;

	unsigned long m_frames;
	
	void Accum( Frame_s *pframe, bool bTwoPoint, float frame_time, SampleId_t sampleId );
	
	void PrintAccu( Frame_s *pframe, float time_end );

	unsigned long m_tracker;

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
int_rectangle, int_trapezoid, 


1. Sample weight function: rgb, avg_rgb, srgb_luma
2. Frame weight function:
   - defined from [0,1]
   - values [0,1]
   - stretch factor (determines frame overlapping)
   - functions: rectangle, gauss approx
*/