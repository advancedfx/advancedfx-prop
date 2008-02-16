#include "SettingsPageElement.h"

hlaeSettingsPageElement::hlaeSettingsPageElement(wxWindow* window, const wxString& name,
	bool is_advanced)
{
	m_pagelist = new hlaeSettingsPageList;
	m_pagelist->DeleteContents(true);

	m_window = window;
	m_name = name;
	m_isadvanced = is_advanced;
}

hlaeSettingsPageElement::~hlaeSettingsPageElement()
{
	m_pagelist->Clear();
	delete m_pagelist;
}

void hlaeSettingsPageElement::Append(hlaeSettingsPageElement* page_element)
{
	m_pagelist->Append(page_element);
}

bool hlaeSettingsPageElement::IsAdvanced()
{
	return m_isadvanced;
}

const wxString& hlaeSettingsPageElement::GetName()
{
	return m_name;
}

wxWindow* hlaeSettingsPageElement::GetWindow()
{
	return m_window;
}

hlaeSettingsPageElement* hlaeSettingsPageElement::GetElement(int index)
{
	return (m_pagelist->Item(index))->GetData();
}

size_t hlaeSettingsPageElement::GetCount()
{
	return m_pagelist->GetCount();
}