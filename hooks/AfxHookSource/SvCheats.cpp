#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-03-31 by dominik.matrixstorm.com
//
// First changes:
// 2010-03-31 by dominik.matrixstorm.com

#include "SvCheats.h"

#include <hooks/shared/detours.h>
#include <shared/StringTools.h>

#include "WrpVEngineClient.h"

#define ADDR_cstrike_commandbuffer_cheats_cmp 0x4FBA4 
#define ADDR_cstrike_commandbuffer_cheats_ok 0x4fc0c

#define ADDR_ep2_commandbuffer_cheats_cmp 0x157B3 
#define ADDR_ep2_commandbuffer_cheats_ok 0x1582A


bool IsInstalled_AllowCheats();

void Install_AllowCheats(const char * gameDir)
{
	HMODULE hm = GetModuleHandle("engine");

	if(!hm)
	{
		Tier0_Msg("failed.\n");
		return;
	}

	if(StringEndsWith(gameDir, "\\cstrike"))
		Asm32ReplaceWithJmp((BYTE *)hm +ADDR_cstrike_commandbuffer_cheats_cmp, 5, (BYTE *)hm +ADDR_cstrike_commandbuffer_cheats_ok);
	if(StringEndsWith(gameDir, "\\ep2"))
		Asm32ReplaceWithJmp((BYTE *)hm +ADDR_ep2_commandbuffer_cheats_cmp, 5, (BYTE *)hm +ADDR_ep2_commandbuffer_cheats_ok);
	else
		Tier0_Msg("%s is not supported\n", gameDir);



	Tier0_Msg("unblocked.\n");
}