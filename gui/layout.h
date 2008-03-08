#ifndef _HLAE_LAYOUT_H_
#define _HLAE_LAYOUT_H_

#include <wx/aui/aui.h>

class hlaeAuiManager;

#include "list.h"
#include "main.h"

class hlaeListElementLayout : public wxObject
{
public:
	bool is_predefined;
	long id;
	wxString describtion;
	wxString layout;
};

class hlaeAuiManager : public wxAuiManager
{
	private:
		hlaeList* m_layoutlist;
		hlaeMainWindow* m_parent;
		void UpdateLayoutMenu();
		void ClearMenu(wxMenu* menu);
	public:
		hlaeAuiManager(hlaeMainWindow* parent);
		~hlaeAuiManager();
		void AddLayout(const wxString& describtion = wxEmptyString,
			bool is_predefined = false);
		void RemoveLayout(hlaeListElementLayout* layoutelement);
		void ShowManager();
		bool AddPane(wxWindow* window, const wxAuiPaneInfo& pane_info);
		void OnLayout(wxCommandEvent& evt);
};

extern hlaeAuiManager g_layout;

#endif