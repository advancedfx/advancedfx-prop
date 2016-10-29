#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-10-17 dominik.matrixstorm.com
//
// First changes:
// 2016-10-17 dominik.matrixstorm.com

#include "csgo_GameEventManager.h"

#include "addresses.h"
#include "SourceInterfaces.h"

#include <shared/detours.h>
 
//bool(ok) FireEvent_Internal( IGameEvent *event, bool bDontBroadcast, bool clientSide );

/*
.text:1020F0F7 loc_1020F0F7:
.text:1020F0F7 mov     ecx, [esp+18h+var_C]
.text:1020F0FB push    edi
.text:1020F0FC mov     eax, [ecx]
.text:1020F0FE call    dword ptr [eax+2Ch]
.text:1020F101 mov     al, 1
.text:1020F103 jmp     short loc_101020F129
*/

typedef bool (__stdcall * csgo_GameEventManager_FireEvent_Internal_t)(DWORD *this_ptr, SOURCESDK::CSGO::IGameEvent * gameEvent, bool bDontBroadCast, bool clientSide);

void call_csgo_GameEventManager_Free(DWORD *this_ptr, SOURCESDK::CSGO::IGameEvent * gameEvent)
{
	__asm mov ecx, this_ptr
	__asm mov eax, gameEvent
	__asm push eax
	__asm mov eax, [ecx]
	__asm call [eax + 0x2c]
}

csgo_GameEventManager_FireEvent_Internal_t detoured_csgo_GameEventManager_FireEvent_Internal;

bool __stdcall touring_csgo_GameEventManager_FireEvent_Internal(DWORD *this_ptr, SOURCESDK::CSGO::IGameEvent * gameEvent, bool bDontBroadCast, bool clientSide)
{
	// not this can be on a thread.

	if(gameEvent)
	{
		char const * gameEventName = gameEvent->GetName();

		Tier0_Warning("touring_csgo_GameEventManager_FireEvent_Internal: %s\n", gameEventName);
		
		if (!strcmp("hltv_changed_target", gameEventName))
		{
			Tier0_Warning("touring_csgo_GameEventManager_FireEvent_Internal, blocked hltv_changed_target\n");
			call_csgo_GameEventManager_Free(this_ptr, gameEvent);
			return true;
		}
	}
	
	return detoured_csgo_GameEventManager_FireEvent_Internal(this_ptr, gameEvent, bDontBroadCast, clientSide);
}

bool csgo_GameEventManager_Install(void)
{
	static bool firstResult = false;
	static bool firstRun = true;
	if (!firstRun) return firstResult;
	firstRun = false;

	if (AFXADDR_GET(csgo_gameeventmanager_fire_internal))
	{
		detoured_csgo_GameEventManager_FireEvent_Internal = (csgo_GameEventManager_FireEvent_Internal_t)DetourClassFunc((BYTE *)AFXADDR_GET(csgo_gameeventmanager_fire_internal), (BYTE *)touring_csgo_GameEventManager_FireEvent_Internal, (int)AFXADDR_GET(csgo_gameeventmanager_fire_internal_DSZ));

		firstResult = true;
	}

	return firstResult;
}
