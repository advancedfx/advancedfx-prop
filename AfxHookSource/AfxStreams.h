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
class CAfxMatteEntityStream;

class CAfxStream : 
public IAfxStream4Streams
{
public:
	enum TopStreamType
	{
		TST_CAfxStream,
		TST_CAfxDeveloperStream,
		TST_CAfxMatteEntityStream
	};

	virtual ~CAfxStream();

	virtual CAfxStream * AsAfxStream(void) { return this; }
	virtual CAfxDeveloperStream * AsAfxDeveloperStream(void) { return 0; }
	virtual CAfxMatteEntityStream * AsAfxMatteEntityStream(void) { return 0; }

	char const * GetStreamName(void);
	
	TopStreamType GetTopStreamType(void);

	virtual void StreamAttach(IAfxStreams4Stream * streams);
	virtual void StreamDetach(IAfxStreams4Stream * streams);

protected:
	CAfxStream(TopStreamType topStreamType, char const * streamName);

	/// <summary>This member is only valid between StreamAttach and StreamDeatach.</summary>
	IAfxStreams4Stream * m_Streams;

private:
	std::string m_StreamName;
	TopStreamType m_TopStreamType;
};

class CAfxDeveloperStream
: public CAfxStream
, public IAfxMatRenderContextBind
{
public:
	CAfxDeveloperStream(char const * streamName);
	virtual ~CAfxDeveloperStream();

	virtual CAfxDeveloperStream * AsAfxDeveloperStream(void) { return this; }

	void MatchName_set(char const * value);
	char const * MatchName_get(void);

	void MatchTextureGroupName_set(char const * value);
	char const * MatchTextureGroupName_get(void);

	void ReplaceName_set(char const * name);
	char const * ReplaceName_get(void);

	virtual void StreamAttach(IAfxStreams4Stream * streams);
	virtual void StreamDetach(IAfxStreams4Stream * streams);

	virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );

private:
	std::string m_MatchTextureGroupName;
	std::string m_MatchName;
	std::string m_ReplaceName;
	bool m_ReplaceUpdate;
	bool m_Replace;

	CAfxMaterial * m_ReplaceMaterial;
};

class CAfxMatteEntityStream
: public CAfxStream
, public IAfxMatRenderContextBind
, public IAfxMatRenderContextDrawInstances
, public IAfxVRenderViewSetColorModulation
{
public:
	CAfxMatteEntityStream(char const * streamName);
	virtual ~CAfxMatteEntityStream();

	virtual CAfxMatteEntityStream * AsAfxMatteEntityStream(void) { return this; }

	virtual void StreamAttach(IAfxStreams4Stream * streams);
	virtual void StreamDetach(IAfxStreams4Stream * streams);

	virtual void Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData = 0 );
	virtual void DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance );
	virtual void SetColorModulation(IAfxVRenderView * rv, float const* blend );

private:
	class CAction
	: public IAfxMatRenderContextBind
	, public IAfxMatRenderContextDrawInstances
	, public IAfxVRenderViewSetColorModulation
	{
	public:
		virtual ~CAction()
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

		virtual void SetColorModulation(IAfxVRenderView * rv, float const* blend )
		{
			rv->GetParent()->SetColorModulation(blend);
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

	CAction * m_CurrentAction;
	CAction * m_MatteAction;
	CAction * m_PassthroughAction;
	CAction * m_InvisibleAction;

	std::map<CAfxMaterialKey, CAction *> m_Map;
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

	void Console_RecordName_set(const char * value);
	const char * Console_RecordName_get();
	void Console_Record_Start();
	void Console_Record_End();
	void Console_AddDeveloperStream(const char * streamName);
	void Console_AddMatteWorldStream(const char * streamName);
	void Console_AddMatteEntityStream(const char * streamName);
	void Console_PrintStreams();
	void Console_RemoveStream(int index);
	void Console_EditStream(int index, IWrpCommandArgs * args, int argcOffset, char const * cmdPrefix);

	/// <param name="index">stream index to preview or 0 if to preview nothing.</param>
	void Console_PreviewStream(int index);

	virtual IMaterialSystem_csgo * GetMaterialSystem(void);
	virtual IAfxFreeMaster * GetFreeMaster(void);
	virtual void OnBind_set(IAfxMatRenderContextBind * value);
	virtual void OnDrawInstances_set(IAfxMatRenderContextDrawInstances * value);
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

	void OnAfxBaseClientDll_Free(void);
	bool Console_CheckStreamName(char const * value);

	bool CheckCanFeedStreams(void);
};

extern CAfxStreams g_AfxStreams;
