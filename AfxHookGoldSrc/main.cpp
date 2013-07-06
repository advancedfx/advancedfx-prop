#include "stdafx.h"

//

// Additional libraries:
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"GLu32.lib")
#pragma comment(lib,"libpng16.lib")

/*
// Direct show:
#ifdef _DEBUG
#pragma comment(lib,"strmbasd.lib") // BaseClasses release will not work in debug.
#else
#pragma comment(lib,"strmbase.lib")
#endif
#pragma comment(lib,"winmm.lib")
*/

// Additonal dependencies:
//#pragma comment(linker, "\"/manifestdependency:type='win32' name='Mozilla.SpiderMonkey.JS' version='1.7.0.0' processorArchitecture='x86' publicKeyToken='0000000000000000'\"")

//

#include "hooks/HookHl.h"

#include "AfxGoldSrcComClient.h"


bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason) 
	{ 
		case DLL_PROCESS_ATTACH:
		{
#if MDT_DEBUG
			MessageBox(0,"DLL_PROCESS_ATTACH","MDT_DEBUG",MB_OK);
#endif

			HookHl();

			break;
		}
		case DLL_PROCESS_DETACH:
		{
			g_AfxGoldSrcComClient.Close();

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
