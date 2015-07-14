#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-06-26 dominik.matrixstorm.com
//
// First changes:
// 2015-06-26 dominik.matrixstorm.com

#include "SourceInterfaces.h"
#include "AfxInterfaces.h"
#include "AfxClasses.h"
#include "WrpConsole.h"

#include <string>
#include <list>
#include <map>

class CAfxDeveloperStream;
class CAfxMatteStream;
class CAfxMatteWorldStream;
class CAfxMatteEntityStream;

class CAfxStream : 
public IAfxStream4Streams
{
public:
	enum TopStreamType
	{
		TST_CAfxStream,
		TST_CAfxDeveloperStream,
		TST_CAfxMatteStream,
		TST_CAfxMatteEntityStream,
		TST_CAfxMatteWorldStream
	};

	CAfxStream(char const * streamName);

	virtual ~CAfxStream();

	virtual CAfxStream * AsAfxStream(void) { return this; }
	virtual CAfxDeveloperStream * AsAfxDeveloperStream(void) { return 0; }
	virtual CAfxMatteStream * AsAfxMatteStream(void) { return 0; }
	virtual CAfxMatteEntityStream * AsAfxMatteEntityStream(void) { return 0; }
	virtual CAfxMatteWorldStream * AsAfxMatteWorldStream(void) { return 0; }

	virtual TopStreamType GetTopStreamType(void) { return TST_CAfxStream; }

	bool Record_get(void);
	void Record_set(bool value);

	char const * GetStreamName(void);

	virtual void StreamAttach(IAfxStreams4Stream * streams);
	virtual void StreamDetach(IAfxStreams4Stream * streams);

protected:
	/// <summary>This member is only valid between StreamAttach and StreamDeatach.</summary>
	IAfxStreams4Stream * m_Streams;

private:
	std::string m_StreamName;
	bool m_Record;
};

class CAfxDeveloperStream
: public CAfxStream
, public IAfxMatRenderContextBind
, public IAfxMatRenderContextDrawInstances
, public IAfxMeshDraw
, public IAfxMeshDraw_2
, public IAfxMeshDrawModulated
{
public:
	CAfxDeveloperStream(char const * streamName);
	virtual ~CAfxDeveloperStream();

	virtual CAfxDeveloperStream * AsAfxDeveloperStream(void) { return this; }

	virtual TopStreamType GetTopStreamType(void) { return TST_CAfxDeveloperStream; }

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

class CAfxMatteStream
: public CAfxStream
, public IAfxMatRenderContextBind
, public IAfxMatRenderContextDrawInstances
, public IAfxMeshDraw
, public IAfxMeshDraw_2
, public IAfxMeshDrawModulated
, public IAfxVRenderViewSetColorModulation
{
public:
	enum MaskableAction {
		MA_Draw,
		MA_Mask,
		MA_Invisible
	};

	enum HideableAction
	{
		HA_Draw,
		HA_NoDraw
	};

	CAfxMatteStream(char const * streamName, bool isEntityStream);
	virtual ~CAfxMatteStream();

	virtual CAfxMatteStream * AsAfxMatteStream(void) { return this; }

	virtual TopStreamType GetTopStreamType(void) { return TST_CAfxMatteStream; }

	virtual void StreamAttach(IAfxStreams4Stream * streams);
	virtual void StreamDetach(IAfxStreams4Stream * streams);

	virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );
	virtual void DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance );

	virtual void Draw(IAfxMesh * am, int firstIndex = -1, int numIndices = 0);
	virtual void Draw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists);
	virtual void DrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex = -1, int numIndices = 0 );

	virtual void SetColorModulation(IAfxVRenderView * rv, float const* blend );

	MaskableAction WorldTexturesAction_get(void);
	void WorldTexturesAction_set(MaskableAction value);

	MaskableAction SkyBoxTexturesAction_get(void);
	void SkyBoxTexturesAction_set(MaskableAction value);

	MaskableAction StaticPropTexturesAction_get(void);
	void StaticPropTexturesAction_set(MaskableAction value);

	MaskableAction CableAction_get(void);
	void CableAction_set(MaskableAction value);

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
	
	bool DebugPrint_get(void);
	void DebugPrint_set(bool value);

	void InvalidateCache(void);

