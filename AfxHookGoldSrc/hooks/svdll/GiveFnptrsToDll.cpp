#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2014-09-22 dominik.matrixstorm.com
//
// First changes
// 2014-04-22 dominik.matrixstorm.com

enginefuncs_t g_engfuncs;
globalvars_t  *gpGlobals;

typedef void(__stdcall *GiveFnptrsToDll_t)( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals );

GiveFnptrsToDll_t Old_GiveFnptrsToDll;

void __stdcall New_GiveFnptrsToDll( enginefuncs_t* pengfuncsFromEngine, globalvars_t *pGlobals )
{
	memcpy(&g_engfuncs, pengfuncsFromEngine, sizeof(enginefuncs_t));
	gpGlobals = pGlobals;

	Old_GiveFnptrsToDll(pengfuncsFromEngine, pGlobals);
}

FARPROC Hook_GiveFnptrsToDll(FARPROC oldProc)
{
	Old_GiveFnptrsToDll = (GiveFnptrsToDll_t)oldProc;

	return (FARPROC)&New_GiveFnptrsToDll;
}
