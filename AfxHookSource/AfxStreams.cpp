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

CAfxStream::CAfxStream(char const * streamName)
: m_StreamName(streamName)
, m_Streams(0)
, m_Record(true)
{
}

CAfxStream::~CAfxStream()
{
}


bool CAfxStream::Record_get(void)
{
	return m_Record;
}

void CAfxStream::Record_set(bool value)
{
	m_Record = value;
}

char const * CAfxStream::GetStreamName(void)
{
	return m_StreamName.c_str();
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
: CAfxStream(streamName)
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
		streams->OnDraw_set(this);
		streams->OnDraw_2_set(this);
		streams->OnDrawModulated_set(this);
		streams->OnDrawInstances_set(this);
	}
}

void CAfxDeveloperStream::StreamDetach(IAfxStreams4Stream * streams)
{
	streams->OnDrawInstances_set(0);
	streams->OnDrawModulated_set(0);
	streams->OnDraw_2_set(0);
	streams->OnDraw_set(0);
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

//	if(replace)	Tier0_Msg("Replaced %s|%s with %s\n", material->GetTextureGroupName(), material->GetName(), m_ReplaceName.c_str());

	ctx->GetParent()->Bind(replace ? m_ReplaceMaterial->GetMaterial() : material, proxyData);
}

void CAfxDeveloperStream::DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
{
	if(!(m_BlockDraw && m_ReplaceMaterialActive)) 
		ctx->GetParent()->DrawInstances(nInstanceCount, pInstance);
}

void CAfxDeveloperStream::Draw(IAfxMesh * am, int firstIndex, int numIndices)
{
	if(!(m_BlockDraw && m_ReplaceMaterialActive)) 
		am->GetParent()->Draw(firstIndex, numIndices);
	else
		am->GetParent()->MarkAsDrawn();
}

void CAfxDeveloperStream::Draw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists)
{
	if(!(m_BlockDraw && m_ReplaceMaterialActive)) 
		am->GetParent()->Draw(pLists, nLists);
	else
		am->GetParent()->MarkAsDrawn();
}

void CAfxDeveloperStream::DrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex, int numIndices)
{
	if(!(m_BlockDraw && m_ReplaceMaterialActive)) 
		am->GetParent()->DrawModulated(vecDiffuseModulation, firstIndex, numIndices);
	else
		am->GetParent()->MarkAsDrawn();
}

// CAfxMatteStream /////////////////////////////////////////////////////////////

CAfxMatteStream::CAfxMatteStream(char const * streamName, bool isEntityStream)
: CAfxStream(streamName)
, m_CurrentAction(0)
, m_MatteAction(0)
, m_PassthroughAction(0)
, m_InvisibleAction(0)
, m_NoDrawAction(0)
, m_BoundAction(false)
, m_WorldTexturesAction(isEntityStream ? MA_Mask : MA_Draw)
, m_SkyBoxTexturesAction(isEntityStream ? MA_Mask : MA_Draw)
, m_StaticPropTexturesAction(isEntityStream ? MA_Mask : MA_Draw)
, m_CableAction(isEntityStream ? MA_Mask : MA_Draw)
, m_PlayerModelsAction(isEntityStream ? MA_Draw : MA_Invisible)
, m_WeaponModelsAction(isEntityStream ? MA_Draw : MA_Invisible)
, m_ShellModelsAction(isEntityStream ? MA_Draw : MA_Invisible)
, m_OtherModelsAction(isEntityStream ? MA_Mask : MA_Draw)
, m_DecalTexturesAction(isEntityStream ? HA_NoDraw : HA_Draw)
, m_EffectsAction(isEntityStream ? HA_NoDraw : HA_Draw)
, m_ShellParticleAction(isEntityStream ? HA_Draw : HA_NoDraw)
, m_OtherParticleAction(isEntityStream ? HA_NoDraw : HA_Draw)
, m_StickerAction(isEntityStream ? MA_Draw : MA_Invisible)
, m_DebugPrint(false)
{
}

