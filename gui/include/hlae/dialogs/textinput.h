#ifndef _HLAE_DIALOGTEXTINPUT_H_

	#define _HLAE_DIALOGTEXTINPUT_H_

	#include <wx/dialog.h>
	#include <wx/textctrl.h>
	#include <wx/button.h>

	class hlaeDialogTextInput : public wxDialog {

		private:

			DECLARE_EVENT_TABLE()

			enum {
				hlaeID_OK = wxID_HIGHEST+1
			};

			bool m_iscanceled;

			wxString m_value;

			wxButton* m_btnok;
			wxTextCtrl* m_textctrl;

			void OnOK(wxCommandEvent& evt);

		public:

			hlaeDialogTextInput(wxWindow* parent, const wxString& label,
				const wxString& input = wxT(""),
				const wxString& title = wxT("Half-Life After Effects"));
			const wxString& GetValue();
			bool IsCancled();

	};

#endif // _HLAE_DIALOGTEXTINPUT_H_