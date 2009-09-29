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

#define MakePtr(cast, ptr, addValue) (cast)((DWORD)(ptr) + (DWORD)(addValue))

void *InterceptDllCall(HMODULE hModule, char *szDllName, char *szFunctionName, DWORD pNewFunction)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	MdtMemBlockInfos mbis;
	void *pOldFunction;


	if (!(pOldFunction = GetProcAddress(GetModuleHandle(szDllName), szFunctionName)))
		return NULL;

	pDosHeader = (PIMAGE_DOS_HEADER) hModule;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	pNTHeader = MakePtr(PIMAGE_NT_HEADERS, pDosHeader, pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE
	|| (pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, pDosHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)) == (PIMAGE_IMPORT_DESCRIPTOR) pNTHeader)
		return NULL;

	while (pImportDesc->Name)
	{
		char *szModuleName = MakePtr(char *, pDosHeader, pImportDesc->Name);
		if (!stricmp(szModuleName, szDllName))
			break;
		pImportDesc++;
	}
	if (pImportDesc->Name == NULL)
		return NULL;

	pThunk = MakePtr(PIMAGE_THUNK_DATA, pDosHeader,	pImportDesc->FirstThunk);
	while (pThunk->u1.Function)
	{
		if (pThunk->u1.Function == (DWORD)pOldFunction)
		{
			MdtMemAccessBegin((void *) &pThunk->u1.Function, sizeof(DWORD), &mbis);
			pThunk->u1.Function = (DWORD) pNewFunction;
			MdtMemAccessEnd(&mbis);

			return pOldFunction;
		}
		pThunk++;
	}

	return NULL;
}

//FARPROC (WINAPI *pGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
FARPROC WINAPI newGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);

	if (HIWORD(lpProcName))
	{
#if 1
//#ifdef _DEBUG
		static bool bFirst = true;
		static FILE *f1=NULL;
		static char ttt[100];

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
	}

	return nResult;
}



HMODULE (WINAPI *p_LoadLibraryA)( LPCSTR lpLibFileName );

HMODULE WINAPI new_LoadLibraryA( LPCSTR lpLibFileName )
{
		static bool bFirstLoad = true;

		MessageBox(0, lpLibFileName, "AFX_DEBUG", MB_OK|MB_ICONINFORMATION);

		if( bFirstLoad && !lstrcmp( lpLibFileName, "bin\\launcher.dll") )
		{
			bFirstLoad = false;
			HMODULE hRet = LoadLibraryA( lpLibFileName );

			if( hRet )
			{
				bool bIcepOk = true;

				// Intercept GetProcAddress:
				if( !InterceptDllCall( hRet, "Kernel32.dll", "GetProcAddress", (DWORD) &newGetProcAddress) ) bIcepOk = false;

				if( !bIcepOk ) MessageBox(0,"One or more interceptions failed", "AFX_ERROR", MB_OK|MB_ICONHAND);
			}

			return hRet;
		}

		return LoadLibraryA( lpLibFileName );
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

			// Intercept LoadLibraryA:
			p_LoadLibraryA = (HMODULE(WINAPI *)( LPCSTR )) InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
			if( !p_LoadLibraryA ) MessageBox(0,"Base interception failed","MDT_ERROR",MB_OK|MB_ICONHAND);

//			pGetProcAddress = (FARPROC(WINAPI *)(HMODULE, LPCSTR)) InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "GetProcAddress", (DWORD) &newGetProcAddress);

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