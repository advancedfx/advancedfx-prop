#ifndef _HLAE_AUIMANAGER_H_

	#define _HLAE_AUIMANAGER_H_

	#include <wx/aui/aui.h>

	class hlaeAuiManager;

	#include <hlae/core/lists/layout.h>
	#include <hlae/windows/main.h> 

	class hlaeAuiManager : public wxAuiManager {

		private:

			hlaeListLayout* m_layoutlist;
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

#endif // _HLAE_AUIMANAGER_H_