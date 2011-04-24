#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-10-07 dominik.matrixstorm.com
//
// First changes:
// 2010-09-27 dominik.matrixstorm.com

#include "addresses.h"

AFXADDR_DEF(cstrike_CalcDemoViewOverride)
AFXADDR_DEF(cstrike_CalcDemoViewOverride_DSZ)
AFXADDR_DEF(cstrike_CViewRender_SetUpView)
AFXADDR_DEF(cstrike_CViewRender_SetUpView_DSZ)
AFXADDR_DEF(cstrike_cl_demoviewoverride)
AFXADDR_DEF(cstrike_gpGLobals)
AFXADDR_DEF(cstrike_gpGlobals_OFS_curtime)
AFXADDR_DEF(cstrike_OFS_CvarFloatValue)


void Addresses_InitClientDll(AfxAddr clientDll)
{
	AFXADDR_SET(cstrike_CalcDemoViewOverride, clientDll +0x13E000);//
	AFXADDR_SET(cstrike_CalcDemoViewOverride_DSZ, 0x09);//
	AFXADDR_SET(cstrike_CViewRender_SetUpView, clientDll +0x13E8C0);//
	AFXADDR_SET(cstrike_CViewRender_SetUpView_DSZ, 0x0d);//
	AFXADDR_SET(cstrike_cl_demoviewoverride, clientDll +0x58C4DC);//
	AFXADDR_SET(cstrike_gpGLobals, clientDll +0x520A48);//
	AFXADDR_SET(cstrike_gpGlobals_OFS_curtime, 3*4);//
	AFXADDR_SET(cstrike_OFS_CvarFloatValue, 11);//
}
