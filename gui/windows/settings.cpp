#include <wx/wx.h>
#include <wx/hyperlink.h>

#include <system/config.h>
#include <defines.h>

#include "settings.h"


hlaeSettingsPageBase::hlaeSettingsPageBase(wxWindow* parent) : wxWindow(parent, wxID_ANY)
{
	Hide();
}


hlaeSettingsPageGeneral::hlaeSettingsPageGeneral(wxWindow* parent) : hlaeSettingsPageBase(parent)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bs_main;
	bs_main = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bs_link;
	bs_link = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* st_link;
	st_link = new wxStaticText( this, wxID_ANY, wxT("You can retrive the latest HLAE, infos, reports bugs and post suggestions at"), wxDefaultPosition, wxDefaultSize, 0 );
	st_link->Wrap( -1 );
	bs_link->Add( st_link, 0, 0, 5 );
	
	wxHyperlinkCtrl* hl_link;
	hl_link = new wxHyperlinkCtrl( this, wxID_ANY, wxT("http://www.madabouthats.org/code-mdt/viewforum.php?f=3"), wxT("http://www.madabouthats.org/code-mdt/viewforum.php?f=3"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	
	hl_link->SetHoverColour( wxColour( 0, 88, 255 ) );
	hl_link->SetNormalColour( wxColour( 0, 0, 255 ) );
	hl_link->SetVisitedColour( wxColour( 0, 0, 255 ) );
	bs_link->Add( hl_link, 0, 0, 5 );
	
	bs_main->Add( bs_link, 0, wxALL|wxEXPAND, 5 );
	
	wxScrolledWindow* sw_info;
	sw_info = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN|wxVSCROLL );
	sw_info->SetScrollRate( 5, 5 );
	wxBoxSizer* bs_info;
	bs_info = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* st_info;
	st_info = new wxStaticText( sw_info, wxID_ANY, wxT("Epilepsy warning:\n\n  This software will cause fast changing colors and images on your screen.\n\n\nUsing the HLAE:\n\n  Extract all contents of this archive into a folder you like.\n  You should not extract into the game's folder, this is not neccessary.\n\n  We suggest using Steam's Offline Mode.\n  Please stick to the manuals and tutorials on the web for more information.\n\n\nDisclaimer:\n\n  Altered versions must be cleary marked as such and must not be misrepresented\n  as being the original software.\n  \n  This software is for your enjoyment and you are using it at your own risk.\n\n\nSincerly,\nyour Half-Life Advanced Effects team.\n\n\n\n\nCredits:\n\nIn addition to the mentions in the changelog we want to credit the following\npeople, companys or organistations:\n\nMicrosoft\n  for VC++, MSDN2, Windows, ...\n  http://www.Microsoft.com\n  \nValve\n  for HLSDK\n  http://www.ValveSoftware.com\n  \nMetaMod\n  for HLSDK p3\n  http://www.MetaMod.org\n\nid Software\n  for Quake 1 source code\n  http://www.idsoftware.com\n  \nwxWidgets Project\n  for wxWidgets\n  http://wxwidgets.org\n  \nAnd everyone that feels he/she/it should have been mentioned here.\n\n\nGreetings:\n\nYou :)\n\n\nThe HLAE Team:\n\nMIRVIN_monkey\n  the creator and programmer\n  http://www.madabouthats.org\n  \nripieces\n  programer\n  http://dominik.matrixstorm.com\n  \nneomic\n  programer\n  http://neomic.xail.net/\n\nmsthavoc\n  documentation, testing, community relations\n  http://www.mst-gaming.de"), wxDefaultPosition, wxDefaultSize, 0 );
	st_info->Wrap( -1 );
	bs_info->Add( st_info, 0, 0, 5 );
	
	sw_info->SetSizer( bs_info );
	sw_info->Layout();
	bs_info->Fit( sw_info );
	bs_main->Add( sw_info, 1, wxALL|wxEXPAND, 5 );
	
	wxButton* bt_ok;
	bt_ok = new wxButton( this, wxID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bs_main->Add( bt_ok, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bs_main );
	this->Layout();
}

void hlaeSettingsPageGeneral::ApplyChanges()
{
	// Save settings
}


hlaeSettingsPageAdvanced::hlaeSettingsPageAdvanced(wxWindow* parent) : hlaeSettingsPageBase(parent)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bs_main;
	bs_main = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bs_link;
	bs_link = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* st_link;
	st_link = new wxStaticText( this, wxID_ANY, wxT("You can retrive the latest HLAE, infos, reports bugs and post suggestions at"), wxDefaultPosition, wxDefaultSize, 0 );
	st_link->Wrap( -1 );
	bs_link->Add( st_link, 0, 0, 5 );
	
	wxHyperlinkCtrl* hl_link;
	hl_link = new wxHyperlinkCtrl( this, wxID_ANY, wxT("http://www.madabouthats.org/code-mdt/viewforum.php?f=3"), wxT("http://www.madabouthats.org/code-mdt/viewforum.php?f=3"), wxDefaultPosition, wxDefaultSize, wxHL_DEFAULT_STYLE );
	
	hl_link->SetHoverColour( wxColour( 0, 88, 255 ) );
	hl_link->SetNormalColour( wxColour( 0, 0, 255 ) );
	hl_link->SetVisitedColour( wxColour( 0, 0, 255 ) );
	bs_link->Add( hl_link, 0, 0, 5 );
	
	bs_main->Add( bs_link, 0, wxALL|wxEXPAND, 5 );
	
	wxScrolledWindow* sw_info;
	sw_info = new wxScrolledWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN|wxVSCROLL );
	sw_info->SetScrollRate( 5, 5 );
	wxBoxSizer* bs_info;
	bs_info = new wxBoxSizer( wxVERTICAL );
	
	wxStaticText* st_info;
	st_info = new wxStaticText( sw_info, wxID_ANY, wxT("Epilepsy warning:\n\n  This software will cause fast changing colors and images on your screen.\n\n\nUsing the HLAE:\n\n  Extract all contents of this archive into a folder you like.\n  You should not extract into the game's folder, this is not neccessary.\n\n  We suggest using Steam's Offline Mode.\n  Please stick to the manuals and tutorials on the web for more information.\n\n\nDisclaimer:\n\n  Altered versions must be cleary marked as such and must not be misrepresented\n  as being the original software.\n  \n  This software is for your enjoyment and you are using it at your own risk.\n\n\nSincerly,\nyour Half-Life Advanced Effects team.\n\n\n\n\nCredits:\n\nIn addition to the mentions in the changelog we want to credit the following\npeople, companys or organistations:\n\nMicrosoft\n  for VC++, MSDN2, Windows, ...\n  http://www.Microsoft.com\n  \nValve\n  for HLSDK\n  http://www.ValveSoftware.com\n  \nMetaMod\n  for HLSDK p3\n  http://www.MetaMod.org\n\nid Software\n  for Quake 1 source code\n  http://www.idsoftware.com\n  \nwxWidgets Project\n  for wxWidgets\n  http://wxwidgets.org\n  \nAnd everyone that feels he/she/it should have been mentioned here.\n\n\nGreetings:\n\nYou :)\n\n\nThe HLAE Team:\n\nMIRVIN_monkey\n  the creator and programmer\n  http://www.madabouthats.org\n  \nripieces\n  programer\n  http://dominik.matrixstorm.com\n  \nneomic\n  programer\n  http://neomic.xail.net/\n\nmsthavoc\n  documentation, testing, community relations\n  http://www.mst-gaming.de"), wxDefaultPosition, wxDefaultSize, 0 );
	st_info->Wrap( -1 );
	bs_info->Add( st_info, 0, 0, 5 );
	
	sw_info->SetSizer( bs_info );
	sw_info->Layout();
	bs_info->Fit( sw_info );
	bs_main->Add( sw_info, 1, wxALL|wxEXPAND, 5 );
	
	wxButton* bt_ok;
	bt_ok = new wxButton( this, wxID_OK, wxT("OK 2 :)"), wxDefaultPosition, wxDefaultSize, 0 );
	bs_main->Add( bt_ok, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );
	
	this->SetSizer( bs_main );
	this->Layout();
}

