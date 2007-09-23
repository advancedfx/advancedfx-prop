#include "hl_addresses.h"

//
// definitions of default addresses that need to be present early in case we use prints for debug:
//

//
// g_hl_addresses initalization:
//


hl_addresses_t g_hl_addresses = {
// be carefull when you change s.th. here and keep hl_addresses_s and g_hl_addresses in sync
// NULL values are calculated at runtime from the .ini system
	(char *)0x01ECCB00, // p_cl_enginefuncs_s
	0, // p_engine_studio_api_s
	0, // p_playermove_s
	0, // p_SCR_UpdateScreen
	0, // p_SCR_SetUpToDrawConsole
	0, // p_V_RenderView
	0, // p_R_RenderView_
	0, // p_GL_Set2D
	0, // p_r_refdef
	0, // p_HudSpectator_tfc
	0, // p_HudSpectator_cmp_tfc
};

/*

01dd0370 SCR_UpdateScreen:
...
01dd041e e83d74f8ff      call    launcher!CreateInterface+0x956471 (01d57860) == SCR_SetUpToDrawConsole ()
01dd0423 e8a8480000      call    launcher!CreateInterface+0x9d38e1 (01dd4cd0) == V_RenderView
01dd0428 e823b5f8ff      call    launcher!CreateInterface+0x95a561 (01d5b950) == GL_Set2D

V_RenderView calls 01d51d90 R_RenderView

*/