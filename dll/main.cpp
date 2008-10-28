/*
	Mirv Demo Tool

*/

#include "mdt_debug.h"

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"GLu32.lib")
#pragma comment(lib,"GLaux.lib")

#include <windows.h>
#include <winbase.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

#include <wrect.h>
#include <cl_dll.h>
#include <cdll_int.h>
#include <r_efx.h>
#include <com_model.h>
#include <r_studioint.h>
#include <pm_defs.h>
#include <cvardef.h>
#include <entity_types.h>

#include "detours.h"
#include "filming.h"
#include "aiming.h"
#include "zooming.h"
#include "cmdregister.h"
#include "ui.h"

#include "supportrender.h" // off-screen recording support

#include "dd_hook.h" // we have to call functions (inGetProcAddress) from here in order to init the hook
#include "dsound_hook.h"

#include "hl_addresses.h" // address definitions
#include "config_mdtdll.h" // used temporary to load i.e. addresses

#include "newsky.h"
#include "cmd_tools.h"

#include "basecomClient.h"

#include <map>
#include <list>

extern Filming g_Filming;
extern Aiming g_Aiming;
extern Zooming g_Zooming;
extern CHlaeCmdTools g_CmdTools;

CHlaeSupportRender *g_pSupportRender = NULL; // inited in basecomClient.cpp

extern const char *pszFileVersion;

typedef std::list <Void_func_t> VoidFuncList;
VoidFuncList &GetCvarList()
{
	static VoidFuncList CvarList;
	return CvarList;
}
VoidFuncList &GetCmdList()
{
	static VoidFuncList CmdList;
	return CmdList;
}

void CvarRegister(Void_func_t func) { GetCvarList().push_front(func); }
void CmdRegister(Void_func_t func) { GetCmdList().push_front(func); }

// Various H-L Engine interface (super) Globals:
cl_enginefuncs_s* pEngfuncs		= (cl_enginefuncs_s*)	HL_ADDR_CL_ENGINEFUNCS_S;
engine_studio_api_s* pEngStudio	= (engine_studio_api_s*)HL_ADDR_ENGINE_STUDIO_API_S;
playermove_s* ppmove			= (playermove_s*)		HL_ADDR_PLAYERMOVE_S;

int		g_nViewports = 0;
bool	g_bIsSucceedingViewport = false;

#define MDT_MAX_PATH_BYTES 1025
#define MDT_CFG_FILE "mdt_addresses.ini"
#define MDT_CFG_FILE_SLEN 15
#define DLL_NAME	"Mirv Demo Tool.dll"

HMODULE g_hMDTDLL=NULL; // handle to our self
static char pg_MDTpath[MDT_MAX_PATH_BYTES];
static char pg_MDTcfgfile[MDT_MAX_PATH_BYTES+MDT_CFG_FILE_SLEN];

//
//  Cvars
//

REGISTER_CVAR(fixforcehltv, "0", 0); // modified by Hook_dem_forcehltv
REGISTER_CVAR(disableautodirector, "0", 0);

REGISTER_DEBUGCVAR(gl_noclear, "0", 0);

//
// Commands
//

// i.e. TFC may trigger -toggle in unwanted situations, i.e.
// when the toggle is bound to any key and the user resumes into the game
REGISTER_CMD_FUNC_BEGIN(debug_toggletest)
{
	pEngfuncs->Con_Printf("+toggle / CALLED BEGIN\n");
}
REGISTER_CMD_FUNC_END(debug_toggletest)
{
	pEngfuncs->Con_Printf("-toggle / CALLED END\n");
}

REGISTER_CMD_FUNC(whereami)
{
	float angles[3];
	pEngfuncs->GetViewAngles(angles);
	pEngfuncs->Con_Printf("Location: %fx %fy %fz\nAngles: %fx %fy %fz\n", ppmove->origin.x, ppmove->origin.y, ppmove->origin.z, angles[0], angles[1], angles[2]);
}

