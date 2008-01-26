#ifndef HEADER_HLAEFRAMEMAIN
	#define HEADER_HLAEFRAMEMAIN

	class hlaeFrameMain;

	/* includes */
	#include <hlae/auimanager.h>
	#include <wx/frame.h>
	#include <wx/menu.h>

	/* declaration */
	class hlaeFrameMain : public wxFrame {

		private:

			DECLARE_EVENT_TABLE()

			enum {
				hlaeID_SaveLayout = wxID_HIGHEST+1,
				hlaeID_LayoutManager
			};

			hlaeAuiManager* m_auimanager;
			wxMenu* m_windowmenu;
			wxMenu*	m_toolbarmenu;
			wxMenu*	m_layoutmenu;

			void CreateMenuBar();
			void OnExit(wxCommandEvent& evt);
			void OnSaveLayout(wxCommandEvent& evt);
			void OnLayoutManager(wxCommandEvent& evt);
			void OnAbout(wxCommandEvent& evt);

		public:

			hlaeFrameMain();
			~hlaeFrameMain();

			wxMenu* hlaeFrameMain::GetWindowMenu() const;
			wxMenu* hlaeFrameMain::GetToolBarMenuMenu() const;
			wxMenu* hlaeFrameMain::GetLayoutMenu() const;

		};

#endif // HEADER_HLAEFRAMEMAIN