CAfxMatteStream::~CAfxMatteStream()
{
	delete m_NoDrawAction;
	delete m_InvisibleAction;
	delete m_PassthroughAction;
	delete m_MatteAction;
}

void CAfxMatteStream::StreamAttach(IAfxStreams4Stream * streams)
{
	CAfxStream::StreamAttach(streams);

	if(!m_PassthroughAction) m_PassthroughAction = new CAction();
	if(!m_MatteAction) m_MatteAction = new CActionMatte(streams->GetFreeMaster(), streams->GetMaterialSystem());
	if(!m_InvisibleAction) m_InvisibleAction = new CActionInvisible(streams->GetFreeMaster(), streams->GetMaterialSystem());
	if(!m_NoDrawAction) m_NoDrawAction = new CActionNoDraw();

	// Set a default action, just in case:
	m_CurrentAction = m_PassthroughAction;

	streams->OnBind_set(this);
	streams->OnDrawInstances_set(this);
	streams->OnDraw_set(this);
	streams->OnDraw_2_set(this);
	streams->OnDrawModulated_set(this);
	streams->OnSetColorModulation_set(this);
}

void CAfxMatteStream::StreamDetach(IAfxStreams4Stream * streams)
{
	if(m_BoundAction) m_CurrentAction->AfxUnbind(streams->GetCurrentContext());

	streams->OnSetColorModulation_set(0);
	streams->OnDrawModulated_set(0);
	streams->OnDraw_2_set(0);
	streams->OnDraw_set(0);
	streams->OnDrawInstances_set(0);
	streams->OnBind_set(0);

	CAfxStream::StreamDetach(streams);
}

void CAfxMatteStream::Bind(IAfxMatRenderContext * ctx, IMaterial_csgo * material, void *proxyData )
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

		if(m_DebugPrint) Tier0_Msg("Stream %s: Caching Material: %s|%s -> ", GetStreamName(), groupName, name);

		if(!strcmp("Decal textures", groupName))
			m_CurrentAction = GetAction(m_DecalTexturesAction);
		else
		if(!strcmp("World textures", groupName))
			m_CurrentAction = GetAction(m_WorldTexturesAction);
		else
		if(!strcmp("SkyBox textures", groupName))
			m_CurrentAction = GetAction(m_SkyBoxTexturesAction);
		else
		if(!strcmp("StaticProp textures", groupName))
		{
			if(StringBeginsWith(name, "models/weapons/"))
				m_CurrentAction = GetAction(m_WeaponModelsAction);
			else
				m_CurrentAction = GetAction(m_StaticPropTexturesAction);
		}
		else
		if(!strcmp("Model textures", groupName))
		{
			if(StringBeginsWith(name, "models/player/"))
				m_CurrentAction = GetAction(m_PlayerModelsAction);
			else
			if(StringBeginsWith(name, "models/weapons/"))
				m_CurrentAction = GetAction(m_WeaponModelsAction);
			else
			if(StringBeginsWith(name, "models/shells/"))
				m_CurrentAction = GetAction(m_ShellModelsAction);
			else
				m_CurrentAction = GetAction(m_OtherModelsAction);
		}
		else
		if(!strcmp("Other textures", groupName))
		{
			if(!strcmp(name, "cable/cable"))
				m_CurrentAction = GetAction(m_CableAction);
			else
			if(StringBeginsWith(name, "cs_custom_material_"))
				m_CurrentAction = GetAction(m_WeaponModelsAction);
			else
			if(StringBeginsWith(name, "effects/"))
				m_CurrentAction = GetAction(m_EffectsAction);
			else
			if(StringBeginsWith(name, "particle/"))
			{
				if(StringBeginsWith(name, "particle/shells/"))
					m_CurrentAction = GetAction(m_ShellParticleAction);
				else
					m_CurrentAction = GetAction(m_OtherParticleAction);
			}
			else
			if(StringBeginsWith(name, "sticker_"))
					m_CurrentAction = GetAction(m_StickerAction);
			else
			{
				if(m_DebugPrint) Tier0_Msg("draw (unhandled)");
				m_CurrentAction = m_PassthroughAction;
			}
		}
		else
		if(!strcmp("Particle textures", groupName))
		{
			if(StringBeginsWith(name, "particle/shells/"))
				m_CurrentAction = GetAction(m_ShellParticleAction);
			else
				m_CurrentAction = GetAction(m_OtherParticleAction);
		}
		else
		if(!strcmp("Precached", groupName))
		{
			if(StringBeginsWith(name, "effects/"))
				m_CurrentAction = GetAction(m_EffectsAction);
			else
			if(StringBeginsWith(name, "particle/"))
			{
				if(StringBeginsWith(name, "particle/shells/"))
					m_CurrentAction = GetAction(m_ShellParticleAction);
				else
					m_CurrentAction = GetAction(m_OtherParticleAction);
			}
			else
			{
				if(m_DebugPrint) Tier0_Msg("draw (unhandled)");
				m_CurrentAction = m_PassthroughAction;
			}
		}
		else
		{
			if(m_DebugPrint) Tier0_Msg("draw (unhandled)");
			m_CurrentAction = m_PassthroughAction;
		}

		m_Map[key] = m_CurrentAction;

		if(m_DebugPrint) Tier0_Msg("\n");
	}

	m_CurrentAction->Bind(ctx, material, proxyData);

	m_BoundAction = true;
}

