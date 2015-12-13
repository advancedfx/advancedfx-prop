#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-12-13 dominik.matrixstorm.com
//
// First changes:
// 2015-12-06 dominik.matrixstorm.com

// WARNING:
//
// This code SHOULD NOT BE USED and is only left here for future redesign:
//
// This code was designed for a single threaded apartment (STA), however
// actually runs in a multi threaded apartment (MTA) and thus is
// not working properly.

#include "csgo_Stdshader_dx9_Hooks.h"

#include "SourceInterfaces.h"
#include "addresses.h"
#include <shared/detours.h>
#include "csgo_ShaderApi_Hooks.h"

#include <string>
#include <map>

class CAfxBasePerMaterialContextDataPiggyBack_csgo
: public CBasePerMaterialContextData_csgo
{
public:
	int m_StaticVshIndex;
	int m_StaticPshIndex;
	std::string m_VertexShaderName;
	std::string m_PixelShaderName;

	CAfxBasePerMaterialContextDataPiggyBack_csgo( void )
	: m_PiggyBack(0)
	, m_StaticVshIndex(-1)
	, m_StaticPshIndex(-1)
	, m_VertexShaderName("(invalid)")
	, m_PixelShaderName("(invalid)")
	{
		m_bMaterialVarsChanged = true;
		m_nVarChangeID = 0xffffffff;
	}

	// virtual destructor so that derived classes can have their own data to be cleaned up on
	// delete of material
	virtual ~CAfxBasePerMaterialContextDataPiggyBack_csgo( void )
	{
		delete m_PiggyBack;
	}

	CBasePerMaterialContextData_csgo * PreUpdatePiggy()
	{
		if(m_PiggyBack)
		{
			m_PiggyBack->m_nVarChangeID = m_bMaterialVarsChanged;
			m_PiggyBack->m_bMaterialVarsChanged = m_bMaterialVarsChanged;
		}

		return m_PiggyBack;
	}

	void PostUpdatePiggy(CBasePerMaterialContextData_csgo * piggy)
	{
		m_PiggyBack = piggy;

		if(m_PiggyBack)
		{
			m_nVarChangeID = m_PiggyBack->m_nVarChangeID;
			m_bMaterialVarsChanged = m_PiggyBack->m_bMaterialVarsChanged;
		}
	}

private:
	CBasePerMaterialContextData_csgo * m_PiggyBack;
};


typedef void (__stdcall *csgo_CShader_OnDrawElements_t)(DWORD *this_ptr, IMaterialVar_csgo **params, IShaderShadow_csgo* pShaderShadow, IShaderDynamicAPI_csgo* pShaderAPI, VertexCompressionType_t_csgo vertexCompression, CBasePerMaterialContextData_csgo **pContextDataPtr);

csgo_CShader_OnDrawElements_t detoured_csgo_VertexLitGeneric_CShader_OnDrawElements;

