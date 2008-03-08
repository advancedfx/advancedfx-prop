/*
	Mirv Demo Tool

*/

#define MDT_COMPILE_FOR_GUI
// Comment this out in case of problems.
// This enables the new code that will check for the HLAE GUI etc..
// Normally you can leave this defined, but since the public versions
// don't have the HLAE Gui included anyways, it's save to comment it out.

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

#include "mdt_gltools.h" // we want g_Mdt_GlTools for having tools to force Buffers and Stuff like that
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


extern UI *gui;

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
bool	g_bMenu = false;

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
REGISTER_DEBUGCVAR(movie_oldmatte, "0", 0);

//
// Commands
//

#if 0
REGISTER_CMD_FUNC(menu)
{
	g_bMenu = !g_bMenu;

	// If we leave the gui then reset the mouse position so view doesn't jump
	if (!g_bMenu)
		SetCursorPos(pEngfuncs->GetWindowCenterX(), pEngfuncs->GetWindowCenterY());
}
#endif

void ToggleMenu()
{
	g_bMenu = !g_bMenu;

	// If we leave the gui then reset the mouse position so view doesn't jump
	if (!g_bMenu)
		SetCursorPos(pEngfuncs->GetWindowCenterX(), pEngfuncs->GetWindowCenterY());
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
	pEngfuncs->Con_Printf("%i (%s): %i, %s, %s, %i, %i, %i, %i, %i, %i\n",pl->index,(pl->curstate.effects & EF_NODRAW) ? "y" : "n",itrueindex,m_hpinfo.name,m_hpinfo.model,m_hpinfo.ping,m_hpinfo.packetloss,m_hpinfo.topcolor,m_hpinfo.bottomcolor,m_hpinfo.spectator,m_hpinfo.thisplayer);
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

REGISTER_DEBUGCMD_FUNC(forcebuffers)
{
	const char* cBType_AppDecides = "APP_DECIDES";
	
	if (pEngfuncs->Cmd_Argc() != 3)
	{
		// user didn't supply 2 arguments, so give him some info about the command:
		pEngfuncs->Con_Printf("Useage: " DEBUG_PREFIX "forcebuffers <readbuffer_type> <drawbuffer_type\n");

		const char* cCurReadBuf = cBType_AppDecides; // when forcing is off that means the app decides
		const char* cCurDrawBuf = cBType_AppDecides; // .

		if (g_Mdt_GlTools.m_bForceReadBuff) cCurReadBuf = g_Mdt_GlTools.GetReadBufferStr();
		if (g_Mdt_GlTools.m_bForceDrawBuff) cCurDrawBuf = g_Mdt_GlTools.GetDrawBufferStr();

		pEngfuncs->Con_Printf("Current: " DEBUG_PREFIX "forcebuffers %s %s\n",cCurReadBuf,cCurDrawBuf);
		pEngfuncs->Con_Printf("Available Types: %s",cBType_AppDecides); // also add APP_DECIDES

		for (int i=0;i<SIZE_Mdt_GlTools_GlBuffs;i++)
		{
			const char* cBuffType=cMdt_GlTools_GlBuffStrings[i];
			pEngfuncs->Con_Printf(", %s",cBuffType); // is not first so add comma

		}

		pEngfuncs->Con_Printf("\n");
	} else {
		// user supplied 2 argument's try to set it

		const char* cReadBuffStr = pEngfuncs->Cmd_Argv(1);
		const char* cDrawBuffStr = pEngfuncs->Cmd_Argv(2);

		// the following code checks for each buffer if the user wants either let app decide (then it turns force off) or if he wants to set a specific type (on success it turns force on and otherwise prints an error):
		// ReadBuffer (for reading color buffers from GL):
		if (!stricmp(cBType_AppDecides,cReadBuffStr)) g_Mdt_GlTools.m_bForceReadBuff=false;
		else if (g_Mdt_GlTools.SetReadBufferFromStr(cReadBuffStr))  g_Mdt_GlTools.m_bForceReadBuff=true;
		else pEngfuncs->Con_Printf("Error: Failed to set ReadBuffer, supplied buffer type not valid.\n");
		// DrawBuffer (for definig the target GL color buffer for pixel writing functions etc.):
		if (!stricmp(cBType_AppDecides,cDrawBuffStr)) g_Mdt_GlTools.m_bForceDrawBuff=false;
		else if (g_Mdt_GlTools.SetDrawBufferFromStr(cDrawBuffStr))  g_Mdt_GlTools.m_bForceDrawBuff=true;
		else pEngfuncs->Con_Printf("Error: Failed to set DrawBuffer, supplied buffer type not valid.\n");
	}



	return;
}

REGISTER_DEBUGCVAR(deltatime, "1.0", 0);

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

// >> fixforcehtlv stuff:

/*
typedef void ( *Cvar_SetValue_t )( char *cvar, float value );

Cvar_SetValue_t detoured_Cvar_SetValue = NULL;

void Hook_Cvar_SetValue( char *cvar, float value )
{
	if (!strcmp("maxclients",cvar)) pEngfuncs->Con_Printf(">>>Maxclients: %f\n",value);
	detoured_Cvar_SetValue(cvar,value);
}

void Install_Hook_Cvar_SetValue()
{
	if (!detoured_Cvar_SetValue)
		detoured_Cvar_SetValue = (Cvar_SetValue_t)DetourApply((BYTE *)(pEngfuncs->Cvar_SetValue), (BYTE *)Hook_Cvar_SetValue,0x07);
}

int g_map_localplayer_to = -1;

typedef cl_entity_s *(* GetLocalPlayer_t)( void );

GetLocalPlayer_t detoured_GetLocalPlayer = NULL;
*/
void DrawActivePlayers()
{
	for (int i = 0; i <= pEngfuncs->GetMaxClients(); i++)
	{
		cl_entity_t *e = pEngfuncs->GetEntityByIndex(i);

		if (e && e->player && e->model && !(e->curstate.effects & EF_NODRAW))
		{
			float flDeltaTime = fabs(pEngfuncs->GetClientTime() - e->curstate.msg_time);

			if (flDeltaTime < deltatime->value)
				pEngfuncs->CL_CreateVisibleEntity(ET_PLAYER, e);

		}
	}
}
/*
cl_entity_s *g_Hook_GetLocalPlayer( void )
{
	//pEngfuncs->Con_DPrintf("g_hook_GetLocalPlayer called.\n");
	static hud_player_info_t m_hpinfo;

	if( fixforcehltv->value != 0.0f && pEngfuncs->IsSpectateOnly() )
	{
		bool bReMap=false;
		if  (g_map_localplayer_to == -1)
		{
			// check if there is already a correctly mapped player:
			cl_entity_s *pe = detoured_GetLocalPlayer();

			if (!pe || pe->player)
				bReMap=true;
			else
			{
				if (pe->index==0)
				{
					bReMap=true;
				} else {
					pEngfuncs->Con_DPrintf("HLAE: g_Hook_GetLocalPlayer: using native map: %i.\n",pe->index);
					g_map_localplayer_to=pe->index;
				}
			}
		} else {
			// check if the current mapping is still correct:
			cl_entity_s *e=pEngfuncs->GetEntityByIndex(g_map_localplayer_to);

			if (e) //&& e->player && e->model && e->model->name && e->model->name[0]!=0)
			{
				memset(&m_hpinfo,0x0,sizeof(hud_player_info_t));
				pEngfuncs->pfnGetPlayerInfo(e->index,&m_hpinfo);
				if (m_hpinfo.model!=NULL) bReMap=true;
			}
		}

		if (bReMap)
		{
			//pEngfuncs->Con_DPrintf("HLAE: g_Hook_GetLocalPlayer: doing remap.\n");
			cl_entity_t *e;
			for (g_map_localplayer_to = 1; g_map_localplayer_to <= pEngfuncs->GetMaxClients(); g_map_localplayer_to++)
			{
				e = pEngfuncs->GetEntityByIndex(g_map_localplayer_to);

				if (!e) break;
				else
				{
					memset(&m_hpinfo,0x0,sizeof(hud_player_info_t));
					pEngfuncs->pfnGetPlayerInfo(e->index,&m_hpinfo);
					if (m_hpinfo.model==NULL) break;
				}
			}
			pEngfuncs->CL_CreateVisibleEntity(ET_NORMAL,e);
			pEngfuncs->Con_DPrintf("HLAE: g_Hook_GetLocalPlayer: did remap: %i.\n",g_map_localplayer_to);
		}

		return pEngfuncs->GetEntityByIndex(g_map_localplayer_to);
	} else {
		g_map_localplayer_to = -1;
		return detoured_GetLocalPlayer();
	}
}

void Install_Hook_GetLocalPlayer()
{
	if (!detoured_GetLocalPlayer)
	{
		// we need an backup of the original function since we still want to call it in our hook
		detoured_GetLocalPlayer = (GetLocalPlayer_t)DetourApply((BYTE *)(pEngfuncs->GetLocalPlayer), (BYTE *)g_Hook_GetLocalPlayer,0x06);
	};
}

void DrawMySelf_InForceHltv()
// doesn't work yet
{
	int i=-1;

	if (detoured_GetLocalPlayer)
	{
		cl_entity_s *pe=detoured_GetLocalPlayer(); // dem_forcehltv might trick us!
		if (pe) i = pe->index;
		else return;
	}
	else
	{
		pEngfuncs->Con_Printf("HLAE error: in DrawMySelf_InForceHltv\n");
		return;
	}
	

	cl_entity_t *e = pEngfuncs->GetEntityByIndex(i);

	if (e && e->player && e->model && !(e->curstate.effects & EF_NODRAW))
	{
		float flDeltaTime = fabs(pEngfuncs->GetClientTime() - e->curstate.msg_time);

		if (flDeltaTime < deltatime->value)
			pEngfuncs->CL_CreateVisibleEntity(ET_PLAYER, e);
	}
}
*/
// <<

bool InMenu()
{
	// TODO - CHECK THAT WE'RE NOT IN MAIN MENU SCREEN
	return g_bMenu;
}

//
//
//

WNDPROC pWndProc;

LRESULT APIENTRY my_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int old_wParam = 0;

	if (InMenu())
	{
		switch (uMsg)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
			if (wParam == VK_ESCAPE)
			{
				ToggleMenu();
				return 0;
			}
			return 0;

		case WM_LBUTTONDOWN:
			gui->MouseDown(MOUSE_BUTTON1);
			return 0;

		case WM_LBUTTONUP:
			gui->MouseUp(MOUSE_BUTTON1);
			return 0;

		case WM_RBUTTONDOWN:
			gui->MouseDown(MOUSE_BUTTON2);
			return 0;

		case WM_RBUTTONUP:
			gui->MouseUp(MOUSE_BUTTON2);
			return 0;

		case WM_MOUSEMOVE:
			if ((old_wParam & MK_LBUTTON) ^ (wParam & MK_LBUTTON))
				(wParam & MK_LBUTTON) ? gui->MouseDown(MOUSE_BUTTON1) : gui->MouseUp(MOUSE_BUTTON1);
			if ((old_wParam & MK_RBUTTON) ^ (wParam & MK_RBUTTON))
				(wParam & MK_LBUTTON) ? gui->MouseDown(MOUSE_BUTTON1) : gui->MouseUp(MOUSE_BUTTON1);

			old_wParam = wParam;
			return 0;
		}
	}

	return CallWindowProc(pWndProc, hwnd, uMsg, wParam, lParam);
}

