#include <wx/wx.h>

#include <windows/console.h>
#include <system/debug.h>

#include <wx/sizer.h>
#include <wx/stattext.h>


hlaeConsoleWindow::hlaeConsoleWindow(wxWindow* parent)
	: wxWindow(parent, wxID_ANY)
{
	// Create the form

	wxBoxSizer* bSizer  = new wxBoxSizer(wxVERTICAL);

	m_textcrtl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxVSCROLL | wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
	bSizer->Add(m_textcrtl, 1, wxEXPAND);

	SetSizer(bSizer);
	Layout();


	// Set the consolewindow

	g_debug.SetConsoleWindow(this);

}

void hlaeConsoleWindow::WriteMessage(const wxString& message,
	const wxDateTime& date, int debuglevel)
{
		wxTextAttr attribute;

		switch(debuglevel)
		{
		case hlaeDEBUG_ERROR:
			attribute = wxTextAttr(wxColour(255, 0, 0));
			break;
		case hlaeDEBUG_WARNING:
			attribute = wxTextAttr(wxColour(255, 127, 0));
			break;
		case hlaeDEBUG_INFO:
			attribute = wxTextAttr(wxColour(0, 127, 0));
			break;
		case hlaeDEBUG_VERBOSE_LEVEL1:
			attribute = wxTextAttr(wxColour(0, 0, 127));
			break;
		case hlaeDEBUG_VERBOSE_LEVEL2:
			attribute = wxTextAttr(wxColour(0, 127, 127));
			break;
		case hlaeDEBUG_VERBOSE_LEVEL3:
			attribute = wxTextAttr(wxColour(127, 127, 127));
			break;
		case hlaeDEBUG_DEBUG:
			attribute = wxTextAttr(wxColour(255, 0, 255));
			break;
		default:
			attribute = wxTextAttr();
			break;
		}

		// if uncommented the program will run into multithreading problems
		// TODO: solve multithreading problems

		/* if (m_textcrtl->GetNumberOfLines() != 1) m_textcrtl->AppendText(_T("\n"));
		m_textcrtl->AppendText(date.Format(_T("[%H:%M] ")));
		m_textcrtl->SetDefaultStyle(attribute);
		m_textcrtl->AppendText(message);
		m_textcrtl->SetDefaultStyle(wxTextAttr()); */
		
}