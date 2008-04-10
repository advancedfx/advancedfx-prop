#ifndef HLAE_LAYOUT_H
#define HLAE_LAYOUT_H

#include <wx/wx.h>
#include <wx/aui/aui.h>

#include <windows/hlae.h>


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
		wxList* m_layoutlist;
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

		wxAuiManager* GetAuiManager() { return m_auimanager; };
};

extern hlaeLayoutManager g_layoutmanager;

#endif