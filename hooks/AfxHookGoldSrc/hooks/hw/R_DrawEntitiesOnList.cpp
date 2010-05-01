#include "stdafx.h"

#include "R_DrawEntitiesOnList.h"

#include <hl_addresses.h>

#include <hooks/shared/detours.h>

#include "../../modules/ModInfo.h"



typedef void (*R_DrawEntitiesOnList_t) (void);
R_DrawEntitiesOnList_t g_Old_R_DrawEntitiesOnList = 0;

void New_R_DrawEntitiesOnList (void)
{
	g_ModInfo.SetIn_R_DrawEntitiesOnList(true);
	g_Old_R_DrawEntitiesOnList();
	g_ModInfo.SetIn_R_DrawEntitiesOnList(false);
}


void Hook_R_DrawEntitiesOnList()
{
	if( !g_Old_R_DrawEntitiesOnList && 0 != HL_ADDR_GET(R_DrawEntitiesOnList) ) 
	{
		g_Old_R_DrawEntitiesOnList = (R_DrawEntitiesOnList_t) DetourApply((BYTE *)HL_ADDR_GET(R_DrawEntitiesOnList), (BYTE *)New_R_DrawEntitiesOnList, (int)HL_ADDR_GET(DTOURSZ_R_DrawEntitiesOnList));
	}
}