private:
	class CAction
	: public IAfxMatRenderContextBind
	, public IAfxMatRenderContextDrawInstances
	, public IAfxMeshDraw
	, public IAfxMeshDraw_2
	, public IAfxMeshDrawModulated
	, public IAfxVRenderViewSetColorModulation
	{
	public:
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

		virtual void SetColorModulation(IAfxVRenderView * rv, float const* blend )
		{
			rv->GetParent()->SetColorModulation(blend);
		}

		virtual void DrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex = -1, int numIndices = 0 )
		{
			am->GetParent()->DrawModulated(vecDiffuseModulation, firstIndex, numIndices);
		}

	};

	class CActionMatte
	: public CAction
	{
	public:
		CActionMatte(IAfxFreeMaster * freeMaster, IMaterialSystem_csgo * matSystem)
		: m_MatteMaterial(freeMaster, matSystem->FindMaterial("afx/greenmatte",NULL))
		{
		}

		virtual ~CActionMatte()
		{
		}

		virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 )
		{
			ctx->GetParent()->Bind(m_MatteMaterial.GetMaterial(), proxyData);
		}

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

		virtual void SetColorModulation(IAfxVRenderView * rv, float const* blend )
		{
			float color[3] = { 1.0f, 1.0f, 1.0f };
			rv->GetParent()->SetColorModulation(color);
		}

	private:
		CAfxMaterial m_MatteMaterial;
	};

	class CActionInvisible
	: public CAction
	{
	public:
		CActionInvisible(IAfxFreeMaster * freeMaster, IMaterialSystem_csgo * matSystem)
		: m_InvisibleMaterial(freeMaster, matSystem->FindMaterial("afx/invisible",NULL))
		{
		}

		virtual ~CActionInvisible()
		{
		}

		virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 )
		{
			ctx->GetParent()->Bind(m_InvisibleMaterial.GetMaterial(), proxyData);
		}

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

		virtual void SetColorModulation(IAfxVRenderView * rv, float const* blend )
		{
			float color[3] = { 0.0f, 0.0f, 0.0f };
			rv->GetParent()->SetColorModulation(color);
		}

	private:
		CAfxMaterial m_InvisibleMaterial;
	};

	class CActionNoDraw
	: public CAction
	{
	public:
		CActionNoDraw()
		{
		}

		virtual ~CActionNoDraw()
		{
		}

		virtual void AfxUnbind(IAfxMatRenderContext * ctx)
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
	CAction * m_MatteAction;
	CAction * m_PassthroughAction;
	CAction * m_InvisibleAction;
	CAction * m_NoDrawAction;
	bool m_BoundAction;
	MaskableAction m_WorldTexturesAction;
	MaskableAction m_SkyBoxTexturesAction;
	MaskableAction m_StaticPropTexturesAction;
	MaskableAction m_CableAction;
	MaskableAction m_PlayerModelsAction;
	MaskableAction m_WeaponModelsAction;
	MaskableAction m_ShellModelsAction;
	MaskableAction m_OtherModelsAction;
	HideableAction m_DecalTexturesAction;
	HideableAction m_EffectsAction;
	HideableAction m_ShellParticleAction;
	HideableAction m_OtherParticleAction;
	MaskableAction m_StickerAction;
	bool m_DebugPrint;

	std::map<CAfxMaterialKey, CAction *> m_Map;

	CAction * GetAction(MaskableAction value);
	CAction * GetAction(HideableAction value);
};