void hlaeSettingsPageAdvanced::ApplyChanges()
{
	// Save settings
}


hlaeSettingsBaseElement::hlaeSettingsBaseElement(hlaeSettingsPageBase* page,
	hlaeSettingsPageID_e page_id)
{
	m_page = page;
	m_pageid = page_id;
}

hlaeSettingsPageID_e hlaeSettingsBaseElement::GetPageID()
{
	return m_pageid;
}

hlaeSettingsPageBase* hlaeSettingsBaseElement::GetPage()
{
	return m_page;
}


hlaeSettingsListElement::hlaeSettingsListElement(hlaeSettingsPageBase* page,
	hlaeSettingsPageID_e page_id, wxTreeItemId tree_id) : hlaeSettingsBaseElement(page, page_id)
{
	m_treeid = tree_id;
}

wxTreeItemId hlaeSettingsListElement::GetTreeID()
{
	return m_treeid;
}


hlaeSettingsTreeElement::hlaeSettingsTreeElement(hlaeSettingsPageBase* page, const wxString& name,
		hlaeSettingsPageID_e page_id, bool is_advanced) : hlaeSettingsBaseElement(page, page_id)
{
	m_name = name;
	m_isadvanced = is_advanced;
}

size_t hlaeSettingsTreeElement::GetSubpagesCount()
{
	return m_subpagetree.size();
}

