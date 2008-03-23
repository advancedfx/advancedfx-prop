#ifndef HLAE_DEBUG_H
#define HLAE_DEBUG_H

#include <wx/wx.h>

#include <windows/console.h>
#include <windows/hlae.h>


enum hlaeDEBUG_MessageType_e {
	hlaeDEBUG_FATALERROR=1,
	hlaeDEBUG_ERROR,
	hlaeDEBUG_WARNING,
	hlaeDEBUG_INFO,
	hlaeDEBUG_VERBOSE_LEVEL1,
	hlaeDEBUG_VERBOSE_LEVEL2,
	hlaeDEBUG_VERBOSE_LEVEL3,
	hlaeDEBUG_DEBUG=0x0f
};

class hlaeLogListElement : public wxObject
{
private:
	hlaeDEBUG_MessageType_e m_debuglevel;
	wxDateTime m_date;
	wxString m_message;
public:
	hlaeLogListElement(const wxString& message, const wxDateTime& date, hlaeDEBUG_MessageType_e debuglevel);
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
	void SendMessage(const wxString& message, hlaeDEBUG_MessageType_e debuglevel);
	void SetConsoleWindow(hlaeConsoleWindow* console_window);
	void UpdateAllMessages();
	void UpdateSettings();
	void SetOutputLevel(hlaeDEBUG_MessageType_e debuglevel);
private:
	#define hlaeDEBUG_DEFAULTLEVEL hlaeDEBUG_VERBOSE_LEVEL3;
	hlaeDEBUG_MessageType_e m_outputlevel;
	int m_logfilesize;
	wxString* m_logfilepath;
	hlaeConsoleWindow* m_consolewindow;
	wxList* m_loglist;
	void UpdateMessage(size_t index);
};

extern hlaeDebug g_debug;

#endif