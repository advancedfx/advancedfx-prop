#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-10-29 dominik.matrixstorm.com
//
// First changes:
// 2016-10-29 dominik.matrixstorm.com

#include "csgo_CBasePlayer.h"

#include "addresses.h"

#include <shared/detours.h>


typedef void csgo_CRecvProxyData_t;

typedef void(*csgo_C_BasePlayer_RecvProxy_ObserverTarget_t)(const csgo_CRecvProxyData_t *pData, void *pStruct, void *pOut);

csgo_C_BasePlayer_RecvProxy_ObserverTarget_t detoured_csgo_C_BasePlayer_RecvProxy_ObserverTarget;

bool g_csgo_Block_C_BasePlayer_RecvProxy_ObserverTarget = false;

void touring_csgo_C_BasePlayer_RecvProxy_ObserverTarget(const csgo_CRecvProxyData_t *pData, void *pStruct, void *pOut)
{
	if (!g_csgo_Block_C_BasePlayer_RecvProxy_ObserverTarget)
		detoured_csgo_C_BasePlayer_RecvProxy_ObserverTarget(pData, pStruct, pOut);
}

bool Hook_csgo_C_BasePlayer_RecvProxy_ObserverTarget(void)
{
	static bool firstResult = false;
	static bool firstRun = true;
	if (!firstRun) return firstResult;
	firstRun = false;

	if (AFXADDR_GET(csgo_C_BasePlayer_RecvProxy_ObserverTarget))
	{
		detoured_csgo_C_BasePlayer_RecvProxy_ObserverTarget = (csgo_C_BasePlayer_RecvProxy_ObserverTarget_t)DetourApply((BYTE *)AFXADDR_GET(csgo_C_BasePlayer_RecvProxy_ObserverTarget), (BYTE *)touring_csgo_C_BasePlayer_RecvProxy_ObserverTarget, (int)0x06);

		firstResult = true;
	}

	return firstResult;
}
