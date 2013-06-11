#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-05-20 by dominik.matrixstorm.com
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

float * g_value_curtime;


// Hook_VClient_RenderView /////////////////////////////////////////////////////

Hook_VClient_RenderView::Hook_VClient_RenderView()
{
	m_Export = false;
	m_FrameTime = 0;
	m_Import = false;
	m_ImportBaseTime = 0;
	m_IsInstalled = false;
	m_LastTime = 0;
	m_SubTime = 0;
}


Hook_VClient_RenderView::~Hook_VClient_RenderView() {
	ExportEnd();
	ImportEnd();
}

bool Hook_VClient_RenderView::ExportBegin(wchar_t const *fileName, float frameTime) {
	ExportEnd();

	g_BvhExport = new BvhExport(fileName, "MdtCam", frameTime);
	m_Export = true;

	m_SubTime = 0;
	m_FrameTime = frameTime;

	return m_Export;
}

void Hook_VClient_RenderView::ExportEnd() {

	delete g_BvhExport;
	g_BvhExport = 0;
	m_Export = false;
}

float Hook_VClient_RenderView::GetCurTime() {
	return *g_value_curtime;
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

void Hook_VClient_RenderView::Install(CGlobalVarsBase * pGlobals)
{
	if(m_IsInstalled)
		return;

	g_value_curtime = (float *)((BYTE *)pGlobals +AFXADDR_GET(cstrike_gpGlobals_OFS_curtime));

	m_IsInstalled = true;
}



bool Hook_VClient_RenderView::IsInstalled(void) {
	return m_IsInstalled;
}


void Hook_VClient_RenderView::OnViewOverride(float &Tx, float &Ty, float &Tz, float &Rx, float &Ry, float &Rz) {
	float curTime = GetCurTime();

	if(m_Import) {
		float Tf[6];

		if(g_BvhImport.GetCamPositon(
			curTime -m_ImportBaseTime,
			Tf
		)) {
			Ty = -Tf[0];
			Tz = +Tf[1];
			Tx = -Tf[2];
			Rz = -Tf[3];
			Rx = -Tf[4];
			Ry = +Tf[5];
		}
	}

	if(m_Export) {
		g_BvhExport->WriteFrame(
			-Ty, +Tz, -Tx,
			-Rz, -Rx, +Ry
		);
	}
}


void Hook_VClient_RenderView::SetImportBaseTime(float value) {
	m_ImportBaseTime = value;
}
