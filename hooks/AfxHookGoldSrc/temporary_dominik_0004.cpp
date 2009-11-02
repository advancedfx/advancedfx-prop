#include "windows.h" // we need access to virtualprotect etc.

// BEGIN HLSDK includes
//
// HACK: prevent cldll_int.h from messing the HSPRITE definition,
// HLSDK's HSPRITE --> MDTHACKED_HSPRITE
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "com_model.h"
#include "r_studioint.h"
#include "pm_defs.h"
#include "cvardef.h"
#include "entity_types.h"
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include "cmdregister.h"

#include <hooks/shared/detours.h>

extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s *pEngStudio;
extern playermove_s* ppmove;

struct cl_entity_s *( *g_orig_GetLocalPlayer )( void )=NULL;


REGISTER_DEBUGCVAR(map_localplayer_to, "-1", 0);

typedef cl_entity_s *(* GetLocalPlayer_t)( void );

GetLocalPlayer_t detoured_GetLocalPlayer = NULL;

cl_entity_s *g_hook_GetLocalPlayer( void )
{
	//pEngfuncs->Con_DPrintf("g_hook_GetLocalPlayer called.\n");

	int ito=map_localplayer_to->value;
	if(ito!=-1)
	{
		return pEngfuncs->GetEntityByIndex(ito);
	}
	else return detoured_GetLocalPlayer(); //return g_orig_GetLocalPlayer();
}

REGISTER_DEBUGCMD_FUNC(map_localplayer_install)
{
	if (!g_orig_GetLocalPlayer)
	{
		// we need an backup of the original function since we still want to call it in our hook
		g_orig_GetLocalPlayer  = pEngfuncs->GetLocalPlayer;
		
		pEngfuncs->GetLocalPlayer = g_hook_GetLocalPlayer;

		detoured_GetLocalPlayer = (GetLocalPlayer_t)DetourApply((BYTE *)g_orig_GetLocalPlayer, (BYTE *)g_hook_GetLocalPlayer,0x06);


		pEngfuncs->Con_Printf("Installed hook.\n");
		pEngfuncs->Con_DPrintf("Old address: 0x%08x\n",g_orig_GetLocalPlayer); // Old address: 0x01d18a60
	};
}
