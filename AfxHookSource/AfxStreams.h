#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-01-02 dominik.matrixstorm.com
//
// First changes:
// 2015-06-26 dominik.matrixstorm.com

#include "SourceInterfaces.h"
#include "AfxInterfaces.h"
#include "AfxClasses.h"
#include "WrpConsole.h"
#include "d3d9Hooks.h"
#include "AfxShaders.h"
#include "csgo_Stdshader_dx9_Hooks.h"
#include <shaders/build/afxHook_vertexlit_and_unlit_generic_ps20.h>
#include <shaders/build/afxHook_vertexlit_and_unlit_generic_ps20b.h>
#include <shaders/build/afxHook_vertexlit_and_unlit_generic_ps30.h>

#include <string>
#include <list>
#include <queue>
#include <map>

class CAfxRecordStream;
class CAfxRenderViewStream;

class CAfxStream
{
public:
	CAfxStream();

	virtual ~CAfxStream();

	virtual CAfxStream * AsAfxStream(void) { return this; }
	virtual CAfxRecordStream * AsAfxRecordStream(void) { return 0; }
	virtual CAfxRenderViewStream * AsAfxRenderViewStream(void) { return 0; }

	virtual void LevelShutdown(IAfxStreams4Stream * streams);
};

class CAfxDeveloperStream;
class CAfxBaseFxStream;

class CAfxRenderViewStream
: public CAfxStream
{
public:
	enum StreamRenderType
	{
		SRT_RenderView,
		SRT_Depth
	};

	CAfxRenderViewStream();

	virtual CAfxRenderViewStream * AsAfxRenderViewStream(void) { return this; }

	virtual CAfxDeveloperStream * AsAfxDeveloperStream(void) { return 0; }
	virtual CAfxBaseFxStream * AsAfxBaseFxStream(void) { return 0; }

	virtual void StreamAttach(IAfxStreams4Stream * streams);
	virtual void StreamDetach(IAfxStreams4Stream * streams);

	char const * AttachCommands_get(void);
	void AttachCommands_set(char const * value);

	char const * DetachCommands_get(void);
	void DetachCommands_set(char const * value);

	bool DrawHud_get(void);
	void DrawHud_set(bool value);

	bool DrawViewModel_get(void);
	void DrawViewModel_set(bool value);

	StreamRenderType StreamRenderType_get(void);
	void StreamRenderType_set(StreamRenderType value);

protected:
	/// <summary>This member is only valid between StreamAttach and StreamDetach.</summary>
	IAfxStreams4Stream * m_Streams;

	StreamRenderType m_StreamRenderType;

private:
	bool m_DrawViewModel;
	bool m_DrawHud;
	std::string m_AttachCommands;
	std::string m_DetachCommands;
};

class CAfxSingleStream;
class CAfxTwinStream;

class CAfxRecordStream
: public CAfxStream
{
public:
	CAfxRecordStream(char const * streamName);

	virtual CAfxRecordStream * AsAfxRecordStream(void) { return this; }

	virtual CAfxSingleStream * AsAfxSingleStream(void) { return 0; }
	virtual CAfxTwinStream * AsAfxTwinStream(void) { return 0; }

	char const * StreamName_get(void);

	bool Record_get(void);
	void Record_set(bool value);

	/// <remarks>This is called regardless of Record value.</remarks>
	void RecordStart();

	/// <remarks>This is only called between RecordStart and RecordEnd and only if Record is true.</remarks>
	bool CreateCapturePath(const std::wstring & takeDir, int frameNumber, bool isBmpAndNotTga, std::wstring &outPath);

	/// <remarks>This is called regardless of Record value.</remarks>
	void RecordEnd();

private:
	std::string m_StreamName;
	std::wstring m_CapturePath;
	bool m_Record;
	bool m_TriedCreatePath;
	bool m_SucceededCreatePath;
};

class CAfxSingleStream
:  public CAfxRecordStream
{
public:
	CAfxSingleStream(char const * streamName, CAfxRenderViewStream * stream);

	virtual ~CAfxSingleStream();

	virtual CAfxSingleStream * AsAfxSingleStream(void) { return this; }

	virtual void LevelShutdown(IAfxStreams4Stream * streams);

	CAfxRenderViewStream * Stream_get(void);

private:
	CAfxRenderViewStream * m_Stream;

};

class CAfxTwinStream
: public CAfxRecordStream
{
public:
	enum StreamCombineType
	{
		SCT_ARedAsAlphaBColor,
		SCT_AColorBRedAsAlpha
	};

	/// <remarks>Takes owenership of given streams and deletes them when this stream is deleted!.</remarks>
	CAfxTwinStream(char const * streamName, CAfxRenderViewStream * streamA, CAfxRenderViewStream * streamB, StreamCombineType streamCombineType);

	virtual ~CAfxTwinStream();

	virtual CAfxTwinStream * AsAfxTwinStream(void) { return this; }

	virtual void LevelShutdown(IAfxStreams4Stream * streams);

	CAfxRenderViewStream * StreamA_get();
	CAfxRenderViewStream * StreamB_get();

	StreamCombineType StreamCombineType_get(void);
	void StreamCombineType_set(StreamCombineType value);

private:
	CAfxRenderViewStream * m_StreamA;
	CAfxRenderViewStream * m_StreamB;
	StreamCombineType m_StreamCombineType;
};

