#include <wx/msgdlg.h>

#include "debug.h"

hlaeDebug g_debug;

hlaeLogListElement::hlaeLogListElement(const wxString& message, const wxDateTime& date, hlaeDEBUG_MessageType_e debuglevel)
	: m_message(message), m_date(date), m_debuglevel(debuglevel), wxObject()
{}

hlaeLogListElement::~hlaeLogListElement()
{}

const wxDateTime& hlaeLogListElement::GetDate()
{
	return m_date;
}

const wxString& hlaeLogListElement::GetMessage()
{
	return m_message;
}

int hlaeLogListElement::GetDebugLevel()
{
	return m_debuglevel;
}

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
	m_outputlevel = hlaeDEBUG_DEFAULTLEVEL;
	m_logfilesize = -1;
}

void hlaeDebug::SetOutputLevel(hlaeDEBUG_MessageType_e debuglevel)
{
	m_outputlevel = debuglevel;
}


void hlaeDebug::SetConsoleWindow(hlaeConsoleWindow* console_window)
{
	m_consolewindow = console_window;
	UpdateAllMessages();
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

void hlaeDebug::SendMessage(const wxString& message, hlaeDEBUG_MessageType_e debuglevel)
{
	if ((debuglevel <= m_outputlevel)
		#ifndef _DEBUG
			&& (debuglevel < hlaeDEBUG_DEBUG)
		#endif
		)
		{
			m_loglist->Append(new hlaeLogListElement(message, wxDateTime::Now(), debuglevel));
			UpdateMessage(m_loglist->GetCount()-1);
		}

	if (debuglevel == hlaeDEBUG_FATALERROR)
	{
		wxMessageDialog* msgdgl = new wxMessageDialog(0, message +
			wxT("\n\nHalf-Life Advanced Effects may not recover from this error and will crash unexpectly. ")
			wxT("If the programm is still running, try to save your work and quit the program as soon as possible."),
			wxT("Fatal error"), wxOK | wxICON_ERROR);
		msgdgl->ShowModal();
	}
}

void hlaeDebug::UpdateAllMessages()
{
	for (unsigned int i=0;i < m_loglist->GetCount();i++) UpdateMessage(i);
}
