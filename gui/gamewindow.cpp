// ? #define WIN32_LEAN_AND_MEAN
#include <windows.h> // necessary for preparsing events

#include <wx/dc.h>
#include <wx/dcclient.h>

#include <wx/scrolwin.h>

#include "debug.h"
#include "basecomserver.h"
#include "gamewindow.h"


//
// Event Table:
//

// all events that need HWND might cause problems, so if there are problems, check those

BEGIN_EVENT_TABLE(CHlaeGameWindow, wxScrolledWindow)
	EVT_SET_FOCUS(OnFocus)
END_EVENT_TABLE()


//
// Event Handlers Implementation:
//

void CHlaeGameWindow::OnFocus(wxFocusEvent& myevent)
{
	if (_pHlaeBcServer) _pHlaeBcServer->OnGameWindowFocus();
}

//
// Implementation of other class functions:
//

void CHlaeGameWindow::CreateInternal()
{
#ifdef __DEBUG
	g_debug.SendMessage(wxT("CHlaeGameWindow::CreateInternal ..."), hlaeDEBUG_DEBUG);
#endif

	bool bRes;

	_pHlaeBcServer = new CHlaeBcServer(this);
	bRes = bRes && _pHlaeBcServer;
}

CHlaeGameWindow::~CHlaeGameWindow()
{
#ifdef __DEBUG
	g_debug.SendMessage(wxT("CHlaeGameWindow:~CHlaeGameWindow ..."), hlaeDEBUG_DEBUG);
#endif

	delete _pHlaeBcServer;
}