class CAfxDeveloperStream
: public CAfxRenderViewStream
, public IAfxMatRenderContextBind
, public IAfxMatRenderContextDrawInstances
, public IAfxMeshDraw
, public IAfxMeshDraw_2
, public IAfxMeshDrawModulated
{
public:
	CAfxDeveloperStream();
	virtual ~CAfxDeveloperStream();

	virtual CAfxDeveloperStream * AsAfxDeveloperStream(void) { return this; }

	void MatchName_set(char const * value);
	char const * MatchName_get(void);

	void MatchTextureGroupName_set(char const * value);
	char const * MatchTextureGroupName_get(void);

	void ReplaceName_set(char const * name);
	char const * ReplaceName_get(void);
	
	void BlockDraw_set(bool value);
	bool BlockDraw_get(void);

	virtual void StreamAttach(IAfxStreams4Stream * streams);
	virtual void StreamDetach(IAfxStreams4Stream * streams);

	virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );
	virtual void DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance );

	virtual void Draw(IAfxMesh * am, int firstIndex = -1, int numIndices = 0);
	virtual void Draw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists);
	virtual void DrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex = -1, int numIndices = 0 );

private:
	std::string m_MatchTextureGroupName;
	std::string m_MatchName;
	std::string m_ReplaceName;
	bool m_ReplaceUpdate;
	bool m_Replace;
	CAfxMaterial * m_ReplaceMaterial;
	bool m_ReplaceMaterialActive;
	bool m_BlockDraw;
};

extern bool g_DebugEnabled;

class CAfxBaseFxStream
: public CAfxRenderViewStream
, public IAfxMatRenderContextBind
, public IAfxMatRenderContextDrawInstances
, public IAfxMeshDraw
, public IAfxMeshDraw_2
, public IAfxMeshDrawModulated
, public IAfxSetVertexShader
, public IAfxSetPixelShader
{
public:
	enum AfxAction {
		AA_NotSet,
		AA_Draw,
		AA_NoDraw,
		AA_Invisible,
		AA_DrawDepth,
		AA_DrawDepth24,
		AA_GreenScreen,
		AA_Black,
		AA_White,
		AA_DebugDump
	};

	CAfxBaseFxStream();

	virtual ~CAfxBaseFxStream();

	virtual CAfxBaseFxStream * AsAfxBaseFxStream(void) { return this; }

	virtual void LevelShutdown(IAfxStreams4Stream * streams);

	virtual void StreamAttach(IAfxStreams4Stream * streams);
	virtual void StreamDetach(IAfxStreams4Stream * streams);

	virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );
	virtual void DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance );

	virtual void Draw(IAfxMesh * am, int firstIndex = -1, int numIndices = 0);
	virtual void Draw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists);
	virtual void DrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex = -1, int numIndices = 0 );

	virtual void SetVertexShader(const char* pFileName, int nStaticVshIndex, int vshIndex = -1);
	virtual void SetPixelShader(const char* pFileName, int nStaticPshIndex = 0, int pshIndex = 0);

	AfxAction ClientEffectTexturesAction_get(void);
	void ClientEffectTexturesAction_set(AfxAction value);

	AfxAction WorldTexturesAction_get(void);
	void WorldTexturesAction_set(AfxAction value);

	AfxAction SkyBoxTexturesAction_get(void);
	void SkyBoxTexturesAction_set(AfxAction value);

	AfxAction StaticPropTexturesAction_get(void);
	void StaticPropTexturesAction_set(AfxAction value);

	AfxAction CableAction_get(void);
	void CableAction_set(AfxAction value);

	AfxAction PlayerModelsAction_get(void);
	void PlayerModelsAction_set(AfxAction value);

	AfxAction WeaponModelsAction_get(void);
	void WeaponModelsAction_set(AfxAction value);

	AfxAction StattrackAction_get(void);
	void StattrackAction_set(AfxAction value);

	AfxAction ShellModelsAction_get(void);
	void ShellModelsAction_set(AfxAction value);

	AfxAction OtherModelsAction_get(void);
	void OtherModelsAction_set(AfxAction value);

	AfxAction DecalTexturesAction_get(void);
	void DecalTexturesAction_set(AfxAction value);

	AfxAction EffectsAction_get(void);
	void EffectsAction_set(AfxAction value);

	AfxAction ShellParticleAction_get(void);
	void ShellParticleAction_set(AfxAction value);

	AfxAction OtherParticleAction_get(void);
	void OtherParticleAction_set(AfxAction value);

	AfxAction StickerAction_get(void);
	void StickerAction_set(AfxAction value);

	AfxAction ErrorMaterialAction_get(void);
	void ErrorMaterialAction_set(AfxAction value);

	bool TestAction_get(void);
	void TestAction_set(bool value);

	float DepthVal_get(void);
	void DepthVal_set(float value);

	float DepthValMax_get(void);
	void DepthValMax_set(float value);
	
	bool DebugPrint_get(void);
	void DebugPrint_set(bool value);

	void InvalidateMap(void);

