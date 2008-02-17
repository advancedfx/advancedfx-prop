#include <wx/button.h>

#include <hlae/settingspages/general.h>

hlaeSettingsPageGeneral::hlaeSettingsPageGeneral(wxWindow* parent)
	: hlaeSettingsPageTemplate(parent)
{}

void hlaeSettingsPageGeneral::ShowPage(bool is_advanced)
{
	new wxButton(this, wxID_ANY, wxT("yes :D"));
	Show();
}

void hlaeSettingsPageGeneral::ApplyChanges()
{

}