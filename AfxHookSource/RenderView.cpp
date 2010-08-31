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

#include "WrpVEngineClient.h"


extern WrpVEngineClient * g_VEngineClient;


//// >>>> Valve SrcSDK


class MdtVector;
typedef MdtVector Vector;

class MdtVector				
{
public:
	// Members
	vec_t x, y, z;

	// shortened.
};


//// <<<< Valve SrcSDK


BvhExport * g_BvhExport = NULL;

BvhImport g_BvhImport;

// Create singelton instance:
Hook_VClient_RenderView g_Hook_VClient_RenderView;



#define ADDR_cstrike_CalcDemoViewOverride 0x136B80
// 0x135D30
#define ADDR_cstrike_CalcDemoViewOverride_DSZ 0x09

#define ADDR_cstrike_CViewRender_SetUpView 0x1373D0
// 0x136570
#define ADDR_cstrike_CViewRender_SetUpView_DSZ 0x0d

#define ADDR_cstrike_cl_demoviewoverride 0x4FCAE4
// 0x4A202C

#define ADDR_cstrike_gpGLobals 0x494884
// 0x43B854
#define OFS_cstrike_gpGlobals_value_curtime +4*3

// OFS_cstrike_CvarFloatValue = (unsigned char *)&(Cvar.floatValue) -(unsigned char *)&Cvar;
#define OFS_cstrike_CvarFloatValue 11


unsigned int g_OfsCvarFloatValue;

float GetCvarFloat(void * pcvar)
{
	float * pf = *(float **)pcvar +g_OfsCvarFloatValue;

	float f = *pf;

	return f;
}

void SetCvarFloat(void * pcvar, float value)
{
	float * pf = *(float **)pcvar +g_OfsCvarFloatValue;

	*pf = value;
}


typedef bool (__stdcall *CViewRender_SetUpView_t)(DWORD *this_ptr);

typedef void * CalcDemoViewOverride_t;

CViewRender_SetUpView_t g_Hooked_CViewRender_SetUpView;
CalcDemoViewOverride_t g_Hooked_CalcDemoViewOverride;

void * g_Cl_DemoViewOverride;

float * g_value_curtime;

float g_Old_Cl_DemoViewOverride;


void __stdcall Hooking_CViewRender_SetUpView(DWORD *this_ptr)
{
	g_Old_Cl_DemoViewOverride = GetCvarFloat(g_Cl_DemoViewOverride);

	SetCvarFloat(g_Cl_DemoViewOverride, 1);

	g_Hooked_CViewRender_SetUpView(this_ptr);
	
	SetCvarFloat(g_Cl_DemoViewOverride, g_Old_Cl_DemoViewOverride);
}

void Hooking2_CalcDemoViewOverride(Vector * origin, QAngle * angles)
{
	g_Hook_VClient_RenderView.OnViewOverride(
		origin->x, origin->y, origin->z,
		angles->x, angles->y, angles->z
	);

	if(g_Old_Cl_DemoViewOverride > 0.0f)
	{
		SetCvarFloat(g_Cl_DemoViewOverride, g_Old_Cl_DemoViewOverride);

		__asm
		{
			mov edi, origin
			mov esi, angles
			call g_Hooked_CalcDemoViewOverride
		}

		SetCvarFloat(g_Cl_DemoViewOverride, 1);
	}
}

__declspec(naked) void Hooking_CalcDemoViewOverride()
{
	__asm
	{
		push esi
		push edi
		call Hooking2_CalcDemoViewOverride
		pop edi
		pop esi
		ret
	}
}




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

void Hook_VClient_RenderView::Install(const char * gameDir)
{
	if(StringEndsWith(gameDir, "\\cstrike"))
		Install_cstrike();
	else
		Tier0_Msg("%s is not supported\n", gameDir);
}


void Hook_VClient_RenderView::Install_cstrike(void) {
	if(m_IsInstalled)
		return;

	HMODULE hm = GetModuleHandle("client");

	if(hm) {
		g_Hooked_CViewRender_SetUpView = (CViewRender_SetUpView_t)DetourClassFunc((BYTE *)hm +ADDR_cstrike_CViewRender_SetUpView, (BYTE *)Hooking_CViewRender_SetUpView, ADDR_cstrike_CViewRender_SetUpView_DSZ);
		g_Hooked_CalcDemoViewOverride = (CalcDemoViewOverride_t)DetourApply((BYTE *)hm +ADDR_cstrike_CalcDemoViewOverride, (BYTE *)Hooking_CalcDemoViewOverride, ADDR_cstrike_CalcDemoViewOverride_DSZ);

		g_Cl_DemoViewOverride = (void *)((BYTE *)hm +ADDR_cstrike_cl_demoviewoverride);
		g_value_curtime = (float *)(*(BYTE **)((BYTE *)hm +ADDR_cstrike_gpGLobals) +OFS_cstrike_gpGlobals_value_curtime);

		g_OfsCvarFloatValue = OFS_cstrike_CvarFloatValue;

		m_IsInstalled = true;
	}
}


bool Hook_VClient_RenderView::IsInstalled(void) {
	return m_IsInstalled;
}


void Hook_VClient_RenderView::OnViewOverride(float &Tx, float &Ty, float &Tz, float &Rx, float &Ry, float &Rz) {
	float curTime = GetCurTime();
/*	if(curTime == m_LastTime) {
		curTime += g_VEngineClient->Time() -m_SubTime;
	}
	else
		m_SubTime = g_VEngineClient->Time();
*/
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