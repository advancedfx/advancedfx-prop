#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-12-07 dominik.matrixstorm.com
//
// First changes:
// 2015-12-06 dominik.matrixstorm.com

#include "csgo_Stdshader_dx9_Hooks.h"

#include "SourceInterfaces.h"
#include "addresses.h"
#include <shared/detours.h>
#include "csgo_ShaderApi_Hooks.h"

#include <string>
#include <map>

class CBasePerMaterialContextData_csgo;

typedef void (__stdcall *csgo_DepthWrite_CShader_OnDrawElements_t)(DWORD *this_ptr, IMaterialVar_csgo **params, IShaderShadow_csgo* pShaderShadow, IShaderDynamicAPI_csgo* pShaderAPI, VertexCompressionType_t_csgo vertexCompression, CBasePerMaterialContextData_csgo **pContextDataPtr);

csgo_DepthWrite_CShader_OnDrawElements_t detoured_csgo_DepthWrite_CShader_OnDrawElements;

class CStdshader_dx9_DepthWrite_Hook
: public IAfxShaderShadowSetVertexShader
, public IAfxShaderShadowSetPixelShader
, public IAfxShaderDynamicAPISetVertexShaderIndex
, public IAfxShaderDynamicAPISetPixelShaderIndex
, public IAfxShaderShadowEnableColorWrites
, public IAfxShaderShadowEnableAlphaWrites
{
public:
	CStdshader_dx9_DepthWrite_Hook()
	: m_Hook(0)
	{
	}

	void Hook(IOnDrawElements_Hook * hook)
	{
		//Tier0_Msg("CStdshader_dx9_DepthWrite_Hook::Hook(0x%08x);\n", hook);
		
		m_Hook = hook;
	}

	virtual	void SetVertexShader(IAfxShaderShadow * caller, const char* pFileName, int nStaticVshIndex )
	{
		//Tier0_Msg("CStdshader_dx9_DepthWrite_Hook::SetVertexShader(...,%s,%i);\n", pFileName, nStaticVshIndex);

		if(m_Hook) m_Hook->SetVertexShader(pFileName, nStaticVshIndex);
	}

	virtual	void SetPixelShader(IAfxShaderShadow * caller, const char* pFileName, int nStaticPshIndex = 0 )
	{
		//Tier0_Msg("CStdshader_dx9_DepthWrite_Hook::SetPixelShader(...,%s,%i);\n", pFileName, nStaticPshIndex);

		if(m_Hook) m_Hook->SetPixelShader(pFileName, nStaticPshIndex);
	}

	virtual void SetVertexShaderIndex(IAfxShaderDynamicAPI * caller, int vshIndex = -1 )
	{
		//Tier0_Msg("CStdshader_dx9_DepthWrite_Hook::SetVertexShaderIndex(...,%i);\n", vshIndex);

		if(m_Hook) m_Hook->SetVertexShaderIndex(vshIndex);
	}

	virtual void SetPixelShaderIndex(IAfxShaderDynamicAPI * caller, int pshIndex = 0 )
	{
		//Tier0_Msg("CStdshader_dx9_DepthWrite_Hook::SetPixelShaderIndex(...,%i);\n", pshIndex);

		if(m_Hook) m_Hook->SetPixelShaderIndex(pshIndex);
	}

	virtual bool EnableColorWrites(IAfxShaderShadow * caller,  bool bEnable )
	{
		return m_Hook ? m_Hook->EnableColorWrites(bEnable) : bEnable;
	}

	virtual bool EnableAlphaWrites(IAfxShaderShadow * caller,  bool bEnable )
	{
		return m_Hook ? m_Hook->EnableAlphaWrites(bEnable) : bEnable;
	}

private:
	IOnDrawElements_Hook * m_Hook;

} g_Stdshader_dx9_DepthWrite_Hook;

void __stdcall touring_csgo_DepthWrite_CShader_OnDrawElements(DWORD *this_ptr, IMaterialVar_csgo **params, IShaderShadow_csgo* pShaderShadow, IShaderDynamicAPI_csgo* pShaderAPI, VertexCompressionType_t_csgo vertexCompression, CBasePerMaterialContextData_csgo **pContextDataPtr)
{
	//Tier0_Msg("touring_csgo_DepthWrite_CShader_OnDrawElements(0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x):%s (0x%08x)\n",this_ptr, params, pShaderShadow, pShaderAPI, vertexCompression, pContextDataPtr, params[0]->GetOwningMaterial()->GetName(), params[0]->GetOwningMaterial());

	detoured_csgo_DepthWrite_CShader_OnDrawElements(
		this_ptr,
		params,
		Wrap_IShaderShadow_csgo(pShaderShadow, &g_Stdshader_dx9_DepthWrite_Hook, &g_Stdshader_dx9_DepthWrite_Hook, &g_Stdshader_dx9_DepthWrite_Hook, &g_Stdshader_dx9_DepthWrite_Hook),
		Wrap_IShaderDynamicAPI_csgo(pShaderAPI, &g_Stdshader_dx9_DepthWrite_Hook, &g_Stdshader_dx9_DepthWrite_Hook),
		vertexCompression,
		pContextDataPtr
	);
}

void csgo_DepthWrite_Hook(IOnDrawElements_Hook * hook)
{
	g_Stdshader_dx9_DepthWrite_Hook.Hook(hook);
}

void csgo_DepthWrite_Unhook()
{
	g_Stdshader_dx9_DepthWrite_Hook.Hook(0);
}

bool csgo_Stdshader_dx9_Hooks_Init(void)
{
	static bool firstResult = false;
	static bool firstRun = true;
	if(!firstRun) return firstResult;
	firstRun = false;

	if(AFXADDR_GET(csgo_DepthWrite_CShader_OnDrawElements))
	{
		detoured_csgo_DepthWrite_CShader_OnDrawElements = (csgo_DepthWrite_CShader_OnDrawElements_t)DetourClassFunc((BYTE *)AFXADDR_GET(csgo_DepthWrite_CShader_OnDrawElements), (BYTE *)touring_csgo_DepthWrite_CShader_OnDrawElements, (int)AFXADDR_GET(csgo_DepthWrite_CShader_OnDrawElements_DSZ));

		firstResult = true;
	}

	return firstResult;
}
