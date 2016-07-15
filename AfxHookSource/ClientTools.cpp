#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-07-13 dominik.matrixstorm.com
//
// First changes:
// 2016-07-06 dominik.matrixstorm.com

#include "ClientTools.h"

#include "RenderView.h"

#include <iostream>
#include <fstream>

using namespace SOURCESDK::CSGO;

ClientTools g_ClientTools;

ClientTools::ClientTools()
: m_Recording(false)
, m_ClientTools(0)
{
}

void ClientTools::SetClientTools(SOURCESDK::CSGO::IClientTools * clientTools)
{
	m_ClientTools = clientTools;
}

/*
char * int2xml(rapidxml::xml_document<> & doc, int value)
{
	return doc.allocate_string(std::to_string(value).c_str());
}

char * bool2xml(rapidxml::xml_document<> & doc, bool value)
{
	return int2xml(doc, value ? (int)1 : 0);
}*/

void ClientTools::OnPostToolMessage(SOURCESDK::CSGO::HTOOLHANDLE hEntity, SOURCESDK::CSGO::KeyValues * msg)
{
	if (!(m_Recording && m_File))
		return;

	if (!(m_ClientTools && (hEntity != SOURCESDK::CSGO::HTOOLHANDLE_INVALID)  && msg))
		return;

	char const * msgName = msg->GetName();

	if (!strcmp("entity_state", msgName))
	{
		SOURCESDK::CSGO::EntitySearchResult ent = m_ClientTools->GetEntity(hEntity);

		int idx = m_ClientTools->GetEntIndex(ent);
		SOURCESDK::IClientEntity_csgo * ce = SOURCESDK::g_Entitylist_csgo->GetClientEntity(idx);
		SOURCESDK::C_BaseEntity_csgo * be = ce ? ce->GetBaseEntity() : 0;

		if (ce
			&& (m_ClientTools->IsPlayer(ent) || m_ClientTools->IsViewModel(ent) || m_ClientTools->IsRagdoll(ent)
				|| (be && !strcmp(be->GetClassname(), "class C_CSRagdoll"))
			)
		)
		{
			int handle = ce->GetRefEHandle().ToInt();

			m_TrackedHandles[hEntity] = handle;

			WriteDictionary("entity_state");
			WriteDictionary("handle"); Write((int)handle);
			{
				SOURCESDK::CSGO::BaseEntityRecordingState_t * pBaseEntityRs = (SOURCESDK::CSGO::BaseEntityRecordingState_t *)(msg->GetPtr("baseentity"));
				if (pBaseEntityRs)
				{
					WriteDictionary("baseentity");
					WriteDictionary("time"); Write((double)pBaseEntityRs->m_flTime);
					WriteDictionary("modelName"); WriteDictionary(pBaseEntityRs->m_pModelName);
					WriteDictionary("visible"); Write((bool)pBaseEntityRs->m_bVisible);
					WriteDictionary("renderOrigin"); Write(pBaseEntityRs->m_vecRenderOrigin);
					WriteDictionary("renderAngles"); Write(pBaseEntityRs->m_vecRenderAngles);
					WriteDictionary("/");
				}
			}

			{
				SOURCESDK::CSGO::BaseAnimatingRecordingState_t * pBaseAnimatingRs = (SOURCESDK::CSGO::BaseAnimatingRecordingState_t *)(msg->GetPtr("baseanimating"));
				if (pBaseAnimatingRs)
				{
					WriteDictionary("baseanimating");
					WriteDictionary("skin"); Write((int)pBaseAnimatingRs->m_nSkin);
					WriteDictionary("body"); Write((int)pBaseAnimatingRs->m_nBody);
					WriteDictionary("sequence"); Write((int)pBaseAnimatingRs->m_nSequence);
					if (pBaseAnimatingRs->m_pBoneList)
					{
						WriteDictionary("boneList"); Write(pBaseAnimatingRs->m_pBoneList);
					}
					WriteDictionary("/");
				}
			}

			bool viewModel = msg->GetBool("viewmodel");

			WriteDictionary("viewmodel"); Write((bool)viewModel);

			WriteDictionary("/");
		}
	}
	else
	if (!strcmp("deleted", msgName))
	{
		// We cannot read the handle from the entity, because by this time it has been invalidated already.
		// so we'll use the cachend handle instead:

		std::map<SOURCESDK::CSGO::HTOOLHANDLE, int>::iterator it = m_TrackedHandles.find(hEntity);
		if(it != m_TrackedHandles.end())
		{
			WriteDictionary("deleted");
			Write((int)(it->second));
			Write((double)g_Hook_VClient_RenderView.GetGlobals()->curtime_get());

			m_TrackedHandles.erase(it);
		}
	}
}

void ClientTools::OnPreRenderAllTools(void)
{
	UpdateRecording();
}

bool ClientTools::GetRecording(void)
{
	return m_Recording;
}

