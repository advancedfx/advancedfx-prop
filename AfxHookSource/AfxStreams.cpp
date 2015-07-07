#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-06-26 dominik.matrixstorm.com
//
// First changes:
// 2015-06-26 dominik.matrixstorm.com

#include "AfxStreams.h"

#include "SourceInterfaces.h"

#include <shared/StringTools.h>

#include <sstream>
#include <iomanip>

CAfxStreams g_AfxStreams;


// CAfxStream //////////////////////////////////////////////////////////////////

CAfxStream::CAfxStream(TopStreamType topStreamType, char const * streamName)
: m_TopStreamType(topStreamType) 
, m_StreamName(streamName)
, m_Streams(0)
{
}

CAfxStream::~CAfxStream()
{
}

char const * CAfxStream::GetStreamName(void)
{
	return m_StreamName.c_str();
}

CAfxStream::TopStreamType CAfxStream::GetTopStreamType(void)
{
	return m_TopStreamType;
}

void CAfxStream::StreamAttach(IAfxStreams4Stream * streams)
{
	m_Streams = streams;
}

void CAfxStream::StreamDetach(IAfxStreams4Stream * streams)
{
	m_Streams = 0;
}

// CAfxDeveloperStream /////////////////////////////////////////////////////////

CAfxDeveloperStream::CAfxDeveloperStream(char const * streamName)
: CAfxStream(TST_CAfxDeveloperStream, streamName)
, m_ReplaceUpdate(false)
, m_Replace (false)
, m_ReplaceMaterial(0)
, m_ReplaceMaterialActive(false)
, m_BlockDraw(false)
{
}
	
CAfxDeveloperStream::~CAfxDeveloperStream()
{
	delete m_ReplaceMaterial;
}

void CAfxDeveloperStream::MatchName_set(char const * value)
{
	m_MatchName.assign(value);
}

char const * CAfxDeveloperStream::MatchName_get(void)
{
	return m_MatchName.c_str();
}

void CAfxDeveloperStream::MatchTextureGroupName_set(char const * value)
{
	m_MatchTextureGroupName.assign(value);
}

char const * CAfxDeveloperStream::MatchTextureGroupName_get(void)
{
	return m_MatchTextureGroupName.c_str();
}

void CAfxDeveloperStream::ReplaceName_set(char const * name)
{
	m_ReplaceName.assign(name);
	m_ReplaceUpdate = true;
}

char const * CAfxDeveloperStream::ReplaceName_get(void)
{
	return m_ReplaceName.c_str();
}

void CAfxDeveloperStream::BlockDraw_set(bool value)
{
	m_BlockDraw = value;
}

bool CAfxDeveloperStream::BlockDraw_get(void)
{
	return m_BlockDraw;
}

void CAfxDeveloperStream::StreamAttach(IAfxStreams4Stream * streams)
{
	CAfxStream::StreamAttach(streams);

	if(m_ReplaceUpdate)
	{
		m_ReplaceUpdate = false;
		m_Replace = !m_ReplaceName.empty();

		if(m_Replace)
		{
			delete m_ReplaceMaterial; m_ReplaceMaterial = 0;
			m_ReplaceMaterial = new CAfxMaterial(streams->GetFreeMaster(), streams->GetMaterialSystem()->FindMaterial(m_ReplaceName.c_str(),NULL));
		}
	}

	if(m_Replace)
	{
		streams->OnBind_set(this);
		streams->OnDrawInstances_set(this);
	}
}

void CAfxDeveloperStream::StreamDetach(IAfxStreams4Stream * streams)
{
	streams->OnDrawInstances_set(0);
	streams->OnBind_set(0);

	CAfxStream::StreamDetach(streams);
}

void CAfxDeveloperStream::Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData)
{
	bool replace =
		m_Replace
		&& !strcmp(material->GetTextureGroupName(), m_MatchTextureGroupName.c_str())
		&& !strcmp(material->GetName(), m_MatchName.c_str())
	;

	m_ReplaceMaterialActive = replace;

	if(replace)
		Tier0_Msg("Replaced %s|%s with %s\n", material->GetTextureGroupName(), material->GetName(), m_ReplaceName.c_str());

	ctx->GetParent()->Bind(replace ? m_ReplaceMaterial->GetMaterial() : material, proxyData);
}

