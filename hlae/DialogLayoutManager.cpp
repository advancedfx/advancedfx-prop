/* includes */
#include "DialogLayoutManager.h"

#include <wx/arrstr.h>
#include <wx/textdlg.h>

/* implementation */
BEGIN_EVENT_TABLE(hlaeDialogLayoutManager, wxDialog)
    EVT_BUTTON(hlaeDialogLayoutManager::hlaeID_Delete, hlaeDialogLayoutManager::OnDelete)
	EVT_BUTTON(hlaeDialogLayoutManager::hlaeID_Clone, hlaeDialogLayoutManager::OnClone)
END_EVENT_TABLE()

hlaeDialogLayoutManager::hlaeDialogLayoutManager(wxWindow* parent, wxWindowID id,
		hlaeLayoutList* layoutlist)
		: wxDialog(parent, id, wxT("LayoutManager"), wxDefaultPosition)
{
	m_layoutlist = layoutlist;

	m_listbox = new wxListBox(this, wxID_ANY, wxPoint(10,10));
	new wxButton(this, wxID_OK, wxT("OK"), wxPoint(200,10));
	new wxButton(this, hlaeID_Delete, wxT("Delete"), wxPoint(200,35));

	wxArrayString describtion;

	for (hlaeLayoutList::iterator iter = m_layoutlist->begin();
			iter != m_layoutlist->end(); ++iter)
	{
		hlaeLayoutElement *current = *iter;
		describtion.Add(current->describtion);
	}

	m_listbox->InsertItems(describtion, 0);
}

void hlaeDialogLayoutManager::OnDelete(wxCommandEvent& WXUNUSED(event)) {

	int sel = m_listbox->GetSelection();
	if (sel!=wxNOT_FOUND) {
		m_listbox->Delete(sel);
		m_layoutlist->Erase(m_layoutlist->Item(sel));
	}

}

void hlaeDialogLayoutManager::OnClone(wxCommandEvent& WXUNUSED(event)) {

}