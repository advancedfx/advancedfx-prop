#include <wx/textctrl.h>

#include <hlae/settingspages/general.h>

hlaeSettingsPageGeneral::hlaeSettingsPageGeneral(wxWindow* parent)
	: hlaeSettingsPageTemplate(parent)
{}

void hlaeSettingsPageGeneral::ShowPage(bool is_advanced)
{
	new wxTextCtrl (this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(400,300));
	Show();
}

void hlaeSettingsPageGeneral::ApplyChanges()
{

}