#ifndef _HLAE_LAYOUT_H_
#define _HLAE_LAYOUT_H_

#include "list.h"
#include "main.h"

#include <wx/aui/aui.h>

class hlaeListElementLayout : public wxObject
{
public:
	bool is_predefined;
	long id;
	wxString describtion;
	wxString layout;
};

class hlaeLayoutManager : public wxObject
{
	private:
		wxAuiManager* m_auimanager;
		hlaeList* m_layoutlist;
		hlaeMainWindow* m_parent;
		void UpdateLayoutMenu();
		void ClearMenu(wxMenu* menu);
	public:
		hlaeLayoutManager();
		~hlaeLayoutManager();
		void AddLayout(const wxString& describtion = wxEmptyString,
			bool is_predefined = false);
		void RemoveLayout(hlaeListElementLayout* layoutelement);
		void SetMainWindow(hlaeMainWindow* main_window);
		void UnInit();
		void ShowManager();
		bool AddPane(wxWindow* window, const wxAuiPaneInfo& pane_info);
		void OnLayout(wxCommandEvent& evt);
};

extern hlaeLayoutManager g_layoutmanager;

#endif