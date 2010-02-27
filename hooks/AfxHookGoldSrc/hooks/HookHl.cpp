#include "stdafx.h"

#include "HookHw.h"

#include <windows.h>

#include <hooks/shared/detours.h>

#include "HookHw.h"


HMODULE WINAPI new_LoadLibraryA( LPCSTR lpLibFileName )
{
		static bool bFirstLoad = true;

		if( bFirstLoad && !lstrcmp( lpLibFileName, "hw.dll") )
		{
			bFirstLoad = false;
			HMODULE hHw = LoadLibraryA( lpLibFileName );//LoadLibraryEx( lpLibFileName, NULL, DONT_RESOLVE_DLL_REFERENCES );

			if( hHw )
				HookHw(hHw);

			return hHw;
		}

		return LoadLibraryA( lpLibFileName );
}

void HookHl()
{
	// Intercept LoadLibraryA:
	if(!(HMODULE(WINAPI *)( LPCSTR )) InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA))
		MessageBox(0,"Base interception failed","MDT_ERROR",MB_OK|MB_ICONHAND);
}
