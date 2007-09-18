#include "hl_addresses.h"

//
// definitions of default addresses:
//

// main, ...
#define HL_DEFAULT_ADDR_CL_ENGINEFUNCS_S 0x01ECCB00
// old: 0x01EA0A08
#define HL_DEFAULT_ADDR_ENGINE_STUDIO_API_S 0x01EE8FC0
// old: 0x01EBC978
#define HL_DEFAULT_ADDR_PLAYERMOVE_S 0x02D82500
// old: 0x02D590A0;

// demoedit:
#define HL_DEFAULT_ADDR_HUDSPECTATOR_FUNC_TFC 0x01909A00
#define HL_DEFAULT_ADDR_HUDSPECTATOR_CMP_TFC (HL_DEFAULT_ADDR_HUDSPECTATOR_FUNC_TFC+0x23)

// Render View and RefDef for information how to find these values reffer to cameraofs.cpp:
#define HL_DEFAULT_ADDR_R_RENDERVIEW 0x01d51d90
// old       : 0x01d50550
#define HL_DEFAULT_ADDR_R_REFDEF (HL_ADDR_PLAYERMOVE_S - 0x13FDC0)
// old        : 0x02c192e0


//
// g_hl_addresses initalization:
//

hl_addresses_t g_hl_addresses = {
// be carefull when you change s.th. here and keep hl_addresses_s and g_hl_addresses in sync
	(char *)HL_DEFAULT_ADDR_CL_ENGINEFUNCS_S,
	(char *)HL_DEFAULT_ADDR_ENGINE_STUDIO_API_S,
	(char *)HL_DEFAULT_ADDR_PLAYERMOVE_S,

	(char *)HL_DEFAULT_ADDR_R_RENDERVIEW,
	(char *)HL_DEFAULT_ADDR_R_REFDEF,

	(char *)HL_DEFAULT_ADDR_HUDSPECTATOR_FUNC_TFC,
	(char *)HL_DEFAULT_ADDR_HUDSPECTATOR_CMP_TFC
};