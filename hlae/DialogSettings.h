#ifndef HEADER_HLAE_DIALOGSETTINGS

	#define HEADER_HLAE_DIALOGSETTINGS

	#include <wx/statline.h>
	#include <wx/checkbox.h>
	#include <wx/button.h>
	#include <wx/sizer.h>
	#include <wx/dialog.h>
	#include <wx/treectrl.h>

	#include "SettingsPageElement.h"

	class hlaeDialogSettings : public wxDialog 
	{
		private:

			DECLARE_EVENT_TABLE()

			enum {
				hlaeID_AdvancedMode = wxID_HIGHEST+1
			};

			void OnAdvancedMode(wxCommandEvent& evt);
			void OnApply(wxCommandEvent& evt);
			void OnOK(wxCommandEvent& evt);

			void UpdateTreeCtrl();
			void UpdateTreeCtrlNodes(hlaeSettingsPageElement* node, wxTreeItemId id);
			hlaeSettingsPageList* m_pagelist;
			wxTreeCtrl* m_treectrl;
			bool m_advancedmode;

			wxStaticLine* m_staticline1;
			wxCheckBox* m_chkAdvanced;
			wxButton* m_btnOK;
			wxButton* m_btnCancel;
			wxButton* m_btnApply;
		
		public:
			hlaeDialogSettings( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Settings"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 564,427 ), long style = wxDEFAULT_DIALOG_STYLE );
			~hlaeDialogSettings();
		
	};

#endif // HEADER_HLAE_DIALOGSETTINGS