protected:
	AfxAction m_ClientEffectTexturesAction;
	AfxAction m_WorldTexturesAction;
	AfxAction m_SkyBoxTexturesAction;
	AfxAction m_StaticPropTexturesAction;
	AfxAction m_CableAction;
	AfxAction m_PlayerModelsAction;
	AfxAction m_WeaponModelsAction;
	AfxAction m_StattrackAction;
	AfxAction m_ShellModelsAction;
	AfxAction m_OtherModelsAction;
	AfxAction m_DecalTexturesAction;
	AfxAction m_EffectsAction;
	AfxAction m_ShellParticleAction;
	AfxAction m_OtherParticleAction;
	AfxAction m_StickerAction;
	AfxAction m_ErrorMaterialAction;
	bool m_TestAction;
	float m_DepthVal;
	float m_DepthValMax;

private:
	class CAction
	: public IAfxMatRenderContextBind
	, public IAfxMatRenderContextDrawInstances
	, public IAfxMeshDraw
	, public IAfxMeshDraw_2
	, public IAfxMeshDrawModulated
	{
	public:
		CAction(CAfxBaseFxStream * parentStream)
		{
			m_ParentStream = parentStream;
		}

		void AddRef(void)
		{
			++m_RefCount;
		}

		void Release(void)
		{
			--m_RefCount;
			if(0 == m_RefCount)
				delete this;
		}

		virtual void AfxUnbind(IAfxMatRenderContext * ctx)
		{
		}

		virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 )
		{
			ctx->GetParent()->Bind(material, proxyData);
		}

		virtual void DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
		{
			ctx->GetParent()->DrawInstances(nInstanceCount, pInstance);
		}

		virtual void Draw(IAfxMesh * am, int firstIndex = -1, int numIndices = 0)
		{
			am->GetParent()->Draw(firstIndex, numIndices);
		}

		virtual void Draw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists)
		{
			am->GetParent()->Draw(pLists, nLists);
		}

		virtual void DrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex = -1, int numIndices = 0 )
		{
			am->GetParent()->DrawModulated(vecDiffuseModulation, firstIndex, numIndices);
		}

		virtual void SetVertexShader(const char* pFileName, int nStaticVshIndex, int vshIndex = -1)
		{
		}

		virtual void SetPixelShader(const char* pFileName, int nStaticPshIndex = 0, int pshIndex = 0)
		{
		}

	protected:
		CAfxBaseFxStream * m_ParentStream;

		virtual ~CAction()
		{
		}

	private:
		int m_RefCount;
	};

	class CActionDebugDump
	: public CAction
	{
	public:
		CActionDebugDump(CAfxBaseFxStream * parentStream)
		: CAction(parentStream)
		{
		}

		virtual void AfxUnbind(IAfxMatRenderContext * ctx)
		{
			m_ParentStream->m_Streams->DebugDump();
		}
	};

	class CActionMatte
	: public CAction
	{
	public:
		CActionMatte(CAfxBaseFxStream * parentStream, IAfxFreeMaster * freeMaster, IMaterialSystem_csgo * matSystem)
		: CAction(parentStream)
		, m_MatteMaterial(freeMaster, matSystem->FindMaterial("afx/greenmatte",NULL))
		{
		}

		virtual void AfxUnbind(IAfxMatRenderContext * ctx);

		virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );

		virtual void DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
		{
			MeshInstanceData_t_csgo * first = const_cast<MeshInstanceData_t_csgo *>(pInstance);

			for(int i = 0; i < nInstanceCount; ++i)
			{
				first->m_DiffuseModulation.x = 0.0;
				first->m_DiffuseModulation.y = 1.0;
				first->m_DiffuseModulation.z = 0.0;
				first->m_DiffuseModulation.w = 1.0;

				++first;
			}

			ctx->GetParent()->DrawInstances(nInstanceCount, pInstance);
		}

	private:
		CAfxMaterial m_MatteMaterial;
	};

	class CActionBlack
	: public CAction
	{
	public:
		CActionBlack(CAfxBaseFxStream * parentStream, IAfxFreeMaster * freeMaster, IMaterialSystem_csgo * matSystem)
		: CAction(parentStream)
		, m_Material(freeMaster, matSystem->FindMaterial("afx/black",NULL))
		{
		}

		virtual void AfxUnbind(IAfxMatRenderContext * ctx);

		virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );

		virtual void DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
		{
			MeshInstanceData_t_csgo * first = const_cast<MeshInstanceData_t_csgo *>(pInstance);

			for(int i = 0; i < nInstanceCount; ++i)
			{
				first->m_DiffuseModulation.x = 0.0;
				first->m_DiffuseModulation.y = 0.0;
				first->m_DiffuseModulation.z = 0.0;
				//first->m_DiffuseModulation.w = 1.0;

				++first;
			}

			ctx->GetParent()->DrawInstances(nInstanceCount, pInstance);
		}

	private:
		CAfxMaterial m_Material;
	};

	class CActionWhite
	: public CAction
	{
	public:
		CActionWhite(CAfxBaseFxStream * parentStream, IAfxFreeMaster * freeMaster, IMaterialSystem_csgo * matSystem)
		: CAction(parentStream)
		, m_Material(freeMaster, matSystem->FindMaterial("afx/white",NULL))
		{
		}

		virtual void AfxUnbind(IAfxMatRenderContext * ctx);

		virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );

		virtual void DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
		{
			MeshInstanceData_t_csgo * first = const_cast<MeshInstanceData_t_csgo *>(pInstance);

			for(int i = 0; i < nInstanceCount; ++i)
			{
				first->m_DiffuseModulation.x = 1.0;
				first->m_DiffuseModulation.y = 1.0;
				first->m_DiffuseModulation.z = 1.0;
				//first->m_DiffuseModulation.w = 1.0;

				++first;
			}

			ctx->GetParent()->DrawInstances(nInstanceCount, pInstance);
		}

	private:
		CAfxMaterial m_Material;
	};


	class CActionAfxVertexLitGenericHookKey
	{
	public:
		ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::AFXMODE_e AFXMODE;
		float AlphaTestReference;

		CActionAfxVertexLitGenericHookKey()
		{
		}

		CActionAfxVertexLitGenericHookKey(
			ShaderCombo_afxHook_vertexlit_and_unlit_generic_ps30::AFXMODE_e a_AFXMODE,
			float a_AlphaTestReference)
		: AFXMODE(a_AFXMODE)
		, AlphaTestReference(a_AlphaTestReference)
		{
		}

		CActionAfxVertexLitGenericHookKey(const CActionAfxVertexLitGenericHookKey & x)
		: AFXMODE(x.AFXMODE)
		, AlphaTestReference(x.AlphaTestReference)
		{
		}
		
		bool operator < (const CActionAfxVertexLitGenericHookKey & y) const
		{
			if(this->AFXMODE < y.AFXMODE)
				return true;

			return this->AFXMODE == y.AFXMODE && this->AlphaTestReference < y.AlphaTestReference;
		}
	};

	class CActionAfxVertexLitGenericHook
	: public CAction
	{
	public:
		CActionAfxVertexLitGenericHook(CAfxBaseFxStream * parentStream, CActionAfxVertexLitGenericHookKey & key);

		virtual void AfxUnbind(IAfxMatRenderContext * ctx);

		virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );

		virtual void SetPixelShader(const char* pFileName, int nStaticPshIndex = 0, int pshIndex = 0);

	private:
		static csgo_Stdshader_dx9_Combos_vertexlit_and_unlit_generic_ps20 m_Combos_ps20;
		static csgo_Stdshader_dx9_Combos_vertexlit_and_unlit_generic_ps20b m_Combos_ps20b;
		static csgo_Stdshader_dx9_Combos_vertexlit_and_unlit_generic_ps30 m_Combos_ps30;
		CActionAfxVertexLitGenericHookKey m_Key;
	};

	class CActionUnlitGenericFallback
	: public CActionAfxVertexLitGenericHook
	{
	public:
		CActionUnlitGenericFallback(CAfxBaseFxStream * parentStream, CActionAfxVertexLitGenericHookKey & key, IAfxFreeMaster * freeMaster, IMaterialSystem_csgo * matSystem, char const * unlitGenericFallbackMaterialName);

		virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );

	private:
		CAfxMaterial m_Material;
	};

	class CActionDepth
	: public CAction
	{
	public:
		CActionDepth(CAfxBaseFxStream * parentStream, IAfxFreeMaster * freeMaster, IMaterialSystem_csgo * matSystem)
		: CAction(parentStream)
		, m_DepthMaterial(freeMaster, matSystem->FindMaterial("afx/depth",NULL))
		, m_DepthValRef("mat_debugdepthval")
		, m_DepthValMaxRef("mat_debugdepthvalmax")
		{
		}

		virtual void AfxUnbind(IAfxMatRenderContext * ctx);

		virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );

	private:
		CAfxMaterial m_DepthMaterial;
		WrpConVarRef m_DepthValRef;
		WrpConVarRef m_DepthValMaxRef;
	};

	class CActionInvisible
	: public CAction
	{
	public:
		CActionInvisible(CAfxBaseFxStream * parentStream, IAfxFreeMaster * freeMaster, IMaterialSystem_csgo * matSystem)
		: CAction(parentStream)
		, m_InvisibleMaterial(freeMaster, matSystem->FindMaterial("afx/invisible",NULL))
		{
		}

		virtual void AfxUnbind(IAfxMatRenderContext * ctx);

		virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );

		virtual void DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
		{
			MeshInstanceData_t_csgo * first = const_cast<MeshInstanceData_t_csgo *>(pInstance);

			for(int i = 0; i < nInstanceCount; ++i)
			{
				first->m_DiffuseModulation.x = 0.0;
				first->m_DiffuseModulation.y = 0.0;
				first->m_DiffuseModulation.z = 0.0;
				first->m_DiffuseModulation.w = 0.0;

				++first;
			}

			ctx->GetParent()->DrawInstances(nInstanceCount, pInstance);
		}

	private:
		CAfxMaterial m_InvisibleMaterial;
	};

	class CActionNoDraw
	: public CAction
	{
	public:
		CActionNoDraw(CAfxBaseFxStream * parentStream)
		: CAction(parentStream)
		{
		}

		virtual void Draw(IAfxMesh * am, int firstIndex = -1, int numIndices = 0)
		{
			am->GetParent()->MarkAsDrawn();
		}

		virtual void Draw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists)
		{
			am->GetParent()->MarkAsDrawn();
		}

		virtual void DrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex = -1, int numIndices = 0 )
		{
			am->GetParent()->MarkAsDrawn();
		}

		virtual void DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
		{
			return;
		}

	};

	CAction * m_CurrentAction;
	CAction * m_DepthAction;
	CAction * m_Depth24Action;
	CAction * m_MatteAction;
	CAction * m_PassthroughAction;
	CAction * m_InvisibleAction;
	CAction * m_NoDrawAction;
	CAction * m_BlackAction;
	CAction * m_WhiteAction;
	CAction * m_DebugDumpAction;
	bool m_StandardActionsInitialized;
	bool m_BoundAction;
	bool m_DebugPrint;
	std::map<CAfxMaterialKey, CAction *> m_Map;
	std::map<CActionAfxVertexLitGenericHookKey, CActionAfxVertexLitGenericHook *> m_VertexLitGenericHookActions;

	CAction * CAfxBaseFxStream::GetAction(IMaterial_csgo * material);
	CAction * CAfxBaseFxStream::GetAction(IMaterial_csgo * material, AfxAction action, bool safeMode);

	CAction * GetVertexLitGenericHookAction(CActionAfxVertexLitGenericHookKey & key);
	
	void InvalidateVertexLitGenericHookActions();
};