void CAfxDeveloperStream::DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
{
	if(!(m_BlockDraw && m_ReplaceMaterialActive)) 
		ctx->GetParent()->DrawInstances(nInstanceCount, pInstance);
}

// CAfxMatteEntityStream ///////////////////////////////////////////////////////

CAfxMatteEntityStream::CAfxMatteEntityStream(char const * streamName)
: CAfxStream(TST_CAfxMatteEntityStream, streamName)
, m_CurrentAction(0)
, m_MatteAction(0)
, m_PassthroughAction(0)
, m_InvisibleAction(0)
, m_NoDrawAction(0)
, m_BoundAction(false)
{
}

CAfxMatteEntityStream::~CAfxMatteEntityStream()
{
	delete m_NoDrawAction;
	delete m_InvisibleAction;
	delete m_PassthroughAction;
	delete m_MatteAction;
}

void CAfxMatteEntityStream::StreamAttach(IAfxStreams4Stream * streams)
{
	CAfxStream::StreamAttach(streams);

	if(!m_PassthroughAction) m_PassthroughAction = new CAction();
	if(!m_MatteAction) m_MatteAction = new CActionMatte(streams->GetFreeMaster(), streams->GetMaterialSystem());
	if(!m_InvisibleAction) m_InvisibleAction = new CActionInvisible(streams->GetFreeMaster(), streams->GetMaterialSystem());
	if(!m_NoDrawAction) m_NoDrawAction = new CActionNoDraw();

	// Set a default action, just in case:
	m_CurrentAction = m_PassthroughAction;

	streams->OnBind_set(this);
	streams->OnOverrideDepthEnable_set(this);
	streams->OnDrawInstances_set(this);
	streams->OnOverrideAlphaWriteEnable_set(this);
	streams->OnOverrideColorWriteEnable_set(this);
	streams->OnSetColorModulation_set(this);
}

void CAfxMatteEntityStream::StreamDetach(IAfxStreams4Stream * streams)
{
	if(m_BoundAction) m_CurrentAction->AfxUnbind(streams->GetCurrentContext());

	streams->OnSetColorModulation_set(0);
	streams->OnOverrideColorWriteEnable_set(0);
	streams->OnOverrideAlphaWriteEnable_set(0);
	streams->OnDrawInstances_set(0);
	streams->OnOverrideDepthEnable_set(0);
	streams->OnBind_set(0);

	CAfxStream::StreamDetach(streams);
}

void CAfxMatteEntityStream::Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData )
{
	if(m_BoundAction) m_CurrentAction->AfxUnbind(ctx);

	CAfxMaterialKey key(material);

	std::map<CAfxMaterialKey, CAction *>::iterator it = m_Map.find(key);

	if(it != m_Map.end())
		m_CurrentAction = it->second;
	else
	{
		// determine current action and cache it.

		const char * groupName =  material->GetTextureGroupName();
		const char * name = material->GetName();

		Tier0_Msg("Material: %s %s -> ", groupName, name);

		if(
			!strcmp("World textures", groupName)
			|| !strcmp("SkyBox textures", groupName)
			|| !strcmp("StaticProp textures", groupName)
			|| (
				!strcmp("Model textures", groupName)
				&& !StringBeginsWith(name, "models/player/")
			)
			|| (
				!strcmp("Other textures", groupName)
				&& (
					!strcmp(name, "cable/cable")
					|| StringBeginsWith(name, "cs_custom_material_")
				)
			)
		)
		{
			Tier0_Msg("matte");
			m_CurrentAction = m_MatteAction;
		}
		else
		if(
			!strcmp("Decal textures", groupName)
			|| (
				!strcmp("Other textures", groupName)
				&& (
					StringBeginsWith(name, "effects/")
					|| StringBeginsWith(name, "particle/")
				)
			)
			|| (
				!strcmp("Precached", groupName)
				&& (
					StringBeginsWith(name, "effects/")
					|| StringBeginsWith(name, "particle/")
				)
			) 
		)
		{
			Tier0_Msg("noDraw");
			m_CurrentAction = m_NoDrawAction;
		}
		else
		{
			Tier0_Msg("passthrough");
			m_CurrentAction = m_PassthroughAction;
		}

		m_Map[key] = m_CurrentAction;

		Tier0_Msg("\n");
	}

	m_CurrentAction->Bind(ctx, material, proxyData);

	m_BoundAction = true;
}

