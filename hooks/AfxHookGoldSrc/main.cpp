// #include "stdafx.h"

/*
	Mirv Demo Tool

*/

// pull in additional libraries:
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

// additonal dependencies:
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Mozilla.SpiderMonkey.JS' version='1.7.0.0' processorArchitecture='x86' publicKeyToken='0000000000000000'\"")


#include <windows.h>
#include <winbase.h>
#include <gl\gl.h>
#include <gl\glu.h>

// BEGIN HLSDK includes
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <hlsdk/multiplayer/cl_dll/wrect.h>
#include <hlsdk/multiplayer/cl_dll/cl_dll.h>
#include <hlsdk/multiplayer/engine/cdll_int.h>
#include <hlsdk/multiplayer/common/cvardef.h>
#include <hlsdk/multiplayer/common/r_efx.h>
#include <hlsdk/multiplayer/common/com_model.h>
#include <hlsdk/multiplayer/common/r_studioint.h>
#include <hlsdk/multiplayer/pm_shared/pm_defs.h>
#include <hlsdk/multiplayer/common/entity_types.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include <hooks/shared/detours.h>
#include "filming.h"
#include "aiming.h"
#include "zooming.h"
#include "cmdregister.h"
#include "ui.h"

#include "supportrender.h" // off-screen recording support

#include "dsound_hook.h"

#include "hl_addresses.h" // address definitions

#include "newsky.h"
#include "cmd_tools.h"

#include "basecomClient.h"

#include "mirv_glext.h"

#include "mirv_scripting.h"

#include "FxColor.h"
#include "FxColorMask.h"
#include "FxHide.h"
#include "FxReplace.h"
#include "MirvInfo.h"

#include "mirv_commands.h"

#include "forceres.h"

#include <string>

extern Filming g_Filming;
extern Aiming g_Aiming;
extern Zooming g_Zooming;
extern CHlaeCmdTools g_CmdTools;

CHlaeSupportRender *g_pSupportRender = NULL; // inited in basecomClient.cpp


// Various H-L Engine interface (super) Globals:
//   (they are filled when the addresses system is loaded)
cl_enginefuncs_s* pEngfuncs		= (cl_enginefuncs_s*)	NULL;
engine_studio_api_s* pEngStudio	= (engine_studio_api_s*)NULL;
playermove_s* ppmove			= (playermove_s*)		NULL;

int		g_nViewports = 0;
bool	g_bIsSucceedingViewport = false;

#define MDT_MAX_PATH_BYTES 1025
#define INIT_SCRIPT_FILE "AfxHookGoldSrc_init.js"
#define SCRIPT_FOLDER "scripts\\"
#define DLL_NAME	"AfxHookGoldSrc.dll"

HMODULE g_hMDTDLL=NULL; // handle to our self
static char pg_MDTpath[MDT_MAX_PATH_BYTES];

//
//  Cvars
//


REGISTER_CVAR(disableautodirector, "0", 0);

REGISTER_DEBUGCVAR(gl_noclear, "0", 0);
REGISTER_DEBUGCVAR(gl_previewclear, "1", 0);

//
// Commands
//


REGISTER_DEBUGCVAR(deltatime, "1.0", 0);

xcommand_t g_Old_connect = NULL;
void Hook_connect(void)
{
	int imbret = MessageBoxA(NULL,
		"WARNING: You are about to connect to a server.\n"
		"It is strongly recommended to NOT connect to any server while HLAE is running!\n"
		"\n"
		"You have the following options now:\n"
		"Press Yes\n"
		"\tto continue connecting.\n"
		"Press NO (recommended)\n"
		"\tto abort connecting (HLAE will terminate and lock-up the game).\n"
		"Cancel\n"
		"\tdoes the same as No\n"
		"\n"
		"Do you want to continue connecting?\n"
		,"Game tries to connect",
		MB_YESNOCANCEL|MB_ICONWARNING|MB_DEFBUTTON2
	);
	if(imbret != IDYES)
	{
		// do everything we can to abbort connection:
		do
		{
			try
			{
				HANDLE hproc = OpenProcess( PROCESS_TERMINATE, true, GetCurrentProcessId() );
				TerminateProcess(hproc, 1);
				CloseHandle(hproc);
			} catch (...)
			{
				do MessageBox(NULL,"Please terminate the game manually in the taskmanager!","Cannot terminate, please help:",MB_OK|MB_ICONERROR);
				while (true);
			}
		} while(true);
	}
	else if (imbret == IDYES) g_Old_connect();
}



