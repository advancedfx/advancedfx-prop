#include "stdafx.h"

#include "UnkDrawHud.h"

#include <shared/detours.h>
#include <hl_addresses.h>

#include "../../filming.h"

typedef void (*UnkDrawHud_t) (void * punk);
UnkDrawHud_t g_UnkDrawHud = 0;
DWORD g_UnkDrawHudCallAfter = 0;

void New_UnkDrawHud (void * punk)
{
	do {
		g_Filming.OnHudBeginEvent();
		g_UnkDrawHud(punk);
	} while(g_Filming.OnHudEndEvnet());
}

__declspec(naked) void New_UnkDrawHudCall() {
	__asm {
		CALL New_UnkDrawHud
		JMP [g_UnkDrawHudCallAfter]
	}
}

void Hook_UnkDrawHud()
{
	if (!g_UnkDrawHud && NULL != HL_ADDR_GET(UnkDrawHud))
	{
		g_UnkDrawHud = (UnkDrawHud_t) HL_ADDR_GET(UnkDrawHud);
		Asm32ReplaceWithJmp((void *)HL_ADDR_GET(UnkDrawHudCall), HL_ADDR_GET(UnkDrawHudCallAfter)-HL_ADDR_GET(UnkDrawHudCall), (void *)New_UnkDrawHudCall);
		g_UnkDrawHudCallAfter = HL_ADDR_GET(UnkDrawHudCallAfter);
		// patch the call address
	}
}

