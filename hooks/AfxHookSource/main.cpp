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

#include "RenderView.h"
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

char const * g_Info_VClient = "NULL";
char const * g_Info_VEngineClient = "NULL";
char const * g_Info_VEngineCvar = "NULL";

void PrintInfo() {
	Tier0_Msg(
		"|" "\n"
		"| AfxHookSource ("  __DATE__ " "__TIME__ ")" "\n"
		"| Copyright (c) advancedfx.org" "\n"
		"|" "\n"
	);

	Tier0_Msg("| VClient: %s\n", g_Info_VClient);
	Tier0_Msg("| VEngineClient: %s\n", g_Info_VEngineClient);
	Tier0_Msg("| VEngineCvar: %s\n", g_Info_VEngineCvar);
	Tier0_Msg("| GameDirectory: %s\n", g_VEngineClient->GetGameDirectory());

	Tier0_Msg("|" "\n");
}

void MySetup(CreateInterfaceFn appSystemFactory) {
	static bool bFirstRun = true;

	if(bFirstRun) {
		void *iface , *iface2;

		bFirstRun = false;

		if(iface = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_013, NULL)) {
			g_Info_VEngineClient = VENGINE_CLIENT_INTERFACE_VERSION_013;
			g_VEngineClient = new WrpVEngineClient_013((IVEngineClient_013 *)iface);
		}
		else if(iface = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_012, NULL)) {
			g_Info_VEngineClient = VENGINE_CLIENT_INTERFACE_VERSION_012;
			g_VEngineClient = new WrpVEngineClient_012((IVEngineClient_012 *)iface);
		}
		else {
			ErrorBox("Could not get a supported VEngineClient interface.");
		}

		if(iface = appSystemFactory( VENGINE_CVAR_INTERFACE_VERSION_004, NULL )) {
			g_Info_VEngineCvar = VENGINE_CVAR_INTERFACE_VERSION_004;
			WrpConCommands::RegisterCommands((ICvar_004 *)iface);
		}
		else if(
			(iface = appSystemFactory( VENGINE_CVAR_INTERFACE_VERSION_003, NULL ))
			&& (iface2 = appSystemFactory(VENGINE_CLIENT_INTERFACE_VERSION_012, NULL))
		) {
			g_Info_VEngineCvar = VENGINE_CVAR_INTERFACE_VERSION_003 " & " VENGINE_CLIENT_INTERFACE_VERSION_012;
			WrpConCommands::RegisterCommands((ICvar_003 *)iface, (IVEngineClient_012 *)iface2);
		}
		else {
			ErrorBox("Could not get a supported VEngineCvar interface.");
		}
	}

	if(g_VEngineClient) {
		g_Hook_VClient_RenderView.Install_cstrike();
	}


	PrintInfo();
}

void * old_Client_Init;

int __stdcall new_Client_Init(DWORD *this_ptr, CreateInterfaceFn appSystemFactory, CreateInterfaceFn physicsFactory, CGlobalVarsBase *pGlobals) {
	static bool bFirstCall = true;
	int myret;

	__asm {
		MOV ecx, pGlobals
		PUSH ecx
		MOV ecx, physicsFactory
		PUSH ecx
		MOV ecx, appSystemFactory
		PUSH ecx

		MOV ecx, this_ptr		
		CALL old_Client_Init
		MOV	myret, eax
	}

	if(bFirstCall) {
		bFirstCall = false;

		MySetup(appSystemFactory);
	}

	return myret;
}

__declspec(naked) void hook_Client_Init() {
	static unsigned char * tempMem[8];
	__asm {
		POP eax
		MOV tempMem[0], eax
		MOV tempMem[4], ecx

		PUSH ecx
		CALL new_Client_Init

		MOV ecx, tempMem[4]
		PUSH 0
		PUSH eax
		MOV eax, tempMem[0]
		MOV [esp+4], eax
		POP eax

		RET
	}
}

