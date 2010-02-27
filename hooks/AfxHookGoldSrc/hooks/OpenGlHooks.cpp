#include "stdafx.h"

#include "OpenGlHooks.h"

#include "HookGameLoaded.h"
#include "../SupportRender.h"
#include "../cmdregister.h"
#include "../mirv_scripting.h"
#include "../newsky.h"
#include "../aiming.h"
#include "../zooming.h"
#include "../filming.h"

#include "HookHw.h"

#include "../modules/ModColor.h"
#include "../modules/ModColorMask.h"
#include "../modules/ModHide.h"
#include "../modules/ModReplace.h"
#include "../modules/ModInfo.h"

#include <hlsdk.h>


REGISTER_CVAR(disableautodirector, "0", 0);

REGISTER_DEBUGCVAR(gl_noclear, "0", 0);
REGISTER_DEBUGCVAR(gl_previewclear, "1", 0);


int		g_nViewports = 0;
bool	g_bIsSucceedingViewport = false;

struct glBegin_saved_s {
	bool restore;
	GLboolean b_GL_DEPTH_TEST;
	GLint i_GL_DEPTH_FUNC;
	GLboolean b_ColorWriteMask[4];
} g_glBegin_saved;

unsigned int g_glBeginStats = 0;

REGISTER_DEBUGCVAR(glbegin_stats, "0", 0)

void APIENTRY NewGlBegin(GLenum mode)
{
	ScriptEvent_OnGlBegin((unsigned int)mode);

	g_glBegin_saved.restore=false;

	g_glBeginStats++;

	g_NewSky.DetectAndProcessSky(mode);

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

	Filming::DRAW_RESULT res = g_Filming.shouldDraw(mode);

	if (res == Filming::DR_HIDE) {
		return;
	}

	else if (res == Filming::DR_MASK)
	{
		if(Filming::MS_ENTITY == g_Filming.GetMatteStage())
		{
			g_glBegin_saved.restore = true;
			glGetBooleanv(GL_DEPTH_TEST,&(g_glBegin_saved.b_GL_DEPTH_TEST));
			glGetIntegerv(GL_DEPTH_FUNC,&(g_glBegin_saved.i_GL_DEPTH_FUNC));
			glGetBooleanv(GL_COLOR_WRITEMASK, g_glBegin_saved.b_ColorWriteMask);

			glColorMask(FALSE, FALSE, FALSE, TRUE);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_DEPTH_TEST);
		}
	}
	else if (!g_Filming.bWantsHudCapture)
		glColorMask(TRUE, TRUE, TRUE, TRUE); // BlendFunc for additive sprites needs special controll, don't override it

	g_ModReplace.OnGlBegin();

	g_ModColor.OnGlBegin();

	g_ModColorMask.OnGlBegin();

	if(g_ModHide.OnGlBegin()) glBegin(mode);
}

void APIENTRY NewGlEnd(void)
{
	ScriptEvent_OnGlEnd();

	glEnd();

	g_ModColorMask.OnGlEnd();

	g_ModColor.OnGlEnd();

	g_ModReplace.OnGlEnd();


	if (g_glBegin_saved.restore)
	{
		g_glBegin_saved.restore = false;
		if(!g_glBegin_saved.b_GL_DEPTH_TEST)
			glDisable(GL_DEPTH_TEST);
		glDepthFunc(g_glBegin_saved.i_GL_DEPTH_FUNC);
		glColorMask(g_glBegin_saved.b_ColorWriteMask[0], g_glBegin_saved.b_ColorWriteMask[1], g_glBegin_saved.b_ColorWriteMask[2], g_glBegin_saved.b_ColorWriteMask[3]);
	}

	g_Filming.DoWorldFxEnd();

}

void APIENTRY NewGlClear(GLbitfield mask)
{
	if (gl_noclear->value)
		return;
	
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

		if(glbegin_stats->value)
			pEngfuncs->Con_Printf("glBegin calls: %u\n", g_glBeginStats);

		g_glBeginStats = 0;

		// Always get rid of auto_director
		if (disableautodirector->value != 0.0f)
			pEngfuncs->Cvar_SetValue("spec_autodirector", 0.0f);

		// This is called whether we're zooming or not
		g_Zooming.handleZoom();

		// this is now done in doCapturePoint() called in swap
		//if (g_Filming.isFilming())
		//	g_Filming.recordBuffers();

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
		g_Filming.clearBuffers();
	}
	else if(gl_previewclear->value)
		g_Filming.clearBuffers();


	return bResWglSwapBuffers;
}