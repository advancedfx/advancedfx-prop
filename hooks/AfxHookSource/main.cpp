#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-09-29 by dominik.matrixstorm.com
//
// First changes:
// 2009-09-29 by dominik.matrixstorm.com

#include <windows.h>

#include <hooks/shared/detours.h>

#include "SourceInterfaces.h"


bool StringEndsWith(char const * target, char const * ending) {
	size_t lenTarget = strlen(target);
	size_t lenEnding = strlen(ending);

	if(lenTarget < lenEnding) return false;

	return !strcmp(target +(lenTarget-lenEnding), ending);
}

void ErrorBox(char const * messageText) {
	MessageBoxA(0, messageText, "Error - AfxHookSource", MB_OK|MB_ICONERROR);
}

void ErrorBox() {
	ErrorBox("Something went wrong.");
}


IVEngineClient_012 * g_VEngineClient = 0;
ICvar_003 * g_Cvar = 0;
bool g_FirstLauncherDllCall = false;
HMODULE g_hClientDll = 0;
HMODULE g_hEngineDll = 0;


HMODULE WINAPI new_LoadLibraryA(LPCSTR lpLibFileName);
HMODULE WINAPI new_LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);


FARPROC WINAPI newGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	static char ttt[100];
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);

	if (HIWORD(lpProcName))
	{
		// lpProcName is a Name and not a Ordinal number.

#ifdef _DEBUG
		static bool bFirst = true;
		static FILE *f1=NULL;

		if( bFirst )
		{
			MessageBox(0,"First getProcAddress", "AFX_DEBUG", MB_OK|MB_ICONINFORMATION);
			bFirst = false;
		}
		if( !f1 ) f1=fopen("mdt_log.txt","wb");
		GetModuleFileName(hModule,ttt,99);
		fprintf(f1,"%s %s\n",ttt, lpProcName);
		fflush(f1);
#endif

		if (!lstrcmp(lpProcName, "GetProcAddress"))
			return (FARPROC) &newGetProcAddress;

		// Hook Source Engine "CreateInterfce" calls:
		if (!lstrcmp(lpProcName, CREATEINTERFACE_PROCNAME)) {
			if(!GetModuleFileNameA(hModule, ttt, 100))
			{
				ErrorBox();
				return nResult;
			}

			if(StringEndsWith(ttt, "engine.dll")) {
				if(g_hEngineDll)
					return nResult; // already hooked

				g_hEngineDll = hModule;

				if(!InterceptDllCall(hModule, "Kernel32.dll", "GetProcAddress", (DWORD) &newGetProcAddress))
					ErrorBox();

				return nResult;
			}
			
			if(StringEndsWith(ttt, "client.dll")) {
				if(g_hClientDll)
					return nResult; // already hooked

				g_hClientDll = hModule;

				CreateInterfaceFn appSystemFactory = Sys_GetFactory(reinterpret_cast<CSysModule *>(g_hEngineDll));

				if(appSystemFactory) {
					g_VEngineClient = (IVEngineClient_012 *)appSystemFactory( VENGINE_CLIENT_INTERFACE_VERSION_012, NULL );
					g_Cvar = (ICvar_003 *)appSystemFactory( VENGINE_CVAR_INTERFACE_VERSION_003, NULL );
				}
				else
					ErrorBox();

				return nResult;
			}

			return nResult;
		}
	}

	return nResult;
}


void LibraryHooksA(HMODULE hModule, LPCSTR lpLibFileName) {

	if(!hModule)
		return;

		MessageBox(0, lpLibFileName, "LibraryHooksA", MB_OK|MB_ICONINFORMATION);


	// Hook Launcher.dll's GetProcAddress:
	if(
		!g_FirstLauncherDllCall && StringEndsWith( lpLibFileName, "bin\\launcher.dll")
	)
	{
		g_FirstLauncherDllCall = true;

		if(!InterceptDllCall(hModule, "Kernel32.dll", "GetProcAddress", (DWORD) &newGetProcAddress))
			ErrorBox();
	}
}


// i.e. called by Counter-Strike Source
HMODULE WINAPI new_LoadLibraryA( LPCSTR lpLibFileName ) {
	HMODULE hRet = LoadLibraryA(lpLibFileName);

	LibraryHooksA(hRet, lpLibFileName);

	return hRet;
}


// i.e. called by Portal First Slice
HMODULE WINAPI new_LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
	HMODULE hRet = LoadLibraryExA(lpLibFileName, hFile, dwFlags);

	LibraryHooksA(hRet, lpLibFileName);

	return hRet;
}


bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason) 
	{ 
		case DLL_PROCESS_ATTACH:
		{
#ifdef _DEBUG
			MessageBox(0,"DllMain - DLL_PROCESS_ATTACH", "AFX_DEBUG", MB_OK|MB_ICONINFORMATION);
#endif

			if(!InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA))
				ErrorBox();
			if(!InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA))
				ErrorBox();

			break;
		}
		case DLL_PROCESS_DETACH:
		{
			// source terminates the process, this will never get called
#ifdef _DEBUG
			MessageBox(0,"DllMain - DLL_PROCESS_DEATTACH", "AFX_DEBUG", MB_OK|MB_ICONINFORMATION);
#endif
			break;
		}
		case DLL_THREAD_ATTACH:
		{
			break;
		}
		case DLL_THREAD_DETACH:
		{
			break;
		}
	}
	return true;
}