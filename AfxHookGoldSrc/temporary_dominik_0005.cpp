#include "stdafx.h"

#include "windows.h" // we need access to virtualprotect etc.

#include <hlsdk.h>

#include "cmdregister.h"

#include <shared/detours.h>

#include <hooks/HookHw.h>

// client!HUD_ProcessPlayerState

typedef cl_entity_s *(* GetLocalPlayer_t)( void );

typedef void (* HUD_ProcessPlayerState_t)( struct entity_state_s *dst, const struct entity_state_s *src );

HUD_ProcessPlayerState_t detoured_HUD_ProcessPlayerState = NULL;

bool g_bInstalled_0005 = false;

REGISTER_DEBUGCVAR(fana0005_blockall, "-1", 0);
REGISTER_DEBUGCVAR(fana0005_iuser1, "-1", 0); // observer mode
REGISTER_DEBUGCVAR(fana0005_iuser2, "-1", 0); // target 1
REGISTER_DEBUGCVAR(fana0005_iuser3, "-1", 0); // target 2
REGISTER_DEBUGCVAR(fana0005_efnodraw, "-1", 0); // force EF_NODRAW effect

void hooking_HUD_ProcessPlayerState( struct entity_state_s *dst, const struct entity_state_s *src )
{

	cl_entity_t *player = pEngfuncs->GetLocalPlayer();	// Get the local player's index
	if ( dst->number == player->index )
	{
		if(fana0005_blockall->value != 1.0f)
		{
			entity_state_s ns = *src;

			if(fana0005_iuser1->value != -1) ns.iuser1 = fana0005_iuser1->value;
			if(fana0005_iuser2->value != -1) ns.iuser2 = fana0005_iuser2->value;
			if(fana0005_iuser3->value != -1) ns.iuser3 = fana0005_iuser3->value;
			if(fana0005_efnodraw->value != -1) ns.effects |= EF_NODRAW;
			detoured_HUD_ProcessPlayerState(dst,&ns);
		}
	}
	else
		detoured_HUD_ProcessPlayerState(dst,src);
}

bool Install_0005()
{
	if(g_bInstalled_0005)
		return true;

	HMODULE hm = GetModuleHandle("client.dll");
	FARPROC fp = GetProcAddress(hm,"HUD_ProcessPlayerState");

	if(hm && fp)
	{
		detoured_HUD_ProcessPlayerState = (HUD_ProcessPlayerState_t)DetourApply((BYTE *)fp, (BYTE *)hooking_HUD_ProcessPlayerState,0x06);
		g_bInstalled_0005 = true;
	}

	return g_bInstalled_0005;
}

REGISTER_DEBUGCMD_FUNC(fana0005_install)
{
	if(Install_0005())
		pEngfuncs->Con_Printf("[OK] Hook installed.\n");
	else
		pEngfuncs->Con_Printf("[ERROR] Hook installation failed.\n");
}

/////////////////


#include "hooks/user32Hooks.h"

REGISTER_DEBUGCMD_FUNC(tst_undock)
{
	UndockGameWindowForCapture();
}

REGISTER_DEBUGCMD_FUNC(tst_dock)
{
	RedockGameWindow();
}


REGISTER_DEBUGCMD_FUNC(tst_info)
{
	DWORD dw = GetWindowLongPtr(g_GameWindow, GWL_STYLE);

	pEngfuncs->Con_Printf("GWL_STYLE = 0x%08x\n", dw);
}

//////

#include "hl_addresses.h"

#include <hlsdk/multiplayer/common/ref_params.h>

#include "hooks/hw/ClientFunctions.h"

#include "hooks/HookHw.h"

typedef void (*HUD_PlayerMove_t)( struct playermove_s *ppmove, int server );

HUD_PlayerMove_t g_Old_Hud_PlayerMove;

struct {
	float angles[3];
	float origin[3];
	bool setAngles;
	bool setOrigin;
} g_Move;


void New_Hud_PlayerMove( struct playermove_s *ppmove, int server )
{
	g_Old_Hud_PlayerMove(ppmove, server);

	if(g_Move.setAngles)
	{
		g_Move.setAngles = false;

		ppmove->angles[0] = g_Move.angles[0];
		ppmove->angles[1] = g_Move.angles[1];
		ppmove->angles[2] = g_Move.angles[2];
	}

	if(g_Move.setOrigin)
	{
		g_Move.setOrigin = false;

		ppmove->origin[0] = g_Move.origin[0];
		ppmove->origin[1] = g_Move.origin[1];
		ppmove->origin[2] = g_Move.origin[2];
	}
}


void Hook_Hud_PalyerMove(void)
{
	static bool firstRun=true;
	if(!firstRun) return;
	firstRun = false;

	g_Move.setAngles = false;
	g_Move.setOrigin = false;

	g_Old_Hud_PlayerMove = (HUD_PlayerMove_t)GetClientFunction(CFTE_HUD_PlayerMove);
	ReplaceClientFunction(CFTE_HUD_PlayerMove, (void *)&New_Hud_PlayerMove);
}


REGISTER_DEBUGCMD_FUNC(moveto)
{
	bool showHelp = true;
	int argc = pEngfuncs->Cmd_Argc();

	Hook_Hud_PalyerMove();

	if(4 == argc || 7 == argc)
	{
		showHelp = false;

		g_Move.origin[0] = atof(pEngfuncs->Cmd_Argv(1));
		g_Move.origin[1] = atof(pEngfuncs->Cmd_Argv(2));
		g_Move.origin[2] = atof(pEngfuncs->Cmd_Argv(3));
		g_Move.setOrigin = true;

		if(7 == argc)
		{
			g_Move.angles[0] = atof(pEngfuncs->Cmd_Argv(4));
			g_Move.angles[1] = atof(pEngfuncs->Cmd_Argv(5));
			g_Move.angles[2] = atof(pEngfuncs->Cmd_Argv(6));
			g_Move.setAngles = true;

			pEngfuncs->SetViewAngles(g_Move.angles);
		}

	}

	if(showHelp)
	{
		pEngfuncs->Con_Printf("Usage: " DEBUG_PREFIX "simorg_set x y z [ang0 ang1 ang2]\n");
	}


}



// 0cffde70 v_valcspectatorrefdef

// 0cffe1a2 68e09d0c0d      push    offset client!V_CalcRefdef+0xcb960 (0d0c9de0)
// 0cffe1a7 68409c0c0d      push    offset client!V_CalcRefdef+0xcb7c0 (0d0c9c40)
// 0cffe1ac 6a00            push    0
// 0cffe1ae 51              push    ecx
// 0cffe1af e8ccf7ffff      call    client!HUD_DrawTransparentTriangles+0x2db0 (0cffd980)
//
// 0cffd980 V_GetChasePos
// so now you know where to look for the switch() : )
