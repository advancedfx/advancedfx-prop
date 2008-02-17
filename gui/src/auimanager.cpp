#include <hlae/auimanager.h>
#include <hlae/dialogs/textinput.h>

hlaeAuiManager::hlaeAuiManager(hlaeFrameMain* parent)
		: wxAuiManager(parent)
{
	m_parent = parent;
	m_layoutlist = new hlaeListLayout();
	m_layoutlist->DeleteContents(true);
}

hlaeAuiManager::~hlaeAuiManager() {

	UnInit();

	m_layoutlist->Clear();
	delete m_layoutlist;
}

void hlaeAuiManager::AddLayout(const wxString& describtion, bool is_predefined) {

	wxString layout_describtion;
	bool dialog_canceled = false;

	if (is_predefined) {
		layout_describtion = describtion;
	}
	else {

		wxString input;

		if (describtion == wxEmptyString) {

			// append a number based on the ones of layouts
			int count = 0;

			for (hlaeListLayout::iterator iter = m_layoutlist->begin();
				iter != m_layoutlist->end(); iter++)
			{
				hlaeListElementLayout *current = *iter;

				if (!current->is_predefined) {
					count++;
				}
			}

			input = wxT("Layout ") + wxString::Format(wxT("%i"),count+1);

		}

		hlaeDialogTextInput* h_textinput = new hlaeDialogTextInput(
			m_parent, wxT("Please define the name of the layout!"),
			input);
		h_textinput->ShowModal();

		layout_describtion = h_textinput->GetValue();
		dialog_canceled = h_textinput->IsCancled();

		delete h_textinput;
	}

	// add the layout to the list

	if (!dialog_canceled) {

		hlaeListElementLayout* layoutelement = new hlaeListElementLayout();
		
		layoutelement->is_predefined = is_predefined;
		layoutelement->id = wxNewId();
		layoutelement->describtion = layout_describtion;
		layoutelement->layout = SavePerspective();

		m_layoutlist->Append(layoutelement);

	}

	UpdateLayoutMenu();
}

void hlaeAuiManager::RemoveLayout(hlaeListElementLayout* layoutelement) {

	m_layoutlist->DeleteObject(layoutelement);

}

void hlaeAuiManager::ShowManager()
{

}

bool hlaeAuiManager::AddPane(wxWindow* window, const wxAuiPaneInfo& pane_info) {

	bool result = wxAuiManager::AddPane(window, pane_info);
	Update();
	return result;

}

void hlaeAuiManager::UpdateLayoutMenu() {

	wxMenu* layoutmenu = m_parent->GetLayoutMenu();

	// clear the menu
	ClearMenu(layoutmenu);

	// write all predefined layouts in the menu
	int count1 = 0;
	int count2 = 0;

	for (hlaeListLayout::iterator iter = m_layoutlist->begin();
			iter != m_layoutlist->end(); ++iter)
	{
		hlaeListElementLayout *current = *iter;

		m_parent->Connect(current->id, wxEVT_COMMAND_MENU_SELECTED,
			wxCommandEventHandler(hlaeAuiManager::OnLayout),NULL,this);

		if (current->is_predefined) {
			count1++;
			layoutmenu->Append(current->id, current->describtion);
		}
	}

	// write the layouts defined by the user in the menu
	for (hlaeListLayout::iterator iter = m_layoutlist->begin();
			iter != m_layoutlist->end(); ++iter)
	{
		hlaeListElementLayout *current = *iter;

		if (!current->is_predefined) {
			count2++;
			layoutmenu->Append(current->id, current->describtion);
		}
	}

	if (count1 > 0 && count2 > 0) layoutmenu->InsertSeparator(count1);
}

void hlaeAuiManager::ClearMenu(wxMenu* menu) {
	while (menu->GetMenuItemCount() != 0) {
		menu->Delete(menu->FindItemByPosition(0));
	}
}

void hlaeAuiManager::OnLayout(wxCommandEvent& evt) {

	for (hlaeListLayout::iterator iter = m_layoutlist->begin();
			iter != m_layoutlist->end(); iter++)
	{
		hlaeListElementLayout *current = *iter;

		if (current->id == evt.GetId()) {
			LoadPerspective(current->layout);
		}
	}
}