void CAfxMatteStream::DrawInstances(IAfxMatRenderContext * ctx, int nInstanceCount, const MeshInstanceData_t_csgo *pInstance )
{
	m_CurrentAction->DrawInstances(ctx, nInstanceCount, pInstance);
}

void CAfxMatteStream::Draw(IAfxMesh * am, int firstIndex, int numIndices)
{
	m_CurrentAction->Draw(am, firstIndex, numIndices);
}

void CAfxMatteStream::Draw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists)
{
	m_CurrentAction->Draw_2(am, pLists, nLists);
}

void CAfxMatteStream::DrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex, int numIndices)
{
	m_CurrentAction->DrawModulated(am, vecDiffuseModulation, firstIndex, numIndices);
}

void CAfxMatteStream::SetColorModulation(IAfxVRenderView * rv, float const* blend)
{
	m_CurrentAction->SetColorModulation(rv, blend);
}

CAfxMatteStream::MaskableAction CAfxMatteStream::WorldTexturesAction_get(void)
{
	return m_WorldTexturesAction;
}

void CAfxMatteStream::WorldTexturesAction_set(MaskableAction value)
{
	InvalidateCache();
	m_WorldTexturesAction = value;
}

CAfxMatteStream::MaskableAction CAfxMatteStream::SkyBoxTexturesAction_get(void)
{
	return m_SkyBoxTexturesAction;
}

void CAfxMatteStream::SkyBoxTexturesAction_set(MaskableAction value)
{
	InvalidateCache();
	m_SkyBoxTexturesAction = value;
}

CAfxMatteStream::MaskableAction CAfxMatteStream::StaticPropTexturesAction_get(void)
{
	return m_StaticPropTexturesAction;
}

void CAfxMatteStream::StaticPropTexturesAction_set(MaskableAction value)
{
	InvalidateCache();
	m_StaticPropTexturesAction = value;
}

CAfxMatteStream::MaskableAction CAfxMatteStream::CableAction_get(void)
{
	return m_CableAction;
}

void CAfxMatteStream::CableAction_set(MaskableAction value)
{
	InvalidateCache();
	m_CableAction = value;
}

CAfxMatteStream::MaskableAction CAfxMatteStream::PlayerModelsAction_get(void)
{
	return m_PlayerModelsAction;
}

void CAfxMatteStream::PlayerModelsAction_set(MaskableAction value)
{
	InvalidateCache();
	m_PlayerModelsAction = value;
}