void Hook_startmovie(void)
{
	if (g_Filming.isFilming())
	{
		pEngfuncs->Con_Printf("Already recording!\n");
		return;
	}

	if(pEngfuncs->Cmd_Argc()<3)
	{
		pEngfuncs->Con_Printf("startmovie <filename> <fps>\n");
		return;
	}

	static char psztemp [513];
	static char pszpath [256];

	const char *paddpath=NULL;
	char *pcmdfname=pEngfuncs->Cmd_Argv(1);
	
	char *pslashpos = strchr(pcmdfname,'/');
	char *pbackslashpos = strchr(pcmdfname,'\\');

	if (!(pslashpos || pbackslashpos))
		paddpath = pEngfuncs->pfnGetGameDirectory();
		//pEngfuncs->CheckParm( "-game", &paddpath );

	psztemp[sizeof(psztemp)-1]=0;
	pszpath[sizeof(pszpath)-1]=0;
	if (paddpath) _snprintf(pszpath,sizeof(pszpath)-1,"%s/%s",paddpath,pcmdfname);
	else strncpy(pszpath,pcmdfname,sizeof(pszpath)-1);

	_snprintf(psztemp,sizeof(psztemp)-1,"mirv_movie_filename \"%s\"",pszpath);

	pEngfuncs->pfnClientCmd(psztemp);

	pEngfuncs->Cvar_SetValue("mirv_movie_fps",atof(pEngfuncs->Cmd_Argv(2)));

	// command won't have finished here, so we do a evil hack:
	// we also can't simply free and alloc here, because Quake 1 / H-L piggy backs infos:
	cvar_t *pIamVictim = pEngfuncs->pfnGetCvarPointer("mirv_movie_filename");
	char *pIamEvil=pIamVictim->string;
	pIamVictim->string = pszpath;

	pEngfuncs->Con_DPrintf("game dir: %s\ncvar is: %s\n",pEngfuncs->pfnGetGameDirectory(),pEngfuncs->pfnGetCvarPointer("mirv_movie_filename")->string);
	g_Filming.Start();
	pIamVictim->string = pIamEvil;
}

void Hook_endmovie(void)
{
	g_Filming.Stop();
}

REGISTER_DEBUGCMD_FUNC(debug_cmdaddress)
{
	if(pEngfuncs->Cmd_Argc()!=2) return;

	char *parg = pEngfuncs->Cmd_Argv(1);

	void * paddr = (void *)g_CmdTools.GiveCommandFn(parg);
	pEngfuncs->Con_Printf("%s: 0x%08x\n",parg,paddr);
}

REGISTER_DEBUGCMD_FUNC(debug_spec)
{
	pEngfuncs->Con_Printf(
		"iuser1: %i\n"
		"iuser2: %i\n",
		ppmove->iuser1,
		ppmove->iuser2
	);

	if(pEngfuncs->Cmd_Argc()!=2) return;
	int idx = atoi(pEngfuncs->Cmd_Argv(1));

	cl_entity_t *e = pEngfuncs->GetEntityByIndex(idx);

	if (e)
		pEngfuncs->Con_Printf(" e");
	else
		return;
	if(e->player)
		pEngfuncs->Con_Printf(" player");
	else
		return;
	if(e->model)
		pEngfuncs->Con_Printf(" model");
	else
		return;
	if(!(e->curstate.effects & EF_NODRAW))
		pEngfuncs->Con_Printf(" NODRAW");
	else
		return;

	float flDeltaTime = fabs(pEngfuncs->GetClientTime() - e->curstate.msg_time);
	pEngfuncs->Con_Printf(" time:%f\n", flDeltaTime);

}

// >> dem_forcehltv fix

//	bool bNotInEye = ppmove->iuser1 != 4;
//	int iwatched = ppmove->iuser2;

REGISTER_DEBUGCVAR(fixforcehltv, "1", 0);
REGISTER_DEBUGCVAR(force_thirdperson, "0", 0);

bool g_FixForceHltvEnabled = false;

int FixForceHltv_CL_IsThirdPerson( void )
{
	if(force_thirdperson->value)
		return 1 == force_thirdperson->value ? 1 : 0;

	return
		fixforcehltv->value
		&& pEngfuncs->IsSpectateOnly()
		&& g_FixForceHltvEnabled
		&& (
			ppmove->iuser1 != 4 // not in-eye
			|| ppmove->iuser2 != pEngfuncs->GetLocalPlayer()->index // not watching ourselfs
		) ? 1: 0
	;
}

