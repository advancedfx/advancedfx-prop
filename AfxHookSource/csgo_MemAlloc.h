#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2017-02-09 dominik.matrixstorm.com
//
// First changes:
// 2017-02-09 dominik.matrixstorm.com

#include "SourceInterfaces.h"

class ITier0MemAllocFreeNotifyee abstract
{
public:
	/// <summary>
	/// When implementing this
	/// YOU MUST NOT call cany of the functions in this header (i.e. NotifiyOnTier0MemAllocFree)
	/// AND NOT call anything of the game, since it might trigger additional Free Notifications
	/// WHICH WOULD DEADLOCK!
	/// </summary>
	virtual void OnTier0MemAllocFree(void * pMem) = 0;
};

void NotifiyOnTier0MemAllocFree(ITier0MemAllocFreeNotifyee * notifyee, void * pMem);

bool Hook_csgo_MemAlloc(void);
