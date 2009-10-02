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
#include "WrpVEngineClient.h"
#include "WrpConsole.h"


WrpVEngineClient * g_VEngineClient = 0;
ICvar_003 * g_Cvar = 0;



bool StringEndsWith(char const * target, char const * ending) {
	size_t lenTarget = strlen(target);
	size_t lenEnding = strlen(ending);

	if(lenTarget < lenEnding) return false;

	return !strcmp(target +(lenTarget-lenEnding), ending);
}

bool StringBeginsWith(char const * target, char const * beginning) {
	while(*target && *beginning) {
		if(*beginning != *target)
			return false;
		target++;
		beginning++;
	}

	if(*beginning && !*target)
		return false;

	return true;
}

void ErrorBox(char const * messageText) {
	MessageBoxA(0, messageText, "Error - AfxHookSource", MB_OK|MB_ICONERROR);
}

void ErrorBox() {
	ErrorBox("Something went wrong.");
}


HMODULE g_hEngineDll = 0;

void MySetup(CreateInterfaceFn appSystemFactory) {
	static bool bFirstRun = true;

	if(bFirstRun) {
		void * iface;

		bFirstRun = false;

		if(iface = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_013, NULL))
			g_VEngineClient = new WrpVEngineClient_013((IVEngineClient_013 *)iface);
		else if(iface = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_012, NULL))
			g_VEngineClient = new WrpVEngineClient_012((IVEngineClient_012 *)iface);
		else
			ErrorBox("Could not get a supported VEngineClient interface.");

		if(iface = appSystemFactory( VENGINE_CVAR_INTERFACE_VERSION_004, NULL ))
			WrpConCommands::RegisterCommands((ICvar_004 *)iface);
		else if(iface = appSystemFactory( VENGINE_CVAR_INTERFACE_VERSION_003, NULL ))
			WrpConCommands::RegisterCommands((ICvar_003 *)iface);
		else
			ErrorBox("Could not get a supported VEngineCvar interface.");

		// Allow Console commands to register:
		if(g_VEngineClient) {
//				MessageBox(0, g_VEngineClient->GetLevelName(), "TEST", MB_OK|MB_ICONINFORMATION);

			g_VEngineClient->ClientCmd("echo \"// ----------------\"");
			g_VEngineClient->ClientCmd("echo \"// AfxHookSource " __DATE__ " "__TIME__ "\"");
			g_VEngineClient->ClientCmd("echo \"// Copyright (c) advancedfx.org\"");
			g_VEngineClient->ClientCmd("echo \"// ----------------\"");
		}
	}
}

typedef int (__thiscall * Client_Init_t)(void *this_ptr, CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory, CGlobalVarsBase *pGlobals);
Client_Init_t old_Client_Init;

// todo
//__declspec(naked) {
int	__stdcall new_Client_Init(CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory, CGlobalVarsBase *pGlobals) {

	void *this_ptr;

	__asm {
		mov this_ptr,ecx
	}

	ErrorBox("Hello World.");

	static bool bFirstCall = true;
	int iRet = old_Client_Init(this_ptr, appSystemFactory, physicsFactory, pGlobals);

	if(bFirstCall) {
		bFirstCall = false;

		MySetup(appSystemFactory);
	}

	return iRet;
}

CreateInterfaceFn old_Client_CreateInterface = 0;
void* new_Client_CreateInterface(const char *pName, int *pReturnCode) {
	static bool bFirstCall = true;

	void * pRet = old_Client_CreateInterface(pName, pReturnCode);

	if(bFirstCall) {
		bFirstCall = false;

		void * iface;

		if(iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_015, NULL)) {
			ErrorBox("me15");
			DWORD dwAdr = **(DWORD **)iface; // access first funtion in VTable
			old_Client_Init = DetourClassFunc(
				(BYTE *) dw,
				(BYTE *) new_Client_Init,
				0x0A
			);		}
		else if(iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_013, NULL)) {
			ErrorBox("me13");
			DWORD dwAdr = **(DWORD **)iface; // access first funtion in VTable
			old_Client_Init = DetourClassFunc(
				(BYTE *) dw,
				(BYTE *) new_Client_Init,
				0x0A
			);
		}
		else
			ErrorBox("Could not get a supported VClient interface.");
	}

	return pRet;
}


HMODULE WINAPI new_LoadLibraryA(LPCSTR lpLibFileName);
HMODULE WINAPI new_LoadLibraryExA(LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags);


void LibraryHooksA(HMODULE hModule, LPCSTR lpLibFileName) {

	static bool bFirstLaucher = true;
	static bool bFirstFileSystemSteam = true;
	static bool bFirstEngine = true;
	static bool bFirstClient = true;

	if(!hModule || !lpLibFileName)
		return;

	// do not use messageboxes here, there is some friggin hooking going on in between by the
	// Surce engine.

	if(bFirstLaucher && StringEndsWith( lpLibFileName, "bin\\launcher.dll"))
	{
		bFirstLaucher = false;

		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
	}
	else
	if(bFirstFileSystemSteam && StringEndsWith( lpLibFileName, "bin\\filesystem_steam.dll"))
	{
		bFirstFileSystemSteam = false;

		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
	}
	else
	if(bFirstEngine && StringEndsWith( lpLibFileName, "bin\\engine.dll"))
	{
		bFirstEngine = false;

		g_hEngineDll = hModule;

		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
	}
	else
	if(bFirstClient && StringEndsWith( lpLibFileName, "bin\\client.dll"))
	{
		bFirstClient = false;

		//
		// hook client!CreateInterface

		CreateInterfaceFn clientFactory = Sys_GetFactory(reinterpret_cast<CSysModule *>(hModule));
		old_Client_CreateInterface = (CreateInterfaceFn)DetourApply((BYTE *)clientFactory, (BYTE *)&new_Client_CreateInterface, 0x09);
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
			ErrorBox("BREAK");

			if(!(
				InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA)
				||InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA)
			))
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