void PrintDebugPlayerInfo(cl_entity_s *pl,int itrueindex)
{
	static hud_player_info_t m_hpinfo;

	memset(&m_hpinfo,0,sizeof(hud_player_info_t));
	pEngfuncs->pfnGetPlayerInfo(pl->index,&m_hpinfo);
	pEngfuncs->Con_Printf("%i (%s): %i, %s, %s, %i, %i, %i, %i, %i, %i\n",itrueindex,(pl->curstate.effects & EF_NODRAW) ? "y" : "n",pl->index,m_hpinfo.name,m_hpinfo.model,m_hpinfo.ping,m_hpinfo.packetloss,m_hpinfo.topcolor,m_hpinfo.bottomcolor,m_hpinfo.spectator,m_hpinfo.thisplayer);
}

REGISTER_DEBUGCMD_FUNC(listplayers)
{
	bool bLocalListed=false;
	int iLocalIndex=-1;
	cl_entity_s *plocal = pEngfuncs->GetLocalPlayer();

	pEngfuncs->Con_Printf("Listing Players (max: %i)\ntrue index (EF_NODRAW): index, name, model, ping, packetloss, topcolor, bottomcolor, spectator, thisplayer\n",pEngfuncs->GetMaxClients());
	for (int i = 0; i <= pEngfuncs->GetMaxClients(); i++)
	{
		cl_entity_t *e = pEngfuncs->GetEntityByIndex(i);
		if (e && e->player)
		{
			PrintDebugPlayerInfo(e,i);
			//if (e->index==plocal->index)
			if (e == plocal)
			{
				bLocalListed=true;
				iLocalIndex=i;//e->index;
			}
		} else if (e == plocal) iLocalIndex=i;
	}

	if (bLocalListed)
	{
		pEngfuncs->Con_Printf("The local player is index %i.\n",iLocalIndex);
	} else {
		pEngfuncs->Con_Printf("The local player is hidden (not flagged as player):\n",iLocalIndex);
		PrintDebugPlayerInfo(plocal,iLocalIndex);
	}

}

// _mirv_info - Print some informations into the console that might be usefull. when people want to report problems they should copy the console output of the command.
REGISTER_DEBUGCMD_FUNC(info)
{
	pEngfuncs->Con_Printf(">>>> >>>> >>>> >>>>\n");
	pEngfuncs->Con_Printf("MDT_DLL_VERSION: v%s (%s)\n", pszFileVersion, __DATE__);
	pEngfuncs->Con_Printf("GL_VENDOR: %s\n",glGetString(GL_VENDOR));
	pEngfuncs->Con_Printf("GL_RENDERER: %s\n",glGetString(GL_RENDERER));
	pEngfuncs->Con_Printf("GL_VERSION: %s\n",glGetString(GL_VERSION));
	pEngfuncs->Con_Printf("GL_EXTENSIONS: %s\n",glGetString(GL_EXTENSIONS));
	pEngfuncs->Con_Printf("<<<< <<<< <<<< <<<<\n");
}

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

xcommand_t g_Old_dem_forcehltv = NULL;

