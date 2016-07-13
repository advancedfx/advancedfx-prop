#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-07-13 dominik.matrixstorm.com
//
// First changes:
// 2016-07-06 dominik.matrixstorm.com

#include "ClientTools.h"

#include <shared/rapidxml/rapidxml_print.hpp>

char * double2xml(rapidxml::xml_document<> & doc, double value);

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

char * int2xml(rapidxml::xml_document<> & doc, int value)
{
	return doc.allocate_string(std::to_string(value).c_str());
}

char * bool2xml(rapidxml::xml_document<> & doc, bool value)
{
	return int2xml(doc, value ? (int)1 : 0);
}

void ClientTools::OnPostToolMessage(SOURCESDK::CSGO::HTOOLHANDLE hEntity, SOURCESDK::CSGO::KeyValues * msg)
{
	if (!m_Recording)
		return;

	if (!(m_ClientTools && (hEntity != SOURCESDK::CSGO::HTOOLHANDLE_INVALID)  && msg))
		return;

	char const * msgNname = msg->GetName();

	if (!strcmp("entity_state", msgNname))
	{
		SOURCESDK::CSGO::EntitySearchResult ent = m_ClientTools->GetEntity(hEntity);

		if (m_ClientTools->IsPlayer(ent) || m_ClientTools->IsViewModel(ent))
		{
			int idx = m_ClientTools->GetEntIndex(ent);
			SOURCESDK::IClientEntity_csgo * ce = SOURCESDK::g_Entitylist_csgo->GetClientEntity(idx);

			if (ce)
			{
				int handle = ce->GetRefEHandle().ToInt();

				rapidxml::xml_node<> * pXEntityState = m_Doc.allocate_node(rapidxml::node_element, "entity_state");
				pXEntityState->append_attribute(m_Doc.allocate_attribute("handle", int2xml(m_Doc, handle)));

				{
					SOURCESDK::CSGO::BaseEntityRecordingState_t * pBaseEntityRs = (SOURCESDK::CSGO::BaseEntityRecordingState_t *)(msg->GetPtr("baseentity"));
					if (pBaseEntityRs)
					{
						rapidxml::xml_node<> * pXBaseEntity = m_Doc.allocate_node(rapidxml::node_element, "baseentity");
						pXBaseEntity->append_attribute(m_Doc.allocate_attribute("time", double2xml(m_Doc, pBaseEntityRs->m_flTime)));
						pXBaseEntity->append_attribute(m_Doc.allocate_attribute("modleName", m_Doc.allocate_string(pBaseEntityRs->m_pModelName)));
						pXBaseEntity->append_attribute(m_Doc.allocate_attribute("visible", bool2xml(m_Doc, pBaseEntityRs->m_bVisible)));

						{
							rapidxml::xml_node<> * pXOrigin = m_Doc.allocate_node(rapidxml::node_element, "renderOrigin");
							pXOrigin->append_attribute(m_Doc.allocate_attribute("x", double2xml(m_Doc, pBaseEntityRs->m_vecRenderOrigin.x)));
							pXOrigin->append_attribute(m_Doc.allocate_attribute("y", double2xml(m_Doc, pBaseEntityRs->m_vecRenderOrigin.y)));
							pXOrigin->append_attribute(m_Doc.allocate_attribute("z", double2xml(m_Doc, pBaseEntityRs->m_vecRenderOrigin.z)));
							pXBaseEntity->append_node(pXOrigin);
						}

						{
							rapidxml::xml_node<> * pXAngles = m_Doc.allocate_node(rapidxml::node_element, "renderAngles");
							pXAngles->append_attribute(m_Doc.allocate_attribute("x", double2xml(m_Doc, pBaseEntityRs->m_vecRenderAngles.x)));
							pXAngles->append_attribute(m_Doc.allocate_attribute("y", double2xml(m_Doc, pBaseEntityRs->m_vecRenderAngles.y)));
							pXAngles->append_attribute(m_Doc.allocate_attribute("z", double2xml(m_Doc, pBaseEntityRs->m_vecRenderAngles.z)));
							pXBaseEntity->append_node(pXAngles);
						}

						pXEntityState->append_node(pXBaseEntity);
					}
				}

				{
					SOURCESDK::CSGO::BaseAnimatingRecordingState_t * pBaseAnimatingRs = (SOURCESDK::CSGO::BaseAnimatingRecordingState_t *)(msg->GetPtr("baseanimating"));
					if (pBaseAnimatingRs)
					{
						rapidxml::xml_node<> * pxBaseAnimating = m_Doc.allocate_node(rapidxml::node_element, "baseanimating");
						pxBaseAnimating->append_attribute(m_Doc.allocate_attribute("skin", int2xml(m_Doc, pBaseAnimatingRs->m_nSkin)));
						pxBaseAnimating->append_attribute(m_Doc.allocate_attribute("body", int2xml(m_Doc, pBaseAnimatingRs->m_nBody)));
						pxBaseAnimating->append_attribute(m_Doc.allocate_attribute("sequence", int2xml(m_Doc, pBaseAnimatingRs->m_nSequence)));

						CBoneList * bl = pBaseAnimatingRs->m_pBoneList;
						if(bl)
						{
							rapidxml::xml_node<> * pXBoneList = m_Doc.allocate_node(rapidxml::node_element, "boneList");
							//pXBoneList->append_attribute(m_Doc.allocate_attribute("bones", int2xml(m_Doc, bl->m_nBones)));

							for (int i = 0; i < bl->m_nBones; ++i)
							{
								rapidxml::xml_node<> * pXBone = m_Doc.allocate_node(rapidxml::node_element, "b");
								pXBone->append_attribute(m_Doc.allocate_attribute("i", int2xml(m_Doc, i)));
								pXBone->append_attribute(m_Doc.allocate_attribute("x", double2xml(m_Doc, bl->m_vecPos[i].x)));
								pXBone->append_attribute(m_Doc.allocate_attribute("y", double2xml(m_Doc, bl->m_vecPos[i].y)));
								pXBone->append_attribute(m_Doc.allocate_attribute("z", double2xml(m_Doc, bl->m_vecPos[i].z)));
								pXBone->append_attribute(m_Doc.allocate_attribute("qx", double2xml(m_Doc, bl->m_quatRot[i].x)));
								pXBone->append_attribute(m_Doc.allocate_attribute("qy", double2xml(m_Doc, bl->m_quatRot[i].y)));
								pXBone->append_attribute(m_Doc.allocate_attribute("qz", double2xml(m_Doc, bl->m_quatRot[i].z)));
								pXBone->append_attribute(m_Doc.allocate_attribute("qw", double2xml(m_Doc, bl->m_quatRot[i].w)));

								pXBoneList->append_node(pXBone);
							}

							pxBaseAnimating->append_node(pXBoneList);
						}

						pXEntityState->append_node(pxBaseAnimating);
					}
				}

				bool viewModel = msg->GetBool("viewmodel");
				pXEntityState->append_attribute(m_Doc.allocate_attribute("viewmodel", bool2xml(m_Doc, viewModel)));

				m_AfxGameRecord->append_node(pXEntityState);
			}
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

	m_FileName.assign(fileName);

	m_Doc.clear();

	rapidxml::xml_node<> * decl = m_Doc.allocate_node(rapidxml::node_declaration);
	decl->append_attribute(m_Doc.allocate_attribute("version", "1.0"));
	decl->append_attribute(m_Doc.allocate_attribute("encoding", "utf-8"));
	m_Doc.append_node(decl);
	
	rapidxml::xml_node<> * cmt = m_Doc.allocate_node(rapidxml::node_comment, 0,
		"Recorded with HLAE / AfxHookSource from http://www.advancedfx.org"
	);

	m_AfxGameRecord = m_Doc.allocate_node(rapidxml::node_element, "afxGameRecord");
	m_AfxGameRecord->append_attribute(m_Doc.allocate_attribute("version", "0.0.1"));
	m_Doc.append_node(m_AfxGameRecord);

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

	std::string xmlString;
	rapidxml::print(std::back_inserter(xmlString), m_Doc);

	FILE * pFile = 0;
	_wfopen_s(&pFile, m_FileName.c_str(), L"wb");

	if (0 != pFile)
	{
		fputs(xmlString.c_str(), pFile);
		fclose(pFile);
	}
	else
		Tier0_Warning("Error: Could not open file to save afxGameRecord data!\n");

	m_Doc.clear();

	m_Recording = false;
}

void ClientTools::UpdateRecording()
{
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
