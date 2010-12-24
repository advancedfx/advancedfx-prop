#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-16 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "ModInfo.h"


#include <hlsdk.h>


ModInfo g_ModInfo;

extern engine_studio_api_s *pEngStudio;


// ModInfo //////////////////////////////////////////////////////////////////

int ModInfo::GetCurrentEntityIndex() {
	cl_entity_t *ce;
	if(pEngStudio && (ce = pEngStudio->GetCurrentEntity()))
		return ce->index;

	return -1;
}
