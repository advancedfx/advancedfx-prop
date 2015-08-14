#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-04-30 by dominik.matrixstorm.com
//
// First changes:
// 2009-11-01 by dominik.matrixstorm.com


//
// These hooks mainly hook into
// CViewRender::SetUpView inside the client.dll
//



#include "RenderView.h"

#include <shared/detours.h>
#include <shared/StringTools.h>

#include <shared/bvhimport.h>
#include <shared/bvhexport.h>

#include "addresses.h"
#include "WrpVEngineClient.h"
#include "AfxHookSourceInput.h"


BvhExport * g_BvhExport = NULL;

BvhImport g_BvhImport;

// Create singelton instance:
Hook_VClient_RenderView g_Hook_VClient_RenderView;


unsigned int g_OfsCvarFloatValue;

float GetCvarFloat(void * pcvar)
{
	float * pf = (float *)(*(unsigned char **)pcvar +g_OfsCvarFloatValue);

	float f = *pf;

	return f;
}

void SetCvarFloat(void * pcvar, float value)
{
	float * pf = (float *)(*(unsigned char **)pcvar +g_OfsCvarFloatValue);

	*pf = value;
}


// Hook_VClient_RenderView /////////////////////////////////////////////////////

Hook_VClient_RenderView::Hook_VClient_RenderView()
: m_Globals(0)
{
	m_Export = false;
	m_FovOverride = false;
	m_FovValue = 0.0;
	m_Import = false;
	m_ImportBaseTime = 0;
	m_IsInstalled = false;

	LastCameraOrigin[0] = 0.0;
	LastCameraOrigin[1] = 0.0;
	LastCameraOrigin[2] = 0.0;
	LastCameraAngles[0] = 0.0;
	LastCameraAngles[1] = 0.0;
	LastCameraAngles[2] = 0.0;

	LastCameraFov = 90.0;
}


Hook_VClient_RenderView::~Hook_VClient_RenderView() {
	ExportEnd();
	ImportEnd();
}

bool Hook_VClient_RenderView::ExportBegin(wchar_t const *fileName, double frameTime) {
	ExportEnd();

	g_BvhExport = new BvhExport(fileName, "MdtCam", frameTime);
	m_Export = true;

	return m_Export;
}

void Hook_VClient_RenderView::ExportEnd() {

	delete g_BvhExport;
	g_BvhExport = 0;
	m_Export = false;
}

void Hook_VClient_RenderView::FovOverride(double value)
{
	m_FovValue = value;
	m_FovOverride = true;
}

void Hook_VClient_RenderView::FovDefault()
{
	m_FovOverride = false;
}

float Hook_VClient_RenderView::GetCurTime() {
	return m_Globals->curtime_get();
}

float Hook_VClient_RenderView::GetImportBasteTime() {
	return m_ImportBaseTime;
}

bool Hook_VClient_RenderView::ImportBegin(wchar_t const *fileName)
{
	ImportEnd();

	m_Import = g_BvhImport.LoadMotionFile(fileName);

	return m_Import;
}

void Hook_VClient_RenderView::ImportEnd() {
	g_BvhImport.CloseMotionFile();

	m_Import = false;
}

void Hook_VClient_RenderView::Install(WrpGlobals * globals)
{
	if(m_IsInstalled)
		return;

	m_Globals = globals;

	m_IsInstalled = true;
}

WrpGlobals * Hook_VClient_RenderView::GetGlobals()
{
	return m_Globals;
}

bool Hook_VClient_RenderView::IsInstalled(void) {
	return m_IsInstalled;
}


void Hook_VClient_RenderView::OnViewOverride(float &Tx, float &Ty, float &Tz, float &Rx, float &Ry, float &Rz, float &Fov)
{
	float curTime = GetCurTime();

	if(m_CamPath.IsEnabled())
	{
		// no extrapolation:
		if(m_CamPath.GetLowerBound() <= curTime && curTime <= m_CamPath.GetUpperBound())
		{
			CamPathValue val = m_CamPath.Eval( curTime );

			//Tier0_Msg("================",curTime);
			//Tier0_Msg("currenTime = %f",curTime);
			//Tier0_Msg("vCp = %f %f %f\n", val.X, val.Y, val.Z);

			Tx = (float)val.X;
			Ty = (float)val.Y;
			Tz = (float)val.Z;

			Rx = (float)val.Pitch;
			Ry = (float)val.Yaw;
			Rz = (float)val.Roll;

			Fov = (float)val.Fov;
		}
	}

	if(m_Import) {
		double Tf[6];

		if(g_BvhImport.GetCamPositon(
			curTime -m_ImportBaseTime,
			Tf
		)) {
			Ty = (float)(-Tf[0]);
			Tz = (float)(+Tf[1]);
			Tx = (float)(-Tf[2]);
			Rz = (float)(-Tf[3]);
			Rx = (float)(-Tf[4]);
			Ry = (float)(+Tf[5]);
		}
	}

	if(m_FovOverride) Fov = (float)m_FovValue;

	if(g_AfxHookSourceInput.GetCameraControlMode() && m_Globals)
	{
		double dT = m_Globals->absoluteframetime_get();
		double dForward = dT * g_AfxHookSourceInput.GetCamDForward();
		double dLeft = dT * g_AfxHookSourceInput.GetCamDLeft();
		double dUp = dT * g_AfxHookSourceInput.GetCamDUp();
		double dPitch = dT * g_AfxHookSourceInput.GetCamDPitch();
		double dRoll = dT * g_AfxHookSourceInput.GetCamDRoll();
		double dYaw = dT * g_AfxHookSourceInput.GetCamDYaw();
		double dFov = dT * g_AfxHookSourceInput.GetCamDFov();
		double forward[3], right[3], up[3];

		Rx = (float)(LastCameraAngles[0] +dPitch);
		Ry = (float)(LastCameraAngles[1] +dYaw);
		Rz = (float)(LastCameraAngles[2] +dRoll);
		Fov = (float)(LastCameraFov +dFov);

		if(g_AfxHookSourceInput.GetCamResetView())
		{
			Rx = 0;
			Ry = 0;
			Rz = 0;
			Fov = 90.0;
		}

		MakeVectors(Rz, Rx, Ry, forward, right, up);

		Tx = (float)(LastCameraOrigin[0] + dForward*forward[0] -dLeft*right[0] +dUp*up[0]);
		Ty = (float)(LastCameraOrigin[1] + dForward*forward[1] -dLeft*right[1] +dUp*up[1]);
		Tz = (float)(LastCameraOrigin[2] + dForward*forward[2] -dLeft*right[2] +dUp*up[2]);
	}

	// limit fov to sane values:
	if(Fov<1) Fov = 1;
	else if(Fov>179) Fov = 179;

	if(m_Export) {
		g_BvhExport->WriteFrame(
			-Ty, +Tz, -Tx,
			-Rz, -Rx, +Ry
		);
	}

	LastCameraOrigin[0] = Tx;
	LastCameraOrigin[1] = Ty;
	LastCameraOrigin[2] = Tz;
	LastCameraAngles[0] = Rx;
	LastCameraAngles[1] = Ry;
	LastCameraAngles[2] = Rz;
	LastCameraFov = Fov;

	g_AfxHookSourceInput.Supply_MouseFrameEnd();

	//Tier0_Msg("Hook_VClient_RenderView::OnViewOverride: curTime = %f, LastCameraOrigin=%f,%f,%f\n",curTime,LastCameraOrigin[0],LastCameraOrigin[1],LastCameraOrigin[2]);
}


void Hook_VClient_RenderView::SetImportBaseTime(float value) {
	m_ImportBaseTime = value;
}
