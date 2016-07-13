#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-07-13 dominik.matrixstorm.com
//
// First changes:
// 2016-07-06 dominik.matrixstorm.com

#include "SourceInterfaces.h"

#include <shared/rapidxml/rapidxml.hpp>
#include <string>

class ClientTools
{
public:
	ClientTools();

	void SetClientTools(SOURCESDK::CSGO::IClientTools * clientTools);

	void OnPostToolMessage(SOURCESDK::CSGO::HTOOLHANDLE hEntity, SOURCESDK::CSGO::KeyValues * msg);

	void OnPreRenderAllTools(void);

	bool GetRecording(void);

	void StartRecording(wchar_t const * fileName);
	void EndRecording();


private:
	bool m_Recording;
	SOURCESDK::CSGO::IClientTools * m_ClientTools;
	rapidxml::xml_document<> m_Doc;
	rapidxml::xml_node<> * m_AfxGameRecord;
	std::wstring m_FileName;

	void UpdateRecording();
};

extern ClientTools g_ClientTools;