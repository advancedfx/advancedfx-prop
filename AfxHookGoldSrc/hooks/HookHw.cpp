#include "stdafx.h"

#include <shared/StringTools.h>

#include <shared/detours.h>

#include "HookHw.h"
#include "OpenGl32Hooks.h"
#include "gdi32Hooks.h"
#include "user32Hooks.h"

#include "hw/Host_Frame.h"
#include "hw/Mod_LeafPvs.h"
#include "hw/R_DrawEntitiesOnList.h"
#include "hw/R_DrawParticles.h"
#include "hw/R_DrawViewModel.h"
#include "hw/R_PolyBlend.h"
#include "hw/R_RenderView.h"
#include "hw/UnkDrawHud.h"

#include "../hl_addresses.h"
#include "../mirv_scripting.h"


struct cl_enginefuncs_s * pEngfuncs		= (struct cl_enginefuncs_s *)0;
struct engine_studio_api_s * pEngStudio	= (struct engine_studio_api_s *)0;
struct playermove_s * ppmove			= (struct playermove_s *)0;

FARPROC WINAPI NewSdlGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);

	if (HIWORD(lpProcName))
	{
#if 0
		static bool bFirst = true;
		static FILE *f1=NULL;
		static char ttt[100];

		if( !f1 ) f1=fopen("mdt_log_NewSdlGetProcAddress.txt","wb");
		GetModuleFileName(hModule,ttt,99);
		fprintf(f1,"%s %s\n",ttt, lpProcName);
		fflush(f1);
#endif

		if (!lstrcmp(lpProcName, "GetProcAddress"))
			return (FARPROC) &NewSdlGetProcAddress;

		if (!lstrcmp(lpProcName, "wglCreateContext"))
			return (FARPROC) &NewWglCreateContext;
		if (!lstrcmp(lpProcName, "wglDeleteContext"))
			return (FARPROC) &NewWglDeleteContext;
		if (!lstrcmp(lpProcName, "wglMakeCurrent"))
			return (FARPROC) &NewWglMakeCurrent;
	}

	return nResult;
}

typedef void * (*SDL_GL_GetProcAddress_t) (const char* proc);
SDL_GL_GetProcAddress_t g_Old_SDL_GL_GetProcAddress;

void *New_SDL_GL_GetProcAddress(const char* proc)
{
	if (!lstrcmp(proc, "glBegin"))
		return (void *) &NewGlBegin;
	if (!lstrcmp(proc, "glEnd"))
		return (void *) &NewGlEnd;
	if (!lstrcmp(proc, "glViewport"))
		return (void *) &NewGlViewport;
	if (!lstrcmp(proc, "glClear"))
		return (void *) &NewGlClear;
	if (!lstrcmp(proc, "glFrustum"))
		return (void *) &NewGlFrustum;
	if (!lstrcmp(proc, "glBlendFunc"))
		return (void *) &NewGlBlendFunc;

	return g_Old_SDL_GL_GetProcAddress(proc);
}

HMODULE WINAPI NewHwLoadLibraryA( LPCSTR lpLibFileName )
{
	static bool bClientLoaded = false;

	HMODULE hRet = LoadLibraryA( lpLibFileName );


	if( !bClientLoaded && StringEndsWith( lpLibFileName, "client.dll") )
	{
		bClientLoaded = true;

		HL_ADDR_SET(clientDll, (HlAddress_t)hRet);
	
		OnClientDllLoaded();
	}

	return hRet;
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
	if(!InterceptDllCall(hHw, "Kernel32.dll", "LoadLibraryA", (DWORD) &NewHwLoadLibraryA)) { bIcepOk = false; MessageBox(0,"Interception failed: Kernel32.dll!LoadLibraryA","MDT_ERROR",MB_OK|MB_ICONHAND); }

	// opengl32.dll:
	if(!(g_Old_SDL_GL_GetProcAddress=(SDL_GL_GetProcAddress_t)InterceptDllCall(hHw, "sdl.dll", "SDL_GL_GetProcAddress", (DWORD) &New_SDL_GL_GetProcAddress) )) { bIcepOk = false; MessageBox(0,"Interception failed: sdl.dll!SDL_GL_GetProcAddress","MDT_ERROR",MB_OK|MB_ICONHAND); }

	HMODULE hSdl = GetModuleHandle("sdl.dll");
	if(hSdl)
	{
		if(!InterceptDllCall(hSdl, "Kernel32.dll", "GetProcAddress", (DWORD) &NewSdlGetProcAddress) ) { bIcepOk = false; MessageBox(0,"Interception failed: Kernel32.dll!GetProcAddress","MDT_ERROR",MB_OK|MB_ICONHAND); }

		// user32.dll:
		if(!InterceptDllCall(hSdl, "user32.dll", "CreateWindowExW", (DWORD) &NewCreateWindowExW) ) { bIcepOk = false; MessageBox(0,"Interception failed: user32.dll!CreateWindowExW","MDT_ERROR",MB_OK|MB_ICONHAND); }
		if(!InterceptDllCall(hSdl, "user32.dll", "DestroyWindow", (DWORD) &NewDestroyWindow) ) { bIcepOk = false; MessageBox(0,"Interception failed: user32.dll!DestroyWindow","MDT_ERROR",MB_OK|MB_ICONHAND); }
		if(!InterceptDllCall(hSdl, "user32.dll", "SetWindowPos", (DWORD) &NewSetWindowPos) ) { bIcepOk = false; MessageBox(0,"Interception failed: user32.dll!SetWindowPos","MDT_ERROR",MB_OK|MB_ICONHAND); }

		// gdi32.dll:
		if(!(OldWglSwapBuffers=(wglSwapBuffers_t)InterceptDllCall(hSdl, "gdi32.dll", "SwapBuffers", (DWORD) &NewWglSwapBuffers) )) { bIcepOk = false; MessageBox(0,"Interception failed: gdi32.dll!SwapBuffers","MDT_ERROR",MB_OK|MB_ICONHAND); }
		if(!InterceptDllCall(hSdl, "gdi32.dll", "ChoosePixelFormat", (DWORD) &NewChoosePixelFormat) ) { bIcepOk = false; MessageBox(0,"Interception failed: gdi32.dll!ChoosePixelFormat","MDT_ERROR",MB_OK|MB_ICONHAND); }
	}
	else
		bIcepOk = false;

	if( !bIcepOk )
		MessageBox(0,"One or more interceptions failed","MDT_ERROR",MB_OK|MB_ICONHAND);

	Hook_Host_Frame();

	Hook_Mod_LeafPvs();

	Hook_R_DrawEntitiesOnList();

	Hook_R_DrawParticles();

	Hook_R_DrawViewModel();

	Hook_R_PolyBlend();

	Hook_R_RenderView();

	Hook_UnkDrawHud();
}