void hlaeSettingsTreeElement::AppendSubpage(const hlaeSettingsTreeElement& subpage_element)
{
	m_subpagetree.push_back(subpage_element);
}

const hlaeSettingsTreeElement& hlaeSettingsTreeElement::GetSubpage(size_t index)
{
	return m_subpagetree.at(index);
}

bool hlaeSettingsTreeElement::IsAdvanced()
{
	return m_isadvanced;
}

wxString hlaeSettingsTreeElement::GetName()
{
	return m_name;
}


hlaeSettingsDialog::hlaeSettingsDialog(wxWindow* parent, hlaeSettingsPageID_e page_id)
	: wxDialog(parent, wxID_ANY, wxString::Format(_T("%s%sSettings"),_T(HLAE_NAME),
	_T(HLAE_TITLESEPERATOR)), wxDefaultPosition, wxSize(640,480))
{
	// Prepare

	hlaeSettingsTreeElement node_general = hlaeSettingsTreeElement(
		new hlaeSettingsPageGeneral(this), _T("General"), ID_General, false);
	node_general.AppendSubpage(hlaeSettingsTreeElement(
		new hlaeSettingsPageGeneral(this), _T("Menu 1"), ID_General, false));
	node_general.AppendSubpage(hlaeSettingsTreeElement(
		new hlaeSettingsPageGeneral(this), _T("Menu 2"), ID_General, false));
	m_pagetree.push_back(node_general);

	hlaeSettingsTreeElement node_advanced =	hlaeSettingsTreeElement(
		new hlaeSettingsPageAdvanced(this), _T("Advanced"), ID_Advanced, true);
	node_advanced.AppendSubpage(hlaeSettingsTreeElement(
		new hlaeSettingsPageAdvanced(this), _T("Advanced"), ID_Advanced, true));
	m_pagetree.push_back(node_advanced);


	// Create the window

	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bs_main;
	bs_main = new wxBoxSizer( wxVERTICAL );
	
	m_bs_treebook = new wxBoxSizer( wxHORIZONTAL );
	
	m_treectrl = new wxTreeCtrl( this, ID_SelectionChanged, wxDefaultPosition, wxSize( 150,-1 ), wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT|wxTR_SINGLE );
	m_bs_treebook->Add( m_treectrl, 0, wxEXPAND, 5 );
	
	bs_main->Add( m_bs_treebook, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticLine* sl_main;
	sl_main = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bs_main->Add( sl_main, 0, wxEXPAND, 5 );
	
	wxFlexGridSizer* fg_input;
	fg_input = new wxFlexGridSizer( 2, 2, 0, 0 );
	fg_input->AddGrowableCol( 0 );
	fg_input->AddGrowableRow( 1 );
	fg_input->SetFlexibleDirection( wxBOTH );
	fg_input->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_ch_advancedview = new wxCheckBox( this, ID_AdvancedMode, wxT("Advanced View"), wxDefaultPosition, wxDefaultSize, 0 );
	
	fg_input->Add( m_ch_advancedview, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	wxBoxSizer* bs_buttons;
	bs_buttons = new wxBoxSizer( wxHORIZONTAL );
	
	wxButton* bt_ok;
	bt_ok = new wxButton( this, ID_OK, wxT("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bt_ok->SetDefault(); 
	bs_buttons->Add( bt_ok, 0, 0, 5 );
	
	wxButton* bt_cancel;
	bt_cancel = new wxButton( this, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bs_buttons->Add( bt_cancel, 0, wxLEFT, 5 );
	
	wxButton* bt_apply;
	bt_apply = new wxButton( this, ID_Apply, wxT("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
	bs_buttons->Add( bt_apply, 0, wxLEFT, 5 );
	
	fg_input->Add( bs_buttons, 1, 0, 5 );
	
	bs_main->Add( fg_input, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bs_main );
	this->Layout();


	// Create the events

	Connect(ID_AdvancedMode, wxEVT_COMMAND_CHECKBOX_CLICKED,
		wxCommandEventHandler(hlaeSettingsDialog::OnAdvancedMode));
	Connect(ID_OK, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(hlaeSettingsDialog::OnOK));
	Connect(ID_Apply, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(hlaeSettingsDialog::OnApply));
	Connect(ID_SelectionChanged, wxEVT_COMMAND_TREE_SEL_CHANGED,
		wxTreeEventHandler(hlaeSettingsDialog::OnSelectionChanged));

	SetEscapeId(wxID_CANCEL);


	// Set controls

	m_advancedmode = g_config.GetPropertyBoolean(_T("general"), _T("advanced_view"));
	m_ch_advancedview->SetValue(m_advancedmode);


	// Update

	UpdateTreeCtrl();


	// Set the startup page

	m_lastpage = m_pagelist.at(0).GetPage();

	m_bs_treebook->Add(m_lastpage, 1, wxLEFT , 5);

	m_lastpage->Show();

	m_bs_treebook->Layout();
	m_lastpage->Layout();
}

void hlaeSettingsDialog::UpdateTreeCtrl()
{
	m_pagelist.erase(m_pagelist.begin(), m_pagelist.end());

	m_treectrl->DeleteAllItems();
	m_treectrl->AddRoot(_T("Settings"));

	for (vector<hlaeSettingsTreeElement>::iterator iter = m_pagetree.begin();
		iter != m_pagetree.end(); iter++)
	{
		UpdateTreeCtrlNodes(*iter, m_treectrl->GetRootItem());
	}
}

void hlaeSettingsDialog::UpdateTreeCtrlNodes(hlaeSettingsTreeElement element, wxTreeItemId id)
{
	if (!element.IsAdvanced() || m_advancedmode)
	{
		wxTreeItemId sub_id = m_treectrl->AppendItem(id, element.GetName());

		m_pagelist.push_back(hlaeSettingsListElement(element.GetPage(), element.GetPageID(), sub_id));

		for (size_t i = 0; i < element.GetSubpagesCount(); i++)
		{
			UpdateTreeCtrlNodes(element.GetSubpage(i), sub_id);
		}
	}
}

void hlaeSettingsDialog::OnSelectionChanged(wxTreeEvent& evt)
{
	for (vector<hlaeSettingsListElement>::iterator iter = m_pagelist.begin();
		iter != m_pagelist.end(); iter++)
	{
		hlaeSettingsListElement current = *iter;
	
		if (evt.GetItem() == current.GetTreeID())
		{
			hlaeSettingsPageBase* current_page = current.GetPage();

			m_bs_treebook->Replace(m_lastpage, current_page);

			m_lastpage->Hide();
			current_page->Show();

			m_bs_treebook->Layout();
			current_page->Layout();

			m_lastpage = current_page;

			break;
		}
	}
}

void hlaeSettingsDialog::OnAdvancedMode(wxCommandEvent& evt)
{
	m_advancedmode = m_ch_advancedview->IsChecked();
	UpdateTreeCtrl();
}
void hlaeSettingsDialog::OnOK(wxCommandEvent& evt)
{
	ApplyChanges();
	Close(true);
}

void hlaeSettingsDialog::OnApply(wxCommandEvent& evt)
{
	ApplyChanges();
}

void hlaeSettingsDialog::ApplyChanges()
{
	m_lastpage->ApplyChanges();
	g_config.SetPropertyBoolean(_T("general"), _T("advanced_view"), m_ch_advancedview->GetValue());
	g_config.Flush();
}