class CAfxDepthStream
: public CAfxBaseFxStream
{
public:
	CAfxDepthStream() : CAfxBaseFxStream()
	{
		m_ClientEffectTexturesAction = AA_DrawDepth;
		m_WorldTexturesAction =  AA_DrawDepth;
		m_SkyBoxTexturesAction =  AA_DrawDepth;
		m_StaticPropTexturesAction =  AA_DrawDepth;
		m_CableAction =  AA_DrawDepth;
		m_PlayerModelsAction =  AA_DrawDepth;
		m_WeaponModelsAction =  AA_DrawDepth;
		m_StattrackAction = AA_DrawDepth;
		m_ShellModelsAction =  AA_DrawDepth;
		m_OtherModelsAction =  AA_DrawDepth;
		m_DecalTexturesAction =  AA_DrawDepth;
		m_EffectsAction =  AA_DrawDepth;
		m_ShellParticleAction =  AA_DrawDepth;
		m_OtherParticleAction =  AA_DrawDepth;
		m_StickerAction =  AA_DrawDepth;
		m_ErrorMaterialAction = AA_DrawDepth;
	}

	virtual ~CAfxDepthStream() {}

protected:
};

class CAfxMatteWorldStream
: public CAfxBaseFxStream
{
public:
	CAfxMatteWorldStream() : CAfxBaseFxStream()
	{
		m_ClientEffectTexturesAction = AA_Draw;
		m_WorldTexturesAction = AA_Draw;
		m_SkyBoxTexturesAction = AA_Draw;
		m_StaticPropTexturesAction = AA_Draw;
		m_CableAction =  AA_Draw;
		m_PlayerModelsAction = AA_Invisible;
		m_WeaponModelsAction = AA_Invisible;
		m_StattrackAction = AA_Invisible;
		m_ShellModelsAction = AA_Invisible;
		m_OtherModelsAction = AA_Draw;
		m_DecalTexturesAction = AA_Draw;
		m_EffectsAction = AA_Draw;
		m_ShellParticleAction = AA_Invisible;
		m_OtherParticleAction = AA_Draw;
		m_StickerAction = AA_Invisible;
		m_ErrorMaterialAction = AA_Draw;
	}

	virtual ~CAfxMatteWorldStream() {}

protected:
};

