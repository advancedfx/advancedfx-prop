#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-11-26 dominik.matrixstorm.com
//
// First changes:
// 2014-11-26 dominik.matrixstorm.com

#include "csgo_viewrender.h"

#include <shared/detours.h>
#include "addresses.h"
#include "MirvShader.h"

struct vrect_t
{
	int x,y,width,height;
	vrect_t *pnext;
};

typedef void (__stdcall *csgo_CViewRender_Render_t)(DWORD *this_ptr, vrect_t *rect);

csgo_CViewRender_Render_t detoured_csgo_CViewRender_Render;

void __stdcall touring_csgo_CViewRender_Render(DWORD *this_ptr, vrect_t *rect)
{
	g_MirvShader.SetInRenderWorld(true);

	detoured_csgo_CViewRender_Render(this_ptr, rect);

	g_MirvShader.SetInRenderWorld(false);
}

bool csgo_viewrender_Install(void)
{
	static bool firstResult = false;
	static bool firstRun = true;
	if(!firstRun) return firstResult;
	firstRun = false;

	if(AFXADDR_GET(csgo_CViewRender_Render))
	{
		detoured_csgo_CViewRender_Render = (csgo_CViewRender_Render_t)DetourClassFunc((BYTE *)AFXADDR_GET(csgo_CViewRender_Render), (BYTE *)touring_csgo_CViewRender_Render, (int)AFXADDR_GET(csgo_CViewRender_Render_DSZ));

		firstResult = true;
	}

	return firstResult;
}