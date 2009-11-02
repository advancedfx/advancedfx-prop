#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-11-01 by dominik.matrixstorm.com
//
// First changes:
// 2009-11-01 by dominik.matrixstorm.com

#include "RenderView.h"

#include <hooks/shared/detours.h>

#include <hooks/shared/bvhimport.h>
#include <hooks/shared/bvhexport.h>

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

// void * gpGlobals;

//#define CLOFS_CallCalcDemoViewOverride 0x169A7D

#define CLOFS_IfIsPlayingTimeDemo 0x169A65
#define CLOFS_CheckDemoviewOverride (CLOFS_IfIsPlayingTimeDemo +0x16)
#define CLOFS_NotPlaying (CLOFS_IfIsPlayingTimeDemo +0x51)
#define CLOFS_cl_demoviewoverride_value (0x3EE7F0 +0x28)
#define CLOFS_gpGLobals 0x392C8C
#define OFS_gpGlobals_value_curtime +4*3

#define OPCODE_NOP			0x90
#define OPCODE_JMP			0xE9
#define OPCODE_JMP32_SZ		5

DWORD g_continue_NotPlaying;
DWORD g_continue_CheckDemoViewOverrideCvar;
float * g_value_cl_demoviewoverride;
float * g_value_curtime;

void __cdecl myViewOverride( Vector *origin, QAngle *angles ) {
	g_Hook_VClient_RenderView.OnViewOverride(
		origin->x, origin->y, origin->z,
		angles->x, angles->y, angles->z
	);
}

bool __cdecl myCheckDemoViewOverride() {
	return 0.0f >= *g_value_cl_demoviewoverride;
}

__declspec(naked) void hook_IfDemoviewOverride() {
	__asm {
		; store IfIsPlayingTimeDemo:
		MOV eax, 0
		JZ __Continue
		MOV eax, 1
	
	__Continue:
		PUSH eax

		PUSH esi
		PUSH edi
		CALL myViewOverride
		POP edi
		POP esi

		; handle IfIsPlayingTimeDemo:
		POP eax
		TEST eax, eax
		JNZ __Playing
		JMP g_continue_NotPlaying

		__Playing:

		CALL myCheckDemoViewOverride;
		TEST eax, eax
		JMP g_continue_CheckDemoViewOverrideCvar
	}
}



// Hook_VClient_RenderView /////////////////////////////////////////////////////

Hook_VClient_RenderView::Hook_VClient_RenderView() {
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

bool Hook_VClient_RenderView::ExportBegin(char const *fileName, float frameTime) {
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

bool Hook_VClient_RenderView::ImportBegin(char const *fileName) {
	ImportEnd();

	m_Import = g_BvhImport.LoadMotionFile(fileName);

	return m_Import;
}

void Hook_VClient_RenderView::ImportEnd() {
	g_BvhImport.CloseMotionFile();

	m_Import = false;
}


void Hook_VClient_RenderView::Install_cstrike(void) {
	if(m_IsInstalled)
		return;

	HMODULE hm = GetModuleHandle("client");

	if(hm) {
		MdtMemBlockInfos mbis;

		BYTE * pmem = (BYTE *)hm +CLOFS_IfIsPlayingTimeDemo;

		g_continue_NotPlaying = (DWORD)((BYTE *)hm +CLOFS_NotPlaying);
		g_continue_CheckDemoViewOverrideCvar = (DWORD)((BYTE *)hm +CLOFS_CheckDemoviewOverride);
		g_value_cl_demoviewoverride = (float *)((BYTE *)hm +CLOFS_cl_demoviewoverride_value);
		g_value_curtime = (float *)(*(BYTE **)((BYTE *)hm +CLOFS_gpGLobals) +OFS_gpGlobals_value_curtime);

		MdtMemAccessBegin(pmem, CLOFS_CheckDemoviewOverride -CLOFS_IfIsPlayingTimeDemo, &mbis);

		memset(pmem, OPCODE_NOP, CLOFS_CheckDemoviewOverride -CLOFS_IfIsPlayingTimeDemo);

		pmem[0] = OPCODE_JMP;
		*(DWORD*)(pmem+1) = (DWORD)((BYTE *)&hook_IfDemoviewOverride -pmem) - OPCODE_JMP32_SZ;

		MdtMemAccessEnd(&mbis);

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