class CAfxDepthWorldStream
: public CAfxBaseFxStream
{
public:
	CAfxDepthWorldStream() : CAfxBaseFxStream()
	{
		m_ClientEffectTexturesAction = AA_DrawDepth;
		m_WorldTexturesAction = AA_DrawDepth;
		m_SkyBoxTexturesAction = AA_DrawDepth;
		m_StaticPropTexturesAction = AA_DrawDepth;
		m_CableAction = AA_DrawDepth;
		m_PlayerModelsAction = AA_Invisible;
		m_WeaponModelsAction = AA_Invisible;
		m_StattrackAction = AA_Invisible;
		m_ShellModelsAction = AA_Invisible;
		m_OtherModelsAction = AA_DrawDepth;
		m_DecalTexturesAction = AA_DrawDepth;
		m_EffectsAction = AA_DrawDepth;
		m_ShellParticleAction = AA_Invisible;
		m_OtherParticleAction = AA_DrawDepth;
		m_StickerAction = AA_Invisible;
		m_ErrorMaterialAction = AA_DrawDepth;
	}

	virtual ~CAfxDepthWorldStream() {}

protected:
};

class CAfxMatteEntityStream
: public CAfxBaseFxStream
{
public:
	CAfxMatteEntityStream() : CAfxBaseFxStream()
	{
		m_ClientEffectTexturesAction = AA_GreenScreen;
		m_WorldTexturesAction = AA_GreenScreen;
		m_SkyBoxTexturesAction = AA_GreenScreen;
		m_StaticPropTexturesAction = AA_GreenScreen;
		m_CableAction = AA_GreenScreen;
		m_PlayerModelsAction = AA_Draw;
		m_WeaponModelsAction = AA_Draw;
		m_StattrackAction = AA_Draw;
		m_ShellModelsAction = AA_Draw;
		m_OtherModelsAction = AA_GreenScreen;
		m_DecalTexturesAction = AA_GreenScreen;
		m_EffectsAction = AA_GreenScreen;
		m_ShellParticleAction = AA_Draw;
		m_OtherParticleAction = AA_GreenScreen;
		m_StickerAction = AA_Draw;
		m_ErrorMaterialAction = AA_GreenScreen;
	}

	virtual ~CAfxMatteEntityStream() {}

protected:
};