class CStdshader_dx9_OnDrawElements_Hook
: public IAfxShaderShadowSetVertexShader
, public IAfxShaderShadowSetPixelShader
, public IAfxShaderDynamicAPISetVertexShaderIndex
, public IAfxShaderDynamicAPISetPixelShaderIndex
, public IAfxShaderShadowEnableColorWrites
, public IAfxShaderShadowEnableAlphaWrites
, public IAfxShaderDynamicAPIExecuteCommandBuffer
{
public:
	CStdshader_dx9_OnDrawElements_Hook()
	: m_Hook(0)
	, m_PiggyBack(0)
	{
	}

	void Hook(IOnDrawElements_Hook * hook)
	{
		//Tier0_Msg("CStdshader_dx9_DepthWrite_Hook::Hook(0x%08x);\n", hook);
		
		m_Hook = hook;
	}

	void SetPiggyBack(CAfxBasePerMaterialContextDataPiggyBack_csgo * piggyBack)
	{
		m_PiggyBack = piggyBack;
	}

	virtual	void SetVertexShader(IAfxShaderShadow * caller, const char* pFileName, int nStaticVshIndex )
	{
		//Tier0_Msg("CStdshader_dx9_DepthWrite_Hook::SetVertexShader(...,%s,%i);\n", pFileName, nStaticVshIndex);

		if(m_PiggyBack)
		{
			m_PiggyBack->m_VertexShaderName.assign(pFileName);
			m_PiggyBack->m_StaticVshIndex = nStaticVshIndex;
		}
	}

	virtual	void SetPixelShader(IAfxShaderShadow * caller, const char* pFileName, int nStaticPshIndex = 0 )
	{
		//Tier0_Msg("CStdshader_dx9_DepthWrite_Hook::SetPixelShader(...,%s,%i);\n", pFileName, nStaticPshIndex);

		if(m_PiggyBack)
		{
			m_PiggyBack->m_PixelShaderName.assign(pFileName);
			m_PiggyBack->m_StaticPshIndex = nStaticPshIndex;
		}
	}

	virtual void SetVertexShaderIndex(IAfxShaderDynamicAPI * caller, int vshIndex = -1 )
	{
		//Tier0_Msg("CStdshader_dx9_DepthWrite_Hook::SetVertexShaderIndex(...,%i);\n", vshIndex);

		DoSetVertexShaderIndex(vshIndex);
	}

	virtual void SetPixelShaderIndex(IAfxShaderDynamicAPI * caller, int pshIndex = 0 )
	{
		//Tier0_Msg("CStdshader_dx9_DepthWrite_Hook::SetPixelShaderIndex(...,%i): m_Hook=0x%08,m_PiggyBack=0x%08;\n", pshIndex, m_Hook, m_PiggyBack);

		DoSetPixelShaderIndex(pshIndex);
	}

	virtual bool EnableColorWrites(IAfxShaderShadow * caller,  bool bEnable )
	{
		return bEnable;
	}

	virtual bool EnableAlphaWrites(IAfxShaderShadow * caller,  bool bEnable )
	{
		return bEnable;
	}

	virtual void ExecuteCommandBuffer(IAfxShaderDynamicAPI * caller, uint8 *pCmdBuffer )
	{
		DoExecuteCommandBuffer(pCmdBuffer);
	}


private:
	IOnDrawElements_Hook * m_Hook;
	CAfxBasePerMaterialContextDataPiggyBack_csgo * m_PiggyBack;

	void DoSetVertexShaderIndex(int vshIndex = -1 )
	{
		if(m_Hook && m_PiggyBack)
			m_Hook->SetVertexShader( m_PiggyBack->m_VertexShaderName.c_str(), m_PiggyBack->m_StaticVshIndex, vshIndex);
	}

	void DoSetPixelShaderIndex(int pshIndex = 0 )
	{
		if(m_Hook && m_PiggyBack)
			m_Hook->SetPixelShader( m_PiggyBack->m_PixelShaderName.c_str(), m_PiggyBack->m_StaticPshIndex, pshIndex);
	}


	void DoExecuteCommandBuffer(uint8 *pCmdBuffer)
	{
		if(!(m_Hook && m_PiggyBack && pCmdBuffer))
			// In that case we can't do anything useful anyway.
			return;

		// implemented according to Valve's commandbuilder.h:

		while(true)
		{
			switch(*(int *)pCmdBuffer)
			{
			case CBCMD_END:
				{
					pCmdBuffer += sizeof(int);
				}
				return;
			case CBCMD_JUMP:
				{
					pCmdBuffer += sizeof(int);
					uint8 * ptr = *(uint8 **)pCmdBuffer;
					pCmdBuffer += sizeof(ptr);
					DoExecuteCommandBuffer(ptr);
				}
				return;
			case CBCMD_JSR:
				{
					pCmdBuffer += sizeof(int);
					uint8 * ptr = *(uint8 **)pCmdBuffer;
					pCmdBuffer += sizeof(ptr);
					DoExecuteCommandBuffer(ptr);
				}
				break;
			case CBCMD_SET_PIXEL_SHADER_FLOAT_CONST:
				{
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					int nregs = *(int *)pCmdBuffer;
					pCmdBuffer += sizeof(int);
					pCmdBuffer += nregs * 4 * sizeof(float);
				}
				break;
			case CBCMD_SET_VERTEX_SHADER_FLOAT_CONST:
				{
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					int nregs = *(int *)pCmdBuffer;
					pCmdBuffer += sizeof(int);
					pCmdBuffer += nregs * 4 * sizeof(float);
				}
				break;
			case CBCMD_SETPIXELSHADERFOGPARAMS:
				{
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
				}
				break;
			case CBCMD_STORE_EYE_POS_IN_PSCONST:
				{
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(float);
				}
				break;
			case CBCMD_SET_DEPTH_FEATHERING_CONST:
				{
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(float);
				}
				break;
			case CBCMD_BIND_STANDARD_TEXTURE:
				{
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
				}
				break;
			case CBCMD_BIND_SHADERAPI_TEXTURE_HANDLE:
				{
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
				}
				break;
			case CBCMD_SET_PSHINDEX:
				{
					pCmdBuffer += sizeof(int);
					int nIndex = *(int *)pCmdBuffer;
					DoSetPixelShaderIndex(nIndex);
					pCmdBuffer += sizeof(int);
				}
				break;
			case CBCMD_SET_VSHINDEX:
				{
					pCmdBuffer += sizeof(int);
					int nIndex = *(int *)pCmdBuffer;
					DoSetVertexShaderIndex(nIndex);
					pCmdBuffer += sizeof(int);
				}
				break;
			case CBCMD_SET_VERTEX_SHADER_FLASHLIGHT_STATE:
				{
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
				}
				break;
			case CBCMD_SET_PIXEL_SHADER_FLASHLIGHT_STATE:
				{
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
				}
				break;
			case CBCMD_SET_PIXEL_SHADER_UBERLIGHT_STATE:
				{
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
				}
				break;
			case CBCMD_SET_VERTEX_SHADER_NEARZFARZ_STATE:
				{
					pCmdBuffer += sizeof(int);
					pCmdBuffer += sizeof(int);
				}
				break;
			default:
				Tier0_Warning("AFXERROR: CStdshader_dx9_OnDrawElements_Hook::DoExecuteCommandBuffer: Unknown command #%i.\n", *(int *)pCmdBuffer);
				return;
			}
		}
	}

} g_Stdshader_dx9_Hook;

