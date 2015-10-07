#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-07-27 dominik.matrixstorm.com
//
// First changes:
// 2015-06-26 dominik.matrixstorm.com

#include "SourceInterfaces.h"
#include "AfxInterfaces.h"
#include "AfxClasses.h"
#include "WrpConsole.h"

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

protected:
	/// <summary>This member is only valid between StreamAttach and StreamDetach.</summary>
	IAfxStreams4Stream * m_Streams;

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
{
public:
	enum MaskableAction {
		MA_NotSet,
		MA_Draw,
		MA_DrawDepth,
		MA_Mask,
		MA_Invisible,
		MA_Black,
		MA_White
	};

	enum HideableAction
	{
		HA_NotSet,
		HA_Draw,
		HA_NoDraw
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

	HideableAction ClientEffectTexturesAction_get(void);
	void ClientEffectTexturesAction_set(HideableAction value);

	MaskableAction WorldTexturesAction_get(void);
	void WorldTexturesAction_set(MaskableAction value);

	MaskableAction SkyBoxTexturesAction_get(void);
	void SkyBoxTexturesAction_set(MaskableAction value);

	MaskableAction StaticPropTexturesAction_get(void);
	void StaticPropTexturesAction_set(MaskableAction value);

	HideableAction CableAction_get(void);
	void CableAction_set(HideableAction value);

	MaskableAction PlayerModelsAction_get(void);
	void PlayerModelsAction_set(MaskableAction value);

	MaskableAction WeaponModelsAction_get(void);
	void WeaponModelsAction_set(MaskableAction value);

	MaskableAction ShellModelsAction_get(void);
	void ShellModelsAction_set(MaskableAction value);

	MaskableAction OtherModelsAction_get(void);
	void OtherModelsAction_set(MaskableAction value);

	HideableAction DecalTexturesAction_get(void);
	void DecalTexturesAction_set(HideableAction value);

	HideableAction EffectsAction_get(void);
	void EffectsAction_set(HideableAction value);

	HideableAction ShellParticleAction_get(void);
	void ShellParticleAction_set(HideableAction value);

	HideableAction OtherParticleAction_get(void);
	void OtherParticleAction_set(HideableAction value);

	MaskableAction StickerAction_get(void);
	void StickerAction_set(MaskableAction value);

	MaskableAction ErrorMaterialAction_get(void);
	void ErrorMaterialAction_set(MaskableAction value);

	float DepthVal_get(void);
	void DepthVal_set(float value);

	float DepthValMax_get(void);
	void DepthValMax_set(float value);
	
	bool DebugPrint_get(void);
	void DebugPrint_set(bool value);

	void InvalidateCache(void);

protected:
	HideableAction m_ClientEffectTexturesAction;
	MaskableAction m_WorldTexturesAction;
	MaskableAction m_SkyBoxTexturesAction;
	MaskableAction m_StaticPropTexturesAction;
	HideableAction m_CableAction;
	MaskableAction m_PlayerModelsAction;
	MaskableAction m_WeaponModelsAction;
	MaskableAction m_ShellModelsAction;
	MaskableAction m_OtherModelsAction;
	HideableAction m_DecalTexturesAction;
	HideableAction m_EffectsAction;
	HideableAction m_ShellParticleAction;
	HideableAction m_OtherParticleAction;
	MaskableAction m_StickerAction;
	HideableAction m_ErrorMaterialAction;
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

		virtual ~CAction()
		{
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

	protected:
		CAfxBaseFxStream * m_ParentStream;

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

		virtual ~CActionMatte()
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
		unsigned long m_OldSrgbWriteEnable;
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

		virtual ~CActionBlack()
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
		unsigned long m_OldSrgbWriteEnable;
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

		virtual ~CActionWhite()
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
		unsigned long m_OldSrgbWriteEnable;
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

		virtual ~CActionDepth()
		{
		}

		virtual void AfxUnbind(IAfxMatRenderContext * ctx);

		virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );

	private:
		CAfxMaterial m_DepthMaterial;
		unsigned long m_OldSrgbWriteEnable;
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

		virtual ~CActionInvisible()
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

		virtual ~CActionNoDraw()
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
	CAction * m_MatteAction;
	CAction * m_PassthroughAction;
	CAction * m_InvisibleAction;
	CAction * m_NoDrawAction;
	CAction * m_BlackAction;
	CAction * m_WhiteAction;
	bool m_BoundAction;
	bool m_DebugPrint;

	std::map<CAfxMaterialKey, CAction *> m_Map;

	CAction * GetAction(MaskableAction value);
	CAction * GetAction(HideableAction value);
};

class CAfxDepthStream
: public CAfxBaseFxStream
{
public:
	CAfxDepthStream() : CAfxBaseFxStream()
	{
		m_ClientEffectTexturesAction = HA_NoDraw;
		m_WorldTexturesAction =  MA_DrawDepth;
		m_SkyBoxTexturesAction =  MA_DrawDepth;
		m_StaticPropTexturesAction =  MA_DrawDepth;
		m_CableAction =  HA_NoDraw;
		m_PlayerModelsAction =  MA_DrawDepth;
		m_WeaponModelsAction =  MA_DrawDepth;
		m_ShellModelsAction =  MA_DrawDepth;
		m_OtherModelsAction =  MA_DrawDepth;
		m_DecalTexturesAction =  HA_NoDraw;
		m_EffectsAction =  HA_NoDraw;
		m_ShellParticleAction =  HA_NoDraw;
		m_OtherParticleAction =  HA_NoDraw;
		m_StickerAction =  MA_Invisible;
		m_ErrorMaterialAction = HA_NoDraw;
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
		m_ClientEffectTexturesAction = HA_Draw;
		m_WorldTexturesAction = MA_Draw;
		m_SkyBoxTexturesAction = MA_Draw;
		m_StaticPropTexturesAction = MA_Draw;
		m_CableAction =  HA_Draw;
		m_PlayerModelsAction = MA_Invisible;
		m_WeaponModelsAction = MA_Invisible;
		m_ShellModelsAction = MA_Invisible;
		m_OtherModelsAction = MA_Draw;
		m_DecalTexturesAction = HA_Draw;
		m_EffectsAction = HA_Draw;
		m_ShellParticleAction = HA_NoDraw;
		m_OtherParticleAction = HA_Draw;
		m_StickerAction = MA_Invisible;
		m_ErrorMaterialAction = HA_Draw;
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
		m_ClientEffectTexturesAction = HA_NoDraw;
		m_WorldTexturesAction = MA_DrawDepth;
		m_SkyBoxTexturesAction = MA_DrawDepth;
		m_StaticPropTexturesAction = MA_DrawDepth;
		m_CableAction =  HA_NoDraw;
		m_PlayerModelsAction = MA_Invisible;
		m_WeaponModelsAction = MA_Invisible;
		m_ShellModelsAction = MA_Invisible;
		m_OtherModelsAction = MA_DrawDepth;
		m_DecalTexturesAction = HA_NoDraw;
		m_EffectsAction = HA_NoDraw;
		m_ShellParticleAction = HA_NoDraw;
		m_OtherParticleAction = HA_NoDraw;
		m_StickerAction = MA_Invisible;
		m_ErrorMaterialAction = HA_NoDraw;
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
		m_ClientEffectTexturesAction = HA_NoDraw;
		m_WorldTexturesAction =  MA_Mask;
		m_SkyBoxTexturesAction =  MA_Mask;
		m_StaticPropTexturesAction =  MA_Mask;
		m_CableAction =  HA_NoDraw;
		m_PlayerModelsAction =  MA_Draw;
		m_WeaponModelsAction =  MA_Draw;
		m_ShellModelsAction =  MA_Draw;
		m_OtherModelsAction =  MA_Mask;
		m_DecalTexturesAction =  HA_NoDraw;
		m_EffectsAction =  HA_NoDraw;
		m_ShellParticleAction =  HA_Draw;
		m_OtherParticleAction =  HA_NoDraw;
		m_StickerAction =  MA_Draw;
		m_ErrorMaterialAction = HA_NoDraw;
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
		m_ClientEffectTexturesAction = HA_NoDraw;
		m_WorldTexturesAction =  MA_DrawDepth;
		m_SkyBoxTexturesAction =  MA_DrawDepth;
		m_StaticPropTexturesAction =  MA_DrawDepth;
		m_CableAction =  HA_NoDraw;
		m_PlayerModelsAction =  MA_DrawDepth;
		m_WeaponModelsAction =  MA_DrawDepth;
		m_ShellModelsAction =  MA_DrawDepth;
		m_OtherModelsAction =  MA_DrawDepth;
		m_DecalTexturesAction =  HA_NoDraw;
		m_EffectsAction =  HA_NoDraw;
		m_ShellParticleAction =  HA_NoDraw;
		m_OtherParticleAction =  HA_NoDraw;
		m_StickerAction =  MA_Invisible;
		m_ErrorMaterialAction = HA_NoDraw;
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
		m_ClientEffectTexturesAction = HA_NoDraw;
		m_WorldTexturesAction =  MA_Black;
		m_SkyBoxTexturesAction =  MA_Black;
		m_StaticPropTexturesAction =  MA_Black;
		m_CableAction =  HA_NoDraw;
		m_PlayerModelsAction =  MA_White;
		m_WeaponModelsAction =  MA_White;
		m_ShellModelsAction =  MA_White;
		m_OtherModelsAction =  MA_Black;
		m_DecalTexturesAction =  HA_NoDraw;
		m_EffectsAction =  HA_NoDraw;
		m_ShellParticleAction =  HA_NoDraw;
		m_OtherParticleAction =  HA_NoDraw;
		m_StickerAction =  MA_Invisible;
		m_ErrorMaterialAction = HA_NoDraw;
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
		m_ClientEffectTexturesAction = HA_Draw;
		m_WorldTexturesAction =  MA_Draw;
		m_SkyBoxTexturesAction =  MA_Draw;
		m_StaticPropTexturesAction =  MA_Draw;
		m_CableAction =  HA_Draw;
		m_PlayerModelsAction =  MA_Draw;
		m_WeaponModelsAction =  MA_Draw;
		m_ShellModelsAction =  MA_Draw;
		m_OtherModelsAction =  MA_Draw;
		m_DecalTexturesAction =  HA_Draw;
		m_EffectsAction =  HA_Draw;
		m_ShellParticleAction =  HA_Draw;
		m_OtherParticleAction =  HA_Draw;
		m_StickerAction =  MA_Draw;
		m_ErrorMaterialAction = HA_Draw;

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
		m_ClientEffectTexturesAction = HA_Draw;
		m_WorldTexturesAction =  MA_Draw;
		m_SkyBoxTexturesAction =  MA_Draw;
		m_StaticPropTexturesAction =  MA_Draw;
		m_CableAction =  HA_Draw;
		m_PlayerModelsAction =  MA_Invisible;
		m_WeaponModelsAction =  MA_Invisible;
		m_ShellModelsAction =  MA_Invisible;
		m_OtherModelsAction =  MA_Draw;
		m_DecalTexturesAction =  HA_Draw;
		m_EffectsAction =  HA_Draw;
		m_ShellParticleAction =  HA_Draw;
		m_OtherParticleAction =  HA_Draw;
		m_StickerAction =  MA_Invisible;
		m_ErrorMaterialAction = HA_Draw;
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
			IBPF_BGRA
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
	WrpConVarRef * m_MatQueueModeRef;
	int m_OldMatQueueMode;
	WrpConVarRef * m_MatPostProcessEnableRef;
	int m_OldMatPostProcessEnable;
	WrpConVarRef * m_MatDynamicTonemappingRef;
	int m_OldMatDynamicTonemapping;
	float m_OriginalColorModulation[4];
	bool m_ColorModulationOverride;
	bool m_BlendOverride;
	float m_OverrideColor[4];
	CImageBuffer m_BufferA;
	CImageBuffer m_BufferB;
	std::wstring m_TakeDir;
	bool m_FormatBmpAndNotTga;
	//ITexture_csgo * m_RgbaRenderTarget;

	void OnAfxBaseClientDll_Free(void);

	void Console_EditStream(CAfxStream * stream, IWrpCommandArgs * args, int argcOffset, char const * cmdPrefix);

	bool Console_CheckStreamName(char const * value);

	bool Console_ToMaskableAction(char const * value, CAfxBaseFxStream::MaskableAction & maskableAction);
	char const * Console_FromMaskableAction(CAfxBaseFxStream::MaskableAction maskableAction);

	bool Console_ToHideableAction(char const * value, CAfxBaseFxStream::HideableAction & hideableAction);
	char const * Console_FromHideableAction(CAfxBaseFxStream::HideableAction hideableAction);

	bool Console_ToStreamCombineType(char const * value, CAfxTwinStream::StreamCombineType & streamCombineType);
	char const * Console_FromStreamCombineType(CAfxTwinStream::StreamCombineType streamCombineType);

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
