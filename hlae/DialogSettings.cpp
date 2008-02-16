#include "DialogSettings.h"

BEGIN_EVENT_TABLE(hlaeDialogSettings, wxDialog)
	EVT_CHECKBOX(hlaeDialogSettings::hlaeID_AdvancedMode, hlaeDialogSettings::OnAdvancedMode)
	EVT_BUTTON(wxID_APPLY, hlaeDialogSettings::OnApply)
	EVT_BUTTON(wxID_OK, hlaeDialogSettings::OnOK)
END_EVENT_TABLE()

hlaeDialogSettings::hlaeDialogSettings(wxWindow* parent, wxWindowID id, const wxString& title,
	const wxPoint& pos, const wxSize& size, long style )
	: wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 4, 1, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableRow( 0 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer(1, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->AddGrowableRow( 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );


	m_treectrl = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT | wxTR_LINES_AT_ROOT);
	m_treectrl->SetMinSize( wxSize( 125,-1 ) );
	fgSizer2->Add(m_treectrl, 0, wxALL | wxEXPAND);

	wxTreeCtrl* m_treectrl2 = new wxTreeCtrl(this, wxID_ANY);
	fgSizer2->Add(m_treectrl2, 0, wxLEFT | wxEXPAND, 5 );

	fgSizer1->Add( fgSizer2, 0, wxALL | wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	fgSizer1->Add( m_staticline1, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 0 );
	fgSizer3->AddGrowableRow( 0 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_chkAdvanced = new wxCheckBox( this, hlaeID_AdvancedMode, wxT("Advanced Settings"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fgSizer3->Add( m_chkAdvanced, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND, 5 );
	
	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );
	
	m_btnOK = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_btnOK, 0, wxLEFT, 5 );
	
	m_btnCancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_btnCancel, 0, wxLEFT, 5 );
	
	m_btnApply = new wxButton( this, wxID_APPLY, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_btnApply, 0, wxLEFT, 5 );

	fgSizer3->Add( bSizer2, 1, 0, 5 );
	
	fgSizer1->Add( fgSizer3, 1, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();	

	m_pagelist = new hlaeSettingsPageList;
	m_pagelist->DeleteContents(true);

	hlaeSettingsPageElement* node_general =	new hlaeSettingsPageElement(0,
		wxT("General"),false);
	node_general->Append(new hlaeSettingsPageElement(0,wxT("Menu 1"),false));
	node_general->Append(new hlaeSettingsPageElement(0,wxT("Menu 2"),false));

	hlaeSettingsPageElement* node_advanced = new hlaeSettingsPageElement(0,
		wxT("Advanced"),true);
	node_advanced->Append(new hlaeSettingsPageElement(0,wxT("Advanced"), true));

	m_pagelist->Append(node_general);
	m_pagelist->Append(node_advanced);

	UpdateTreeCtrl();

	SetEscapeId(wxID_CANCEL);
}

hlaeDialogSettings::~hlaeDialogSettings()
{
	m_pagelist->Clear();
	delete m_pagelist;
}

void hlaeDialogSettings::UpdateTreeCtrl()
{
	m_treectrl->DeleteAllItems();
	m_treectrl->AddRoot(wxT("Settings"));

	for (hlaeSettingsPageList::iterator iter = m_pagelist->begin();
		iter != m_pagelist->end(); iter++)
	{
		hlaeSettingsPageElement* current = *iter;
		UpdateTreeCtrlNodes(current, m_treectrl->GetRootItem());
	}
}

void hlaeDialogSettings::UpdateTreeCtrlNodes(hlaeSettingsPageElement* node, wxTreeItemId id)
{
	if (!node->IsAdvanced() || m_advancedmode)
	{
		wxTreeItemId sub_id = m_treectrl->AppendItem(id, node->GetName());

		for (unsigned int i = 0; i < node->GetCount(); i++)
			UpdateTreeCtrlNodes(node->GetElement(i), sub_id);
	}
}

void hlaeDialogSettings::OnAdvancedMode(wxCommandEvent& WXUNUSED(evt))
{
	m_advancedmode = m_chkAdvanced->IsChecked();
	UpdateTreeCtrl();
}

void hlaeDialogSettings::OnApply(wxCommandEvent& WXUNUSED(evt))
{

}

void hlaeDialogSettings::OnOK(wxCommandEvent& WXUNUSED(evt))
{

}