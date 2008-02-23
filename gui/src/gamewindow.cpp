// ? #define WIN32_LEAN_AND_MEAN
#include <windows.h> // necessary for preparsing events

#include <wx/scrolwin.h>
#include <wx/dc.h>

#include <hlae/basecomserver.h>
#include <hlae/gamewindow.h>


//
// Event Table:
//

// all events that need HWND might cause problems, so if there are problems, check those

BEGIN_EVENT_TABLE(CHlaeGameWindow, wxScrolledWindow)

	// wxActivateEvent:
	EVT_ACTIVATE			(CHlaeGameWindow::OnActivate)
	
	// wxCloseEvent:
	EVT_CLOSE				(CHlaeGameWindow::OnClose) 
	
	// wxEraseEvent:
	EVT_ERASE_BACKGROUND	(CHlaeGameWindow::OnEraseBackground)
	
	// wxFocusEvent:
	EVT_SET_FOCUS			(CHlaeGameWindow::OnSetFocus)
	EVT_KILL_FOCUS			(CHlaeGameWindow::OnKillFocus)
	
	// wxKeyEvent:
	EVT_KEY_DOWN			(CHlaeGameWindow::OnKeyDown)
	EVT_KEY_UP				(CHlaeGameWindow::OnKeyUp)
	EVT_CHAR				(CHlaeGameWindow::OnChar)

	// wxIdleEvent:
	// not handled
	EVT_IDLE				(CHlaeGameWindow::OnIdle)

	// wxMouseEvent:
	EVT_LEFT_DOWN			(CHlaeGameWindow::OnMouseLeftDown)
	EVT_LEFT_UP				(CHlaeGameWindow::OnMouseLeftUp)
	EVT_MIDDLE_DOWN			(CHlaeGameWindow::OnMouseMiddleDown)
	EVT_MIDDLE_UP			(CHlaeGameWindow::OnMouseMiddleUp)
	EVT_RIGHT_DOWN			(CHlaeGameWindow::OnMouseRightDown)
	EVT_RIGHT_UP			(CHlaeGameWindow::OnMouseRightUp)
	EVT_MOTION				(CHlaeGameWindow::OnMouseMotion)
	EVT_MOUSEWHEEL			(CHlaeGameWindow::OnMouseWheel)

	// wxMoveEvent:
	EVT_MOVE				(CHlaeGameWindow::OnMove)

	// wxPaintEvent:
	EVT_PAINT				(CHlaeGameWindow::OnPaint)
	
	// wxSizeEvent:
	EVT_SIZE				(CHlaeGameWindow::OnSize)

END_EVENT_TABLE()


//
// Event Handlers Implementation:
//

void CHlaeGameWindow::OnActivate( wxActivateEvent &myevent )
{
	// we map this to activate app
	_pHlaeBcServer->PassEventPreParsed(WM_ACTIVATE,(unsigned int)((unsigned short)(myevent.GetActive())&0x0ffff),(unsigned int)GetHWND());
	myevent.Skip();
}
void CHlaeGameWindow::OnClose( wxCloseEvent &myevent )
{
	_pHlaeBcServer->PassEventPreParsed(WM_CLOSE,0,0);
	myevent.Skip();
}
void CHlaeGameWindow::OnEraseBackground( wxEraseEvent &myevent )
{
	_pHlaeBcServer->PassEventPreParsed(WM_ERASEBKGND,(unsigned int)(myevent.GetDC()->GetHDC()),0);
	myevent.Skip();
}
void CHlaeGameWindow::OnSetFocus( wxFocusEvent &myevent )
{
	_pHlaeBcServer->PassEventPreParsed(WM_SETFOCUS,(unsigned int)GetHWND(),0);
	myevent.Skip();
}
void CHlaeGameWindow::OnKillFocus( wxFocusEvent &myevent )
{
	_pHlaeBcServer->PassEventPreParsed(WM_KILLFOCUS,(unsigned int)GetHWND(),0);
	myevent.Skip();
}
void CHlaeGameWindow::OnKeyDown( wxKeyEvent &myevent )
{
	if(_pHlaeBcServer->PassEventPreParsed(WM_KEYDOWN,(unsigned int)(myevent.GetRawKeyCode()),(unsigned int)(myevent.GetRawKeyFlags())))
		myevent.Skip();
}
void CHlaeGameWindow::OnKeyUp( wxKeyEvent &myevent )
{
	if(_pHlaeBcServer->PassEventPreParsed(WM_KEYUP,(unsigned int)(myevent.GetRawKeyCode()),(unsigned int)(myevent.GetRawKeyFlags())))
		myevent.Skip();
}
void CHlaeGameWindow::OnChar( wxKeyEvent &myevent )
{
	if(_pHlaeBcServer->PassEventPreParsed(WM_CHAR,(unsigned int)(myevent.GetRawKeyCode()),(unsigned int)(myevent.GetRawKeyFlags())))
		myevent.Skip();
}
void CHlaeGameWindow::OnIdle( wxIdleEvent &myevent )
{
	// not handled
	myevent.Skip();
}