void CAfxMatteEntityStream::OverrideDepthEnable(IAfxMatRenderContext * ctx, bool bEnable, bool bDepthEnable, bool bUnknown)
{
	m_CurrentAction->OverrideDepthEnable(ctx, bEnable, bDepthEnable, bUnknown);
}

void CAfxMatteEntityStream::DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
{
	m_CurrentAction->DrawInstances(ctx, nInstanceCount, pInstance);
}

void CAfxMatteEntityStream::OverrideAlphaWriteEnable(IAfxMatRenderContext * ctx, bool bOverrideEnable, bool bAlphaWriteEnable )
{
	m_CurrentAction->OverrideAlphaWriteEnable(ctx, bOverrideEnable, bAlphaWriteEnable);
}

void CAfxMatteEntityStream::OverrideColorWriteEnable(IAfxMatRenderContext * ctx, bool bOverrideEnable, bool bColorWriteEnable )
{
	m_CurrentAction->OverrideColorWriteEnable(ctx, bOverrideEnable, bColorWriteEnable);
}

void CAfxMatteEntityStream::SetColorModulation(IAfxVRenderView * rv, float const* blend)
{
	m_CurrentAction->SetColorModulation(rv, blend);
}

// CAfxStreams /////////////////////////////////////////////////////////////////

CAfxStreams::CAfxStreams()
: m_RecordName("untitled")
, m_OnAfxBaseClientDll_Free(0)
, m_MaterialSystem(0)
, m_VRenderView(0)
, m_AfxBaseClientDll(0)
, m_CurrentContext(0)
, m_PreviewStream(0)
, m_Recording(false)
, m_Frame(0)
{
}

CAfxStreams::~CAfxStreams()
{
	while(!m_Streams.empty())
	{
		delete m_Streams.front();
		m_Streams.pop_front();
	}

	delete m_OnAfxBaseClientDll_Free;
}


void CAfxStreams::OnMaterialSystem(IMaterialSystem_csgo * value)
{
	m_MaterialSystem = value;
}

void CAfxStreams::OnAfxVRenderView(IAfxVRenderView * value)
{
	m_VRenderView = value;
}

void CAfxStreams::OnAfxBaseClientDll(IAfxBaseClientDll * value)
{
	if(m_OnAfxBaseClientDll_Free) { delete m_OnAfxBaseClientDll_Free; m_OnAfxBaseClientDll_Free = 0; }
	m_AfxBaseClientDll = value;
	if(m_AfxBaseClientDll)
	{
		m_OnAfxBaseClientDll_Free = new CFreeDelegate(m_AfxBaseClientDll->GetFreeMaster(), this, &CAfxStreams::OnAfxBaseClientDll_Free);
		m_AfxBaseClientDll->OnView_Render_set(this);
	}
}

void CAfxStreams::OnAfxBaseClientDll_Free(void)
{
	if(m_AfxBaseClientDll) { m_AfxBaseClientDll->OnView_Render_set(0); m_AfxBaseClientDll = 0; }
}

void CAfxStreams::Console_RecordName_set(const char * value)
{
	if(StringIsEmpty(value))
	{
		Tier0_Msg("Error: Record name can not be emty.\n");
		return;
	}
	if(!StringIsAlNum(value))
	{
		Tier0_Msg("Error: Record name must be alphanumeric.\n");
		return;
	}

	m_RecordName.assign(value);
}

const char * CAfxStreams::Console_RecordName_get()
{
	return m_RecordName.c_str();
}


void CAfxStreams::Console_Record_Start()
{
	Console_Record_End();

	m_Recording = true;
	m_Frame = 0;
}

void CAfxStreams::Console_Record_End()
{
	m_Recording = false;
}

void CAfxStreams::Console_AddDeveloperStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	m_Streams.push_back(new CAfxDeveloperStream(streamName));
}

void CAfxStreams::Console_AddMatteWorldStream(const char * streamName)
{
	Tier0_Msg("Error: Not implemented yet.\n");
}

