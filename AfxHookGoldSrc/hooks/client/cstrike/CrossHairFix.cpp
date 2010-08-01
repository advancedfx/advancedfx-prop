#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-08-01 dominik.matrixstorm.com
//
// First changes
// 2008-11-10 dominik.matrixstorm.com

// See also: /doc/notes_goldsrc/debug_cstrike_crosshair.txt

#include "CrossHairFix.h"

#include <windows.h>

#include <shared/detours.h>
#include <hlsdk.h>
#include "../../../hl_addresses.h"

#include "../../hw/Host_Frame.h"


typedef void (__stdcall *UnkCstrikeCrosshairFn_t)( DWORD *this_ptr, float dwUnk1, DWORD dwUnk2);

bool g_Cstrike_CrossHair_Block = false;
UnkCstrikeCrosshairFn_t g_pfnCrosshairFix_Hooked_Func = NULL;
double *g_f_ch_mul_fac = NULL;
double *g_f_ch_add_fac = NULL;

double g_cstrike_ch_deltaT = 0;
double g_cstrike_ch_frameT = 1.0/72.0;

void __stdcall CrosshairFix_Hooking_Func(  DWORD *this_ptr, float fUnkTime, DWORD dwUnkWeaponCode )
{
	bool freezeCh = g_Cstrike_CrossHair_Block;
	bool fix = !freezeCh && 0.0 < g_cstrike_ch_frameT;
	
	if(fix)
	{
		g_cstrike_ch_deltaT += g_Host_Frame_time;
		
		bool coolDown = g_cstrike_ch_frameT <= g_cstrike_ch_deltaT;

		if(coolDown)
		{
			// apply cooldown:
			do
			{
				g_pfnCrosshairFix_Hooked_Func( this_ptr, fUnkTime, dwUnkWeaponCode );

				g_cstrike_ch_deltaT -= g_cstrike_ch_frameT;
			}
			while(g_cstrike_ch_frameT <= g_cstrike_ch_deltaT);

			return; // done.
		}
		else
			// keep it frozen:
			freezeCh = true;
	}

	if(freezeCh)
	{
		// do not apply any cool down, just make it drawn:

		MdtMemBlockInfos mbisMul, mbisAdd;
		double fOldMulFac, fOldAddFac;

		MdtMemAccessBegin(g_f_ch_mul_fac, sizeof(double), &mbisMul);
		MdtMemAccessBegin(g_f_ch_add_fac, sizeof(double), &mbisAdd);

		fOldMulFac = *g_f_ch_mul_fac;
		fOldAddFac = *g_f_ch_add_fac;

		*g_f_ch_mul_fac = 0.0f;
		*g_f_ch_add_fac = 0.0f;

		g_pfnCrosshairFix_Hooked_Func( this_ptr, fUnkTime, dwUnkWeaponCode );

		*g_f_ch_mul_fac = fOldMulFac;
		*g_f_ch_add_fac = fOldAddFac;

		MdtMemAccessEnd(&mbisAdd);
		MdtMemAccessEnd(&mbisMul);
	}
	else
		// Normal (unfixed) operation.
		g_pfnCrosshairFix_Hooked_Func( this_ptr, fUnkTime, dwUnkWeaponCode );
}


double Cstrike_CrossHair_Fps_get()
{
	return 0.0 != g_cstrike_ch_frameT ? 1.0 / g_cstrike_ch_frameT : 0.0;
}


void Cstrike_CrossHair_Fps_set(double value)
{
	g_cstrike_ch_frameT = 0.0 != value ? 1.0 / value : 0.0;
}


void Hook_Cstrike_CrossHair_Fix()
{
	static bool firstRun = true;
	if(!firstRun) return;
	firstRun = false;

	double * addrMul = (double *)HL_ADDR_GET(cstrike_UnkCrosshairFn_mul_fac);
	double * addrAdd = (double *)HL_ADDR_GET(cstrike_UnkCrosshairFn_add_fac);
	BYTE * addrFn = (BYTE *)HL_ADDR_GET(cstrike_UnkCrosshairFn);
	int addrFnDsz = (int)HL_ADDR_GET(cstrike_UnkCrosshairFn_DSZ);

	if(!(
		addrMul && addrAdd && addrFn && addrFnDsz
	)) return;

	g_pfnCrosshairFix_Hooked_Func = (UnkCstrikeCrosshairFn_t)DetourClassFunc(addrFn, (BYTE *)CrosshairFix_Hooking_Func, addrFnDsz);
	g_f_ch_mul_fac = addrMul;
	g_f_ch_add_fac = addrAdd;
}
