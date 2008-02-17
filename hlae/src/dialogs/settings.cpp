#include <wx/statline.h>

#include <hlae/dialogs/settings.h>
#include <hlae/settingspages/general.h>

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
	m_pagelist = new hlaeListSettingsPage;
	m_pageidlist = new hlaeListSettingsPageID;
	m_pagelist->DeleteContents(true);
	m_pageidlist->DeleteContents(true);


	wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(4, 1, 0, 0);
	fgSizer1->AddGrowableCol(0);
	fgSizer1->AddGrowableRow(0);
	fgSizer1->SetFlexibleDirection(wxBOTH);
	fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);

	m_pagesizer = new wxFlexGridSizer(1, 2, 0, 0);
	m_pagesizer->AddGrowableCol(1);
	m_pagesizer->AddGrowableRow(0);
	m_pagesizer->SetFlexibleDirection(wxBOTH);
	m_pagesizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_ALL);

	m_treectrl = new wxTreeCtrl(this, hlaeID_SelectionChanged, wxDefaultPosition,
		wxSize(125,-1), wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT | wxTR_LINES_AT_ROOT);

	m_pagesizer->Add(m_treectrl, 0, wxALL | wxEXPAND);
	m_pagesizer->Add(new hlaeSettingsPageTemplate(this), 0, wxLEFT | wxEXPAND, 5);

	fgSizer1->Add( m_pagesizer, 0, wxALL | wxEXPAND, 5);
	fgSizer1->Add( new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxLI_HORIZONTAL ), 0, wxEXPAND, 5);

	wxFlexGridSizer* fgSizer2 = new wxFlexGridSizer(1, 2, 0, 0);
	fgSizer2->AddGrowableCol(0);
	fgSizer2->AddGrowableRow(0);
	fgSizer2->SetFlexibleDirection(wxBOTH);
	fgSizer2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	
	m_chkAdvanced = new wxCheckBox( this, hlaeID_AdvancedMode, wxT("Advanced Settings"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer2->Add(m_chkAdvanced, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND, 5 );

	wxBoxSizer* bSizer2 = new wxBoxSizer(wxHORIZONTAL);
	
	m_btnOK = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_btnOK, 0, wxLEFT, 5 );
	
	m_btnCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"));
	bSizer2->Add( m_btnCancel, 0, wxLEFT, 5 );
	
	m_btnApply = new wxButton( this, wxID_APPLY, wxT("Apply"));
	bSizer2->Add( m_btnApply, 0, wxLEFT, 5 );

	fgSizer2->Add(bSizer2, 1, 0, 5);
	
	fgSizer1->Add(fgSizer2, 1, wxALL|wxEXPAND, 5);
	
	SetEscapeId(wxID_CANCEL);
	SetSizer(fgSizer1);

	Layout();

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
	m_settingspage->ApplyChanges();
}

void hlaeDialogSettings::OnOK(wxCommandEvent& WXUNUSED(evt))
{
	m_settingspage->ApplyChanges();
	Close();
}

void hlaeDialogSettings::UpdateTreeCtrl()
{
	m_pageidlist->Clear();
	m_treectrl->DeleteAllItems();
	m_treectrl->AddRoot(wxT("Settings"));

	for (hlaeListSettingsPage::iterator iter = m_pagelist->begin();
		iter != m_pagelist->end(); iter++)
	{
		hlaeListElementSettingsPage* current = *iter;
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

		node->GetWindow()->Hide();

		pageid_element->window = node->GetWindow();
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
	for (hlaeListSettingsPageID::iterator iter = m_pageidlist->begin();
		iter != m_pageidlist->end(); iter++)
	{
		hlaeListElementSettingsPageID* current = *iter;
	
		if (evt.GetItem() == current->id)
		{
			m_settingspage = dynamic_cast<hlaeSettingsPageTemplate*>(
				m_pagesizer->GetItem(1)->GetWindow());
			m_settingspage->Hide();
			m_pagesizer->Replace(m_settingspage, current->window);
			m_settingspage = current->window;
			m_settingspage->ShowPage(m_advancedmode);
			m_pagesizer->Layout();

			break;
		}
	}
}