void CAfxStreams::Console_AddMatteEntityStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	m_Streams.push_back(new CAfxMatteEntityStream(streamName));
}

void CAfxStreams::Console_PrintStreams()
{
	Tier0_Msg("index: name\n");
	int index = 0;
	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		Tier0_Msg("%i: %s\n", index, (*it)->GetStreamName());
		++index;
	}
	Tier0_Msg(
		"=== Total streams: %i ===\n",
		index
	);
}

void CAfxStreams::Console_RemoveStream(int index)
{
	int curIndex = 0;
	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if(curIndex == index)
		{
			CAfxStream * cur = *it;
			m_Streams.erase(it);

			if(m_PreviewStream == cur) m_PreviewStream = 0;
			return;
		}

		++curIndex;
	}
	Tier0_Msg("Error: invalid index.\n");
}

void CAfxStreams::Console_PreviewStream(int index)
{
	if(index == -1)
	{
		m_PreviewStream = 0;
		return;
	}

	int curIndex = 0;
	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if(curIndex == index)
		{
			CAfxStream * cur = *it;
			m_PreviewStream = cur;
			return;
		}

		++curIndex;
	}
	Tier0_Msg("Error: invalid index.\n");
}

void CAfxStreams::Console_EditStream(int index, IWrpCommandArgs * args, int argcOffset, char const * cmdPrefix)
{
	int curIndex = 0;
	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if(curIndex == index)
		{
			CAfxStream * cur = *it;
			CAfxDeveloperStream * curDeveloper = cur->AsAfxDeveloperStream();
			CAfxMatteEntityStream * curMatteEntitiy = cur->AsAfxMatteEntityStream();

			int argc = args->ArgC() -argcOffset;

			if(1 <= argc)
			{
				char const * cmd0 = args->ArgV(argcOffset +0);

				if(curDeveloper && !_stricmp(cmd0, "matchTextureGroupName"))
				{
					if(2 <= argc)
					{
						char const * cmd1 = args->ArgV(argcOffset +1);

						curDeveloper->MatchTextureGroupName_set(cmd1);

						return;
					}

					Tier0_Msg(
						"%s matchTextureGroupName <name> - Set new texture group name to match.\n"
						"Current value: %s.\n"
						, cmdPrefix
						, curDeveloper->MatchTextureGroupName_get()
					);
					return;
				}
				else
				if(curDeveloper && !_stricmp(cmd0, "matchName"))
				{
					if(2 <= argc)
					{
						char const * cmd1 = args->ArgV(argcOffset +1);

						curDeveloper->MatchName_set(cmd1);

						return;
					}

					Tier0_Msg(
						"%s matchName <name> - Set new name to match.\n"
						"Current value: %s.\n"
						, cmdPrefix
						, curDeveloper->MatchName_get()
					);
					return;
				}
				else
				if(curDeveloper && !_stricmp(cmd0, "replaceName"))
				{
					if(2 <= argc)
					{
						char const * cmd1 = args->ArgV(argcOffset +1);

						curDeveloper->ReplaceName_set(cmd1);

						return;
					}

					Tier0_Msg(
						"%s replaceName <name> - Set the name of the replacement material, set an empty string(\"\") to replace nothing.\n"
						"Current value: %s.\n"
						, cmdPrefix
						, curDeveloper->ReplaceName_get()
					);
					return;
				}
				else
				if(curDeveloper && !_stricmp(cmd0, "blockDraw"))
				{
					if(2 <= argc)
					{
						char const * cmd1 = args->ArgV(argcOffset +1);

						curDeveloper->BlockDraw_set(0 != atoi(cmd1));

						return;
					}

					Tier0_Msg(
						"%s blockDraw 0|1 - Whether to block drawing when replaceMaterial is active.\n"
						"Current value: %i.\n"
						, cmdPrefix
						, curDeveloper->BlockDraw_get() ? 1L : 0L
					);
					return;
				}
			}
			
			if(curDeveloper)
			{
				Tier0_Msg("%s matchTextureGroupName [....]\n", cmdPrefix);
				Tier0_Msg("%s matchName [....]\n", cmdPrefix);
				Tier0_Msg("%s replaceName [....]\n", cmdPrefix);
				Tier0_Msg("%s blockDraw [....]\n", cmdPrefix);
			}
			Tier0_Msg("No further options for this stream.\n");
			return;
		}

		++curIndex;
	}
	Tier0_Msg("Error: invalid index.\n");
}