xcommand_t g_Old_dem_forcehltv = NULL;

void Hook_dem_forcehltv(void)
{
	char *ptmp="";
	if (pEngfuncs->Cmd_Argc()>=1) ptmp=pEngfuncs->Cmd_Argv(1);
	g_FixForceHltvEnabled = (0 != atof(ptmp));
	g_Old_dem_forcehltv();
}

// << dem_forcehltv fix


//
//	OpenGl Hooking
//

struct glBegin_saved_s {
	bool restore;
	GLboolean b_GL_DEPTH_TEST;
	GLint i_GL_DEPTH_FUNC;
	GLboolean b_ColorWriteMask[4];
} g_glBegin_saved;

unsigned int g_glBeginStats = 0;

REGISTER_DEBUGCVAR(glbegin_stats, "0", 0)

void APIENTRY my_glBegin(GLenum mode)
{
	ScriptEvent_OnGlBegin((unsigned int)mode);

	g_glBegin_saved.restore=false;

	g_glBeginStats++;

	g_NewSky.DetectAndProcessSky(mode);

	if (g_Filming.doWireframe(mode) == Filming::DR_HIDE) {
		return;
	}

	g_Filming.DoWorldFxBegin(mode); // WH fx

	g_Filming.DoWorldFx2(mode); // lightmap fx

	if (!g_Filming.isFilming())
	{
		glBegin(mode);
		return;
	}

	Filming::DRAW_RESULT res = g_Filming.shouldDraw(mode);

	if (res == Filming::DR_HIDE) {
		return;
	}

	else if (res == Filming::DR_MASK)
	{
		if(Filming::MS_ENTITY == g_Filming.GetMatteStage())
		{
			g_glBegin_saved.restore = true;
			glGetBooleanv(GL_DEPTH_TEST,&(g_glBegin_saved.b_GL_DEPTH_TEST));
			glGetIntegerv(GL_DEPTH_FUNC,&(g_glBegin_saved.i_GL_DEPTH_FUNC));
			glGetBooleanv(GL_COLOR_WRITEMASK, g_glBegin_saved.b_ColorWriteMask);

			glColorMask(FALSE, FALSE, FALSE, TRUE);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST);
		}
	}
	else if (!g_Filming.bWantsHudCapture)
		glColorMask(TRUE, TRUE, TRUE, TRUE); // BlendFunc for additive sprites needs special controll, don't override it

	g_FxReplace.OnGlBegin();

	g_FxColor.OnGlBegin();

	g_FxColorMask.OnGlBegin();

	if(g_FxHide.OnGlBegin()) glBegin(mode);
}

void APIENTRY my_glEnd(void)
{
	ScriptEvent_OnGlEnd();

	glEnd();

	g_FxColorMask.OnGlEnd();

	g_FxColor.OnGlEnd();

	g_FxReplace.OnGlEnd();


	if (g_glBegin_saved.restore)
	{
		g_glBegin_saved.restore = false;
		if(!g_glBegin_saved.b_GL_DEPTH_TEST)
			glDisable(GL_DEPTH_TEST);
		glDepthFunc(g_glBegin_saved.i_GL_DEPTH_FUNC);
		glColorMask(g_glBegin_saved.b_ColorWriteMask[0], g_glBegin_saved.b_ColorWriteMask[1], g_glBegin_saved.b_ColorWriteMask[2], g_glBegin_saved.b_ColorWriteMask[3]);
	}

	g_Filming.DoWorldFxEnd();

}

void APIENTRY my_glClear(GLbitfield mask)
{
	if (gl_noclear->value)
		return;
	
	// check if we want to clear (it also might set clearcolor and stuff like that):
	if (!g_Filming.checkClear(mask))
		return;

	glClear(mask);
}




SCREENINFO screeninfo;

