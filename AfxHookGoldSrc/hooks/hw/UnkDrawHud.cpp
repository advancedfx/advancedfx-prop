#include "stdafx.h"

#include "UnkDrawHud.h"

#include <shared/detours.h>
#include <hl_addresses.h>

#include "../../filming.h"

typedef void (*UnkDrawHud_t) (void);
UnkDrawHud_t g_Old_UnkDrawHud = 0;

void New_UnkDrawHud (void)
{
	g_Filming.OnHudBeginEvent();
	do {
		g_Old_UnkDrawHud();
	} while(g_Filming.OnHudEndEvnet());	
}

void Hook_UnkDrawHud()
{
	if (!g_Old_UnkDrawHud && NULL != HL_ADDR_GET(UnkDrawHud))
		g_Old_UnkDrawHud = (UnkDrawHud_t) DetourApply((BYTE *)HL_ADDR_GET(UnkDrawHud), (BYTE *)New_UnkDrawHud, (int)HL_ADDR_GET(UnkDrawHud_DSZ));
}

