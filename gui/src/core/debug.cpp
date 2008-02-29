#include <wx/msgdlg.h>

#include <hlae/core/debug.h>

#include <hlae/core/lists/layout.h>

hlaeDebug g_debug;

hlaeDebug::hlaeDebug()
	: wxObject()
{
	m_loglist = new hlaeList();
	m_loglist->DeleteContents(true);

	m_verboselevel = hlaeDEBUG_VERBOSE_LEVEL3;
}

hlaeDebug::~hlaeDebug()
{
	m_loglist->Clear();
	delete m_loglist;
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

	// Write to console window
	if (m_consolewindow && (debuglevel != hlaeDEBUG_FATALERROR))
		m_consolewindow->WriteMessage(element->GetMessage(), element->GetDate(), debuglevel);

	// Write to logfile
	// TODO
}

void hlaeDebug::SendMessage(const wxString& message, int debuglevel)
{
	if ((debuglevel <= m_verboselevel)
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
			wxT("\n Half-Life Advanced Effects could not recover from this error. ")
			wxT("The program will shut down."), wxT("FATAL ERROR"), wxOK | wxICON_ERROR);
		msgdgl->ShowModal();
	}
}

void hlaeDebug::UpdateAllMessages()
{
	for (unsigned int i=0;i < m_loglist->GetCount();i++) UpdateMessage(i);
}