void APIENTRY my_glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	static bool bFirstRun = true;

	g_bIsSucceedingViewport = true;

	if (bFirstRun)
	{
#ifdef MDT_DEBUG
		MessageBox(0,"First my_glViewport","MDT_DEBUG",MB_OK|MB_ICONINFORMATION);
#endif

		g_Script_CanConsolePrint = true;

		//
		// Install OpenGL Extensions:

		if(!Install_All_Gl_Extensions())
		{
			pEngfuncs->Con_Printf("MDT WARNING: Could not install all OpenGL extensions. Some features might not work.\n");
		}

		// Register commands:
		Mirv_Commands_Register();


		pEngfuncs->Con_Printf("Mirv Demo Tool %s\nBy Mirvin_Monkey 02/05/2004\n\n", __DATE__);


		screeninfo.iSize = sizeof(SCREENINFO);
		pEngfuncs->pfnGetScreenInfo(&screeninfo);
		pEngfuncs->Con_DPrintf("ScreenRes: %dx%d\n", screeninfo.iWidth, screeninfo.iHeight);

		g_Filming.SupplySupportRenderer(g_pSupportRender);

		g_Filming.setScreenSize(screeninfo.iWidth,screeninfo.iHeight);

		// Init the CmdTools (might be a bit late here for hooking some funcs):
		g_CmdTools.Init(pEngfuncs);
		pEngfuncs->Con_DPrintf("CommandTree at: 0x%08x\n", g_CmdTools.GiveCommandTreePtr());

		// install some hooks:
		if (!(g_Old_connect = g_CmdTools.HookCommand("connect",Hook_connect))) pEngfuncs->Con_Printf("HLAE warning: Failed hooking connect");
		if (!(g_Old_dem_forcehltv = g_CmdTools.HookCommand("dem_forcehltv",Hook_dem_forcehltv))) pEngfuncs->Con_Printf("HLAE warning: Failed hooking dem_forcehltv");
		if (!(g_CmdTools.HookCommand("startmovie",Hook_startmovie))) pEngfuncs->Con_Printf("HLAE warning: Failed hooking startmovie");
		if (!(g_CmdTools.HookCommand("endmovie",Hook_endmovie))) pEngfuncs->Con_Printf("HLAE warning: Failed hooking endmovie");

		bFirstRun = false;
	}


	// Only on the first viewport
	if (g_nViewports == 0)
	{
		//g_Filming.setScreenSize(width, height);

		if(glbegin_stats->value)
			pEngfuncs->Con_Printf("glBegin calls: %u\n", g_glBeginStats);

		g_glBeginStats = 0;

		// Always get rid of auto_director
		if (disableautodirector->value != 0.0f)
			pEngfuncs->Cvar_SetValue("spec_autodirector", 0.0f);

		// This is called whether we're zooming or not
		g_Zooming.handleZoom();

		// this is now done in doCapturePoint() called in swap
		//if (g_Filming.isFilming())
		//	g_Filming.recordBuffers();

		if (g_Aiming.isAiming())
			g_Aiming.aim();
	}

	// Not necessarily 5 viewports anymore, keep counting until reset
	// by swapbuffers hook.
	g_nViewports++;

	g_Zooming.adjustViewportParams(x, y, width, height);
	glViewport(x, y, width, height);

}

void APIENTRY my_glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	g_Filming.SupplyZClipping(zNear, zFar);
	g_Zooming.adjustFrustumParams(left, right, top, bottom);
	glFrustum(left, right, bottom, top, zNear, zFar);
}

void APIENTRY my_glBlendFunc (GLenum sfactor, GLenum dfactor)
{
	switch(g_Filming.giveHudRqState())
	{
	case Filming::HUDRQ_CAPTURE_ALPHA:
		if (dfactor == GL_ONE)//(sfactor == dfactor == GL_ONE)
		{
			// block the Alpha chan of Additive sprites
			glColorMask(TRUE, TRUE, TRUE, FALSE); // block alpha for additive HUD sprites
			glBlendFunc(sfactor,dfactor);
		}
		else
		{
			// don't let sprites lower alpha value:
			glColorMask(TRUE, TRUE, TRUE,TRUE); // allow alpha
			if (sfactor==GL_SRC_ALPHA) sfactor=GL_SRC_ALPHA_SATURATE;
			if (dfactor==GL_SRC_ALPHA) dfactor=GL_SRC_ALPHA_SATURATE;
			glBlendFunc(sfactor,dfactor);
		}
		break;
	default :
		glBlendFunc(sfactor,dfactor);
	}
}

//
// Hooking
//