CAfxMatteStream::MaskableAction CAfxMatteStream::WeaponModelsAction_get(void)
{
	return m_WeaponModelsAction;
}

void CAfxMatteStream::WeaponModelsAction_set(MaskableAction value)
{
	InvalidateCache();
	m_WeaponModelsAction = value;
}

CAfxMatteStream::MaskableAction CAfxMatteStream::ShellModelsAction_get(void)
{
	return m_ShellModelsAction;
}

void CAfxMatteStream::ShellModelsAction_set(MaskableAction value)
{
	InvalidateCache();
	m_ShellModelsAction = value;
}

CAfxMatteStream::MaskableAction CAfxMatteStream::OtherModelsAction_get(void)
{
	return m_OtherModelsAction;
}

void CAfxMatteStream::OtherModelsAction_set(MaskableAction value)
{
	InvalidateCache();
	m_OtherModelsAction = value;
}

CAfxMatteStream::HideableAction CAfxMatteStream::DecalTexturesAction_get(void)
{
	return m_DecalTexturesAction;
}


void CAfxMatteStream::DecalTexturesAction_set(HideableAction value)
{
	InvalidateCache();
	m_DecalTexturesAction = value;
}

CAfxMatteStream::HideableAction CAfxMatteStream::EffectsAction_get(void)
{
	return m_EffectsAction;
}

void CAfxMatteStream::EffectsAction_set(HideableAction value)
{
	InvalidateCache();
	m_EffectsAction = value;
}

CAfxMatteStream::HideableAction CAfxMatteStream::ShellParticleAction_get(void)
{
	return m_ShellParticleAction;
}

void CAfxMatteStream::ShellParticleAction_set(HideableAction value)
{
	InvalidateCache();
	m_ShellParticleAction = value;
}

CAfxMatteStream::HideableAction CAfxMatteStream::OtherParticleAction_get(void)
{
	return m_OtherParticleAction;
}

void CAfxMatteStream::OtherParticleAction_set(HideableAction value)
{
	InvalidateCache();
	m_OtherParticleAction = value;
}

CAfxMatteStream::MaskableAction CAfxMatteStream::StickerAction_get(void)
{
	return m_StickerAction;
}

void CAfxMatteStream::StickerAction_set(MaskableAction value)
{
	InvalidateCache();
	m_StickerAction = value;
}

bool CAfxMatteStream::DebugPrint_get(void)
{
	return m_DebugPrint;
}

void CAfxMatteStream::DebugPrint_set(bool value)
{
	m_DebugPrint = value;
}

void CAfxMatteStream::InvalidateCache(void)
{
	if(m_DebugPrint) Tier0_Msg("Stream %s: Invalidating material cache.\n");
	m_Map.clear();
}

CAfxMatteStream::CAction * CAfxMatteStream::GetAction(MaskableAction value)
{
	switch(value)
	{
	case MA_Mask:
		if(m_DebugPrint) Tier0_Msg("mask");
		return m_MatteAction;
	case MA_Invisible:
		if(m_DebugPrint) Tier0_Msg("invisible");
		return m_InvisibleAction;
	};

	if(m_DebugPrint) Tier0_Msg("draw");
	return m_PassthroughAction;
}

