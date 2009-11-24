//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-16 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "MirvInfo.h"


// BEGIN HLSDK includes
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <hlsdk/multiplayer/cl_dll/wrect.h>
#include <hlsdk/multiplayer/cl_dll/cl_dll.h>
#include <hlsdk/multiplayer/common/cl_entity.h>
#include <hlsdk/multiplayer/common/r_studioint.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes


MirvInfo g_MirvInfo;

extern engine_studio_api_s *pEngStudio;


// MirvInfo //////////////////////////////////////////////////////////////////

int MirvInfo::GetCurrentEntityIndex() {
	cl_entity_t *ce;
	if(pEngStudio && (ce = pEngStudio->GetCurrentEntity()))
		return ce->index;

	return -1;
}