//
//	OpenGl Hooking
//

struct glBegin_saved_s {
	bool restore;
	//GLclampf fColorv [4];
	GLint i_GL_TEXTURE_ENV_MODE;
	GLint i_GL_TEXTURE_BINDING_2D;

	// those are accessed idenpendently from restore:
} g_glBegin_saved;

union my_glMatteTexture_t {
	struct {
		GLbyte px1[4];
		GLbyte px2[4];
		GLbyte px3[4];
		GLbyte px4[4];
	} px;
	GLbyte data[16];
} my_glMatteTexture;

GLuint my_tex_dude;
bool bMatteTextureBound=false;

void doGenMatteTex()
{
	my_glMatteTexture.px.px1[0]=255*g_Filming.m_MatteColour[0];
	my_glMatteTexture.px.px1[1]=255*g_Filming.m_MatteColour[1];
	my_glMatteTexture.px.px1[2]=255*g_Filming.m_MatteColour[2];
	my_glMatteTexture.px.px1[3]=255;

	memcpy(my_glMatteTexture.px.px2,my_glMatteTexture.px.px1,4);
	memcpy(my_glMatteTexture.px.px3,my_glMatteTexture.px.px1,4);
	memcpy(my_glMatteTexture.px.px4,my_glMatteTexture.px.px1,4);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, my_glMatteTexture.data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

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
		if (movie_oldmatte->value==1.0f)
			glColorMask(FALSE, FALSE, FALSE, TRUE); // this is illegal, since you can't asume a specific drawing order of polygons
		else
		{
			// save some old environment properties we will overwrite:
			glGetIntegerv(GL_TEXTURE_BINDING_2D,&g_glBegin_saved.i_GL_TEXTURE_BINDING_2D);
			glGetTexEnviv(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,&g_glBegin_saved.i_GL_TEXTURE_ENV_MODE);

			if (!bMatteTextureBound)
			{
				glGenTextures(1,&my_tex_dude);
				glBindTexture(GL_TEXTURE_2D,my_tex_dude);

				doGenMatteTex();

				bMatteTextureBound=true;

			} else {
				glBindTexture(GL_TEXTURE_2D,my_tex_dude);
				if (g_Filming.bRequestingMatteTextUpdate)
				{
					doGenMatteTex();
					g_Filming.bRequestingMatteTextUpdate = false;
				}
			}

			glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);

			glColorMask(TRUE, TRUE, TRUE, TRUE); // we need it to be drawn

			g_glBegin_saved.restore = true; // we request to restore parts of the environment after glEnd()
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
		// restore old texture mode:
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,g_glBegin_saved.i_GL_TEXTURE_ENV_MODE);
		glBindTexture(GL_TEXTURE_2D,g_glBegin_saved.i_GL_TEXTURE_BINDING_2D);
		g_glBegin_saved.restore=false;
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

	HlaeBcCl_AdjustViewPort(x,y,width,height);

