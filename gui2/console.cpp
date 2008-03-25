#include "console.h"
#include "debug.h"

#include <wx/sizer.h>

hlaeConsoleWindow::hlaeConsoleWindow(wxWindow* parent)
	: wxWindow(parent, wxID_ANY)
{
	wxBoxSizer* bSizer  = new wxBoxSizer(wxVERTICAL);

	m_richtextcrtl = new wxRichTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxHSCROLL);
	bSizer->Add(m_richtextcrtl, 1, wxEXPAND);

	SetSizer(bSizer);
	Layout();

	g_debug.SetConsoleWindow(this);

	m_richtextcrtl->SetEditable(false);
}

hlaeConsoleWindow::~hlaeConsoleWindow()
{

}

void hlaeConsoleWindow::WriteMessage(const wxString& message,
	const wxDateTime& date, int debuglevel)
{
		wxColour colour;

		switch(debuglevel)
		{
		case hlaeDEBUG_ERROR:
			colour = wxColour(255, 0, 0);
			break;
		case hlaeDEBUG_WARNING:
			colour = wxColour(255, 127, 0);
			break;
		case hlaeDEBUG_INFO:
			colour = wxColour(0, 127, 0);
			break;
		case hlaeDEBUG_VERBOSE_LEVEL1:
			colour = wxColour(0, 0, 127);
			break;
		case hlaeDEBUG_VERBOSE_LEVEL2:
			colour = wxColour(0, 127, 127);
			break;
		case hlaeDEBUG_VERBOSE_LEVEL3:
			colour = wxColour(127, 127, 127);
			break;
		case hlaeDEBUG_DEBUG:
			colour = wxColour(255, 0, 255);
			break;
		default:
			colour = wxColour(0, 0, 0);
			break;
		}

		m_richtextcrtl->AppendText(wxT("> "));
		m_richtextcrtl->AppendText(date.Format(wxT("%H:%M ")));
		m_richtextcrtl->BeginTextColour(colour);
		m_richtextcrtl->AppendText(message);
		m_richtextcrtl->EndTextColour();
		m_richtextcrtl->Newline();
}