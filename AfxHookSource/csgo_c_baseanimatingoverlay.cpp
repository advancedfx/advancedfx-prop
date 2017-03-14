#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2017-03-14 dominik.matrixstorm.com
//
// First changes:
// 2017-03-12 dominik.matrixstorm.com

#include "csgo_c_baseanimatingoverlay.h"

#include "addresses.h"
#include "SourceInterfaces.h"
#include "RenderView.h"
#include "csgo_MemAlloc.h"

#include <shared/detours.h>

#include <map>

typedef void csgo_CRecvProxyData_t;
typedef void(*csgo_C_BasePlayer_RecvProxy_t)(const csgo_CRecvProxyData_t *pData, void *pStruct, void *pOut);

csgo_C_BasePlayer_RecvProxy_t detoured_csgo_RecvProxy_m_flCycle;
csgo_C_BasePlayer_RecvProxy_t detoured_csgo_RecvProxy_m_flPrevCycle;


int g_csgo_PlayerAnimStateFix = 0;

float mirv_cycle_mod(float value)
{
	if (value < 0)
		while (value < 0) value += 1.0f;
	else
		while (1 < value) value -= 1.0f;

	return value;
}

class CCsgoPlayerAnimStateFix : public ITier0MemAllocFreeNotifyee
{
public:

	/// <returns> If pValue was changed. </returns>
	bool Fix(float * pValue, float * pOut, float & outOrgValue)
	{
		outOrgValue = *pValue;

		float newNetValue = outOrgValue;
		float newEngineValue = *pOut;

		std::pair<std::map<float *, Entry>::iterator, bool> res = m_Map.insert(std::make_pair(pOut, Entry(newNetValue, newEngineValue, 0)));

		if (!res.second)
		{
			// Known.

			if (0.0f != newNetValue)
			{
				float oldNetValue = res.first->second.oldNet;
				res.first->second.oldNet = newNetValue;

				float oldEngineValue = res.first->second.oldEngine;
				res.first->second.oldEngine = newEngineValue;

				float deltaNet = newNetValue >= oldNetValue ? newNetValue - oldNetValue : newNetValue + 1.0f - oldNetValue;

				float centeredNet = (oldNetValue < 0.5f ? oldNetValue : oldNetValue - 1.0f) + 0.5f;
				float centeredEngine = (newEngineValue < 0.5f ? newEngineValue : newEngineValue - 1.0f) + 0.5f;

				float error = centeredNet - centeredEngine;

				float totalError = error + res.first->second.oldError;
				if (1.0f <= fabs(totalError)) totalError = 0; // give up

				float correctionError = 0.4f * error + 0.6f * totalError;

				res.first->second.oldError = totalError;

				float targetDelta = deltaNet + correctionError;
				if (targetDelta < 0) targetDelta = 0.001 * deltaNet; // we'll try to catch up later.

				float targetVal = mirv_cycle_mod(newEngineValue +targetDelta);

				*pValue = targetVal;

				if (1 < g_csgo_PlayerAnimStateFix)
					Tier0_Msg("CCsgoPlayerAnimStateFix::Fix on m_flCycle at 0x%08x: Engine: %f  | Net: %f -> %f (%f) | current error: %f, total error: %f, error correction value: %f | New target value: %f (%f).\n", pOut, newEngineValue, oldNetValue, newNetValue, deltaNet, error, totalError, correctionError, targetVal, targetDelta);

				return true;
			}

			if (1 < g_csgo_PlayerAnimStateFix)
				Tier0_Msg("CCsgoPlayerAnimStateFix::Fix on m_flCycle at 0x%08x: Got %f, RE-STARTING.\n", pOut, newNetValue);

			return false;
		}

		if (1 < g_csgo_PlayerAnimStateFix)
			Tier0_Msg("CCsgoPlayerAnimStateFix::Fix on m_flCycle at 0x%08x: Got %f, STARTING.\n", pOut, newNetValue);

		return false;
	}

	virtual void OnTier0MemAllocFree(void * pMem)
	{
		m_Map.erase((float *)pMem);
	}

private:
	struct Entry
	{
		float oldNet;
		float oldEngine;
		float oldError;

		Entry(float oldNet, float oldEngine, float oldError)
		{
			this->oldNet = oldNet;
			this->oldEngine = oldEngine;
			this->oldError = oldError;
		}
	};

	std::map<float *, Entry> m_Map;

} g_CsgoPlayerAnimStateFix;

void touring_csgo__RecvProxy_m_flCycle(const csgo_CRecvProxyData_t *pData, void *pStruct, void *pOut)
{
	volatile float * pValue = (float *)((char const *)pData + 0x8);

	float oldCycle = *(float *)pOut;
	float newCycle = *pValue;

	bool broken = 0.0f != newCycle && (
		oldCycle > newCycle && 0.5f > oldCycle -newCycle
		|| oldCycle < newCycle && 0.5f < newCycle -oldCycle
	);

	if (g_csgo_PlayerAnimStateFix)
	{
		float orgValue;
		bool fixed = g_CsgoPlayerAnimStateFix.Fix((float *)pValue, (float *)pOut, orgValue);

		detoured_csgo_RecvProxy_m_flCycle(pData, pStruct, pOut);

		if(fixed) *pValue = orgValue;

		//float * p_m_flPlaybackRate = (float *)((char *)pStruct + 0x28);
		//g_csgo_PlayerAnimStateFix_origValues.emplace_back(p_m_flPlaybackRate); // we want to get at m_flPlayBackRate later on :-)

		return;
	}
	
	if(broken)
	{
	
		Tier0_Warning("touring_csgo__RecvProxy_m_flCycle: HLAE detected cycle inconsistency at 0x%08x: %f -> %f! You may want to use \"mirv_fix playerAnimState 1\"!\n", pOut, oldCycle, newCycle);
	}

	detoured_csgo_RecvProxy_m_flCycle(pData, pStruct, pOut);
}