#ifdef MDT_DEBUG
	if (bFirstRun)
	{
		char sztemp[50];
		_snprintf(sztemp,sizeof(sztemp),"x: %i y: %i w: %i h: %i",x,y,width,height);
		MessageBoxA(0,sztemp,"my_glViewPort - Firstrun",MB_OK|MB_ICONINFORMATION);
	}
#endif

	g_bIsSucceedingViewport = true;

	if (bFirstRun)
	{
		// Register the commands
		std::list<Void_func_t>::iterator i = GetCmdList().begin();
		while (i != GetCmdList().end())
			(*i++)();

		// Register the cvars
		i = GetCvarList().begin();
		while (i != GetCvarList().end())
			(*i++)();

		pEngfuncs->Con_Printf("Mirv Demo Tool v%s (%s) Loaded\nBy Mirvin_Monkey 02/05/2004\n\n", pszFileVersion, __DATE__);

		gui->Initialise();

		screeninfo.iSize = sizeof(SCREENINFO);
		pEngfuncs->pfnGetScreenInfo(&screeninfo);
		pEngfuncs->Con_DPrintf("ScreenRes: %dx%d\n", screeninfo.iWidth, screeninfo.iHeight);

		g_Filming.setScreenSize(screeninfo.iWidth,screeninfo.iHeight);

		// Init the CmdTools (might be a bit later here for hooking some funcs):
		g_CmdTools.Init(pEngfuncs);
		pEngfuncs->Con_DPrintf("CommandTree at: 0x%08x\n", g_CmdTools.GiveCommandTreePtr());

		// install some hooks:
		if (!(g_Old_dem_forcehltv = g_CmdTools.HookCommand("dem_forcehltv",Hook_dem_forcehltv))) pEngfuncs->Con_Printf("HLAE warning: Failed hooking dem_forcehltv");
		if (!(g_CmdTools.HookCommand("startmovie",Hook_startmovie))) pEngfuncs->Con_Printf("HLAE warning: Failed hooking startmovie");
		if (!(g_CmdTools.HookCommand("endmovie",Hook_endmovie))) pEngfuncs->Con_Printf("HLAE warning: Failed hooking endmovie");

		//Install_Hook_Cvar_SetValue();
		//Install_Hook_GetLocalPlayer();

		bFirstRun = false;
	}


	// Only on the first viewport
	if (g_nViewports == 0)
	{
		//g_Filming.setScreenSize(width, height);

		// Make sure we can see the local player if dem_forcehltv is on
		// dem_forcehtlv is not a cvar, so don't bother checking
		// however mdt tries to keep track a bit of dem_forcehltv
		if (fixforcehltv->value != 0.0f && pEngfuncs->IsSpectateOnly() && ppmove->iuser1 != 4)
		{
			// doesn't work yet: DrawMySelf_InForceHltv();
			DrawActivePlayers();
		}

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

#ifdef MDT_DEBUG
	MessageBox(0,"InterceptDllCall - starting","MDT_DEBUG",MB_OK|MB_ICONINFORMATION);
#endif

	if (!(pOldFunction = GetProcAddress(GetModuleHandle(szDllName), szFunctionName)))
		return 0;

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

#ifdef MDT_DEBUG
	MessageBox(0,"InterceptDllCall - finished as desired","MDT_DEBUG",MB_OK|MB_ICONINFORMATION);
#endif

			return pOldFunction;
		}
		pThunk++;
	}

	return NULL;
}