class CAfxMatteWorldStream
: public CAfxMatteStream
{
public:
	CAfxMatteWorldStream(char const * streamName) : CAfxMatteStream(streamName, false) {}
	virtual ~CAfxMatteWorldStream() {}

	virtual CAfxMatteWorldStream * AsAfxMatteWorldStream(void) { return this; }

	virtual TopStreamType GetTopStreamType(void) { return TST_CAfxMatteWorldStream; }

};

class CAfxMatteEntityStream
: public CAfxMatteStream
{
public:
	CAfxMatteEntityStream(char const * streamName) : CAfxMatteStream(streamName, true) {}
	virtual ~CAfxMatteEntityStream() {}

	virtual CAfxMatteEntityStream * AsAfxMatteEntityStream(void) { return this; }

	virtual TopStreamType GetTopStreamType(void) { return TST_CAfxMatteEntityStream; }
};


class CAfxStreams :
public IAfxStreams4Stream,
public IAfxBaseClientDllView_Render
{
public:
	CAfxStreams();
	~CAfxStreams();

	void OnMaterialSystem(IMaterialSystem_csgo * value);
	void OnAfxVRenderView(IAfxVRenderView * value);
	void OnAfxBaseClientDll(IAfxBaseClientDll * value);
	void OnDraw(IAfxMesh * am, int firstIndex = -1, int numIndices = 0);
	void OnDraw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists);
	void OnDrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex = -1, int numIndices = 0 );


	void Console_RecordName_set(const char * value);
	const char * Console_RecordName_get();
	void Console_Record_Start();
	void Console_Record_End();
	void Console_AddStream(const char * streamName);
	void Console_AddDeveloperStream(const char * streamName);
	void Console_AddMatteWorldStream(const char * streamName);
	void Console_AddMatteEntityStream(const char * streamName);
	void Console_PrintStreams();
	void Console_RemoveStream(const char * streamName);
	void Console_EditStream(const char * streamName, IWrpCommandArgs * args, int argcOffset, char const * cmdPrefix);

	/// <param name="index">stream index to preview or 0 if to preview nothing.</param>
	void Console_PreviewStream(const char * streamName);

	virtual IMaterialSystem_csgo * GetMaterialSystem(void);
	virtual IAfxFreeMaster * GetFreeMaster(void);
	virtual IAfxMatRenderContext * GetCurrentContext(void) ;

	virtual void OnBind_set(IAfxMatRenderContextBind * value);
	virtual void OnDrawInstances_set(IAfxMatRenderContextDrawInstances * value);

	virtual void OnDraw_set(IAfxMeshDraw * value);
	virtual void OnDraw_2_set(IAfxMeshDraw_2 * value);
	virtual void OnDrawModulated_set(IAfxMeshDrawModulated * value);

	virtual void OnSetColorModulation_set(IAfxVRenderViewSetColorModulation * value);

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

	std::string m_RecordName;
	CFreeDelegate * m_OnAfxBaseClientDll_Free;
	IMaterialSystem_csgo * m_MaterialSystem;
	IAfxVRenderView * m_VRenderView;
	IAfxBaseClientDll * m_AfxBaseClientDll;
	IAfxMatRenderContext * m_CurrentContext;
	std::list<CAfxStream *> m_Streams;
	CAfxStream * m_PreviewStream;
	bool m_Recording;
	int m_Frame;
	IAfxMeshDraw * m_OnDraw;
	IAfxMeshDraw_2 * m_OnDraw_2;
	IAfxMeshDrawModulated * m_OnDrawModulated;

	void OnAfxBaseClientDll_Free(void);
	bool Console_CheckStreamName(char const * value);
	bool Console_ToMaskableAction(char const * value, CAfxMatteStream::MaskableAction & maskableAction);
	bool Console_ToHideableAction(char const * value, CAfxMatteStream::HideableAction & hideableAction);
	char const * Console_FromMaskableAction(CAfxMatteStream::MaskableAction maskableAction);
	char const * Console_FromHideableAction(CAfxMatteStream::HideableAction hideableAction);

	bool CheckCanFeedStreams(void);
};

extern CAfxStreams g_AfxStreams;
