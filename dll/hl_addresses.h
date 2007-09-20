/*
File        : dll/hl_addresses.h
Started     : 2007-09-13 13:37:00
Project     : Mirv Demo Tool
Authors     : Gavin Bramhill, Dominik Tugend
Description : Address defintions inside Half-Life until we have an better system.
*/

#ifndef HL_ADDRESSES_H
#define HL_ADDRESSES_H

//
// macros for accessing the g_hl_addresses variable:
//

#define HL_ADDR_CL_ENGINEFUNCS_S g_hl_addresses.p_cl_enginefuncs_s
#define HL_ADDR_ENGINE_STUDIO_API_S g_hl_addresses.p_engine_studio_api_s
#define HL_ADDR_PLAYERMOVE_S g_hl_addresses.p_playermove_s

#define HL_ADDR_HUDSPECTATOR_FUNC_TFC g_hl_addresses.p_HudSpectator_tfc
#define HL_ADDR_HUDSPECTATOR_CMPA0_TFC g_hl_addresses.p_HudSpectator_cmp_tfc

#define HL_ADDR_R_RenderView g_hl_addresses.p_R_RenderView
#define HL_ADDR_r_refdef g_hl_addresses.p_r_refdef


//
// g_hl_addresses - this structure holds the actual addresses:
//

typedef struct hl_addresses_s
// be carefull when you change s.th. here and keep hl_addresses_s and g_hl_addresses in sync
{
	// engine addresses directly visible to the client.dll:
	char* p_cl_enginefuncs_s;
	char* p_engine_studio_api_s;
	char* p_playermove_s;

	// engine addresses:
	char* p_R_RenderView;
	char* p_r_refdef;

	// client.dll addresses:
	char* p_HudSpectator_tfc;
	char* p_HudSpectator_cmp_tfc; // actually I think that shouldn't be defined globally

} hl_addresses_t;

extern hl_addresses_t g_hl_addresses;



#endif // #ifndef HL_ADDRESSES_H