#include "stdafx.h"

#include "HookGameLoaded.h"
#include "OpenGl32Hooks.h"
#include "user32Hooks.h"

#include "../AfxGoldSrcComClient.h"
#include "../aiming.h"
#include "../cmdregister.h"
#include "../filming.h"
#include "../GlPrimMods.h"
#include "../mirv_glext.h"
#include "../mirv_scripting.h"
#include "../supportrender.h"
#include "../zooming.h"

#include "HookHw.h"

#include "../modules/ModColor.h"
#include "../modules/ModColorMask.h"
#include "../modules/ModHide.h"
#include "../modules/ModReplace.h"
#include "../modules/ModInfo.h"

#include <hlsdk.h>


REGISTER_CVAR(disableautodirector, "0", 0);

REGISTER_DEBUGCVAR(gl_previewclear, "1", 0);


struct {
	bool restore;

	// Matte key:
	GLboolean b_GL_DEPTH_TEST;
	GLint i_GL_DEPTH_FUNC;
	GLboolean b_ColorWriteMask[4];

	// Mate alpha:
	GLboolean old_enabled;
	GLint old_texture;
	GLint old_active_texture;
	GLint old_env_param;

} g_ModeKey_saved;


bool ModeKey_Begin(GLenum mode)
{
	g_ModeKey_saved.restore=false;

	Filming::DRAW_RESULT res = g_Filming.shouldDraw(mode);

	if (res == Filming::DR_HIDE) {
		return false;
	}

	else if (res == Filming::DR_MASK)
	{
		if(Filming::MS_ENTITY == g_Filming.GetMatteStage())
		{
			g_ModeKey_saved.restore = true;
			glGetBooleanv(GL_DEPTH_TEST,&(g_ModeKey_saved.b_GL_DEPTH_TEST));
			glGetIntegerv(GL_DEPTH_FUNC,&(g_ModeKey_saved.i_GL_DEPTH_FUNC));
			glGetBooleanv(GL_COLOR_WRITEMASK, g_ModeKey_saved.b_ColorWriteMask);

			glColorMask(FALSE, FALSE, FALSE, TRUE);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST);
		}
	}
	else if (Filming::HUDRQ_NORMAL == g_Filming.giveHudRqState())
		glColorMask(TRUE, TRUE, TRUE, TRUE); // BlendFunc for additive sprites needs special controll, don't override it

	return true;
}


void ModeKey_End()
{
	if (g_ModeKey_saved.restore)
	{
		g_ModeKey_saved.restore = false;
		if(!g_ModeKey_saved.b_GL_DEPTH_TEST)
			glDisable(GL_DEPTH_TEST);
		glDepthFunc(g_ModeKey_saved.i_GL_DEPTH_FUNC);
		glColorMask(g_ModeKey_saved.b_ColorWriteMask[0], g_ModeKey_saved.b_ColorWriteMask[1], g_ModeKey_saved.b_ColorWriteMask[2], g_ModeKey_saved.b_ColorWriteMask[3]);
	}
}

struct {
	GlPrimMod::Replace replaceBlack;
	GlPrimMod::Replace replaceWhite;
	GlPrimMod::Color   color;
	bool restore;
	bool wasWhite;
} g_ModeAlpha;

bool ModeAlpha_Begin(GLenum mode)
{
	static bool firstRun = true;

	if(firstRun)
	{
		firstRun = false;

		g_ModeAlpha.replaceBlack.SetRgb(0, 0, 0);

		g_ModeAlpha.replaceWhite.SetRgb(255, 255, 255);
	}

	g_ModeAlpha.restore=false;

	if (Filming::MS_ENTITY == g_Filming.GetMatteStage())
	{
		Filming::DRAW_RESULT res = g_Filming.shouldDraw(mode);

		if(Filming::DR_NORMAL == res)
		{
			// positive (white).

			g_ModeAlpha.restore = true;
			g_ModeAlpha.wasWhite = true;

			g_ModeAlpha.replaceWhite.OnGlBegin(mode);

			g_ModeAlpha.color.SetRgba(1, 1, 1, -1);
			g_ModeAlpha.color.OnGlBegin(mode);
		}
		else if(Filming::DR_MASK == res)
		{
			// negative (black).

			g_ModeAlpha.restore = true;
			g_ModeAlpha.wasWhite = false;

			g_ModeAlpha.replaceBlack.OnGlBegin(mode);

			g_ModeAlpha.color.SetRgba(0, 0, 0, -1);
			g_ModeAlpha.color.OnGlBegin(mode);
		}
		else
		{
			// (x-ray).
			return false;
		}
	}

	return true;
}

