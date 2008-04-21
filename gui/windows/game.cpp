#include <wx/wx.h>

#include <system/debug.h>
#include <system/basecom.h>

#include "game.h"


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
	myevent.Skip();
}

//
// Implementation of other class functions:
//

void CHlaeGameWindow::CreateInternal()
{
#ifdef _DEBUG
	g_debug.SendMessage(_T("CHlaeGameWindow::CreateInternal ..."), hlaeDEBUG_DEBUG);
#endif

	bUndockOnFilming = false;

	_pHlaeBcServer = new CHlaeBcServer(this);
	if (!_pHlaeBcServer) throw "could not create CHlaeBcServer object";
}

CHlaeGameWindow::~CHlaeGameWindow()
{
#ifdef _DEBUG
	#pragma message("CHlaeGameWindow::~CHlaeGameWindow cannot enable debugmessage because g_debug may be not present anymore")
	//g_debug.SendMessage(_T("CHlaeGameWindow:~CHlaeGameWindow ..."), hlaeDEBUG_DEBUG);
#endif

	delete _pHlaeBcServer;
}

void CHlaeGameWindow::OnHlaeActivate(bool bActive)
{
	if (_pHlaeBcServer) _pHlaeBcServer->OnHlaeActivate(bActive);
}