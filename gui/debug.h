#ifndef _HLAE_DEBUG_H_
#define _HLAE_DEBUG_H_

#include "list.h"
#include "console.h"
#include "main.h"

#include <wx/string.h>
#include <wx/datetime.h>

enum {
	hlaeDEBUG_FATALERROR,
	hlaeDEBUG_ERROR,
	hlaeDEBUG_WARNING,
	hlaeDEBUG_VERBOSE_LEVEL1,
	hlaeDEBUG_VERBOSE_LEVEL2,
	hlaeDEBUG_VERBOSE_LEVEL3
};

class hlaeLogListElement : public wxObject
{
private:
	int m_debuglevel;
	wxDateTime m_date;
	wxString m_message;
public:
	hlaeLogListElement(const wxString& message, const wxDateTime& date, int debuglevel);
	~hlaeLogListElement();
	const wxDateTime& GetDate();
	const wxString& GetMessage();
	int GetDebugLevel();
};

class hlaeDebug : public wxObject
{
public:
	hlaeDebug();
	~hlaeDebug();
	void SendMessage(const wxString& message, int debuglevel);
	void SetConsoleWindow(hlaeConsoleWindow* console_window);
	void UpdateAllMessages();
	void UpdateSettings();
private:
	int m_outputlevel;
	int m_logfilesize;
	wxString* m_logfilepath;
	hlaeConsoleWindow* m_consolewindow;
	hlaeList* m_loglist;
	void UpdateMessage(size_t index);
};

extern hlaeDebug g_debug;

#endif