void ClientTools::StartRecording(wchar_t const * fileName)
{
	EndRecording();

	m_Recording = true;

	Dictionary_Clear();
	m_File = 0;

	_wfopen_s(&m_File, fileName, L"wb");

	if (m_File)
	{
		fputs("afxGameRecord", m_File);
		fputc('\0', m_File);
		int version = 0;
		fwrite(&version, sizeof(version), 1, m_File);
	}
	else
		Tier0_Warning("ERROR opening file \"%s\" for writing.\n", fileName);

	if (m_ClientTools)
	{
		m_ClientTools->EnableRecordingMode(true);
	}
	else
	{
		Tier0_Warning("ERROR: Missing ClientTools dependency.\n");
	}

	UpdateRecording();
}

void ClientTools::EndRecording()
{
	if (!m_Recording)
		return;

	if (m_ClientTools)
	{
		m_ClientTools->EnableRecordingMode(false);
	}

	if (m_File)
	{
		fclose(m_File);
	}

	Dictionary_Clear();

	m_Recording = false;
}

void ClientTools::UpdateRecording()
{
	if (!m_Recording)
		return;

	if (!m_ClientTools)
		return;

	for(EntitySearchResult ent = m_ClientTools->FirstEntity(); 0 != ent; ent = m_ClientTools->NextEntity(ent))
	{
		SOURCESDK::CSGO::HTOOLHANDLE hEnt = m_ClientTools->AttachToEntity(ent);

		if (hEnt != SOURCESDK::CSGO::HTOOLHANDLE_INVALID)
		{
			m_ClientTools->SetRecording(hEnt, true);
		}

		// never detach, the ToolsSystem does that already when the entity is removed:
		// m_ClientTools->DetachFromEntity(ent);
	}
}

void ClientTools::WriteDictionary(char const * value)
{
	int idx = Dictionary_Get(value);

	Write(idx);

	if (-1 == idx)
	{
		Write(value);
	}
}

void ClientTools::Write(bool value)
{
	fwrite(&value, sizeof(value), 1, m_File);
}

void ClientTools::Write(int value)
{
	fwrite(&value, sizeof(value), 1, m_File);
}

void ClientTools::Write(double value)
{
	fwrite(&value, sizeof(value), 1, m_File);
}

void ClientTools::Write(char const * value)
{
	fputs(value, m_File);
	fputc('\0', m_File);
}

void ClientTools::Write(SOURCESDK::Vector const & value)
{
	Write((double)value.x);
	Write((double)value.y);
	Write((double)value.z);
}

void ClientTools::Write(SOURCESDK::QAngle const & value)
{
	Write((double)value.x);
	Write((double)value.y);
	Write((double)value.z);
}

void ClientTools::Write(SOURCESDK::CSGO::CBoneList const * value)
{
	Write((int)value->m_nBones);

	for (int i = 0; i < value->m_nBones; ++i)
	{
		Write(value->m_vecPos[i]);
		Write(value->m_quatRot[i]);
	}
}

void ClientTools::Write(SOURCESDK::Quaternion const & value)
{
	Write((double)value.x);
	Write((double)value.y);
	Write((double)value.z);
	Write((double)value.w);
}

void ClientTools::DebugEntIndex(int index)
{
	if (!m_ClientTools)
		return;

	SOURCESDK::CSGO::HTOOLHANDLE hHandle = m_ClientTools->GetToolHandleForEntityByIndex(index);

	if (SOURCESDK::CSGO::HTOOLHANDLE_INVALID == hHandle)
	{
		Tier0_Msg("Invalid tool handle\n");
		return;
	}

	SOURCESDK::CSGO::EntitySearchResult sResult = m_ClientTools->GetEntity(hHandle);

	if (!sResult)
	{
		Tier0_Msg("Invalid search result\n");
		return;
	}

	Tier0_Msg(
		"IsPlayer: %i\n"
		"IsCombatCharacter: %i\n"
		"IsNPC: %i\n"
		"IsRagdoll: %i\n"
		"IsViewModel: %i\n"
		"IsViewModelOrAttachment: %i\n"
		"IsWeapon: %i\n"
		"IsSprite: %i\n"
		"IsProp: %i\n"
		"IsBrush: %i\n"
		, m_ClientTools->IsPlayer(sResult) ? 1 : 0
		, m_ClientTools->IsCombatCharacter(sResult) ? 1 : 0
		, m_ClientTools->IsNPC(sResult) ? 1 : 0
		, m_ClientTools->IsRagdoll(sResult) ? 1 : 0
		, m_ClientTools->IsViewModel(sResult) ? 1 : 0
		, m_ClientTools->IsViewModelOrAttachment(sResult) ? 1 : 0
		, m_ClientTools->IsWeapon(sResult) ? 1 : 0
		, m_ClientTools->IsSprite(sResult) ? 1 : 0
		, m_ClientTools->IsProp(sResult) ? 1 : 0
		, m_ClientTools->IsBrush(sResult) ? 1 : 0
	);

	SOURCESDK::Vector vec = m_ClientTools->GetAbsOrigin(hHandle);

	Tier0_Msg("GetAbsOrigin: %f %f %f\n", vec.x, vec.y, vec.z);

	SOURCESDK::QAngle ang = m_ClientTools->GetAbsAngles(hHandle);

	Tier0_Msg("GetAbsAngles: %f %f %f\n", ang.x, ang.y, ang.z);
}
