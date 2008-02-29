#include <wx/msgdlg.h>

#include <hlae/core/debug.h>

hlaeDebug g_debug;

hlaeDebug::hlaeDebug()
	: wxObject()
{
	m_loglist = new hlaeList();
	m_loglist->DeleteContents(true);

	UpdateSettings();
}

hlaeDebug::~hlaeDebug()
{
	m_loglist->Clear();
	delete m_loglist;
}

void hlaeDebug::UpdateSettings()
{
	// Load from config
	m_outputlevel = hlaeDEBUG_VERBOSE_LEVEL3;
	m_logfilesize = -1;
}


void hlaeDebug::SetConsoleWindow(hlaeConsoleWindow* console_window)
{
	m_consolewindow = console_window;
	UpdateAllMessages();
}

void hlaeDebug::SetApp(hlaeApp* application)
{
	m_application = application;
}

void hlaeDebug::UpdateMessage(size_t index)
{
	hlaeLogListElement* element = dynamic_cast<hlaeLogListElement*>(m_loglist->Item(index)->GetData());
	
	int debuglevel = element->GetDebugLevel();

	// Write to console window (if it exists)
	if (m_consolewindow && (debuglevel != hlaeDEBUG_FATALERROR))
		m_consolewindow->WriteMessage(element->GetMessage(), element->GetDate(), debuglevel);

	// Write to logfile
	// TODO
}

void hlaeDebug::SendMessage(const wxString& message, int debuglevel)
{
	if ((debuglevel <= m_outputlevel)
		#ifndef _DEBUG
			&& (debuglevel != hlaeDEBUG_VERBOSE_LEVEL3)
		#endif
		)
		{
			m_loglist->Append(new hlaeLogListElement(message, wxDateTime::Now(), debuglevel));
			UpdateMessage(m_loglist->GetCount()-1);
		}

	if (debuglevel == hlaeDEBUG_FATALERROR)
	{
		wxMessageDialog* msgdgl = new wxMessageDialog(0, message +
			wxT("\n\nHalf-Life Advanced Effects could not recover from this error. ")
			wxT("The program will shut down."), wxT("Fatal error"), wxOK | wxICON_ERROR);
		msgdgl->ShowModal();
	}
}

void hlaeDebug::UpdateAllMessages()
{
	for (unsigned int i=0;i < m_loglist->GetCount();i++) UpdateMessage(i);
}
