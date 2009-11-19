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
#include <wrect.h>
#include <cl_dll.h>
#include <cl_entity.h>
#include <r_studioint.h>
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