void ModeAlpha_End()
{
	if (g_ModeAlpha.restore)
	{
		g_ModeAlpha.restore = false;

		g_ModeAlpha.color.OnGlEnd();

		if(g_ModeAlpha.wasWhite)
			g_ModeAlpha.replaceWhite.OnGlEnd();
		else
			g_ModeAlpha.replaceBlack.OnGlEnd();
	}
}


int		g_nViewports = 0;
bool	g_bIsSucceedingViewport = false;


void APIENTRY NewGlBegin(GLenum mode)
{
	ScriptEvent_OnGlBegin((unsigned int)mode);

	if (g_Filming.doWireframe(mode) == Filming::DR_HIDE) {
		return;
	}

	g_Filming.DoWorldFxBegin(mode); // WH fx

	g_Filming.DoWorldFx2(mode); // lightmap fx

	if (!g_Filming.isFilming())
	{
		glBegin(mode);
		return;
	}

	if(Filming::MM_KEY == g_Filming.GetMatteMethod())
	{
		if(!ModeKey_Begin(mode))
			return;
	}
	else
	{
		if(!ModeAlpha_Begin(mode))
			return;
	}

	g_ModReplace.OnGlBegin(mode);

	g_ModColor.OnGlBegin(mode);

	g_ModColorMask.OnGlBegin(mode);

	if(g_ModHide.OnGlBegin(mode)) glBegin(mode);
}

void APIENTRY NewGlEnd(void)
{
	ScriptEvent_OnGlEnd();

	glEnd();

	g_ModColorMask.OnGlEnd();

	g_ModColor.OnGlEnd();

	g_ModReplace.OnGlEnd();

	if(Filming::MM_KEY == g_Filming.GetMatteMethod())
		ModeKey_End();
	else
		ModeAlpha_End();

	g_Filming.DoWorldFxEnd();

}

void APIENTRY NewGlClear(GLbitfield mask)
{
	// check if we want to clear (it also might set clearcolor and stuff like that):
	if (!g_Filming.checkClear(mask))
		return;

	glClear(mask);
}


void APIENTRY NewGlViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	static bool bFirstRun = true;

	g_bIsSucceedingViewport = true;

	if (bFirstRun)
	{
#ifdef MDT_DEBUG
		MessageBox(0,"First my_glViewport","MDT_DEBUG",MB_OK|MB_ICONINFORMATION);
#endif

		HookGameLoaded();

		bFirstRun = false;
	}


	// Only on the first viewport
	if (g_nViewports == 0)
	{
		//g_Filming.setScreenSize(width, height);

		// Always get rid of auto_director
		if (disableautodirector->value != 0.0f)
			pEngfuncs->Cvar_SetValue("spec_autodirector", 0.0f);

		// This is called whether we're zooming or not
		g_Zooming.handleZoom();

		if (g_Aiming.isAiming())
			g_Aiming.aim();
	}

	// Not necessarily 5 viewports anymore, keep counting until reset
	// by swapbuffers hook.
	g_nViewports++;

	g_Zooming.adjustViewportParams(x, y, width, height);
	glViewport(x, y, width, height);

}

void APIENTRY NewGlFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar)
{
	g_Filming.SupplyZClipping(zNear, zFar);
	g_Zooming.adjustFrustumParams(left, right, top, bottom);
	glFrustum(left, right, bottom, top, zNear, zFar);
}

