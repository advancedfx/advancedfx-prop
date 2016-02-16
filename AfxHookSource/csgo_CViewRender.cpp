#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-01-19 dominik.matrixstorm.com
//
// First changes:
// 2014-11-26 dominik.matrixstorm.com

#include "csgo_CViewRender.h"

#include "AfxStreams.h"
#include "addresses.h"
#include <shared/detours.h>


float g_SmokeOverlay_AlphaMod = 1.0f;

void * detoured_csgo_CViewRender_RenderSmokeOverlay_OnStoreAlpha;

void __declspec(naked) touring_csgo_CViewRender_RenderSmokeOverlay_OnStoreAlpha(void)
{
	__asm mulss xmm1, g_SmokeOverlay_AlphaMod

	__asm jmp detoured_csgo_CViewRender_RenderSmokeOverlay_OnStoreAlpha
}

void * detoured_csgo_CViewRender_RenderView_AfterVGui_DrawHud;

void DoOnDrawingHud(void)
{
	g_AfxStreams.OnDrawingHud();
}

void __declspec(naked) touring_csgo_CViewRender_RenderView_AfterVGui_DrawHud(void)
{
	__asm ; it's safe to do a call without preauctions here, because a call would happen right after anyways
	__asm call DoOnDrawingHud

	__asm jmp detoured_csgo_CViewRender_RenderView_AfterVGui_DrawHud
}

bool csgo_CViewRender_Install(void)
{
	static bool firstResult = true;
	static bool firstRun = true;
	if(!firstRun) return firstResult;
	firstRun = false;

	if(AFXADDR_GET(csgo_CViewRender_RenderView_AfterVGui_DrawHud))
	{
		detoured_csgo_CViewRender_RenderView_AfterVGui_DrawHud = (void *)DetourApply((BYTE *)AFXADDR_GET(csgo_CViewRender_RenderView_AfterVGui_DrawHud), (BYTE *)touring_csgo_CViewRender_RenderView_AfterVGui_DrawHud, 0x5);
		
		// update original call offset:
		DWORD * pCalladdr = (DWORD *)((BYTE *)detoured_csgo_CViewRender_RenderView_AfterVGui_DrawHud +0x1);
		*pCalladdr = *pCalladdr -((DWORD)detoured_csgo_CViewRender_RenderView_AfterVGui_DrawHud -AFXADDR_GET(csgo_CViewRender_RenderView_AfterVGui_DrawHud));
	}
	else
		firstResult = false;

	if(AFXADDR_GET(csgo_CViewRender_RenderSmokeOverlay_OnStoreAlpha))
	{
		detoured_csgo_CViewRender_RenderSmokeOverlay_OnStoreAlpha = (void *)DetourApply((BYTE *)AFXADDR_GET(csgo_CViewRender_RenderSmokeOverlay_OnStoreAlpha), (BYTE *)touring_csgo_CViewRender_RenderSmokeOverlay_OnStoreAlpha, 0x8);
	}
	else
		firstResult = false;

	return firstResult;
}