void touring_csgo__RecvProxy_m_flPrevCycle(const csgo_CRecvProxyData_t *pData, void *pStruct, void *pOut)
{
	detoured_csgo_RecvProxy_m_flPrevCycle(pData, pStruct, pOut);
}


void Enable_csgo_PlayerAnimStateFix_set(int value)
{
	if (value < 0) value = 0;
	else if (2 < value) value = 2;

	g_csgo_PlayerAnimStateFix = value;
}

int Enable_csgo_PlayerAnimStateFix_get(void)
{
	return g_csgo_PlayerAnimStateFix;
}

typedef void * csgo_mystique_animation_t;

csgo_mystique_animation_t detoured_csgo_mystique_animation;

extern bool g_csgo_FirstFrameAfterNetUpdateEnd;

float g_csgo_mystique_annimation_factor = 0.0f;

void __stdcall touring_csgo_mystique_animation(DWORD * this_ptr, DWORD arg0, float argXmm1, float argXmm2)
{
	//volatile float * mystique = (float *)((char *)this_ptr + 0x98); // new cycle value
	//volatile float * p_m_flCycle = (float *)((char*)this_ptr + 0x4FC);
	volatile float * p_m_unk_flLastCurTime = (float *)((char*)this_ptr + 0x6c);
	volatile bool * p_m_somecheck = (bool *)((char*)this_ptr + 0x50);

	if (g_csgo_PlayerAnimStateFix)
	{

		//float newTime = *p_m_unk_flLastCurTime + g_csgo_mystique_annimation_factor;

		//if (1 < g_csgo_PlayerAnimStateFix) Tier0_Msg("touring_csgo_mystique_animation (probably CBasePlayerAnimState::Update) on 0x%08x: p_m_unk_flLastCurTime: %f -> %f (curtime=%f)", this_ptr, *p_m_unk_flLastCurTime, newTime, g_Hook_VClient_RenderView.GetGlobals()->curtime_get());

		//*p_m_unk_flLastCurTime = newTime;
	}

	{
		__asm mov ecx, this_ptr
		__asm push arg0
		__asm movss xmm2, argXmm2
		__asm movss xmm1, argXmm1
		__asm call detoured_csgo_mystique_animation
	}

	if (g_csgo_PlayerAnimStateFix)
	{
		//if (1 < g_csgo_PlayerAnimStateFix) Tier0_Msg(" --> afterwards: %f\n", *p_m_unk_flLastCurTime);
	}
}

void __declspec(naked) naked_touring_csgo_mystique_animation(void)
{
	__asm push ebp
	__asm mov ebp, esp
	__asm push ecx

	__asm sub esp, 4
	__asm movss dword ptr[esp], xmm2
	__asm sub esp, 4
	__asm movss dword ptr[esp], xmm1
	__asm mov eax, [ebp + 8]
	__asm push eax
	__asm push ecx
	__asm call touring_csgo_mystique_animation

	__asm pop ecx
	__asm mov esp, ebp
	__asm pop ebp

	__asm ret 4
}

bool Hook_csgo_PlayerAnimStateFix(void)
{
	static bool firstResult = false;
	static bool firstRun = true;
	if (!firstRun) return firstResult;
	firstRun = false;

	if (AFXADDR_GET(csgo_DT_Animationlayer_m_flCycle_fn) && AFXADDR_GET(csgo_DT_Animationlayer_m_flPrevCycle_fn) && AFXADDR_GET(csgo_mystique_animation))
	{
		//MdtMemBlockInfos mbis;

		csgo_C_BasePlayer_RecvProxy_t * pMovArgFn;

		pMovArgFn = *(csgo_C_BasePlayer_RecvProxy_t **)AFXADDR_GET(csgo_DT_Animationlayer_m_flCycle_fn);
		//MdtMemAccessBegin(pMovArgFn, sizeof(void *), &mbis);
		detoured_csgo_RecvProxy_m_flCycle = *pMovArgFn;
		*pMovArgFn = touring_csgo__RecvProxy_m_flCycle;
		//MdtMemAccessEnd(&mbis);

		pMovArgFn = *(csgo_C_BasePlayer_RecvProxy_t **)AFXADDR_GET(csgo_DT_Animationlayer_m_flPrevCycle_fn);
		//MdtMemAccessBegin(pMovArgFn, sizeof(void *), &mbis);
		detoured_csgo_RecvProxy_m_flPrevCycle = *pMovArgFn;
		*pMovArgFn = touring_csgo__RecvProxy_m_flPrevCycle;
		//MdtMemAccessEnd(&mbis);

		detoured_csgo_mystique_animation = DetourApply((BYTE *)AFXADDR_GET(csgo_mystique_animation), (BYTE *)naked_touring_csgo_mystique_animation, 0x0a);

		firstResult = true;
	}

	return firstResult;
}


