#ifndef _HLAE_DIALOGSETTINGS_H_

	#define _HLAE_DIALOGSETTINGS_H_

	#include <wx/sizer.h>
	#include <wx/button.h>
	#include <wx/checkbox.h>
	#include <wx/treectrl.h>
	#include <wx/dialog.h>

	#include <hlae/settingspages/template.h>
	#include <hlae/lists/settingspage.h>
	#include <hlae/lists/settingspageid.h>

	class hlaeDialogSettings : public wxDialog
	{
		private:

			DECLARE_EVENT_TABLE()

			enum {
				hlaeID_AdvancedMode = wxID_HIGHEST+1,
				hlaeID_SelectionChanged
			};

			void OnAdvancedMode(wxCommandEvent& evt);
			void OnApply(wxCommandEvent& evt);
			void OnOK(wxCommandEvent& evt);
			void OnSelectionChanged(wxTreeEvent& evt);

			void CreateForm();
			void UpdateTreeCtrl();
			void UpdateTreeCtrlNodes(hlaeListElementSettingsPage* node, wxTreeItemId id);

			hlaeListSettingsPage* m_pagelist;
			hlaeListSettingsPageID* m_pageidlist;
			hlaeSettingsPageTemplate* m_lastpage;
			bool m_advancedmode;

			wxTreeCtrl* m_treectrl;
			wxFlexGridSizer* m_pagesizer;
			wxCheckBox* m_chkAdvanced;
			wxButton* m_btnOK;
			wxButton* m_btnCancel;
			wxButton* m_btnApply;
		
		public:

			hlaeDialogSettings( wxWindow* parent, wxWindowID id = wxID_ANY,
				const wxString& title = wxT("Settings"),
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxSize( 564,427 ));
			~hlaeDialogSettings();
			void Append(hlaeListElementSettingsPage* page_element);
		
	};

#endif // _HLAE_DIALOGSETTINGS_H_