IMaterialSystem_csgo * CAfxStreams::GetMaterialSystem(void)
{
	return m_MaterialSystem;
}

IAfxFreeMaster * CAfxStreams::GetFreeMaster(void)
{
	if(m_AfxBaseClientDll) return m_AfxBaseClientDll->GetFreeMaster();
	return 0;
}

IAfxMatRenderContext * CAfxStreams::GetCurrentContext(void)
{
	return m_CurrentContext;
}

void CAfxStreams::OnBind_set(IAfxMatRenderContextBind * value)
{
	if(m_CurrentContext) m_CurrentContext->OnBind_set(value);
}

void CAfxStreams::OnOverrideDepthEnable_set(IAfxMatRenderContextOverrideDepthEnable * value)
{
	if(m_CurrentContext) m_CurrentContext->OnOverrideDepthEnable_set(value);
}

void CAfxStreams::OnDrawInstances_set(IAfxMatRenderContextDrawInstances * value)
{
	if(m_CurrentContext) m_CurrentContext->OnDrawInstances_set(value);
}

void CAfxStreams::OnOverrideAlphaWriteEnable_set(IAfxMatRenderContextOverrideAlphaWriteEnable * value)
{
	if(m_CurrentContext) m_CurrentContext->OnOverrideAlphaWriteEnable_set(value);
}

void CAfxStreams::OnOverrideColorWriteEnable_set(IAfxMatRenderContextOverrideColorWriteEnable * value)
{
	if(m_CurrentContext) m_CurrentContext->OnOverrideColorWriteEnable_set(value);
}

void CAfxStreams::OnSetColorModulation_set(IAfxVRenderViewSetColorModulation * value)
{
	if(m_VRenderView) m_VRenderView->OnSetColorModulation_set(value);
}

void CAfxStreams::View_Render(IAfxBaseClientDll * cl, IAfxMatRenderContext * cx, vrect_t_csgo *rect)
{
	m_CurrentContext = cx;

	bool canFeed = CheckCanFeedStreams();

	if(m_PreviewStream)
	{
		if(!canFeed)
			Tier0_Warning("Error: Cannot preview stream %s due to missing dependencies!\n", m_PreviewStream->GetStreamName());
		else
			m_PreviewStream->StreamAttach(this);
	}

	cl->GetParent()->View_Render(rect);

	if(m_PreviewStream && canFeed)
		m_PreviewStream->StreamDetach(this);

	if(m_Recording)
	{
		if(!canFeed)
		{
			Tier0_Warning("Error: Cannot record streams due to missing dependencies!\n");
		}
		else
		{
			for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
			{
				std::ostringstream oss;
	
				oss << m_RecordName << "_"
					<< (*it)->GetStreamName() << "_"
					<< std::setfill('0') << std::setw(5) << m_Frame
					<< ".tga"
				;

				(*it)->StreamAttach(this);

				m_MaterialSystem->SwapBuffers();

				cl->GetParent()->WriteSaveGameScreenshotOfSize(oss.str().c_str(), rect->width, rect->height);

				(*it)->StreamDetach(this);
			}

		}

		++m_Frame;
	}

	m_CurrentContext = 0;
}

bool CAfxStreams::Console_CheckStreamName(char const * value)
{
	if(StringIsEmpty(value))
	{
		Tier0_Msg("Error: Stream name can not be emty.\n");
		return false;
	}
	if(!StringIsAlNum(value))
	{
		Tier0_Msg("Error: Stream name must be alphanumeric.\n");
		return false;
	}

	// Check if name is unique:
	{
		int index = 0;
		for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
		{
			if(!_stricmp((*it)->GetStreamName(), value))
			{
				Tier0_Msg("Error: Stream name must be unique, \"%s\" is already in use by stream with index %i.\n", value, index);
				return false;
			}

			++index;
		}
	}

	return true;
}

bool CAfxStreams::CheckCanFeedStreams(void)
{
	return 0 != m_MaterialSystem
		&& 0 != m_VRenderView
		&& 0 != m_AfxBaseClientDll
		&& 0 != m_CurrentContext
	;
}