bool Mdt_LoadConfig(HMODULE hHwDll)
{
	bool bCfgres = false;

	g_hMDTDLL = GetModuleHandle(DLL_NAME);
	pg_MDTpath[0]=NULL;
	if (g_hMDTDLL) {
		HL_ADDR_SET(hw_dll, (HlAddress_t)hHwDll);

		GetModuleFileName(g_hMDTDLL, pg_MDTpath,MDT_MAX_PATH_BYTES-1);

		std::string strFolder(pg_MDTpath);
		size_t fp = strFolder.find_last_of('\\');
		if(std::string::npos != fp) {
			strFolder.resize(fp+1);

/*			fp = 0;
			while(std::string::npos != fp) {
				fp = strFolder.find_first_of('\\', fp);
				if(std::string::npos != fp) {
					strFolder.insert(fp, "\\");
					fp += 2;
				}
			}
*/
		}

		strFolder += SCRIPT_FOLDER;

		JsSetScriptFolder(strFolder.c_str());

		bCfgres = JsExecute("load('" INIT_SCRIPT_FILE "');");

		if(!bCfgres) {
			strFolder = "Failed to load:\n\""
				+strFolder +INIT_SCRIPT_FILE "\"."
			;

			MessageBox(0, strFolder.c_str(), "MDT_ERROR",MB_OK|MB_ICONHAND);
		}
	}

	if (bCfgres)
	{
		// update unregistered local copies manually:
		pEngfuncs		= (cl_enginefuncs_s*)HL_ADDR_GET(p_cl_enginefuncs_s);
		pEngStudio	= (engine_studio_api_s*)HL_ADDR_GET(p_engine_studio_api_s);
		ppmove			= (playermove_s*)HL_ADDR_GET(p_playermove_s);
	}

	
	return bCfgres;
}


BOOL (APIENTRY *pwglSwapBuffers)(HDC hDC);
BOOL APIENTRY my_wglSwapBuffers(HDC hDC)
{
	BOOL bResWglSwapBuffers;
	bool bRecordSwapped=false;

	// Next viewport will be the first of the new frame
	g_nViewports = 0;

	if (g_Filming.isFilming())
	{
		// we are filming, force buffers and capture our image:
		
		// record the selected buffer (capture):
		bRecordSwapped = g_Filming.recordBuffers(hDC,&bResWglSwapBuffers);
	}

	// do the switching of buffers as requersted:
	if (!bRecordSwapped)
	{
		if (g_pSupportRender)
			bResWglSwapBuffers = g_pSupportRender->hlaeSwapBuffers(hDC);
		else
			bResWglSwapBuffers = (*pwglSwapBuffers)(hDC);
	}

	// no we have captured the image (by default from backbuffer) and display it on the front, now we can prepare the new backbuffer image if required.

	if (g_Filming.isFilming())
	{
		// we are filming, do required clearing and restore buffers:

		// carry out preparerations on the backbuffer for the next frame:
		g_Filming.clearBuffers();
	}
	else if(gl_previewclear->value)
		g_Filming.clearBuffers();


	return bResWglSwapBuffers;
}


//FARPROC (WINAPI *pGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
FARPROC WINAPI newGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);

	if (HIWORD(lpProcName))
	{
#ifdef MDT_DEBUG
		static bool bFirst = true;
		static FILE *f1=NULL;
		static char ttt[100];

		if( bFirst )
		{
			MessageBox(0,"First getProcAddress","MDT_DEBUG",MB_OK|MB_ICONINFORMATION);
			bFirst = false;
		}
		if( !f1 ) f1=fopen("mdt_log.txt","wb");
		GetModuleFileName(hModule,ttt,99);
		fprintf(f1,"%s %s\n",ttt, lpProcName);
		fflush(f1);

#endif

		if (!lstrcmp(lpProcName, "GetProcAddress"))
			return (FARPROC) &newGetProcAddress;

		if (!lstrcmp(lpProcName, "glBegin"))
			return (FARPROC) &my_glBegin;
		if (!lstrcmp(lpProcName, "glEnd"))
			return (FARPROC) &my_glEnd;
		if (!lstrcmp(lpProcName, "glViewport"))
		{
			return (FARPROC) &my_glViewport;
		}
		if (!lstrcmp(lpProcName, "glClear"))
			return (FARPROC) &my_glClear;
		if (!lstrcmp(lpProcName, "glFrustum"))
			return (FARPROC) &my_glFrustum;
		if (!lstrcmp(lpProcName, "glBlendFunc"))
			return (FARPROC) &my_glBlendFunc;

		if (!lstrcmp(lpProcName, "wglSwapBuffers"))
		{
			pwglSwapBuffers = (BOOL (APIENTRY *)(HDC hDC)) nResult;
			return (FARPROC) &my_wglSwapBuffers;
		}

		if (!lstrcmp(lpProcName, "wglCreateContext"))
			return (FARPROC) &HlaeBcClt_wglCreateContext;
		if (!lstrcmp(lpProcName, "wglDeleteContext"))
			return (FARPROC) &HlaeBcClt_wglDeleteContext;
		if (!lstrcmp(lpProcName, "wglMakeCurrent"))
			return (FARPROC) &HlaeBcClt_wglMakeCurrent;

		if (!lstrcmp(lpProcName, "CL_IsThirdPerson"))
			return (FARPROC) &FixForceHltv_CL_IsThirdPerson;

		//if (!lstrcmp(lpProcName,"DirectSoundCreate"))
		//	return Hook_DirectSoundCreate(nResult);

		//if (!lstrcmp(lpProcName,"DirectInputCreateA"))
			//return (FARPROC) &my_DirectInputCreateA;
			// DirectInputCreateA" - imported but never called?
			// Half-Life uses DirectInputCreateW (uni code) instead
			// (Steam's vgui)
	}

	return nResult;


}

