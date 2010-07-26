#include "stdafx.h"

//  cstrike_ch.cpp - Counter-Strike 1.6 crosshair fix
//  Copyright (c) Half-Life Advanced Effects project

//  Last changes:
//	2008-11-10 by dominik.matrixstorm.com

//  First changes:
//	2008-11-10 by dominik.matrixstorm.com

// See also: /doc/notes_goldsrc/debug_cstrike_crosshair.txt

#include <windows.h>
#include <winbase.h>
#include <gl\gl.h>

#include <hlsdk.h>

#include "cmdregister.h"

#include "hl_addresses.h"
#include <shared/detours.h>

#include "filming.h"

extern cl_enginefuncs_s *pEngfuncs;

extern Filming g_Filming;

// client.dll: 0x0D4c0000
// 0x93C4

typedef void (__stdcall *UnkCstrikeCrosshairFn_t)( DWORD *this_ptr, float dwUnk1, DWORD dwUnk2);

UnkCstrikeCrosshairFn_t g_pfnCrosshairFix_Hooked_Func = NULL;

REGISTER_DEBUGCVAR(crosshair_base_fps, "72.0", 0);

bool g_bBlockCh = false;
double g_fChTargetFps = 0;

double *g_f_ch_mul_fac = NULL;
double *g_f_ch_add_fac = NULL;

double g_f_SubFrameTime = 0;

void __stdcall CrosshairFix_Hooking_Func(  DWORD *this_ptr, float fUnkTime, DWORD dwUnkWeaponCode )
{

	if(g_bBlockCh)
	{
		// detect if we are in a multipass for the hud and thus don't want any cooldown:
		bool b_SecondHudPass = Filming::HUDRQ_CAPTURE_ALPHA == g_Filming.giveHudRqState();

		if(!b_SecondHudPass )
			g_f_SubFrameTime += g_fChTargetFps != 0.0f ? crosshair_base_fps->value / g_fChTargetFps : 1.0f;

		if( b_SecondHudPass || g_f_SubFrameTime < 1.0f)
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

		} else {
			// apply cooldown:
			while( g_f_SubFrameTime >= 1.0f )
			{
				g_pfnCrosshairFix_Hooked_Func( this_ptr, fUnkTime, dwUnkWeaponCode );

				g_f_SubFrameTime -= 1.0f;
			}
		}
	}
	else
		g_pfnCrosshairFix_Hooked_Func( this_ptr, fUnkTime, dwUnkWeaponCode );
}

void CrosshairFix_Install()
{
	if(!g_pfnCrosshairFix_Hooked_Func)
	{
		const char *gamedir = pEngfuncs->pfnGetGameDirectory();
		DWORD dwClientDLL = (DWORD)GetModuleHandle("client.dll");

		if( !strcmp("cstrike",gamedir) )
		{
			pEngfuncs->Con_DPrintf("0x%08x\n",dwClientDLL);

			// install hook:
			g_pfnCrosshairFix_Hooked_Func = (UnkCstrikeCrosshairFn_t) DetourClassFunc((BYTE *)HL_ADDR_GET(UnkCstrikeCrosshairFn), (BYTE *)CrosshairFix_Hooking_Func, (int)HL_ADDR_GET(DTOURSZ_UnkCstrikeCrosshairFn));

			// install factor pointers:
			g_f_ch_mul_fac = (double *)HL_ADDR_GET(UnkCstrikeCh_mul_fac);
			g_f_ch_add_fac = (double *)HL_ADDR_GET(UnkCstrikeCh_add_fac);

			pEngfuncs->Con_DPrintf("0x%08x,0x%08x\n",g_f_ch_mul_fac,g_f_ch_add_fac);
		}
		else
		{
			pEngfuncs->Con_Printf( "Sorry, your mod (%s) is not supported for this command.\n",	gamedir );
			return; // quit
		}
	}
}

REGISTER_CMD_FUNC(crosshair_fps)
{
	bool bShowHelp = true;

	CrosshairFix_Install();

	if(1 < pEngfuncs->Cmd_Argc() )
	{
		double fVal = atof(pEngfuncs->Cmd_Argv(1) );
		bool bEnable  = 0.0f != fVal;
		g_bBlockCh = bEnable;

		g_fChTargetFps = fVal;

		bShowHelp = false;
	}

	if (bShowHelp)
	{
		pEngfuncs->Con_Printf( "Usage: " PREFIX "crosshair_fps n (n=target fps, 0=disable)\n" );
	}
}

