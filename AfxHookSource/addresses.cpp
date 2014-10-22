#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-10-21 by dominik.matrixstorm.com
//
// First changes:
// 2010-09-27 dominik.matrixstorm.com

#include "addresses.h"

AFXADDR_DEF(csgo_CHudDeathNotice_FireGameEvent)
AFXADDR_DEF(csgo_CHudDeathNotice_FireGameEvent_DSZ)
AFXADDR_DEF(csgo_gpGlobals_OFS_curtime)
AFXADDR_DEF(cstrike_gpGlobals_OFS_curtime)
AFXADDR_DEF(cstrike_OFS_CvarFloatValue)

void Addresses_InitClientDll(AfxAddr clientDll)
{
	AFXADDR_SET(csgo_gpGlobals_OFS_curtime, 4*4);

	AFXADDR_SET(csgo_CHudDeathNotice_FireGameEvent, clientDll + 0x48EB20);
	AFXADDR_SET(csgo_CHudDeathNotice_FireGameEvent_DSZ, 0x0b);

	AFXADDR_SET(cstrike_gpGlobals_OFS_curtime, 3*4);
	AFXADDR_SET(cstrike_OFS_CvarFloatValue, 0x2c);
}
