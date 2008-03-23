#ifndef HLAE_CONSOLE_H
#define HLAE_CONSOLE_H

#include <wx/wx.h>


class hlaeConsoleWindow : public wxWindow
{

public:

	hlaeConsoleWindow(wxWindow* parent);

	void WriteMessage(const wxString& message, const wxDateTime& date = wxDefaultDateTime,
		int debuglevel = -1);

private:

	wxTextCtrl* m_textcrtl;

};

#endif