class CAfxDepthEntityStream
: public CAfxBaseFxStream
{
public:
	CAfxDepthEntityStream() : CAfxBaseFxStream()
	{
		m_ClientEffectTexturesAction = AA_DrawDepth;
		m_WorldTexturesAction =  AA_DrawDepth;
		m_SkyBoxTexturesAction =  AA_DrawDepth;
		m_StaticPropTexturesAction =  AA_DrawDepth;
		m_CableAction =  AA_DrawDepth;
		m_PlayerModelsAction =  AA_DrawDepth;
		m_WeaponModelsAction =  AA_DrawDepth;
		m_StattrackAction = AA_DrawDepth;
		m_ShellModelsAction =  AA_DrawDepth;
		m_OtherModelsAction =  AA_DrawDepth;
		m_DecalTexturesAction =  AA_DrawDepth;
		m_EffectsAction =  AA_DrawDepth;
		m_ShellParticleAction =  AA_DrawDepth;
		m_OtherParticleAction =  AA_DrawDepth;
		m_StickerAction =  AA_DrawDepth;
		m_ErrorMaterialAction = AA_DrawDepth;
	}

	virtual ~CAfxDepthEntityStream() {}

protected:
};

class CAfxAlphaMatteStream
: public CAfxBaseFxStream
{
public:
	CAfxAlphaMatteStream() : CAfxBaseFxStream()
	{
		m_ClientEffectTexturesAction = AA_Black;
		m_WorldTexturesAction = AA_Black;
		m_SkyBoxTexturesAction = AA_Black;
		m_StaticPropTexturesAction = AA_Black;
		m_CableAction = AA_Black;
		m_PlayerModelsAction = AA_White;
		m_WeaponModelsAction = AA_White;
		m_StattrackAction = AA_White;
		m_ShellModelsAction = AA_White;
		m_OtherModelsAction = AA_Black;
		m_DecalTexturesAction = AA_Black;
		m_EffectsAction = AA_Black;
		m_ShellParticleAction = AA_White;
		m_OtherParticleAction = AA_Black;
		m_StickerAction = AA_White;
		m_ErrorMaterialAction = AA_Black;
	}

	virtual ~CAfxAlphaMatteStream() {}

protected:
};

class CAfxAlphaEntityStream
: public CAfxBaseFxStream
{
public:
	CAfxAlphaEntityStream() : CAfxBaseFxStream()
	{
		m_ClientEffectTexturesAction = AA_Draw;
		m_WorldTexturesAction =  AA_Draw;
		m_SkyBoxTexturesAction =  AA_Draw;
		m_StaticPropTexturesAction =  AA_Draw;
		m_CableAction =  AA_Draw;
		m_PlayerModelsAction =  AA_Draw;
		m_WeaponModelsAction =  AA_Draw;
		m_StattrackAction = AA_Draw;
		m_ShellModelsAction =  AA_Draw;
		m_OtherModelsAction =  AA_Draw;
		m_DecalTexturesAction =  AA_Draw;
		m_EffectsAction =  AA_Draw;
		m_ShellParticleAction =  AA_Draw;
		m_OtherParticleAction =  AA_Draw;
		m_StickerAction =  AA_Draw;
		m_ErrorMaterialAction = AA_Draw;
	}

	virtual ~CAfxAlphaEntityStream() {}

protected:
};

