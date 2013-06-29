#include "stdafx.h"

#include "UnkDrawHud.h"

#include <shared/detours.h>
#include <hl_addresses.h>

#include "../../filming.h"

void * g_UnkDrawHudInContinue = 0;
void * g_UnkDrawHudOutContinue;
void * g_UnkDrawHudInCall;
void * g_UnkDrawHudOutCall;

__declspec(naked) void New_UnkDrawHudIn(void)
{
	__asm push ecx
	g_Filming.OnHudBeginEvent();
	__asm pop ecx

	__asm push ebx
	__asm call [g_UnkDrawHudInCall]
	__asm JMP [g_UnkDrawHudInContinue]
}

__declspec(naked) void New_UnkDrawHudOut(void)
{
	static bool tempMem;

	__asm call [g_UnkDrawHudOutCall]
	
	__asm push ecx
	tempMem = g_Filming.OnHudEndEvent();
	__asm pop ecx

	__asm cmp tempMem, 0
	__asm JNZ New_UnkDrawHudIn
	__asm JMP [g_UnkDrawHudOutContinue]
}

void Hook_UnkDrawHud()
{
	if (!g_UnkDrawHudInContinue && NULL != HL_ADDR_GET(UnkDrawHudIn))
	{
		Asm32ReplaceWithJmp((void *)HL_ADDR_GET(UnkDrawHudIn), HL_ADDR_GET(UnkDrawHudInContinue)-HL_ADDR_GET(UnkDrawHudIn), (void *)New_UnkDrawHudIn);
		Asm32ReplaceWithJmp((void *)HL_ADDR_GET(UnkDrawHudOut), HL_ADDR_GET(UnkDrawHudOutContinue)-HL_ADDR_GET(UnkDrawHudOut), (void *)New_UnkDrawHudOut);
		
		g_UnkDrawHudInContinue = (void *)HL_ADDR_GET(UnkDrawHudInContinue);
		g_UnkDrawHudOutContinue = (void *)HL_ADDR_GET(UnkDrawHudOutContinue);
		g_UnkDrawHudInCall = (void *)HL_ADDR_GET(UnkDrawHudInCall);
		g_UnkDrawHudOutCall = (void *)HL_ADDR_GET(UnkDrawHudOutCall);
	}
}

