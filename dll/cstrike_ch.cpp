//  cstrike_ch.cpp - Counter-Strike 1.6 crosshair fix
//  Copyright (c) Half-Life Advanced Effects project

//  Last changes:
//	2008-11-10 by dominik.matrixstorm.com

//  First changes:
//	2008-11-10 by dominik.matrixstorm.com

#include <windows.h>
#include <winbase.h>
#include <gl\gl.h>

// BEGIN HLSDK includes
//
// HACK: prevent cldll_int.h from messing the HSPRITE definition,
// HLSDK's HSPRITE --> MDTHACKED_HSPRITE
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <wrect.h>
#include <cl_dll.h>
#include <cdll_int.h>
#include <r_efx.h>
#include <com_model.h>
#include <r_studioint.h>
#include <pm_defs.h>
#include <cvardef.h>
#include <entity_types.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include "cmdregister.h"

#include "hl_addresses.h"
#include "detours.h"

#include "filming.h"

extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s* pEngStudio;
extern playermove_s* ppmove;

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

			DWORD dwOldPort1,dwOldPort2;
			double fOldMulFac, fOldAddFac;

			VirtualProtect(g_f_ch_mul_fac,sizeof(double),PAGE_READWRITE,&dwOldPort1);
			VirtualProtect(g_f_ch_add_fac,sizeof(double),PAGE_READWRITE,&dwOldPort2);

			fOldMulFac = *g_f_ch_mul_fac;
			fOldAddFac = *g_f_ch_add_fac;

			*g_f_ch_mul_fac = 0.0f;
			*g_f_ch_add_fac = 0.0f;

			g_pfnCrosshairFix_Hooked_Func( this_ptr, fUnkTime, dwUnkWeaponCode );

			*g_f_ch_mul_fac = fOldMulFac;
			*g_f_ch_add_fac = fOldAddFac;

			VirtualProtect(g_f_ch_mul_fac,sizeof(double),dwOldPort1,NULL);
			VirtualProtect(g_f_ch_add_fac,sizeof(double),dwOldPort2,NULL);

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
			g_pfnCrosshairFix_Hooked_Func = (UnkCstrikeCrosshairFn_t) DetourClassFunc((BYTE *)(dwClientDLL + HL_ADDR_CLOFS_UnkCstrikeCrosshairFn), (BYTE *)CrosshairFix_Hooking_Func, (int)HL_ADDR_DTOURSZ_UnkCstrikeCrosshairFn);

			// install factor pointers:
			g_f_ch_mul_fac = (double *)(dwClientDLL + HL_ADDR_CLOFS_UnkCstrikeCh_mul_fac);
			g_f_ch_add_fac = (double *)(dwClientDLL + HL_ADDR_CLOFS_UnkCstrikeCh_add_fac);

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


