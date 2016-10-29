#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-10-18 dominik.matrixstorm.com
//
// First changes:
// 2016-10-18 dominik.matrixstorm.com

#include "csgo_spec_player.h"

#include "addresses.h"

#include <shared/detours.h>


bool g_csgo_spec_player_Block = false;

typedef void(*csgo_spec_player_t)(DWORD arg1);

csgo_spec_player_t detourded_csgo_spec_player;

void touring_csgo_spec_player(DWORD arg1)
{
	if (!g_csgo_spec_player_Block)
		detourded_csgo_spec_player(arg1);
}

bool Hook_csgo_spec_player(void)
{
	static bool firstRun = true;
	static bool firstResult = false;

	if (!firstRun) return firstResult;
	firstRun = false;

	if (AFXADDR_GET(csgo_spec_player))
	{
		detourded_csgo_spec_player = (csgo_spec_player_t)DetourApply((BYTE *)AFXADDR_GET(csgo_spec_player), (BYTE *)touring_csgo_spec_player, (int)AFXADDR_GET(csgo_spec_player_DSZ));

		firstResult = true;
	}

	return firstResult;
}
