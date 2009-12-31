
// BEGIN HLSDK includes
//
// HACK: prevent cldll_int.h from messing the HSPRITE definition,
// HLSDK's HSPRITE --> MDTHACKED_HSPRITE
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <hlsdk/multiplayer/cl_dll/wrect.h>
#include <hlsdk/multiplayer/cl_dll/cl_dll.h>
#include <hlsdk/multiplayer/common/cvardef.h>
#include <hlsdk/multiplayer/engine/cdll_int.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include "cmdregister.h"
#include "zooming.h"

extern cl_enginefuncs_s *pEngfuncs;

extern float clamp(float i, float min, float max);

REGISTER_CVAR(zoom_optical, "20", 0);
REGISTER_CVAR(zoom_digital, "100", 0);
REGISTER_CVAR(zoom_digital_x, "0.5", 0);
REGISTER_CVAR(zoom_digital_y, "0.5", 0);
REGISTER_CVAR(zoom_speed, "200", 0);

// Our zooming singleton
Zooming g_Zooming;

void Zooming::adjustViewportParams(GLint &x, GLint &y, GLsizei &width, GLsizei &height)
{
	if (!isZooming())
		return;

	static int iDims[2] = { 0, 0 };
	glGetIntegerv(GL_MAX_VIEWPORT_DIMS, iDims);

	float t = m_flZoom * 0.01f * clamp(zoom_digital->value, 0.0f, 100.0f) * 0.01f;;
	float flZoomAmountX = t * iDims[0] + (1.0f - t) * width;
	float flZoomAmountY = flZoomAmountX * ((float) height / width);

	x -= (int) ((flZoomAmountX - width) * clamp(zoom_digital_x->value, 0.0f, 1.0f));
	y -= (int) ((flZoomAmountY - height) * clamp(zoom_digital_y->value, 0.0f, 1.0f)) ;
	width = (int) flZoomAmountX;
	height = (int) flZoomAmountY;
}

void Zooming::adjustFrustumParams(GLdouble &left, GLdouble &right, GLdouble &bottom, GLdouble &top)
{
	if (!isZooming())
		return;

	float t = m_flZoom * 0.01f * clamp(zoom_optical->value, 0.0f, 100.0f) * 0.01f;
	GLdouble m_flZoomX = t * 0.4f + (1.0f - t) * right;
	GLdouble m_flZoomY = m_flZoomX * (top / right);

	left	= -m_flZoomX;
	right	= +m_flZoomX;
	bottom	= -m_flZoomY;
	top		= +m_flZoomY;
}

void Zooming::handleZoom()
{
	static float flLastTime = pEngfuncs->GetClientTime();

	if (!isZooming())
	{
		flLastTime = pEngfuncs->GetClientTime();
		return;
	}

	float flTimeDelta = pEngfuncs->GetClientTime() - flLastTime;
	// this is a bad hack to reduce the impact of timing problems for users, but I was lazy heh:
	if (flTimeDelta<0) flTimeDelta=0;
#ifdef MDT_DEBUG
	if (flTimeDelta<0) pEngfuncs->Con_Printf("ERORR: DELTA<0 due to old=%f new=%f",flLastTime,flLastTime+flTimeDelta);
#endif
	m_flZoom += flTimeDelta * (m_bActive ? zoom_speed->value : -zoom_speed->value);
	m_flZoom = clamp(m_flZoom, 0.0f, 100.0f);

	flLastTime = pEngfuncs->GetClientTime();
}

REGISTER_CMD_FUNC_BEGIN(zoom)
{
	g_Zooming.Start();
}

REGISTER_CMD_FUNC_END(zoom)
{
	g_Zooming.Stop();
}

REGISTER_CMD_FUNC(zoom_start)
{
	CALL_CMD_BEGIN(zoom);
}

REGISTER_CMD_FUNC(zoom_stop)
{
	CALL_CMD_END(zoom);
}
