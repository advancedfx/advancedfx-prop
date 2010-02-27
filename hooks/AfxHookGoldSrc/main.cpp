#include "stdafx.h"

//

// Additional libraries:
#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"GLu32.lib")
#pragma comment(lib,"js32.lib")

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
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Mozilla.SpiderMonkey.JS' version='1.7.0.0' processorArchitecture='x86' publicKeyToken='0000000000000000'\"")

//

#include "mirv_scripting.h"
#include "mirv_glext.h"
#include "hooks/HookHl.h"


bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason) 
	{ 
		case DLL_PROCESS_ATTACH:
		{
#ifdef MDT_DEBUG
			MessageBox(0,"DllMain - DLL_PROCESS_ATTACH","MDT_DEBUG",MB_OK|MB_ICONINFORMATION);
#endif
			JsStartUp();

			Install_All_Gl_Extensions();

			HookHl();

			break;
		}
		case DLL_PROCESS_DETACH:
		{
			g_Script_CanConsolePrint = false;

			JsShutDown();
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
