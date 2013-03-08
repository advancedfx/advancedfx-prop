#include "stdafx.h"

#include "windows.h" // we need access to virtualprotect etc.

#include <hlsdk.h>

#include "cmdregister.h"

#include <shared/detours.h>

#include <hooks/HookHw.h>
#include "hooks/user32Hooks.h"

#include "filming.h"
#include "hl_addresses.h"

typedef void (*R_RecursiveWorldNode_t) (void * node);

R_RecursiveWorldNode_t R_RecursiveWorldNode = 0;
DWORD g_CallContinue = 0;

void DoDebugCall (void * node) {
	g_Filming.FullClear();
	R_RecursiveWorldNode(node);
	g_Filming.DoCanDebugCapture();
}

__declspec(naked) void DoRecursiveWorldNodeCall (void)
{
	__asm {
		CALL DoDebugCall
		JMP [g_CallContinue]
	}
}

REGISTER_DEBUGCMD_FUNC(tst_debugcapture)
{
	g_Filming.EnableDebugCapture(true);

	if(!R_RecursiveWorldNode)
	{
		R_RecursiveWorldNode = (R_RecursiveWorldNode_t)((DWORD)HL_ADDR_GET(hwDll) +0x486E0);
		Asm32ReplaceWithJmp((void *)((DWORD)HL_ADDR_GET(hwDll) +0x48A56), 0x05, DoRecursiveWorldNodeCall);
		g_CallContinue = ((DWORD)HL_ADDR_GET(hwDll) +0x48A56 +0x05);
	}
}

REGISTER_DEBUGCMD_FUNC(tst_undock)
{
	UndockGameWindowForCapture();
}

REGISTER_DEBUGCMD_FUNC(tst_dock)
{
	RedockGameWindow();
}


REGISTER_DEBUGCMD_FUNC(tst_info)
{
	DWORD dw = (DWORD)pEngfuncs->pEfxAPI->R_DecalShoot;

	pEngfuncs->Con_Printf("pEngfuncs->pEfxAPI->R_DecalShoot = 0x%08x\n", dw);
}