class CAfxAlphaWorldStream
: public CAfxBaseFxStream
{
public:
	CAfxAlphaWorldStream() : CAfxBaseFxStream()
	{
		m_ClientEffectTexturesAction = AA_Draw;
		m_WorldTexturesAction = AA_Draw;
		m_SkyBoxTexturesAction = AA_Draw;
		m_StaticPropTexturesAction = AA_Draw;
		m_CableAction = AA_Draw;
		m_PlayerModelsAction = AA_Invisible;
		m_WeaponModelsAction = AA_Invisible;
		m_StattrackAction = AA_Invisible;
		m_ShellModelsAction =  AA_Invisible;
		m_OtherModelsAction =  AA_Draw;
		m_DecalTexturesAction = AA_Draw;
		m_EffectsAction = AA_Draw;
		m_ShellParticleAction = AA_Invisible;
		m_OtherParticleAction = AA_Draw;
		m_StickerAction = AA_Invisible;
		m_ErrorMaterialAction = AA_Draw;
	}

	virtual ~CAfxAlphaWorldStream() {}

protected:
};

class CAfxFileTracker
{
public:
	void TrackFile(char const * filePath);

	void WaitForFiles(unsigned int maxUnfinishedFiles);

private:
	std::queue<std::string> m_FilePaths;

};

class CAfxStreams
: public IAfxStreams4Stream
, public IAfxBaseClientDllLevelShutdown
, public IAfxBaseClientDllView_Render
, public IAfxVRenderViewSetBlend
, public IAfxVRenderViewSetColorModulation
{
public:
	CAfxStreams();
	~CAfxStreams();

	void OnMaterialSystem(IMaterialSystem_csgo * value);
	void OnAfxVRenderView(IAfxVRenderView * value);
	void OnAfxBaseClientDll(IAfxBaseClientDll * value);
	void OnShaderShadow(IShaderShadow_csgo * value);

	void OnDraw(IAfxMesh * am, int firstIndex = -1, int numIndices = 0);
	void OnDraw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists);
	void OnDrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex = -1, int numIndices = 0 );

	/// <remarks>This function can be called from several threads simutaneously.</remarks>
	void OnSetVertexShader(const char* pFileName, int nStaticVshIndex, int vshIndex = -1);

	/// <remarks>This function can be called from several threads simutaneously.</remarks>
	void OnSetPixelShader(const char* pFileName, int nStaticPshIndex = 0, int pshIndex = 0);

	virtual void SetBlend(IAfxVRenderView * rv, float blend );
	virtual void SetColorModulation(IAfxVRenderView * rv, float const* blend );

	void Console_RecordName_set(const char * value);
	const char * Console_RecordName_get();
	void Console_RecordFormat_set(const char * value);
	const char * Console_RecordFormat_get();
	void Console_Record_Start();
	void Console_Record_End();
	void Console_AddStream(const char * streamName);
	void Console_AddBaseFxStream(const char * streamName);
	void Console_AddDeveloperStream(const char * streamName);
	void Console_AddDepthStream(const char * streamName);
	void Console_AddMatteWorldStream(const char * streamName);
	void Console_AddDepthWorldStream(const char * streamName);
	void Console_AddMatteEntityStream(const char * streamName);
	void Console_AddDepthEntityStream(const char * streamName);
	void Console_AddAlphaMatteStream(const char * streamName);
	void Console_AddAlphaEntityStream(const char * streamName);
	void Console_AddAlphaWorldStream(const char * streamName);
	void Console_AddAlphaMatteEntityStream(const char * streamName);
	void Console_PrintStreams();
	void Console_RemoveStream(const char * streamName);
	void Console_EditStream(const char * streamName, IWrpCommandArgs * args, int argcOffset, char const * cmdPrefix);

	/// <param name="index">stream name to preview or empty string if to preview nothing.</param>
	void Console_PreviewStream(const char * streamName);

	virtual void DebugDump();

	virtual IMaterialSystem_csgo * GetMaterialSystem(void);
	virtual IAfxFreeMaster * GetFreeMaster(void);
	virtual IAfxMatRenderContext * GetCurrentContext(void);
	virtual IShaderShadow_csgo * GetShaderShadow(void);

	virtual std::wstring GetTakeDir(void);

	virtual void GetBlend(float &outBlend);
	virtual void GetColorModulation(float (& outColor)[3]);

	virtual void OverrideSetColorModulation(float const color[3]);
	virtual void EndOverrideSetColorModulation();

	virtual void OverrideSetBlend(float blend);
	virtual void EndOverrideSetBlend();

	virtual void OnBind_set(IAfxMatRenderContextBind * value);
	virtual void OnDrawInstances_set(IAfxMatRenderContextDrawInstances * value);

	virtual void OnDraw_set(IAfxMeshDraw * value);
	virtual void OnDraw_2_set(IAfxMeshDraw_2 * value);
	virtual void OnDrawModulated_set(IAfxMeshDrawModulated * value);

	virtual void OnSetVertexShader_set(IAfxSetVertexShader * value);
	virtual void OnSetPixelShader_set(IAfxSetPixelShader * value);

	virtual void LevelShutdown(IAfxBaseClientDll * cl);

	virtual void View_Render(IAfxBaseClientDll * cl, IAfxMatRenderContext * cx, vrect_t_csgo *rect);

