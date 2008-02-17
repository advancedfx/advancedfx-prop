#include <wx/statbmp.h>
#include <wx/stattext.h>
#include <wx/artprov.h>

#include <hlae/dialogs/textinput.h>

BEGIN_EVENT_TABLE(hlaeDialogTextInput, wxDialog)
    EVT_BUTTON(hlaeDialogTextInput::hlaeID_OK, hlaeDialogTextInput::OnOK)
END_EVENT_TABLE()

hlaeDialogTextInput::hlaeDialogTextInput(wxWindow* parent,
	const wxString& label, const wxString& input, const wxString& title)
		: wxDialog(parent, wxID_ANY, title)
{
	
	m_iscanceled = true;
	m_value = wxEmptyString;

	new wxStaticBitmap(this, wxID_ANY,
		wxArtProvider::GetBitmap(wxART_QUESTION),
		wxPoint(10,10));
		new wxStaticText(this, wxID_ANY, label, wxPoint(50,10));
	m_textctrl = new wxTextCtrl(this, wxID_ANY, input, wxPoint(50,30),
		wxSize(100,20));
	m_btnok = new wxButton(this, hlaeID_OK, wxT("OK"), wxPoint(10,60));
	new wxButton(this, wxID_CANCEL, wxT("Cancel"),
		wxPoint(90,60));
}

void hlaeDialogTextInput::OnOK(wxCommandEvent& evt) {

	m_iscanceled = false;
	m_value = m_textctrl->GetValue();

	Close(true);

}

const wxString& hlaeDialogTextInput::GetValue() {
	return m_value;
}

bool hlaeDialogTextInput::IsCancled() {
	return m_iscanceled;
}