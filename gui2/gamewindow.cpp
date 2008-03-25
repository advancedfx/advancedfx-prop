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
#ifdef _DEBUG
	g_debug.SendMessage(wxT("CHlaeGameWindow::CreateInternal ..."), hlaeDEBUG_DEBUG);
#endif

	_pHlaeBcServer = new CHlaeBcServer(this);
	if (!_pHlaeBcServer) throw "could not create CHlaeBcServer object";
}

CHlaeGameWindow::~CHlaeGameWindow()
{
#ifdef _DEBUG
	#pragma message("CHlaeGameWindow::~CHlaeGameWindow cannot enable debugmessage because g_debug may be not present anymore")
	//g_debug.SendMessage(wxT("CHlaeGameWindow:~CHlaeGameWindow ..."), hlaeDEBUG_DEBUG);
#endif

	delete _pHlaeBcServer;
}