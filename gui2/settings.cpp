#include "settings.h"

hlaeSettingsPageTemplate::hlaeSettingsPageTemplate(wxWindow* parent)
	: wxWindow(parent, wxID_ANY)
{}

void hlaeSettingsPageTemplate::ShowPage(bool is_advanced)
{}

void hlaeSettingsPageTemplate::ApplyChanges()
{}

hlaeSettingsPageGeneral::hlaeSettingsPageGeneral(wxWindow* parent)
	: hlaeSettingsPageTemplate(parent)
{}

void hlaeSettingsPageGeneral::ShowPage(bool is_advanced)
{
	new wxTextCtrl (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400,300));
	Show();
}

void hlaeSettingsPageGeneral::ApplyChanges()
{

}

hlaeListElementSettingsPage::hlaeListElementSettingsPage(
	hlaeSettingsPageTemplate* page, const wxString& name, bool is_advanced)
{
	m_pagelist = new hlaeList;
	m_pagelist->DeleteContents(true);

	m_page = page;
	m_name = name;
	m_isadvanced = is_advanced;
}

hlaeListElementSettingsPage::~hlaeListElementSettingsPage()
{
	m_pagelist->Clear();
	delete m_pagelist;
}

bool hlaeListElementSettingsPage::IsAdvanced()
{
	return m_isadvanced;
}

const wxString& hlaeListElementSettingsPage::GetName()
{
	return m_name;
}

hlaeSettingsPageTemplate* hlaeListElementSettingsPage::GetPage()
{
	return m_page;
}

hlaeListElementSettingsPage* hlaeListElementSettingsPage::GetElement(int index)
{
	return dynamic_cast<hlaeListElementSettingsPage*>(m_pagelist->Item(index)->GetData());
}

size_t hlaeListElementSettingsPage::GetCount()
{
	return m_pagelist->GetCount();
}

void hlaeListElementSettingsPage::Append(hlaeListElementSettingsPage* page_element)
{
	m_pagelist->Append(page_element);
}


BEGIN_EVENT_TABLE(hlaeDialogSettings, wxDialog)
	EVT_CHECKBOX(hlaeDialogSettings::hlaeID_AdvancedMode,
		hlaeDialogSettings::OnAdvancedMode)
	EVT_BUTTON(wxID_APPLY, hlaeDialogSettings::OnApply)
	EVT_BUTTON(wxID_OK, hlaeDialogSettings::OnOK)
	EVT_TREE_SEL_CHANGED(hlaeDialogSettings::hlaeID_SelectionChanged,
		hlaeDialogSettings::OnSelectionChanged)
END_EVENT_TABLE()

hlaeDialogSettings::hlaeDialogSettings(wxWindow* parent, wxWindowID id, const wxString& title,
	const wxPoint& pos, const wxSize& size)
	: wxDialog( parent, id, title, pos, size)
{
	m_pagelist = new hlaeList;
	m_pageidlist = new hlaeList;
	m_lastpage = new hlaeSettingsPageTemplate(this);

	m_pagelist->DeleteContents(true);
	m_pageidlist->DeleteContents(true);


	wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(4, 1, 0, 0);
	fgSizer1->AddGrowableRow(0);
	fgSizer1->AddGrowableCol(0);


	m_pagesizer = new wxFlexGridSizer(1, 2, 0, 0);
	m_pagesizer->AddGrowableCol(1);
	m_pagesizer->AddGrowableRow(0);

	m_treectrl = new wxTreeCtrl(this, hlaeID_SelectionChanged, wxDefaultPosition,
		wxSize(125,-1), wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT | wxTR_LINES_AT_ROOT);

	m_pagesizer->Add(m_treectrl, 0, wxRIGHT | wxEXPAND, 5);
	m_pagesizer->Add(m_lastpage, 0, wxLEFT | wxALIGN_CENTER , 5);

	fgSizer1->Add(m_pagesizer, 0, wxALL | wxEXPAND, 5);


	fgSizer1->Add( new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxLI_HORIZONTAL ), 0, wxEXPAND, 5);


	wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer(1, 2, 0, 0);
	fgSizer2->AddGrowableCol(0);
	
	m_chkAdvanced = new wxCheckBox( this, hlaeID_AdvancedMode, wxT("Advanced Settings"),
		wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer2->Add(m_chkAdvanced, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND, 5 );

	wxBoxSizer* bSizer = new wxBoxSizer(wxHORIZONTAL);
	
	m_btnOK = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer->Add( m_btnOK, 0, wxLEFT, 5 );
	
	m_btnCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"));
	bSizer->Add( m_btnCancel, 0, wxLEFT, 5 );
	
	m_btnApply = new wxButton( this, wxID_APPLY, wxT("Apply"));
	bSizer->Add( m_btnApply, 0, wxLEFT, 5 );

	fgSizer2->Add(bSizer, 1, 0, 5);

	fgSizer1->Add(fgSizer2, 1, wxALL|wxEXPAND, 5);

	SetSizer(fgSizer1);

	Layout();

	SetEscapeId(wxID_CANCEL);

	// All settingspages

	hlaeListElementSettingsPage* node_general =	new hlaeListElementSettingsPage(
		new hlaeSettingsPageGeneral(this), wxT("General"), false);
	node_general->Append(new hlaeListElementSettingsPage(
		new hlaeSettingsPageGeneral(this), wxT("Menu 1"), false));
	node_general->Append(new hlaeListElementSettingsPage(
		new hlaeSettingsPageGeneral(this), wxT("Menu 2"), false));
	m_pagelist->Append(node_general);

	hlaeListElementSettingsPage* node_advanced = new hlaeListElementSettingsPage(
		new hlaeSettingsPageGeneral(this), wxT("Advanced"), true);
	node_advanced->Append(new hlaeListElementSettingsPage(
		new hlaeSettingsPageGeneral(this),wxT("Advanced"), true));
	m_pagelist->Append(node_advanced);

	// Update the control
	UpdateTreeCtrl();
}