void Hook_dem_forcehltv(void)
{
	char *ptmp="";
	if (pEngfuncs->Cmd_Argc()>=1) ptmp=pEngfuncs->Cmd_Argv(1);
	pEngfuncs->Cvar_SetValue(PREFIX "fixforcehltv",atof(ptmp));
	g_Old_dem_forcehltv();
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

void DrawActivePlayers()
{
	bool bNotInEye = ppmove->iuser1 != 4;
	int iwatched = ppmove->iuser2;

	for (int i = 0; i <= pEngfuncs->GetMaxClients(); i++)
	{
		cl_entity_t *e = pEngfuncs->GetEntityByIndex(i);

		if(!e) continue;

		if (e && e->player && e->model && !(e->curstate.effects & EF_NODRAW) && (bNotInEye || e->index != iwatched))
		{
			float flDeltaTime = fabs(pEngfuncs->GetClientTime() - e->curstate.msg_time);

			if (flDeltaTime < deltatime->value)
				pEngfuncs->CL_CreateVisibleEntity(ET_PLAYER, e);

		}
	}
}

//
//	OpenGl Hooking
//

struct glBegin_saved_s {
	bool restore;
	GLboolean b_GL_DEPTH_TEST;
	GLint i_GL_DEPTH_FUNC;
	GLboolean b_ColorWriteMask[4];
} g_glBegin_saved;

void APIENTRY my_glBegin(GLenum mode)
{
	g_glBegin_saved.restore=false;

	g_NewSky.DetectAndProcessSky(mode);

	if (g_Filming.doWireframe(mode) == Filming::DR_HIDE)
		return;

	g_Filming.DoWorldFxBegin(mode); // WH fx

	g_Filming.DoWorldFx2(mode); // lightmap fx

	if (!g_Filming.isFilming())
	{
		glBegin(mode);
		return;
	}

	Filming::DRAW_RESULT res = g_Filming.shouldDraw(mode);

	if (res == Filming::DR_HIDE)
		return;

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

	glBegin(mode);
}

void APIENTRY my_glEnd(void)
{
	glEnd();

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

void retriveMDTConfigFile(char *frompath,char *tocfg)
// Warning: Doesn't check buffers.
{
	char* spos=strrchr(frompath,'\\');
	tocfg[0]=NULL;

	if (spos!=NULL){
		memcpy(tocfg,frompath,spos-frompath+1);
		tocfg+=spos-frompath+1;
	}

	strcpy(tocfg,MDT_CFG_FILE);
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

		// Register the commands
		std::list<Void_func_t>::iterator i = GetCmdList().begin();
		while (i != GetCmdList().end())
			(*i++)();

		// Register the cvars
		i = GetCvarList().begin();
		while (i != GetCvarList().end())
			(*i++)();

		pEngfuncs->Con_Printf("Mirv Demo Tool v%s (%s) Loaded\nBy Mirvin_Monkey 02/05/2004\n\n", pszFileVersion, __DATE__);

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

		// Make sure we can see the local player if dem_forcehltv is on
		// dem_forcehtlv is not a cvar, so don't bother checking
		// however mdt tries to keep track a bit of dem_forcehltv
		if ( fixforcehltv->value != 0.0f && pEngfuncs->IsSpectateOnly() )
			DrawActivePlayers();

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

bool Mdt_LoadAddressConfig(HMODULE hHwDll)
{
	cConfig_mdtdll* pg_Config_mdtdll;
	
	// the very first thing we have to do is to set the addresses from the config, cuz really much stuff relays on that:
	g_hMDTDLL = GetModuleHandle(DLL_NAME);
	pg_MDTpath[0]=NULL;
	if (g_hMDTDLL) GetModuleFileName(g_hMDTDLL,pg_MDTpath,MDT_MAX_PATH_BYTES-1);

	retriveMDTConfigFile(pg_MDTpath,pg_MDTcfgfile);
	// not allowed before addresses are validied pEngfuncs->Con_Printf("Path: %s | %s \n",pg_MDTpath,pg_MDTcfgfile);

	bool bCfgres = false;

	if (g_hMDTDLL)
	{
		pg_Config_mdtdll = new cConfig_mdtdll(pg_MDTcfgfile);
		bCfgres = pg_Config_mdtdll->LoadAndApplyAddresses( hHwDll );

		if (!bCfgres) MessageBox(0,"mdt_addresses.ini syntax or semantics were invalid.\nTrying to continue ...","MDT_WARNING",MB_OK|MB_ICONEXCLAMATION);

		// update unregistered local copies manually:
		pEngfuncs		= (cl_enginefuncs_s*)	HL_ADDR_CL_ENGINEFUNCS_S;
		pEngStudio	= (engine_studio_api_s*)HL_ADDR_ENGINE_STUDIO_API_S;
		ppmove			= (playermove_s*)		HL_ADDR_PLAYERMOVE_S;


	} else MessageBox(0,"Could not locate mdt_addresses.ini.","MDT_ERROR",MB_OK|MB_ICONHAND);

	
	delete pg_Config_mdtdll;

	return bCfgres;
}

#pragma warning(disable: 4312)
#pragma warning(disable: 4311)
#define MakePtr(cast, ptr, addValue) (cast)((DWORD)(ptr) + (DWORD)(addValue))

void *InterceptDllCall(HMODULE hModule, char *szDllName, char *szFunctionName, DWORD pNewFunction)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	DWORD dwOldProtect;
	DWORD dwOldProtect2;
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
			VirtualProtect((void *) &pThunk->u1.Function, sizeof(DWORD), PAGE_EXECUTE_READWRITE, &dwOldProtect);
			pThunk->u1.Function = (DWORD) pNewFunction;
			VirtualProtect((void *) &pThunk->u1.Function, sizeof(DWORD), dwOldProtect, &dwOldProtect2);

			return pOldFunction;
		}
		pThunk++;
	}

	return NULL;
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

			if( hRet )
			{
				// load addresses form config:
				Mdt_LoadAddressConfig( hRet );

				bool bIcepOk = true;

				// Intercept GetProcAddress:
				if( !InterceptDllCall( hRet, "Kernel32.dll", "GetProcAddress", (DWORD) &newGetProcAddress) ) bIcepOk = false;

				// we have to manually intercept things windows already loaded with the LoadLibrary call:

				// replace DirectDraw:
				FARPROC old_dd = GetProcAddress( GetModuleHandle("ddraw.dll"), "DirectDrawCreate" );
				if( old_dd )
				{
					FARPROC new_dd = Hook_DirectDrawCreate(old_dd);
					if( !InterceptDllCall( hRet, "ddraw.dll", "DirectDrawCreate", (DWORD) new_dd) ) bIcepOk = false;
					
				} else {
					MessageBox(0,"Querying DirectDraw failed.","MDT_ERROR",MB_OK|MB_ICONHAND);
					bIcepOk = false;
				}

				// WindowAPI related:
				if( !InterceptDllCall( hRet, "User32.dll", "CreateWindowExA", (DWORD) &HlaeBcClt_CreateWindowExA) ) bIcepOk = false;
				if( !InterceptDllCall( hRet, "User32.dll", "DestroyWindow", (DWORD) &HlaeBcClt_DestroyWindow) ) bIcepOk = false;
				if( !InterceptDllCall( hRet, "User32.dll", "RegisterClassA", (DWORD) &HlaeBcClt_RegisterClassA) ) bIcepOk = false;
				if( !InterceptDllCall( hRet, "User32.dll", "SetWindowPos", (DWORD) &HlaeBcClt_SetWindowPos) ) bIcepOk = false;
				if( !InterceptDllCall( hRet, "gdi32.dll", "ChoosePixelFormat", (DWORD) &HlaeBcClt_ChoosePixelFormat) ) bIcepOk = false;
				if( !InterceptDllCall( hRet, "User32.dll", "ReleaseDC", (DWORD) &HlaeBcClt_ReleaseDC) ) bIcepOk = false;

				if( !bIcepOk ) MessageBox(0,"One or more interceptions failed","MDT_ERROR",MB_OK|MB_ICONHAND);
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
			// Intercept GetProcAddress:
			//pGetProcAddress = (FARPROC(WINAPI *)(HMODULE, LPCSTR)) InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "GetProcAddress", (DWORD) &newGetProcAddress);

			// Intercept LoadLibraryA:
			p_LoadLibraryA = (HMODULE(WINAPI *)( LPCSTR )) InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "LoadLibraryA", (DWORD) &new_LoadLibraryA);
			if( !p_LoadLibraryA ) MessageBox(0,"Base interception failed","MDT_ERROR",MB_OK|MB_ICONHAND);

			break;
		}
		case DLL_PROCESS_DETACH:
		{
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
