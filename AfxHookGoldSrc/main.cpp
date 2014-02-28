#include "stdafx.h"

//
// Additional libraries:

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"mozjs-24.lib")

#ifdef AFX_GUI
#pragma comment(lib,"RocketCore.lib")
#endif

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

#include "scripting.h"

#ifdef AFX_GUI
#include "gui/Gui.h"
#endif

#include "Rocket/Core/ReferenceCountable.h"

bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason) 
	{ 
		case DLL_PROCESS_ATTACH:
		{
#if 0
			MessageBox(0,"DLL_PROCESS_ATTACH","MDT_DEBUG",MB_OK);
#endif
			HookHl();

			break;
		}
		case DLL_PROCESS_DETACH:
		{
#if 0
			MessageBox(0,"DLL_PROCESS_DETACH","MDT_DEBUG",MB_OK);
#endif

			g_Script_CanConsolePrint = false;

			g_AfxGoldSrcComClient.Close();

			ScriptEngine_ShutDown();

#ifdef AFX_GUI
			AfxGui_ShutDown();
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