hlaeDialogSettings::~hlaeDialogSettings()
{
	m_pagelist->Clear();
	m_pageidlist->Clear();

	delete m_pagelist;
	delete m_pageidlist;
}

void hlaeDialogSettings::OnApply(wxCommandEvent& WXUNUSED(evt))
{
	m_lastpage->ApplyChanges();
}

void hlaeDialogSettings::OnOK(wxCommandEvent& WXUNUSED(evt))
{
	m_lastpage->ApplyChanges();
	Close();
}

void hlaeDialogSettings::UpdateTreeCtrl()
{
	m_pageidlist->Clear();
	m_treectrl->DeleteAllItems();
	m_treectrl->AddRoot(wxT("Settings"));

	for (hlaeList::iterator iter = m_pagelist->begin();
		iter != m_pagelist->end(); iter++)
	{
		wxObject* object = *iter;
		hlaeListElementSettingsPage* current = dynamic_cast<hlaeListElementSettingsPage*>(object);
	

		UpdateTreeCtrlNodes(current, m_treectrl->GetRootItem());
	}
}

void hlaeDialogSettings::UpdateTreeCtrlNodes(hlaeListElementSettingsPage* node,
	wxTreeItemId id)
{
	if (!node->IsAdvanced() || m_advancedmode)
	{
		wxTreeItemId sub_id = m_treectrl->AppendItem(id, node->GetName());

		hlaeListElementSettingsPageID* pageid_element = new hlaeListElementSettingsPageID;

		hlaeSettingsPageTemplate* page = node->GetPage();
		page->Hide();

		pageid_element->page = page;
		pageid_element->id = sub_id;
		m_pageidlist->Append(pageid_element);

		for (unsigned int i = 0; i < node->GetCount(); i++)
			UpdateTreeCtrlNodes(node->GetElement(i), sub_id);
	}
}

void hlaeDialogSettings::OnAdvancedMode(wxCommandEvent& WXUNUSED(evt))
{
	m_advancedmode = m_chkAdvanced->IsChecked();
	UpdateTreeCtrl();
}

void hlaeDialogSettings::OnSelectionChanged(wxTreeEvent& evt)
{
	for (hlaeList::iterator iter = m_pageidlist->begin();
		iter != m_pageidlist->end(); iter++)
	{
		wxObject* object = *iter;
		hlaeListElementSettingsPageID* current = dynamic_cast<hlaeListElementSettingsPageID*>(object);
	
		if (evt.GetItem() == current->id)
		{
			m_lastpage->Hide();
			current->page->ShowPage(m_advancedmode);

			m_pagesizer->Replace(m_lastpage, current->page);
			m_pagesizer->Layout();

			m_lastpage = current->page;

			break;
		}
	}
}