BOOL (APIENTRY *pwglSwapBuffers)(HDC hDC);
BOOL APIENTRY my_wglSwapBuffers(HDC hDC)
{
#ifndef MDT_COMPILE_FOR_GUI
// only install old gui hooks when not compiling for new GUI
	static bool bHaveWindowHandle = false;

	if (!bHaveWindowHandle && hDC != 0)
	{
		HWND hWnd = WindowFromDC(hDC);
		pWndProc = (WNDPROC) SetWindowLong(hWnd, GWL_WNDPROC, (long) my_WndProc);
		bHaveWindowHandle = true;
	}
#endif

	BOOL bResWglSwapBuffers;
	bool bRecordSwapped=false;

	// Next viewport will be the first of the new frame
	g_nViewports = 0;

	if (g_Filming.isFilming())
	{
		// we are filming, force buffers and capture our image:
		
		// save current buffers:
		g_Mdt_GlTools.SaveDrawBuffer();
		g_Mdt_GlTools.SaveReadBuffer();

		// force selected buffers(if any):
		g_Mdt_GlTools.AdjustReadBuffer();
		g_Mdt_GlTools.AdjustDrawBuffer();
		
		// record the selected buffer (capture):
		bRecordSwapped = g_Filming.recordBuffers(hDC,&bResWglSwapBuffers);
	}

	// Obviously use 
	if (InMenu())
	{
		gui->Update(pEngfuncs->GetClientTime());
		gui->Render(screeninfo.iWidth, screeninfo.iHeight);
	}

	// do the switching of buffers as requersted:
	if (!bRecordSwapped) bResWglSwapBuffers = (*pwglSwapBuffers)(hDC);

	// no we have captured the image (by default from backbuffer) and display it on the front, now we can prepare the new backbuffer image if required.

	if (g_Filming.isFilming())
	{
		// we are filming, do required clearing and restore buffers:

		// carry out preparerations on the backbuffer for the next frame:
		if (g_Filming.bCustomDump()) g_Filming.clearBuffers();

		// restore saved buffers:
		g_Mdt_GlTools.AdjustDrawBuffer(g_Mdt_GlTools.m_iSavedDrawBuff,false);
		g_Mdt_GlTools.AdjustReadBuffer(g_Mdt_GlTools.m_iSavedReadBuff,false);
	}

	return bResWglSwapBuffers;
}



