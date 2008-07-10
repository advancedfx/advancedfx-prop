// Project :  Half-Life Advanced Effects
// File    :  dll/sampling.cpp

// Authors : last change / first change / name

// 2008-07-10 / 2008-07-10 / Dominik Tugend

#include "sampling.h"

using namespace hlae;
using namespace hlae::sampler;


////////////////////////////////////////////////////////////////////////////////
//
//	C_FrameSampler

C_FrameSampler::C_FrameSampler()
{
	m_SamplerState = E_SS_none;
}

C_FrameSampler::~C_FrameSampler()
{
	EndSampling();
}

C_FrameSampler::E_SamplerState C_FrameSampler::Get_SamplerState()
{
	return m_SamplerState;
}

