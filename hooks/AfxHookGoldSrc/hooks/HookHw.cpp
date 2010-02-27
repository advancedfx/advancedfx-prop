#include "stdafx.h"

#include <hooks/shared/detours.h>

#include "HookHw.h"
#include "OpenGlHooks.h"
#include "../HltvFix.h"

#include "../hl_addresses.h"
#include "../mirv_scripting.h"

#include "../basecomClient.h"

#include "../forceres.h"

#include <string>


struct cl_enginefuncs_s * pEngfuncs		= (struct cl_enginefuncs_s *)0;
struct engine_studio_api_s * pEngStudio	= (struct engine_studio_api_s *)0;
struct playermove_s * ppmove			= (struct playermove_s *)0;


#define INIT_SCRIPT_FILE "AfxHookGoldSrc_init.js"
#define SCRIPT_FOLDER "scripts\\"
#define DLL_NAME	"AfxHookGoldSrc.dll"

bool Mdt_LoadConfig(HMODULE hHwDll)
{
	char hookPath[1025];
	bool bCfgres = false;
	HMODULE hHookDll = GetModuleHandle(DLL_NAME);

	hookPath[0]=NULL;
	
	if (hHookDll)
	{
		HL_ADDR_SET(hw_dll, (HlAddress_t)hHwDll);

		GetModuleFileName(hHookDll, hookPath, sizeof(hookPath)/sizeof(*hookPath) -1);

		std::string strFolder(hookPath);
		size_t fp = strFolder.find_last_of('\\');
		if(std::string::npos != fp)
		{
			strFolder.resize(fp+1);
		}

		strFolder += SCRIPT_FOLDER;

		JsSetScriptFolder(strFolder.c_str());

		bCfgres = JsExecute("load('" INIT_SCRIPT_FILE "');");

		if(!bCfgres) {
			strFolder = "Failed to load:\n\""
				+strFolder +INIT_SCRIPT_FILE "\"."
			;

			MessageBox(0, strFolder.c_str(), "MDT_ERROR",MB_OK|MB_ICONHAND);
		}
	}

	if (bCfgres)
	{
		// update unregistered local copies manually:
		pEngfuncs		= (cl_enginefuncs_s*)HL_ADDR_GET(p_cl_enginefuncs_s);
		pEngStudio	= (engine_studio_api_s*)HL_ADDR_GET(p_engine_studio_api_s);
		ppmove			= (playermove_s*)HL_ADDR_GET(p_playermove_s);
	}

	
	return bCfgres;
}


FARPROC WINAPI NewHwGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);

	if (HIWORD(lpProcName))
	{
#ifdef _DEBUG
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

void HookHw(HMODULE hHw)
{
	// load addresses form config:
	Mdt_LoadConfig( hHw );

	bool bIcepOk = true;

	// Intercept GetProcAddress:
	if( !InterceptDllCall( hHw, "Kernel32.dll", "GetProcAddress", (DWORD) &NewHwGetProcAddress) ) bIcepOk = false;

	// WindowAPI related:
	if( !InterceptDllCall( hHw, "User32.dll", "CreateWindowExA", (DWORD) &HlaeBcClt_CreateWindowExA) ) bIcepOk = false;
	if( !InterceptDllCall( hHw, "User32.dll", "DestroyWindow", (DWORD) &HlaeBcClt_DestroyWindow) ) bIcepOk = false;
	if( !InterceptDllCall( hHw, "User32.dll", "RegisterClassA", (DWORD) &HlaeBcClt_RegisterClassA) ) bIcepOk = false;
	if( !InterceptDllCall( hHw, "User32.dll", "SetWindowPos", (DWORD) &HlaeBcClt_SetWindowPos) ) bIcepOk = false;
	if( !InterceptDllCall( hHw, "gdi32.dll", "ChoosePixelFormat", (DWORD) &HlaeBcClt_ChoosePixelFormat) ) bIcepOk = false;
	if( !InterceptDllCall( hHw, "User32.dll", "ReleaseDC", (DWORD) &HlaeBcClt_ReleaseDC) ) bIcepOk = false;

	if( !bIcepOk ) MessageBox(0,"One or more interceptions failed","MDT_ERROR",MB_OK|MB_ICONHAND);

	HandleForceRes();
}
