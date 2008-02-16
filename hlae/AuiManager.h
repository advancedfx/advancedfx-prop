#ifndef HEADER_HLAEAUIMANAGER
	#define HEADER_HLAEAUIMANAGER

	class hlaeAuiManager;

	/* includes */
	#include "DialogLayoutManager.h"
	#include "DialogTextInput.h"
	#include "hlae/framemain.h"
	#include "Lists.h"
	#include "wx/aui/aui.h"
	#include "wx/menu.h"
	#include "wx/window.h"


	/* declaration */
	class hlaeAuiManager : public wxAuiManager {

		private:

			hlaeLayoutList* m_layoutlist;
			hlaeFrameMain* m_parent;

			void UpdateLayoutMenu();
			void ClearMenu(wxMenu* menu);

		public:

			hlaeAuiManager(hlaeFrameMain* parent);
			~hlaeAuiManager();

			void AddLayout(const wxString& describtion = wxEmptyString,
				bool is_predefined = false);
			void RemoveLayout(hlaeLayoutElement* layoutelement);

			void ShowManager();

			bool AddPane(wxWindow* window, const wxAuiPaneInfo& pane_info);

			void OnLayout(wxCommandEvent& evt);

	};

#endif