// Don't let HL reset the cursor position if we're in our own gui
BOOL APIENTRY my_SetCursorPos(int x, int y)
{
	if (InMenu())
		return 1;

	return SetCursorPos(x, y);
}

// Get the mouse position for our menu and tell HL that the mouse is in the centre
// of the screen (to stop player from spinning while in menu)
BOOL APIENTRY my_GetCursorPos(LPPOINT lpPoint)
{
	pEngfuncs->Con_Printf("LOBOB");
	BOOL bRet = HlaeBcCl_GetCursorPos(lpPoint);

	if (!InMenu())return bRet;

	if (!bRet)
		return FALSE;

	int iMidx = pEngfuncs->GetWindowCenterX();
	int iMidy = pEngfuncs->GetWindowCenterY();

	gui->UpdateMouse((int) lpPoint->x - (iMidx - 800 / 2),
					 (int) lpPoint->y - (iMidy - 600 / 2));

	lpPoint->x = (long) iMidx;
	lpPoint->y = (long) iMidy;
	return TRUE;
}

FARPROC (WINAPI *pGetProcAddress)(HMODULE hModule, LPCSTR lpProcName);
FARPROC WINAPI newGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	FARPROC nResult;
	nResult = GetProcAddress(hModule, lpProcName);

	if (HIWORD(lpProcName))
	{
		if (!lstrcmp(lpProcName, "GetProcAddress"))
			return (FARPROC) &newGetProcAddress;
		if (!lstrcmp(lpProcName, "glBegin"))
			return (FARPROC) &my_glBegin;
		if (!lstrcmp(lpProcName, "glEnd"))
			return (FARPROC) &my_glEnd;
		if (!lstrcmp(lpProcName, "glViewport"))
			return (FARPROC) &my_glViewport;
		if (!lstrcmp(lpProcName, "glClear"))
			return (FARPROC) &my_glClear;
		if (!lstrcmp(lpProcName, "glFrustum"))
			return (FARPROC) &my_glFrustum;
		if (!lstrcmp(lpProcName, "glBlendFunc"))
			return (FARPROC) &my_glBlendFunc;

		if (!lstrcmp(lpProcName, "GetCursorPos"))
			return (FARPROC) &my_GetCursorPos;
		if (!lstrcmp(lpProcName, "SetCursorPos"))

			return (FARPROC) &my_SetCursorPos;
		if (!lstrcmp(lpProcName, "wglSwapBuffers"))
		{
			pwglSwapBuffers = (BOOL (APIENTRY *)(HDC hDC)) nResult;
			return (FARPROC) &my_wglSwapBuffers;
		}

		if (!lstrcmp(lpProcName,"DirectDrawCreate"))
			return Hook_DirectDrawCreate(nResult); // give our hook original address and return new (it remembers the original one from it's first call, it also cares about the commandline options (if to force the res or not and does not install the hook if not needed))
		if (!lstrcmp(lpProcName,"DirectSoundCreate"))
			return Hook_DirectSoundCreate(nResult);
		//if (!lstrcmp(lpProcName,"DirectInputCreateA"))
			// imported but never called?

#ifdef MDT_COMPILE_FOR_GUI
		if (!lstrcmp(lpProcName, "CreateWindowExA"))
			return (FARPROC) &HlaeBcClt_CreateWindowExA;
		if (!lstrcmp(lpProcName, "DestroyWindow"))
			return (FARPROC) &HlaeBcClt_DestroyWindow;
		if (!lstrcmp(lpProcName, "RegisterClassA"))
			return (FARPROC) &HlaeBcClt_RegisterClassA;
		if (!lstrcmp(lpProcName, "GetDC"))
			return (FARPROC) &HlaeBcClt_GetDC;
		if (!lstrcmp(lpProcName, "ReleaseDC"))
			return (FARPROC) &HlaeBcClt_ReleaseDC;
		if (!lstrcmp(lpProcName, "SetCapture"))
			return (FARPROC) &HlaeBcClt_SetCapture;
		if (!lstrcmp(lpProcName, "ReleaseCapture"))
			return (FARPROC) &HlaeBcClt_ReleaseCapture;
#endif

	}

	return nResult;
}

bool Mdt_LoadAddressConfig()
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
		bCfgres = pg_Config_mdtdll->LoadAndApplyAddresses();

		if (!bCfgres) MessageBox(0,"mdt_addresses.ini syntax or semantics were invalid.\nTrying to continue ...","MDT_WARNING",MB_OK|MB_ICONEXCLAMATION);

		// update unregistered local copies manually:
		pEngfuncs		= (cl_enginefuncs_s*)	HL_ADDR_CL_ENGINEFUNCS_S;
		pEngStudio	= (engine_studio_api_s*)HL_ADDR_ENGINE_STUDIO_API_S;
		ppmove			= (playermove_s*)		HL_ADDR_PLAYERMOVE_S;


	} else MessageBox(0,"Could not locate mdt_addresses.ini.","MDT_ERROR",MB_OK|MB_ICONHAND);

	
	delete pg_Config_mdtdll;

	return bCfgres;
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
			pGetProcAddress = (FARPROC(WINAPI *)(HMODULE, LPCSTR)) InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "GetProcAddress", (DWORD) &newGetProcAddress);

			// load addresses form config:
			Mdt_LoadAddressConfig();
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
