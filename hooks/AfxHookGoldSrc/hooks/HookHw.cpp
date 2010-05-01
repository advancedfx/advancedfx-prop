#include "stdafx.h"

#include <shared/StringTools.h>

#include <hooks/shared/detours.h>

#include "HookHw.h"
#include "OpenGl32Hooks.h"
#include "gdi32Hooks.h"
#include "user32Hooks.h"

#include "hw/Mod_LeafPvs.h"
#include "hw/R_DrawEntitiesOnList.h"
#include "hw/R_DrawParticles.h"
#include "hw/R_DrawViewModel.h"
#include "hw/R_PolyBlend.h"

#include "../HltvFix.h"
#include "../hl_addresses.h"
#include "../mirv_scripting.h"
#include "../forceres.h"


struct cl_enginefuncs_s * pEngfuncs		= (struct cl_enginefuncs_s *)0;
struct engine_studio_api_s * pEngStudio	= (struct engine_studio_api_s *)0;
struct playermove_s * ppmove			= (struct playermove_s *)0;

FARPROC WINAPI NewHwGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);

	if (HIWORD(lpProcName))
	{
		static bool clientDllLoaded = false;
#if 0
		static bool bFirst = true;
		static FILE *f1=NULL;
		static char ttt[100];

		if( bFirst )
		{
			MessageBox(0,"First getProcAddress","MDT_DEBUG",MB_OK|MB_ICONINFORMATION);
			bFirst = false;
		}
		if( !f1 ) f1=fopen("mdt_log.txt","wb");
		GetModuleFileName(hModule,ttt,99);
		fprintf(f1,"%s %s\n",ttt, lpProcName);
		fflush(f1);

#endif

		if (!lstrcmp(lpProcName, "GetProcAddress"))
			return (FARPROC) &NewHwGetProcAddress;

		if (!lstrcmp(lpProcName, "glBegin"))
			return (FARPROC) &NewGlBegin;
		if (!lstrcmp(lpProcName, "glEnd"))
			return (FARPROC) &NewGlEnd;
		if (!lstrcmp(lpProcName, "glViewport"))
			return (FARPROC) &NewGlViewport;
		if (!lstrcmp(lpProcName, "glClear"))
			return (FARPROC) &NewGlClear;
		if (!lstrcmp(lpProcName, "glFrustum"))
			return (FARPROC) &NewGlFrustum;
		if (!lstrcmp(lpProcName, "glBlendFunc"))
			return (FARPROC) &NewGlBlendFunc;

		if (!lstrcmp(lpProcName, "wglSwapBuffers"))
		{
			OldWglSwapBuffers = (BOOL (APIENTRY *)(HDC hDC)) nResult;
			return (FARPROC) &NewWglSwapBuffers;
		}

		if (!lstrcmp(lpProcName, "wglCreateContext"))
			return (FARPROC) &NewWglCreateContext;
		if (!lstrcmp(lpProcName, "wglDeleteContext"))
			return (FARPROC) &NewWglDeleteContext;
		if (!lstrcmp(lpProcName, "wglMakeCurrent"))
			return (FARPROC) &NewWglMakeCurrent;


		if (!lstrcmp(lpProcName, "CL_IsThirdPerson")) {
			g_OldClientCL_IsThirdPerson = (CL_IsThirdPerson_t)nResult;
			return (FARPROC) &NewClientCL_IsThirdPerson;
		}

		if (!clientDllLoaded && !lstrcmp(lpProcName, "Initialize") && GetProcAddress(hModule, "HUD_VidInit"))
		{
			clientDllLoaded = true;

			HL_ADDR_SET(clientDll, (HlAddress_t)hModule);
	
			OnClientDllLoaded();
		}
	}

	return nResult;
}

void HookHw(HMODULE hHw)
{
	bool bIcepOk = true;

	HL_ADDR_SET(hwDll, (HlAddress_t)hHw);

	OnHwDllLoaded();

	// hw.dll:
	pEngfuncs		= (cl_enginefuncs_s*)HL_ADDR_GET(p_cl_enginefuncs_s);
	pEngStudio	= (engine_studio_api_s*)HL_ADDR_GET(p_engine_studio_api_s);
	ppmove			= (playermove_s*)HL_ADDR_GET(p_playermove_s);

	// Kernel32.dll:
	if(!InterceptDllCall(hHw, "Kernel32.dll", "GetProcAddress", (DWORD) &NewHwGetProcAddress) ) bIcepOk = false;

	// user32.dll:
	if(!InterceptDllCall(hHw, "user32.dll", "CreateWindowExA", (DWORD) &NewCreateWindowExA) ) bIcepOk = false;
	if(!InterceptDllCall(hHw, "user32.dll", "DestroyWindow", (DWORD) &NewDestroyWindow) ) bIcepOk = false;
	if(!InterceptDllCall(hHw, "user32.dll", "RegisterClassA", (DWORD) &NewRegisterClassA) ) bIcepOk = false;
	if(!InterceptDllCall(hHw, "user32.dll", "SetWindowPos", (DWORD) &NewSetWindowPos) ) bIcepOk = false;

	// gdi32.dll:
	if(!InterceptDllCall(hHw, "gdi32.dll", "ChoosePixelFormat", (DWORD) &NewChoosePixelFormat) ) bIcepOk = false;

	if( !bIcepOk )
		MessageBox(0,"One or more interceptions failed","MDT_ERROR",MB_OK|MB_ICONHAND);

	HandleForceRes();

	Hook_Mod_LeafPvs();

	Hook_R_DrawEntitiesOnList();

	Hook_R_DrawParticles();

	Hook_R_DrawViewModel();

	Hook_R_PolyBlend();
}