void APIENTRY NewGlBlendFunc (GLenum sfactor, GLenum dfactor)
{
	switch(g_Filming.giveHudRqState())
	{
	case Filming::HUDRQ_CAPTURE_ALPHA:
		if (dfactor == GL_ONE)//(sfactor == dfactor == GL_ONE)
		{
			// block the Alpha chan of Additive sprites
			glColorMask(TRUE, TRUE, TRUE, FALSE); // block alpha for additive HUD sprites
			glBlendFunc(sfactor,dfactor);
		}
		else
		{
			// don't let sprites lower alpha value:
			glColorMask(TRUE, TRUE, TRUE,TRUE); // allow alpha
			if (sfactor==GL_SRC_ALPHA) sfactor=GL_SRC_ALPHA_SATURATE;
			if (dfactor==GL_SRC_ALPHA) dfactor=GL_SRC_ALPHA_SATURATE;
			glBlendFunc(sfactor,dfactor);
		}
		break;
	default :
		glBlendFunc(sfactor,dfactor);
	}
}


BOOL (APIENTRY *OldWglSwapBuffers)(HDC hDC);

BOOL APIENTRY NewWglSwapBuffers(HDC hDC)
{
	BOOL bResWglSwapBuffers;
	bool bRecordSwapped=false;

	// Next viewport will be the first of the new frame
	g_nViewports = 0;

	if (g_Filming.isFilming())
	{
		// we are filming, force buffers and capture our image:
		
		// record the selected buffer (capture):
		bRecordSwapped = g_Filming.recordBuffers(hDC, &bResWglSwapBuffers);
	}

	// do the switching of buffers as requersted:
	if (!bRecordSwapped)
	{
		if (g_pSupportRender)
			bResWglSwapBuffers = g_pSupportRender->hlaeSwapBuffers(hDC);
		else
			bResWglSwapBuffers = OldWglSwapBuffers(hDC);
	}

	// no we have captured the image (by default from backbuffer) and display it on the front, now we can prepare the new backbuffer image if required.

	if (g_Filming.isFilming())
	{
		// we are filming, do required clearing and restore buffers:

		// carry out preparerations on the backbuffer for the next frame:
		g_Filming.FullClear();
	}
	else if(gl_previewclear->value)
		g_Filming.FullClear();

	return bResWglSwapBuffers;
}


HGLRC Init_Support_Renderer(HWND hMainWindow, HDC hMainWindowDC, int iWidth, int iHeight);

HGLRC WINAPI NewWglCreateContext(HDC hDc)
{
	return Init_Support_Renderer( g_GameWindow, hDc, g_Width, g_Height );
}


BOOL WINAPI NewWglMakeCurrent(HDC hDc, HGLRC hGlRc)
{
	if (hGlRc && g_pSupportRender && g_pSupportRender->GetHGLRC() == hGlRc)
		return g_pSupportRender->hlaeMakeCurrent(hDc, hGlRc);

	BOOL bRet = wglMakeCurrent(hDc, hGlRc);

	return bRet;

}


BOOL WINAPI NewWglDeleteContext(HGLRC hGlRc)
{
	if (hGlRc && g_pSupportRender && g_pSupportRender->GetHGLRC() == hGlRc)
		return g_pSupportRender->hlaeDeleteContext(hGlRc);

	return wglDeleteContext(hGlRc);
}


//
// support functions:
//

HGLRC Init_Support_Renderer(HWND hMainWindow, HDC hMainWindowDC, int iWidth, int iHeight)
{
	if(g_pSupportRender)
		return NULL; // already created
	
	// determine desired target renderer:
	CHlaeSupportRender::ERenderTarget eRenderTarget = CHlaeSupportRender::RT_GAMEWINDOW;

	switch(g_AfxGoldSrcComClient.GetRenderMode())
	{
	case RM_FrameBufferObject:
		eRenderTarget = CHlaeSupportRender::RT_FRAMEBUFFEROBJECT;
		break;
	case RM_MemoryDc:
		eRenderTarget = CHlaeSupportRender::RT_MEMORYDC;
	}

	// Init support renderer:
	g_pSupportRender = new CHlaeSupportRender(hMainWindow, iWidth, iHeight);

	HGLRC tHGLRC;
	tHGLRC = g_pSupportRender->hlaeCreateContext(eRenderTarget,hMainWindowDC);

	if (!tHGLRC)
		MessageBoxA(0, "hlaeCreateContext failed.", "Init_Support_Renderer ERROR", MB_OK|MB_ICONERROR);

	return tHGLRC;
}