void __stdcall touring_csgo_VertexLitGeneric_CShader_OnDrawElements(DWORD *this_ptr, IMaterialVar_csgo **params, IShaderShadow_csgo* pShaderShadow, IShaderDynamicAPI_csgo* pShaderAPI, VertexCompressionType_t_csgo vertexCompression, CBasePerMaterialContextData_csgo **pContextDataPtr)
{
	//Tier0_Msg("touring_csgo_VertexLitGeneric_CShader_OnDrawElements(0x%08x,0x%08x,0x%08x,0x%08x,0x%08x,0x%08x):%s (0x%08x)\n",this_ptr, params, pShaderShadow, pShaderAPI, vertexCompression, pContextDataPtr, params[0]->GetOwningMaterial()->GetName(), params[0]->GetOwningMaterial());

	CAfxBasePerMaterialContextDataPiggyBack_csgo * pContextData = reinterpret_cast< CAfxBasePerMaterialContextDataPiggyBack_csgo *> ( *pContextDataPtr );

	if(!pContextData) pContextData = new CAfxBasePerMaterialContextDataPiggyBack_csgo();

	g_Stdshader_dx9_Hook.SetPiggyBack(pContextData);

	CBasePerMaterialContextData_csgo * payLoad = pContextData->PreUpdatePiggy();

	detoured_csgo_VertexLitGeneric_CShader_OnDrawElements(
		this_ptr,
		params,
		Wrap_IShaderShadow_csgo(pShaderShadow, &g_Stdshader_dx9_Hook, &g_Stdshader_dx9_Hook, &g_Stdshader_dx9_Hook, &g_Stdshader_dx9_Hook),
		Wrap_IShaderDynamicAPI_csgo(pShaderAPI, &g_Stdshader_dx9_Hook, &g_Stdshader_dx9_Hook, &g_Stdshader_dx9_Hook),
		vertexCompression,
		&payLoad
	);

	pContextData->PostUpdatePiggy(payLoad);

	*pContextDataPtr = pContextData;
}

void csgo_StdShader_OnDrawElements_Hook(IOnDrawElements_Hook * hook)
{
	g_Stdshader_dx9_Hook.Hook(hook);
}

bool csgo_Stdshader_dx9_Hooks_Init(void)
{
	static bool firstResult = false;
	static bool firstRun = true;
	if(!firstRun) return firstResult;
	firstRun = false;

	if(AFXADDR_GET(csgo_VertexLitGeneric_CShader_OnDrawElements))
	{
		detoured_csgo_VertexLitGeneric_CShader_OnDrawElements = (csgo_CShader_OnDrawElements_t)DetourClassFunc((BYTE *)AFXADDR_GET(csgo_VertexLitGeneric_CShader_OnDrawElements), (BYTE *)touring_csgo_VertexLitGeneric_CShader_OnDrawElements, (int)AFXADDR_GET(csgo_VertexLitGeneric_CShader_OnDrawElements_DSZ));

		firstResult = true;
	}

	return firstResult;
}
