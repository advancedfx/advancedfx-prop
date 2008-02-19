#include "windows.h" // we need access to virtualprotect etc.

#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "com_model.h"
#include "r_studioint.h"
#include "pm_defs.h"
#include "cvardef.h"
#include "entity_types.h"
#include "cmdregister.h"

#include "detours.h"

extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s *pEngStudio;
extern playermove_s* ppmove;

struct cl_entity_s *( *g_orig_GetLocalPlayer )( void )=NULL;


REGISTER_DEBUGCVAR(map_localplayer_to, "-1", 0);

cl_entity_s *g_hook_GetLocalPlayer( void )
{
	int ito=map_localplayer_to->value;
	if(ito!=-1)
	{
		return pEngfuncs->GetEntityByIndex(ito);
	}
	else return g_orig_GetLocalPlayer();
}

REGISTER_DEBUGCMD_FUNC(map_localplayer_install)
{
	if (!g_orig_GetLocalPlayer)
	{
		// we need an backup of the original function since we still want to call it in our hook
		g_orig_GetLocalPlayer  = pEngfuncs->GetLocalPlayer;
		// don't forget: pEngfuncs->pEfxAPI->Draw_DecalIndexFromName is now replaced with the address (pointer) of our own function!
		pEngfuncs->GetLocalPlayer = g_hook_GetLocalPlayer;
		pEngfuncs->Con_Printf("Installed hook.\n");
	};// else pEngfuncs->Con_Printf("Already hooked.\n");

	/*if (pEngfuncs->Cmd_Argc()==2)
	{
		int iw=pEngfuncs->Cmd_Argv(1);
		
		pEngfuncs->pfnSe
	}*/
}