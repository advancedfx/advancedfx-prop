#include "stdafx.h"

#include "HookHw.h"

#include <windows.h>

#include <hooks/shared/detours.h>

#include "HookHw.h"


HMODULE WINAPI new_LoadLibraryA( LPCSTR lpLibFileName )
{
		static bool bHwLoaded = false;
		static bool bClientLoaded = false;

		if( !bHwLoaded && !lstrcmp( lpLibFileName, "hw.dll") )
		{
			bHwLoaded = true;
			HMODULE hHw = LoadLibraryA( lpLibFileName );

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
