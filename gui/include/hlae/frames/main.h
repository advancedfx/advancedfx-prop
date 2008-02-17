#ifndef _HLAE_FRAMEMAIN_H_
	#define _HLAE_FRAMEMAIN_H_

	#include <wx/menu.h>
	#include <wx/frame.h>

	class hlaeFrameMain;
	
	#include <hlae/auimanager.h>

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

#endif // _HLAE_FRAMEMAIN_H_