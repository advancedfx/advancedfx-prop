#include "stdafx.h"

#include "windows.h" // we need access to virtualprotect etc.

#include <hlsdk.h>

#include "cmdregister.h"

#include <shared/detours.h>

extern cl_enginefuncs_s *pEngfuncs;

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

