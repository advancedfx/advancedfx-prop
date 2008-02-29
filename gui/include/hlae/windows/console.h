#ifndef _HLAE_CONSOLEWINDOW_H_
#define _HLAE_CONSOLEWINDOW_H_

#include <wx/richtext/richtextctrl.h>
#include <wx/datetime.h>
#include <wx/string.h>

class hlaeConsoleWindow : public wxWindow
{

public:

	hlaeConsoleWindow(wxWindow* parent);
	~hlaeConsoleWindow();
	void WriteMessage(const wxString& message, const wxDateTime& date = wxDefaultDateTime,
		int debuglevel = -1);

private:

	wxRichTextCtrl* m_richtextcrtl;
	
};

#endif // _HLAE_CONSOLEWINDOW_H_