HMODULE (WINAPI *p_LoadLibraryA)( LPCSTR lpLibFileName );
HMODULE WINAPI new_LoadLibraryA( LPCSTR lpLibFileName )
{
		static bool bFirstLoad = true;

		if( bFirstLoad && !lstrcmp( lpLibFileName, "hw.dll") )
		{
			bFirstLoad = false;
			HMODULE hRet = LoadLibraryA( lpLibFileName );//LoadLibraryEx( lpLibFileName, NULL, DONT_RESOLVE_DLL_REFERENCES );

#ifdef MDT_DEBUG
			MessageBox(0,"new_LoadLibraryA","MDT_DEBUG",MB_OK|MB_ICONINFORMATION);
#endif
			if( hRet )
			{
				// load addresses form config:
				Mdt_LoadConfig( hRet );

				bool bIcepOk = true;

				// Intercept GetProcAddress:
				if( !InterceptDllCall( hRet, "Kernel32.dll", "GetProcAddress", (DWORD) &newGetProcAddress) ) bIcepOk = false;

				// WindowAPI related:
				if( !InterceptDllCall( hRet, "User32.dll", "CreateWindowExA", (DWORD) &HlaeBcClt_CreateWindowExA) ) bIcepOk = false;
				if( !InterceptDllCall( hRet, "User32.dll", "DestroyWindow", (DWORD) &HlaeBcClt_DestroyWindow) ) bIcepOk = false;
				if( !InterceptDllCall( hRet, "User32.dll", "RegisterClassA", (DWORD) &HlaeBcClt_RegisterClassA) ) bIcepOk = false;
				if( !InterceptDllCall( hRet, "User32.dll", "SetWindowPos", (DWORD) &HlaeBcClt_SetWindowPos) ) bIcepOk = false;
				if( !InterceptDllCall( hRet, "gdi32.dll", "ChoosePixelFormat", (DWORD) &HlaeBcClt_ChoosePixelFormat) ) bIcepOk = false;
				if( !InterceptDllCall( hRet, "User32.dll", "ReleaseDC", (DWORD) &HlaeBcClt_ReleaseDC) ) bIcepOk = false;

				if( !bIcepOk ) MessageBox(0,"One or more interceptions failed","MDT_ERROR",MB_OK|MB_ICONHAND);

				HandleForceRes();

				// we have to manually intercept things windows already loaded with the LoadLibrary call:

/*				// replace DirectDraw:
				FARPROC old_dd = GetProcAddress( GetModuleHandle("ddraw.dll"), "DirectDrawCreate" );
				if( old_dd )
				{
					FARPROC new_dd = Hook_DirectDrawCreate(old_dd);
					if( !InterceptDllCall( hRet, "ddraw.dll", "DirectDrawCreate", (DWORD) new_dd) ) bIcepOk = false;
					
				} else {
					MessageBox(0,"Querying DirectDraw failed.","MDT_ERROR",MB_OK|MB_ICONHAND);
					bIcepOk = false;
				}
*/

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
#ifdef MDT_DEBUG
			MessageBox(0,"DllMain - DLL_PROCESS_ATTACH","MDT_DEBUG",MB_OK|MB_ICONINFORMATION);
#endif
			JsStartUp();

			// Intercept GetProcAddress:
			//pGetProcAddress = (FARPROC(WINAPI *)(HMODULE, LPCSTR)) InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "GetProcAddress", (DWORD) &newGetProcAddress);

			// Intercept LoadLibraryA:
			p_LoadLibraryA = (HMODULE(WINAPI *)( LPCSTR )) InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
			if( !p_LoadLibraryA ) MessageBox(0,"Base interception failed","MDT_ERROR",MB_OK|MB_ICONHAND);

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