CAfxMatteStream::CAction * CAfxMatteStream::GetAction(HideableAction value)
{
	switch(value)
	{
	case HA_NoDraw:
		if(m_DebugPrint) Tier0_Msg("noDraw");
		return m_NoDrawAction;
	};

	if(m_DebugPrint) Tier0_Msg("draw");
	return m_PassthroughAction;
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
, m_OnDraw(0)
, m_OnDraw_2(0)
, m_OnDrawModulated(0)
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

void CAfxStreams::OnDraw(IAfxMesh * am, int firstIndex, int numIndices)
{
	if(m_OnDraw)
		m_OnDraw->Draw(am, firstIndex, numIndices);
	else
		am->GetParent()->Draw(firstIndex, numIndices);
}

void CAfxStreams::OnDraw_2(IAfxMesh * am, CPrimList_csgo *pLists, int nLists)
{
	if(m_OnDraw_2)
		m_OnDraw_2->Draw_2(am, pLists, nLists);
	else
		am->GetParent()->Draw(pLists, nLists);
}

void CAfxStreams::OnDrawModulated(IAfxMesh * am, const Vector4D_csgo &vecDiffuseModulation, int firstIndex, int numIndices)
{
	if(m_OnDrawModulated)
		m_OnDrawModulated->DrawModulated(am, vecDiffuseModulation, firstIndex, numIndices);
	else
		am->GetParent()->DrawModulated(vecDiffuseModulation, firstIndex, numIndices);
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

void CAfxStreams::Console_AddStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	m_Streams.push_back(new CAfxStream(streamName));
}


void CAfxStreams::Console_AddDeveloperStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	m_Streams.push_back(new CAfxDeveloperStream(streamName));
}

void CAfxStreams::Console_AddMatteWorldStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	m_Streams.push_back(new CAfxMatteWorldStream(streamName));
}

void CAfxStreams::Console_AddMatteEntityStream(const char * streamName)
{
	if(!Console_CheckStreamName(streamName))
		return;

	m_Streams.push_back(new CAfxMatteEntityStream(streamName));
}

void CAfxStreams::Console_PrintStreams()
{
	Tier0_Msg("index: name record\n");
	int index = 0;
	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		Tier0_Msg("%i: %s %s\n", index, (*it)->GetStreamName(), (*it)->Record_get() ? "1" : "0");
		++index;
	}
	Tier0_Msg(
		"=== Total streams: %i ===\n",
		index
	);
}

void CAfxStreams::Console_RemoveStream(const char * streamName)
{
	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if(!_stricmp(streamName, (*it)->GetStreamName()))
		{
			CAfxStream * cur = *it;
			m_Streams.erase(it);

			if(m_PreviewStream == cur) m_PreviewStream = 0;
			return;
		}
	}
	Tier0_Msg("Error: invalid streamName.\n");
}

void CAfxStreams::Console_PreviewStream(const char * streamName)
{
	if(StringIsEmpty(streamName))
	{
		m_PreviewStream = 0;
		return;
	}

	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if(!_stricmp(streamName, (*it)->GetStreamName()))
		{
			CAfxStream * cur = *it;
			m_PreviewStream = cur;
			return;
		}
	}
	Tier0_Msg("Error: invalid streamName.\n");
}