// macro for mausposition to lparam wrapping:
#define MY_GET_MPOS ((unsigned short)(myevent.GetY())<<16) + ((unsigned short)(myevent.GetX()) & 0x0ffff)

void CHlaeGameWindow::OnMouseLeftDown( wxMouseEvent &myevent )
{
	_pHlaeBcServer->PassEventPreParsed(WM_LBUTTONDOWN,0,MY_GET_MPOS);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseLeftUp( wxMouseEvent &myevent )
{
	_pHlaeBcServer->PassEventPreParsed(WM_LBUTTONUP,0,MY_GET_MPOS);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseMiddleDown( wxMouseEvent &myevent )
{
	_pHlaeBcServer->PassEventPreParsed(WM_MBUTTONDOWN,0,MY_GET_MPOS);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseMiddleUp( wxMouseEvent &myevent )
{
	_pHlaeBcServer->PassEventPreParsed(WM_MBUTTONUP,0,MY_GET_MPOS);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseRightDown( wxMouseEvent &myevent )
{
	_pHlaeBcServer->PassEventPreParsed(WM_RBUTTONDOWN,0,MY_GET_MPOS);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseRightUp( wxMouseEvent &myevent )
{
	_pHlaeBcServer->PassEventPreParsed(WM_RBUTTONUP,0,MY_GET_MPOS);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseMotion( wxMouseEvent &myevent )
{
	_pHlaeBcServer->PassEventPreParsed(WM_MOUSEMOVE,0,MY_GET_MPOS);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseWheel( wxMouseEvent &myevent )
{
#ifndef WM_MOUSEWHEEL
	#define WM_MOUSEWHEEL                   0x020A
#endif
	_pHlaeBcServer->PassEventPreParsed(WM_MOUSEWHEEL,(unsigned int)(myevent.GetWheelRotation()),MY_GET_MPOS);
	myevent.Skip();
}
void CHlaeGameWindow::OnMove( wxMoveEvent &myevent)
{
	wxPoint mypos = myevent.GetPosition();
	_pHlaeBcServer->PassEventPreParsed(WM_MOVE,0,(((unsigned int)(mypos.y)&0x0ffff)<<16)+((unsigned int)(mypos.x)&0x0ffff));
	myevent.Skip();
}
void CHlaeGameWindow::OnPaint( wxPaintEvent &myevent)
{
	_pHlaeBcServer->PassEventPreParsed(WM_PAINT,0,0);
	myevent.Skip();
}
void CHlaeGameWindow::OnSize( wxSizeEvent &myevent)
{
	wxSize mysize = myevent.GetSize();
	_pHlaeBcServer->PassEventPreParsed(WM_MOVE,SIZE_RESTORED,(((unsigned int)(mysize.GetHeight())&0x0ffff)<<16)+((unsigned int)(mysize.GetWidth())&0x0ffff));
	myevent.Skip();
}

//
// Implementation of other class functions:
//

/*bool CHlaeGameWindow::ProcessEvent(wxEvent &myevent)
{
	bool bRes = false;

	//
	// Pre Filter events
	//

	//
	// Pass events too hook
	//
	bRes = _pHlaeBcServer->PassEventToHook(myevent);

	//
	// Post Filter events
	//
	if(!bRes)
	{
		bRes=wxScrolledWindow::ProcessEvent(myevent);
	}

	return bRes;
}*/