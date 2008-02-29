#ifndef _HLAE_DEBUG_H_
#define _HLAE_DEBUG_H_

#include <wx/string.h>
#include <wx/datetime.h>

#include <hlae/core/list.h>
#include <hlae/core/lists/log.h>
#include <hlae/windows/console.h>
#include <hlae/core/app.h>
#include <hlae/windows/main.h>

enum {
	hlaeDEBUG_FATALERROR,
	hlaeDEBUG_ERROR,
	hlaeDEBUG_WARNING,
	hlaeDEBUG_VERBOSE_LEVEL1,
	hlaeDEBUG_VERBOSE_LEVEL2,
	hlaeDEBUG_VERBOSE_LEVEL3
};

class hlaeDebug : public wxObject
{

public:

	hlaeDebug();
	~hlaeDebug();

	void SendMessage(const wxString& message, int debuglevel);
	void SetApp(hlaeApp* application);
	void SetConsoleWindow(hlaeConsoleWindow* console_window);
	void UpdateAllMessages();
	void UpdateSettings();


private:

	int m_outputlevel;
	int m_logfilesize;

	wxString* m_logfilepath;

	hlaeConsoleWindow* m_consolewindow;
	hlaeApp* m_application;
	hlaeList* m_loglist;

	void UpdateMessage(size_t index);
	
};

extern hlaeDebug g_debug;

#endif // _HLAE_DEBUG_H_