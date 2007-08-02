/*
	Mirv Demo Tool

*/

#ifndef MDT_DEBUG
#define MDT_DEBUG
#endif

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"GLu32.lib")
#pragma comment(lib,"GLaux.lib")

#include <windows.h>
#include <winbase.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "com_model.h"
#include "r_studioint.h"
#include "pm_defs.h"
#include "cvardef.h"
#include "entity_types.h"

#include "detours.h"
#include "filming.h"
#include "aiming.h"
#include "zooming.h"
#include "cmdregister.h"

#include "mdt_gltools.h" // we want g_Mdt_GlTools for having tools to force Buffers and Stuff like that

#include <map>
#include <list>

extern Filming g_Filming;
extern Aiming g_Aiming;
extern Zooming g_Zooming;

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

// Todo: Ini files for these?
cl_enginefuncs_s* pEngfuncs		= (cl_enginefuncs_s*)	0x01EA0A08;
engine_studio_api_s* pEngStudio	= (engine_studio_api_s*)0x01EBC978;
playermove_s* ppmove			= (playermove_s*)		0x02D590A0;

int		g_nViewports = 0;
bool	g_bIsSucceedingViewport = false;

//
//  Cvars
//

REGISTER_CVAR(disableautodirector, "0", 0);
REGISTER_CVAR(fixforcehltv, "1", 0);

//
// Commands
//

REGISTER_CMD_FUNC(whereami)
{
	float angles[3];
	pEngfuncs->GetViewAngles(angles);
	pEngfuncs->Con_Printf("Location: %fx %fy %fz\nAngles: %fx %fy %fz\n", ppmove->origin.x, ppmove->origin.y, ppmove->origin.z, angles[0], angles[1], angles[2]);
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
		pEngfuncs->Con_Printf("Useage: " DEBUG_PREFIX "forcebuffers <readbuffer_type> <drawbuffer_type\n");

		const char* cCurReadBuf = cBType_AppDecides; // when forcing is off that means the app decides
		const char* cCurDrawBuf = cBType_AppDecides; // .

		if (g_Mdt_GlTools.m_bForceReadBuff) cCurReadBuf = g_Mdt_GlTools.GetReadBufferStr();
		if (g_Mdt_GlTools.m_bForceDrawBuff) cCurDrawBuf = g_Mdt_GlTools.GetDrawBufferStr();

		pEngfuncs->Con_Printf("Current: " DEBUG_PREFIX "forcebuffers %s %s\n",cCurReadBuf,cCurDrawBuf);
		pEngfuncs->Con_Printf("Available Types: ");

		for (int i=0;i<SIZE_Mdt_GlTools_GlBuffs;i++)
		{
			const char* cBuffType=cMdt_GlTools_GlBuffStrings[i];
			if (i) pEngfuncs->Con_Printf(", %s",cBuffType); // is not first so add comma
			else pEngfuncs->Con_Printf("%s",cBuffType); // first
		}

		pEngfuncs->Con_Printf("\n");
		return;
	}
}

REGISTER_DEBUGCVAR(deltatime, "1.0", 0);

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

//
//	OpenGl Hooking
//

void APIENTRY my_glBegin(GLenum mode)
{
	if (g_Filming.doWireframe(mode) == Filming::DR_HIDE)
		return;

	if (!g_Filming.isFilming())
	{
		glBegin(mode);
		return;
	}

	Filming::DRAW_RESULT res = g_Filming.shouldDraw(mode);

	if (res == Filming::DR_HIDE)
		return;

	else if (res == Filming::DR_MASK)
		glColorMask(FALSE, FALSE, FALSE, TRUE);

	else
		glColorMask(TRUE, TRUE, TRUE, TRUE);

	glBegin(mode);
}

void APIENTRY my_glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	static bool bFirstRun = true;

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

		bFirstRun = false;
	}

	// Only on the first viewport
	if (g_nViewports == 0)
	{
		g_Filming.setScreenSize(width, height);

		// Make sure we can see the local player if dem_forcehltv is on
		// dem_forcehtlv is not a cvar, so don't bother checking
		if (fixforcehltv->value != 0.0f && pEngfuncs->IsSpectateOnly() && ppmove->iuser1 != 4)
			DrawActivePlayers();

		// Always get rid of auto_director
		if (disableautodirector->value != 0.0f)
			pEngfuncs->Cvar_SetValue("spec_autodirector", 0.0f);

		// This is called whether we're zooming or not
		g_Zooming.handleZoom();
			
		if (g_Filming.isFilming())
#ifdef MDT_DEBUG
		{
			pEngfuncs->Con_Printf("filming, glViewPort is %dx%d\n",width,height);
#endif
			g_Filming.recordBuffers();
#ifdef MDT_DEBUG
		}
#endif
		if (g_Aiming.isAiming())
			g_Aiming.aim();
	}

	g_nViewports = (g_nViewports + 1) % 5;

	g_Zooming.adjustViewportParams(x, y, width, height);
	glViewport(x, y, width, height);

}



void APIENTRY my_glClear(GLbitfield mask)
{
	if (!g_Filming.checkClear(mask))
		return;

	glClear(mask);
}

void APIENTRY my_glMatrixMode(GLenum mode)
{
	g_bIsSucceedingViewport = false;
	glMatrixMode(mode);
}

void APIENTRY my_glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	// This is to stop the viewports from drifting out of sync
	// The viewport followed by a frustum (rather than a matrix mode) should be the
	// first one after the viewport that we use.
	if (g_bIsSucceedingViewport)
		g_nViewports = 1;

	g_bIsSucceedingViewport = false;

	g_Zooming.adjustFrustumParams(left, right, top, bottom);
	glFrustum(left, right, bottom, top, zNear, zFar);
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
			return pOldFunction;
		}
		pThunk++;
	}
	return NULL;
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
		if (!lstrcmp(lpProcName, "glViewport"))
			return (FARPROC) &my_glViewport;
		if (!lstrcmp(lpProcName, "glClear"))
			return (FARPROC) &my_glClear;
		if (!lstrcmp(lpProcName, "glFrustum"))
			return (FARPROC) &my_glFrustum;
		if (!lstrcmp(lpProcName, "glMatrixMode"))
			return (FARPROC) &my_glMatrixMode;
	}

	return nResult;
}

bool WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason) 
	{ 
		case DLL_PROCESS_ATTACH:
		{
			pGetProcAddress = (FARPROC(WINAPI *)(HMODULE, LPCSTR)) InterceptDllCall(GetModuleHandle(NULL), "Kernel32.dll", "GetProcAddress", (DWORD) &newGetProcAddress);
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