/*

Clues :

0:003> s -a client L1000000 "cl_crosshair_size"
0d5e6df8  63 6c 5f 63 72 6f 73 73-68 61 69 72 5f 73 69 7a  cl_crosshair_siz
0d5e6fd3  63 6c 5f 63 72 6f 73 73-68 61 69 72 5f 73 69 7a  cl_crosshair_siz
0d895f8c  63 6c 5f 63 72 6f 73 73-68 61 69 72 5f 73 69 7a  cl_crosshair_siz
0d895fb0  63 6c 5f 63 72 6f 73 73-68 61 69 72 5f 73 69 7a  cl_crosshair_siz
....

frist match:
0:003> s client L1000000 f8 6d 5e 0d
0d541198  f8 6d 5e 0d ff 15 c8 1b-62 0d 8b 46 10 b9 40 0a  .m^.....b..F..@.
0d542326  f8 6d 5e 0d ff 15 d0 1b-62 0d 8b e8 83 c4 04 85  .m^.....b.......

the first address belongs to a function that set's the default
cl_crosshair_size value.
the second address belongs to a function that handles the switching of the
crosshair based on cl_crosshair_size during hud drawing.
it seems to do some adjustments based on the resolution used.

second match:
is just a usage string that is spammed into the console by the function above

i.e. this is what the function does when cl_crosshair_size is auto:

0d5423f1 a13819620d      mov     eax,dword ptr [client!V_CalcRefdef+0xb33e8 (0d621938)]
0d5423f6 3d80020000      cmp     eax,280h
0d5423fb 7e17            jle     client!IN_ClearStates+0x12ee4 (0d542414)
0d5423fd 3d00040000      cmp     eax,400h
0d542402 7c20            jl      client!IN_ClearStates+0x12ef4 (0d542424)
0d542404 5f              pop     edi
0d542405 c7436c80020000  mov     dword ptr [ebx+6Ch],280h
0d54240c 5d              pop     ebp
0d54240d 5b              pop     ebx
0d54240e c3              ret

ebx is a copy of what was in ecx when the funcion was called:

0d503169 8bcf            mov     ecx,edi
0d50316b e8b0f1ffff      call    client!IN_ClearStates+0x12df0 (0d502320)
0d503170 8b4f6c          mov     ecx,dword ptr [edi+6Ch] ds:0023:0d5dd618=00000400

anways, this function is called by a mor ecomplex unknown function that is called as follows:

0d50283e 50              push    eax
0d50283f 56              push    esi
0d502840 8bcd            mov     ecx,ebp
0d502842 e869060000      call    client!IN_ClearStates+0x13980 (0d502eb0)
0d502847 391df0cd5b0d    cmp     dword ptr [client!V_CalcRefdef+0x8e8a0 (0d5bcdf0)],ebx

this is actually a class function (probably client hud class or s.th.)
which gets passed two parameters, one seems to be the time, the other the weaponcode (id):

0d502eb0 83ec08          sub     esp,8
0d502eb3 53              push    ebx
0d502eb4 55              push    ebp
0d502eb5 56              push    esi
0d502eb6 57              push    edi
0d502eb7 8bf9            mov     edi,ecx
0d502eb9 be05000000      mov     esi,5
0d502ebe 8b4c2420        mov     ecx,dword ptr [esp+20h] ss:0023:0012f8a8=00000011
0d502ec2 897c2414        mov     dword ptr [esp+14h],edi
0d502ec6 8d69ff          lea     ebp,[ecx-1]
0d502ec9 83fd1d          cmp     ebp,1Dh
0d502ecc 0f87a1000000    ja      client!IN_ClearStates+0x13a43 (0d502f73)

// here it would do some adjustments accordings to the weapon

....

// this checks cl_dynamic crosshair:

0d502f96 a1d8d25d0d      mov     eax,dword ptr [client!V_CalcRefdef+0xaed88 (0d5dd2d8)]
0d502f9b 85c0            test    eax,eax
0d502f9d 0f84c9000000    je      client!IN_ClearStates+0x13b3c (0d50306c)
0d502fa3 d9400c          fld     dword ptr [eax+0Ch]
0d502fa6 d81df031580d    fcomp   dword ptr [client!V_CalcRefdef+0x54ca0 (0d5831f0)] ds:0023:0d5831f0=00000000
0d502fac dfe0            fnstsw  ax

....

// i.e. here it checks some walking stuff:
0d50306c 8b0d94d25d0d    mov     ecx,dword ptr [client!V_CalcRefdef+0xaed44 (0d5dd294)]
0d503072 8b4740          mov     eax,dword ptr [edi+40h]
0d503075 3bc8            cmp     ecx,eax
...

// here it loads from the class pointer (edi) the old drawing position (so it always draws a delta somewhat)
0d5030b0 d94744          fld     dword ptr [edi+44h]
0d5030b3 d9c0            fld     st(0)
0d5030b5 dc0d50d4580d    fmul    qword ptr [client!V_CalcRefdef+0x5ef00 (0d58d450)]
0d5030bb 8b4748          mov     eax,dword ptr [edi+48h]
0d5030be 83c002          add     eax,2
0d5030c1 dc059832580d    fadd    qword ptr [client!V_CalcRefdef+0x54d48 (0d583298)]

// as you can see two constants get used here, one for multiplication (fmul) and one for addition (fadd)

// this is what we need to patch upon fps change

// and here is where the drawing function is called:

...
0d503324 a138195e0d      mov     eax,dword ptr [client!V_CalcRefdef+0xb33e8 (0d5e1938)]
0d503329 99              cdq
0d50332a 2bc2            sub     eax,edx
0d50332c d1f8            sar     eax,1
0d50332e 50              push    eax
0d50332f ff15bc1b5e0d    call    dword ptr [client!V_CalcRefdef+0xb366c (0d5e1bbc)] ds:0023:0d5e1bbc=03568db0
0d503335 83c440          add     esp,40h
0d503338 b801000000      mov     eax,1
0d50333d 5f              pop     edi
0d50333e 5e              pop     esi
0d50333f 5d              pop     ebp
0d503340 5b              pop     ebx
0d503341 83c408          add     esp,8
0d503344 c20800          ret     8
*/
