#include "stdafx.h"

#include <shared/StringTools.h>

#include <hooks/shared/detours.h>

#include "HookHw.h"
#include "OpenGlHooks.h"
#include "../HltvFix.h"

#include "../hl_addresses.h"
#include "../mirv_scripting.h"

#include "../basecomClient.h"

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
			return (FARPROC) &HlaeBcClt_wglCreateContext;
		if (!lstrcmp(lpProcName, "wglDeleteContext"))
			return (FARPROC) &HlaeBcClt_wglDeleteContext;
		if (!lstrcmp(lpProcName, "wglMakeCurrent"))
			return (FARPROC) &HlaeBcClt_wglMakeCurrent;

		if (!lstrcmp(lpProcName, "CL_IsThirdPerson")) {
			OldClientCL_IsThirdPerson = (CL_IsThirdPerson_t)nResult;
			return (FARPROC) &NewClientCL_IsThirdPerson;
		}

	}

	return nResult;
}

HMODULE WINAPI NewHwLoadLibraryA( LPCSTR lpLibFileName )
{
	if(StringEndsWith(lpLibFileName, "cl_dlls\client.dll"))
	{
		HMODULE hClient = LoadLibraryA(lpLibFileName);

		HL_ADDR_SET(clientDll, (HlAddress_t)hClient);

		OnClientDllLoaded();

		return hClient;
	}

	return LoadLibraryA( lpLibFileName );
}

void HookHw(HMODULE hHw)
{
	HL_ADDR_SET(hwDll, (HlAddress_t)hHw);

	OnHwDllLoaded();

	// update unregistered local copies manually:

	pEngfuncs		= (cl_enginefuncs_s*)HL_ADDR_GET(p_cl_enginefuncs_s);
	pEngStudio	= (engine_studio_api_s*)HL_ADDR_GET(p_engine_studio_api_s);
	ppmove			= (playermove_s*)HL_ADDR_GET(p_playermove_s);


	bool bIcepOk = true;

	if(!InterceptDllCall(hHw, "Kernel32.dll", "GetProcAddress", (DWORD) &NewHwGetProcAddress) ) bIcepOk = false;
	if(!InterceptDllCall(hHw, "Kernel32.dll", "LoadLibraryA", (DWORD) &NewHwLoadLibraryA) ) bIcepOk = false;

	// WindowAPI related:
	if(!InterceptDllCall(hHw, "User32.dll", "CreateWindowExA", (DWORD) &HlaeBcClt_CreateWindowExA) ) bIcepOk = false;
	if(!InterceptDllCall(hHw, "User32.dll", "DestroyWindow", (DWORD) &HlaeBcClt_DestroyWindow) ) bIcepOk = false;
	if(!InterceptDllCall(hHw, "User32.dll", "RegisterClassA", (DWORD) &HlaeBcClt_RegisterClassA) ) bIcepOk = false;
	if(!InterceptDllCall(hHw, "User32.dll", "SetWindowPos", (DWORD) &HlaeBcClt_SetWindowPos) ) bIcepOk = false;
	if(!InterceptDllCall(hHw, "gdi32.dll", "ChoosePixelFormat", (DWORD) &HlaeBcClt_ChoosePixelFormat) ) bIcepOk = false;
	if(!InterceptDllCall(hHw, "User32.dll", "ReleaseDC", (DWORD) &HlaeBcClt_ReleaseDC) ) bIcepOk = false;

	if( !bIcepOk ) MessageBox(0,"One or more interceptions failed","MDT_ERROR",MB_OK|MB_ICONHAND);

	HandleForceRes();
}