private:
	class CFreeDelegate : public IAfxFreeable
	{
	public:
		CFreeDelegate(IAfxFreeMaster * freeMaster, CAfxStreams * classPtr, void (CAfxStreams::*classFn)(void))
		: m_FreeMaster(freeMaster)
		, m_ClassPtr(classPtr)
		, m_ClassFn(classFn)
		{
			m_FreeMaster->AfxFreeable_Register(this);
		}

		~CFreeDelegate()
		{
			AfxFree();
		}

		virtual void AfxFree(void)
		{
			if(m_ClassPtr) { (m_ClassPtr->*m_ClassFn)(); m_ClassPtr = 0; }
			if(m_FreeMaster) { m_FreeMaster->AfxFreeable_Unregister(this); m_FreeMaster = 0; }
		}

	private:
		IAfxFreeMaster * m_FreeMaster;
		CAfxStreams * m_ClassPtr;
		void (CAfxStreams::*m_ClassFn)(void);

	};

	class CImageBuffer
	{
	public:
		enum ImageBufferPixelFormat
		{
			IBPF_BGR,
			IBPF_BGRA,
			IBPF_A
		};

		void * Buffer;

		ImageBufferPixelFormat PixelFormat;
		int Width;
		int Height;
		size_t ImagePitch;
		size_t ImageBytes;

		CImageBuffer();
		~CImageBuffer();

		bool AutoRealloc(ImageBufferPixelFormat pixelFormat, int width, int height);

	private:
		size_t m_BufferBytesAllocated;
	};

	std::string m_RecordName;
	CFreeDelegate * m_OnAfxBaseClientDll_Free;
	IMaterialSystem_csgo * m_MaterialSystem;
	IAfxVRenderView * m_VRenderView;
	IAfxBaseClientDll * m_AfxBaseClientDll;
	IShaderShadow_csgo * m_ShaderShadow;
	IAfxMatRenderContext * m_CurrentContext;
	std::list<CAfxRecordStream *> m_Streams;
	CAfxRecordStream * m_PreviewStream;
	bool m_Recording;
	int m_Frame;
	IAfxMeshDraw * m_OnDraw;
	IAfxMeshDraw_2 * m_OnDraw_2;
	IAfxMeshDrawModulated * m_OnDrawModulated;
	IAfxSetVertexShader * m_OnSetVertexShader;
	IAfxSetPixelShader * m_OnSetPixelShader;

	WrpConVarRef * m_MatQueueModeRef;
	int m_OldMatQueueMode;
	WrpConVarRef * m_MatPostProcessEnableRef;
	int m_OldMatPostProcessEnable;
	WrpConVarRef * m_MatDynamicTonemappingRef;
	int m_OldMatDynamicTonemapping;
	WrpConVarRef * m_MatMotionBlurEnabledRef;
	int m_OldMatMotionBlurEnabled;
	float m_OriginalColorModulation[4];
	bool m_ColorModulationOverride;
	bool m_BlendOverride;
	float m_OverrideColor[4];
	CImageBuffer m_BufferA;
	CImageBuffer m_BufferB;
	std::wstring m_TakeDir;
	bool m_FormatBmpAndNotTga;
	//ITexture_csgo * m_RgbaRenderTarget;
	//ITexture_csgo * m_RenderTargetDummy;
	//ITexture_csgo * m_RenderTargetDepth;
	//CAfxMaterial * m_ShowzMaterial;
	DWORD m_Current_View_Render_ThreadId;

	void SetCurrent_View_Render_ThreadId(DWORD id);

	DWORD GetCurrent_View_Render_ThreadId();

	void OnAfxBaseClientDll_Free(void);

	void Console_EditStream(CAfxStream * stream, IWrpCommandArgs * args, int argcOffset, char const * cmdPrefix);

	bool Console_CheckStreamName(char const * value);

	bool Console_ToAfxAction(char const * value, CAfxBaseFxStream::AfxAction & action);
	char const * Console_FromAfxAction(CAfxBaseFxStream::AfxAction action);

	bool Console_ToStreamCombineType(char const * value, CAfxTwinStream::StreamCombineType & streamCombineType);
	char const * Console_FromStreamCombineType(CAfxTwinStream::StreamCombineType streamCombineType);

	bool Console_ToStreamRenderType(char const * value, CAfxRenderViewStream::StreamRenderType & streamRenderType);
	char const * Console_FromStreamRenderType(CAfxRenderViewStream::StreamRenderType streamRenderType);

	bool CheckCanFeedStreams(void);

	void BackUpMatVars();
	void SetMatVarsForStreams();
	void RestoreMatVars();
	void EnsureMatVars();

	void AddStream(CAfxRecordStream * stream);

	void CreateRenderTargets(IMaterialSystem_csgo * materialSystem);

	bool CaptureStreamToBuffer(CAfxRenderViewStream * stream, CImageBuffer & buffer, IAfxMatRenderContext * cx);

	bool WriteBufferToFile(const CImageBuffer & buffer, const std::wstring & path);
};

extern CAfxStreams g_AfxStreams;
