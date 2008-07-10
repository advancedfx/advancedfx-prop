#pragma once

// Project :  Half-Life Advanced Effects
// File    :  dll/sampling.h

// Authors : last change / first change / name

// 2008-07-10 / 2008-07-10 / Dominik Tugend

// Comment: sampling related classes

////////////////////////////////////////////////////////////////////////////////

namespace hlae
{

namespace sampler
{


struct S_SampleFormatDescription
{

};

struct C_SampleFormat
{
	S_SampleFormatDescription GetFormatDescription();
};

class C_FrameSampler
{
public:
	enum E_SampleMethod
	{
		//	Class chooses setting
		E_SM_default,

		E_SM_int_const,

		E_SM_int_linear
	};

	enum E_SamplerState
	{
		E_SS_none,
		E_SS_okay,
		E_SS_error
	};


	C_FrameSampler();
	~C_FrameSampler();


	// Prepares the class for sampling data, any previous settings are discarded
	E_SamplerState BeginSampling(
		const C_SampleFormat &SampleFormat,
		const E_SampleMethod SampleMethod
	);

	//	Ends sampling, discards any current data or settings and frees memory
	E_SamplerState EndSampling()
	{
		// todo
		return E_SS_error;
	};


	//	Tells the class that we are about to begin a new frame
	E_SamplerState BeginFrame();

	//	Supplies data to be sampled for the current frame.
	//	The data has to be compatible with the SampleFormat set in BeginSampling
	//	WARNING: no range / buffer checking etc.
	E_SamplerState SupplySample( const void *pData );

	// Tells the class that the current frame is finished
	E_SamplerState EndFrame();


	E_SamplerState Get_SamplerState();


private:
	E_SamplerState m_SamplerState;

	struct S_SamplerSettings
	{
		S_SampleFormatDescription SampleFormat;
		E_SampleMethod SampleMethod;
	} m_SamplerSettings;
};

}

}


/*
mirv_sample factor
  1, 2, 3, 4
  samples per frame

mirv_sample colorhandling
rgb | avg_rgb | luma_srgb

mirv_sample method
int_const | int_linear
*/