#ifndef _HLAE_CONSOLE_H_
#define _HLAE_CONSOLE_H_

#include <wx/datetime.h>
#include <wx/string.h>
#include <wx/textctrl.h>


class hlaeConsoleWindow : public wxWindow
{

public:

	hlaeConsoleWindow(wxWindow* parent);

	void WriteMessage(const wxString& message, const wxDateTime& date = wxDefaultDateTime,
		int debuglevel = -1);

private:

	wxTextCtrl* m_textcrtl;
	wxTextAttr m_textdefaultstyle;

	void SetDefaultStyle();

};

#endif