#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-01-04 dominik.matrixtstorm.com
//
// First changes:
// 2010-01-04 dominik.matrixtstorm.com

#include "forceres.h"

#include <windows.h>

#include "hl_addresses.h"
#include <shared/detours.h>

#define NOP			0x90

void HandleForceRes()
{
	if(lstrcmpi(GetCommandLine(), "-forceres")) {
		MdtMemBlockInfos mbis;

		MdtMemAccessBegin((LPVOID)HL_ADDR_GET(UnkDevCapsChecks), (size_t)HL_ADDR_GET(UnkDevCapsChecks_BYTES), &mbis);

		memset((void *)HL_ADDR_GET(UnkDevCapsChecks), NOP, (size_t)HL_ADDR_GET(UnkDevCapsChecks_BYTES));

		MdtMemAccessEnd(&mbis);
	}
}