void CAfxStreams::Console_EditStream(const char * streamName, IWrpCommandArgs * args, int argcOffset, char const * cmdPrefix)
{
	for(std::list<CAfxStream *>::iterator it = m_Streams.begin(); it != m_Streams.end(); ++it)
	{
		if(!_stricmp(streamName, (*it)->GetStreamName()))
		{
			CAfxStream * cur = *it;
			CAfxMatteStream * curMatte = cur->AsAfxMatteStream();
			CAfxDeveloperStream * curDeveloper = cur->AsAfxDeveloperStream();

			int argc = args->ArgC() -argcOffset;

			if(cur)
			{
				if(1 <= argc)
				{
					char const * cmd0 = args->ArgV(argcOffset +0);

					if(!_stricmp(cmd0, "record"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);

							cur->Record_set(atoi(cmd1) != 0 ? true : false);

							return;
						}

						Tier0_Msg(
							"%s record 0|1 - Disable / enable recording.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, cur->Record_get() ? "1" : "0"
						);
						return;
					}
				}
			}

			if(curDeveloper)
			{
				if(1 <= argc)
				{
					char const * cmd0 = args->ArgV(argcOffset +0);

					if(!_stricmp(cmd0, "matchTextureGroupName"))
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
					if(!_stricmp(cmd0, "matchName"))
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
					if(!_stricmp(cmd0, "replaceName"))
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
					if(!_stricmp(cmd0, "blockDraw"))
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
			}

			if(curMatte)
			{
				if(1 <= argc)
				{
					char const * cmd0 = args->ArgV(argcOffset +0);

					if(!_stricmp(cmd0, "worldTexturesAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curMatte->WorldTexturesAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s worldTexturesAction draw|mask|invisible - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curMatte->WorldTexturesAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "skyBoxTexturesAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curMatte->SkyBoxTexturesAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s skyBoxTexturesAction draw|mask|invisible - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curMatte->SkyBoxTexturesAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "staticPropTexturesAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curMatte->StaticPropTexturesAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s staticPropTexturesAction draw|mask|invisible - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curMatte->StaticPropTexturesAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "cableAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curMatte->CableAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s cableAction draw|mask|invisible - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curMatte->CableAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "playerModelsAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curMatte->PlayerModelsAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s playerModelsAction draw|mask|invisible - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curMatte->PlayerModelsAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "weaponModelsAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curMatte->WeaponModelsAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s weaponModelsAction draw|mask|invisible - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curMatte->WeaponModelsAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "shellModelsAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curMatte->ShellModelsAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s shellModelsAction draw|mask|invisible - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curMatte->ShellModelsAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "otherModelsAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curMatte->OtherModelsAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s otherModelsAction draw|mask|invisible - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curMatte->OtherModelsAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "decalTexturesAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::HideableAction value;

							if(Console_ToHideableAction(cmd1, value))
							{
								curMatte->DecalTexturesAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s decalTexturesAction draw|noDraw - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromHideableAction(curMatte->DecalTexturesAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "effectsAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::HideableAction value;

							if(Console_ToHideableAction(cmd1, value))
							{
								curMatte->EffectsAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s effectsAction draw|noDraw - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromHideableAction(curMatte->EffectsAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "shellParticleAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::HideableAction value;

							if(Console_ToHideableAction(cmd1, value))
							{
								curMatte->ShellParticleAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s shellParticleAction draw|noDraw - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromHideableAction(curMatte->ShellParticleAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "otherParticleAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::HideableAction value;

							if(Console_ToHideableAction(cmd1, value))
							{
								curMatte->OtherParticleAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s otherParticleAction draw|noDraw - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromHideableAction(curMatte->OtherParticleAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "stickerAction"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);
							CAfxMatteStream::MaskableAction value;

							if(Console_ToMaskableAction(cmd1, value))
							{
								curMatte->StickerAction_set(value);
								return;
							}
						}

						Tier0_Msg(
							"%s stickerAction draw|mask|invisible - Set new action.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, Console_FromMaskableAction(curMatte->StickerAction_get())
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "debugPrint"))
					{
						if(2 <= argc)
						{
							char const * cmd1 = args->ArgV(argcOffset +1);

							curMatte->DebugPrint_set(0 != atoi(cmd1) ? true : false);
							return;
						}

						Tier0_Msg(
							"%s debugPrint 0|1 - Disable / enable debug console output.\n"
							"Current value: %s.\n"
							, cmdPrefix
							, curMatte->DebugPrint_get() ? "1" : "0"
						);
						return;
					}
					else
					if(!_stricmp(cmd0, "invalidateCache"))
					{
						curMatte->InvalidateCache();
						return;
					}
				}
			}

			if(cur)
			{
				Tier0_Msg("%s record [...]\n", cmdPrefix);
			}
			
			if(curDeveloper)
			{
				Tier0_Msg("%s matchTextureGroupName [...]\n", cmdPrefix);
				Tier0_Msg("%s matchName [...]\n", cmdPrefix);
				Tier0_Msg("%s replaceName [...]\n", cmdPrefix);
				Tier0_Msg("%s blockDraw [...]\n", cmdPrefix);
			}

			if(curMatte)
			{
				Tier0_Msg("%s worldTexturesAction [...]\n", cmdPrefix);
				Tier0_Msg("%s skyBoxTexturesAction [...]\n", cmdPrefix);
				Tier0_Msg("%s staticPropTexturesAction [...]\n", cmdPrefix);
				Tier0_Msg("%s cableAction [...]\n", cmdPrefix);
				Tier0_Msg("%s playerModelsAction [...]\n", cmdPrefix);
				Tier0_Msg("%s weaponModelsAction [...]\n", cmdPrefix);
				Tier0_Msg("%s shellModelsAction [...]\n", cmdPrefix);
				Tier0_Msg("%s otherModelsAction [...]\n", cmdPrefix);
				Tier0_Msg("%s decalTexturesAction [...]\n", cmdPrefix);
				Tier0_Msg("%s effectsAction [...]\n", cmdPrefix);
				Tier0_Msg("%s shellParticleAction [...]\n", cmdPrefix);
				Tier0_Msg("%s otherParticleAction [...]\n", cmdPrefix);
				Tier0_Msg("%s stickerAction [...]\n", cmdPrefix);
				Tier0_Msg("%s debugPrint [...]\n", cmdPrefix);
				Tier0_Msg("%s invalidateCache - invaldiates the material cache.\n", cmdPrefix);
			}

			Tier0_Msg("No further options for this stream.\n");
			return;
		}
	}
	Tier0_Msg("Error: invalid streamName.\n");
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

void CAfxStreams::OnDrawInstances_set(IAfxMatRenderContextDrawInstances * value)
{
	if(m_CurrentContext) m_CurrentContext->OnDrawInstances_set(value);
}

void CAfxStreams::OnDraw_set(IAfxMeshDraw * value)
{
	m_OnDraw = value;
}

void CAfxStreams::OnDraw_2_set(IAfxMeshDraw_2 * value)
{
	m_OnDraw_2 = value;
}

void CAfxStreams::OnDrawModulated_set(IAfxMeshDrawModulated * value)
{
	m_OnDrawModulated = value;
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
				if(!(*it)->Record_get()) continue;

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

bool CAfxStreams::Console_ToMaskableAction(char const * value, CAfxMatteStream::MaskableAction & maskableAction)
{
	if(!_stricmp(value, "draw"))
	{
		maskableAction = CAfxMatteStream::MA_Draw;
		return true;
	}
	else
	if(!_stricmp(value, "mask"))
	{
		maskableAction = CAfxMatteStream::MA_Mask;
		return true;
	}
	else
	if(!_stricmp(value, "invisible"))
	{
		maskableAction = CAfxMatteStream::MA_Invisible;
		return true;
	}

	return false;
}

bool CAfxStreams::Console_ToHideableAction(char const * value, CAfxMatteStream::HideableAction & hideableAction)
{
	if(!_stricmp(value, "draw"))
	{
		hideableAction = CAfxMatteStream::HA_Draw;
		return true;
	}
	else
	if(!_stricmp(value, "noDraw"))
	{
		hideableAction = CAfxMatteStream::HA_NoDraw;
		return true;
	}

	return false;
}

char const * CAfxStreams::Console_FromMaskableAction(CAfxMatteStream::MaskableAction maskableAction)
{
	switch(maskableAction)
	{
	case CAfxMatteStream::MA_Draw:
		return "draw";
	case CAfxMatteStream::MA_Mask:
		return "mask";
	case CAfxMatteStream::MA_Invisible:
		return "invisible";
	}

	return "[unknown]";
}

char const * CAfxStreams::Console_FromHideableAction(CAfxMatteStream::HideableAction hideableAction)
{
	switch(hideableAction)
	{
	case CAfxMatteStream::HA_Draw:
		return "draw";
	case CAfxMatteStream::HA_NoDraw:
		return "noDraw";
	}

	return "[unknown]";
}


bool CAfxStreams::CheckCanFeedStreams(void)
{
	return 0 != m_MaterialSystem
		&& 0 != m_VRenderView
		&& 0 != m_AfxBaseClientDll
		&& 0 != m_CurrentContext
	;
}