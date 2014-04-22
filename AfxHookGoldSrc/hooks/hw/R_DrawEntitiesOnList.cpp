#include "stdafx.h"

#include "R_DrawEntitiesOnList.h"

#include <hl_addresses.h>

#include <shared/detours.h>

#include "Host_Frame.h"
#include "../../sv_hitboxes.h"
#include "../../cmdregister.h"

REGISTER_CVAR(draw_sv_hitboxes, "0", 0);

bool g_In_R_DrawEntitiesOnList = false;

typedef void (*R_DrawEntitiesOnList_t) (void);
R_DrawEntitiesOnList_t g_Old_R_DrawEntitiesOnList = 0;

void New_R_DrawEntitiesOnList (void)
{
	g_In_R_DrawEntitiesOnList = true;
	g_Old_R_DrawEntitiesOnList();
	g_In_R_DrawEntitiesOnList = false;

	if(g_Host_Frame_Called && draw_sv_hitboxes->value)
		Draw_SV_Hitboxes();
}


void Hook_R_DrawEntitiesOnList()
{
	if( !g_Old_R_DrawEntitiesOnList && 0 != HL_ADDR_GET(R_DrawEntitiesOnList) ) 
	{
		g_Old_R_DrawEntitiesOnList = (R_DrawEntitiesOnList_t) DetourApply((BYTE *)HL_ADDR_GET(R_DrawEntitiesOnList), (BYTE *)New_R_DrawEntitiesOnList, (int)HL_ADDR_GET(DTOURSZ_R_DrawEntitiesOnList));
	}
}