CreateInterfaceFn old_Client_CreateInterface = 0;
void* new_Client_CreateInterface(const char *pName, int *pReturnCode) {
	static bool bFirstCall = true;
	MdtMemBlockInfos mbis;

	void * pRet = old_Client_CreateInterface(pName, pReturnCode);

	if(bFirstCall) {
		bFirstCall = false;

		void * iface = NULL;




		if(iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_015, NULL)) {
			g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_015;
		}
		else if(iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_013, NULL)) {
			g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_013;
		}
		else if(iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_012, NULL)) {
			g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_012;
		}
		else if(iface = old_Client_CreateInterface(CLIENT_DLL_INTERFACE_VERSION_011, NULL)) {
			g_Info_VClient = CLIENT_DLL_INTERFACE_VERSION_011;
		}
		else {
			ErrorBox("Could not get a supported VClient interface.");
		}

		if(iface) {
			void **padr =  *(void ***)iface;
			old_Client_Init = *padr;
			MdtMemAccessBegin(padr, sizeof(void *), &mbis);
			*padr = (void *)hook_Client_Init;
			MdtMemAccessEnd(&mbis);
		}

	}

	return pRet;
}


HMODULE g_H_ClientDll = 0;

FARPROC WINAPI new_Engine_GetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);

	if (HIWORD(lpProcName))
	{
		if (!lstrcmp(lpProcName, "GetProcAddress"))
			return (FARPROC) &new_Engine_GetProcAddress;

		if (
			hModule == g_H_ClientDll
			&& !lstrcmp(lpProcName, "CreateInterface")
		) {
			old_Client_CreateInterface = (CreateInterfaceFn)nResult;
			return (FARPROC) &new_Client_CreateInterface;
		}

	}

	return nResult;


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

	if(bFirstLaucher && StringEndsWith( lpLibFileName, "launcher.dll"))
	{
		bFirstLaucher = false;

		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
	}
	else
	if(bFirstFileSystemSteam && StringEndsWith( lpLibFileName, "filesystem_steam.dll"))
	{
		bFirstFileSystemSteam = false;

		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
	}
	else
	if(bFirstEngine && StringEndsWith( lpLibFileName, "engine.dll"))
	{
		bFirstEngine = false;

		InterceptDllCall(hModule, "Kernel32.dll", "GetProcAddress", (DWORD) &new_Engine_GetProcAddress);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryExA", (DWORD) &new_LoadLibraryExA);
		InterceptDllCall(hModule, "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
	}
	else
	if(bFirstClient && StringEndsWith( lpLibFileName, "client.dll"))
	{
		bFirstClient = false;

		g_H_ClientDll = hModule;
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
	HMODULE hTier0;

	switch (fdwReason) 
	{ 
		case DLL_PROCESS_ATTACH:
		{
#ifdef _DEBUG
			MessageBox(0,"DllMain - DLL_PROCESS_ATTACH", "AFX_DEBUG", MB_OK|MB_ICONINFORMATION);
#endif
			hTier0 = LoadLibraryA("bin\\tier0.dll");
			Tier0_Msg = (Tier0MsgFn)GetProcAddress(hTier0, "Msg");
			Tier0_DMsg = (Tier0DMsgFn)GetProcAddress(hTier0, "DMsg");
			Tier0_Warning = (Tier0MsgFn)GetProcAddress(hTier0, "Warning");
			Tier0_DWarning = (Tier0DMsgFn)GetProcAddress(hTier0, "DWarning");
			Tier0_Log = (Tier0MsgFn)GetProcAddress(hTier0, "Log");
			Tier0_DLog = (Tier0DMsgFn)GetProcAddress(hTier0, "DLog");
			Tier0_Error = (Tier0MsgFn)GetProcAddress(hTier0, "Error");
			Tier0_ConMsg = (Tier0MsgFn)GetProcAddress(hTier0, "ConMsg");
			Tier0_ConWarning = (Tier0MsgFn)GetProcAddress(hTier0, "ConWarning");
			Tier0_ConLog = (Tier0MsgFn)GetProcAddress(hTier0, "ConLog");

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