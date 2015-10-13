#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-10-13 dominik.matrixstorm.com
//
// First changes:
// 2015-10-13 dominik.matrixstorm.com

#include "csgo_ScaleForm_Hooks.h"

#include "addresses.h"

#include <shared/detours.h>
#include <shared/StringTools.h>

#include "SourceInterfaces.h"
#include "hlaeFolder.h"

#include <string>

typedef bool (__stdcall *csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf_t)(DWORD *this_ptr,char const * path, void * pData, DWORD fileLength);

csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf_t detoured_csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf;

bool __stdcall touring_csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf(DWORD *this_ptr,char const * path, void * pData, DWORD fileLength)
{
	return true;

	//bool result = detoured_csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf(this_ptr, path, pData, fileLength);
	//Tier0_Msg("detoured_csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf(\"%s\",[data],%u) --> %s\n", path, fileLength, result ? "TRUE" : "FALSE");
	//return result;
}

bool csgo_ScaleForm_Hooks_Init(void)
{
	static bool firstResult = false;
	static bool firstRun = true;
	if(!firstRun) return firstResult;
	firstRun = false;

	if(AFXADDR_GET(csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf))
	{
		detoured_csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf = (csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf_t)DetourClassFunc((BYTE *)AFXADDR_GET(csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf), (BYTE *)touring_csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf, (int)AFXADDR_GET(csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf_DSZ));

		firstResult = true;
	}

	return firstResult;
}

