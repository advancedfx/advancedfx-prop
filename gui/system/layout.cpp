#include <wx/wx.h>

#include <system/debug.h>

#include "layout.h"


hlaeLayoutManager g_layoutmanager;

hlaeLayoutManager::hlaeLayoutManager() : wxObject()
{
	m_auimanager = new wxAuiManager();
	m_layoutlist = new wxList();
	m_layoutlist->DeleteContents(true);
}

void hlaeLayoutManager::UnInit()
{
	m_auimanager->UnInit();
}

hlaeLayoutManager::~hlaeLayoutManager()
{
	m_layoutlist->Clear();
	delete m_layoutlist;

	delete m_auimanager;
}

void hlaeLayoutManager::SetMainWindow(hlaeMainWindow* main_window)
{
	m_parent = main_window;
	m_auimanager->SetManagedWindow(main_window);
}

void hlaeLayoutManager::AddLayout(const wxString& describtion, bool is_predefined) {

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

			for (wxList::iterator iter = m_layoutlist->begin();
				iter != m_layoutlist->end(); iter++)
			{
				wxObject* object = *iter;
				hlaeListElementLayout* current = dynamic_cast<hlaeListElementLayout*>(object);


				if (!current->is_predefined) {
					count++;
				}
			}

			input = _T("Layout ") + wxString::Format(_T("%i"),count+1);

		}

		/* hlaeDialogTextInput* h_textinput = new hlaeDialogTextInput(
			m_parent, _T("Please define the name of the layout!"),
			input);
		h_textinput->ShowModal();*/

		layout_describtion = _T("Layout"); // h_textinput->GetValue();
		dialog_canceled = false; // h_textinput->IsCancled();

		// delete h_textinput;
	}

	// add the layout to the list

	if (!dialog_canceled) {

		hlaeListElementLayout* layoutelement = new hlaeListElementLayout();
		
		layoutelement->is_predefined = is_predefined;
		layoutelement->id = wxNewId();
		layoutelement->describtion = layout_describtion;
		layoutelement->layout = m_auimanager->SavePerspective();

		m_layoutlist->Append(layoutelement);

	}

	UpdateLayoutMenu();
}

void hlaeLayoutManager::RemoveLayout(hlaeListElementLayout* layoutelement) {

	m_layoutlist->DeleteObject(layoutelement);

}

void hlaeLayoutManager::ShowManager()
{

}

bool hlaeLayoutManager::AddPane(wxWindow* window, const wxAuiPaneInfo& pane_info) {

	bool result = m_auimanager->AddPane(window, pane_info);
	m_auimanager->Update();
	window->Layout();
	return result;

}

void hlaeLayoutManager::UpdateLayoutMenu() {

	wxMenu* layoutmenu = m_parent->GetLayoutMenu();

	// clear the menu
	ClearMenu(layoutmenu);

	// write all predefined layouts in the menu
	int count1 = 0;
	int count2 = 0;

	for (wxList::iterator iter = m_layoutlist->begin();
			iter != m_layoutlist->end(); ++iter)
	{
		wxObject* object = *iter;
		hlaeListElementLayout* current = dynamic_cast<hlaeListElementLayout*>(object);

		// m_parent->Connect(current->id, wxEVT_COMMAND_MENU_SELECTED,
		//	wxCommandEventHandler(hlaeLayoutManager::OnLayout), NULL, this);

		if (current->is_predefined) {
			count1++;
			layoutmenu->Append(current->id, current->describtion);
		}
	}

	// write the layouts defined by the user in the menu
	for (wxList::iterator iter = m_layoutlist->begin();
			iter != m_layoutlist->end(); ++iter)
	{
		wxObject* object = *iter;
		hlaeListElementLayout* current = dynamic_cast<hlaeListElementLayout*>(object);

		if (!current->is_predefined) {
			count2++;
			layoutmenu->Append(current->id, current->describtion);
		}
	}

	if (count1 > 0 && count2 > 0) layoutmenu->InsertSeparator(count1);
}

void hlaeLayoutManager::ClearMenu(wxMenu* menu) {
	while (menu->GetMenuItemCount() != 0) {
		menu->Delete(menu->FindItemByPosition(0));
	}
}

void hlaeLayoutManager::OnLayout(wxCommandEvent& evt) {

	for (wxList::iterator iter = m_layoutlist->begin();
			iter != m_layoutlist->end(); iter++)
	{
		wxObject* object = *iter;
		hlaeListElementLayout* current = dynamic_cast<hlaeListElementLayout*>(object);

		if (current->id == evt.GetId()) {
			m_auimanager->LoadPerspective(current->layout);
		}
	}
}


