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
	//if (_bTransmitAllowed) _pHlaeBcServer->PassEventPreParsed(WM_ACTIVATE,(unsigned int)((unsigned short)(myevent.GetActive())&0x0ffff),(unsigned int)GetHWND());
	myevent.Skip();
}
void CHlaeGameWindow::OnClose( wxCloseEvent &myevent )
{
	//if (_bTransmitAllowed) _pHlaeBcServer->PassEventPreParsed(WM_CLOSE,0,0);
	myevent.Skip();
}
void CHlaeGameWindow::OnEraseBackground( wxEraseEvent &myevent )
{
	if (_bTransmitAllowed) _pHlaeBcServer->PassEventPreParsed(WM_ERASEBKGND,(unsigned int)(myevent.GetDC()->GetHDC()),0);
	myevent.Skip();
}
void CHlaeGameWindow::OnSetFocus( wxFocusEvent &myevent )
{
	//if (_bTransmitAllowed) _pHlaeBcServer->PassEventPreParsed(WM_SETFOCUS,(unsigned int)GetHWND(),0);
	myevent.Skip();
}
void CHlaeGameWindow::OnKillFocus( wxFocusEvent &myevent )
{
	//if (_bTransmitAllowed) _pHlaeBcServer->PassEventPreParsed(WM_KILLFOCUS,(unsigned int)GetHWND(),0);
	myevent.Skip();
}
void CHlaeGameWindow::OnKeyDown( wxKeyEvent &myevent )
{
	if (_bTransmitAllowed) 
	{
		//myevent.GetKey
		if(_pHlaeBcServer->Pass_KeyBoardEvent(WM_KEYDOWN,(unsigned int)(myevent.GetRawKeyCode()),(unsigned int)(myevent.GetRawKeyFlags())))
			myevent.Skip();
	} else myevent.Skip();
}
void CHlaeGameWindow::OnKeyUp( wxKeyEvent &myevent )
{
	if (_bTransmitAllowed)
	{
		if(_pHlaeBcServer->Pass_KeyBoardEvent(WM_KEYUP,(unsigned int)(myevent.GetRawKeyCode()),(unsigned int)(myevent.GetRawKeyFlags())))
			myevent.Skip();
	} else myevent.Skip();
}
void CHlaeGameWindow::OnChar( wxKeyEvent &myevent )
{
	if (_bTransmitAllowed)
	{
		if(_pHlaeBcServer->Pass_KeyBoardEvent(WM_CHAR,(unsigned int)(myevent.GetRawKeyCode()),(unsigned int)(myevent.GetRawKeyFlags())))
			myevent.Skip();
	} else myevent.Skip();
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
	int ix,iy;
	CalcUnscrolledPosition(myevent.GetX(),myevent.GetY(),&ix,&iy);
	if (_bTransmitAllowed) _pHlaeBcServer->Pass_MouseEvent(WM_LBUTTONDOWN,0,ix,iy);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseLeftUp( wxMouseEvent &myevent )
{
	int ix,iy;
	CalcUnscrolledPosition(myevent.GetX(),myevent.GetY(),&ix,&iy);
	if (_bTransmitAllowed) _pHlaeBcServer->Pass_MouseEvent(WM_LBUTTONUP,0,ix,iy);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseMiddleDown( wxMouseEvent &myevent )
{
	int ix,iy;
	CalcUnscrolledPosition(myevent.GetX(),myevent.GetY(),&ix,&iy);
	if (_bTransmitAllowed) _pHlaeBcServer->Pass_MouseEvent(WM_MBUTTONDOWN,0,ix,iy);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseMiddleUp( wxMouseEvent &myevent )
{
	int ix,iy;
	CalcUnscrolledPosition(myevent.GetX(),myevent.GetY(),&ix,&iy);
	if (_bTransmitAllowed) _pHlaeBcServer->Pass_MouseEvent(WM_MBUTTONUP,0,ix,iy);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseRightDown( wxMouseEvent &myevent )
{
	int ix,iy;
	CalcUnscrolledPosition(myevent.GetX(),myevent.GetY(),&ix,&iy);
	if (_bTransmitAllowed) _pHlaeBcServer->Pass_MouseEvent(WM_RBUTTONDOWN,0,ix,iy);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseRightUp( wxMouseEvent &myevent )
{
	int ix,iy;
	CalcUnscrolledPosition(myevent.GetX(),myevent.GetY(),&ix,&iy);
	if (_bTransmitAllowed) _pHlaeBcServer->Pass_MouseEvent(WM_RBUTTONUP,0,ix,iy);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseMotion( wxMouseEvent &myevent )
{
	int ix,iy;
	CalcUnscrolledPosition(myevent.GetX(),myevent.GetY(),&ix,&iy);
	if (_bTransmitAllowed) _pHlaeBcServer->Pass_MouseEvent(WM_MOUSEMOVE,0,ix,iy);
	myevent.Skip();
}
void CHlaeGameWindow::OnMouseWheel( wxMouseEvent &myevent )
{
#ifndef WM_MOUSEWHEEL
	#define WM_MOUSEWHEEL                   0x020A
#endif
	int ix,iy;
	CalcUnscrolledPosition(myevent.GetX(),myevent.GetY(),&ix,&iy);
	if (_bTransmitAllowed) _pHlaeBcServer->Pass_MouseEvent(WM_MOUSEWHEEL,(unsigned int)(myevent.GetWheelRotation()),ix,iy);
	myevent.Skip();
}
void CHlaeGameWindow::OnMove( wxMoveEvent &myevent)
{
	int ix,iy;
	wxPoint mypos = myevent.GetPosition();
	CalcUnscrolledPosition(mypos.x,mypos.y,&ix,&iy);
	if (_bTransmitAllowed) _pHlaeBcServer->Pass_MouseEvent(WM_MOVE,0,ix,iy);
	myevent.Skip();
}
void CHlaeGameWindow::OnPaint( wxPaintEvent &myevent)
{
	//if (_bTransmitAllowed) _pHlaeBcServer->Do_DoPepareDC();
	//if (_bTransmitAllowed) _pHlaeBcServer->PassEventPreParsed(WM_PAINT,0,0);
	myevent.Skip();
}
void CHlaeGameWindow::OnSize( wxSizeEvent &myevent)
{
	//int ix,iy,iw,ih;
	//wxRect mypos = myevent.GetRect();
	//CalcUnscrolledPosition(mypos.GetLeft(),mypos.GetTop(),&ix,&iy);
	//CalcUnscrolledPosition(mypos.GetRight(),mypos.GetBottom(),&ix,&ih);

	//GetViewStart(&ix,&iy);
	//GetVirtualSize(&iw,&ih);

	//char sztemp[50];
	//_snprintf(sztemp,sizeof(sztemp),"x: %i y: %i w: %i h: %i",ix,iy,ix+iw,iy+ih);
	//MessageBoxA(0,sztemp,"CHlaeGameWindow::OnSize",MB_OK|MB_ICONINFORMATION);

	//if (_bTransmitAllowed) _pHlaeBcServer->Pass_WndRectUpdate(ix,iy,iw,ih);
	//if (_bTransmitAllowed) _pHlaeBcServer->PassEventPreParsed(WM_SIZE,SIZE_RESTORED,(((unsigned int)(mysize.GetHeight())&0x0ffff)<<16)+((unsigned int)(mysize.GetWidth())&0x0ffff));
	myevent.Skip();
}

//
// Implementation of other class functions:
//



void CHlaeGameWindow::CreateInternal(CHlaeBcServer *pHlaeBcServer)
{
	_pHlaeBcServer = pHlaeBcServer;
	_bTransmitAllowed = false;

	// pay attention that we suplly the hwnd here (use the 5 param version),
	// since _pHlaeBcServer doesn't know the CHlaeGameWindow pointer yet!

	//_pHlaeBcServer->PassEventPreParsed(this->GetHWND(),WM_CREATE,0,0);
	_bTransmitAllowed = true;
}


bool CHlaeGameWindow::Create(wxWindow *parent,
                wxWindowID winid,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& name)
// WARNING: we skipp the WM_CREATE CREATESTRUCT STRUCTURE in lParam
{
	bool bRes;
	// WARNING, wrong order?
	bRes=wxScrolledWindow::Create(parent,winid,pos,size,style,name);
	//_pHlaeBcServer->PassEventPreParsed(WM_CREATE,0,0);
	_bTransmitAllowed = true;
	return bRes;
}

bool CHlaeGameWindow::Destroy()
{
	//_pHlaeBcServer->PassEventPreParsed(WM_DESTROY,0,0);
	_bTransmitAllowed = false;
	return wxScrolledWindow::Destroy();
}

void CHlaeGameWindow::OnDraw(wxDC &dc)
{
	if (_bTransmitAllowed) _pHlaeBcServer->Do_DoPepareDC();
	int isx,isy,ix,iy,iwt,iht,iwv,ihv;

	GetScrollPixelsPerUnit(&isx,&isy);
	GetViewStart(&ix,&iy);
	GetVirtualSize(&iwt,&iht);
	GetClientSize(&iwv,&ihv);

	if (_bTransmitAllowed) _pHlaeBcServer->Pass_WndRectUpdate(isx*ix,isy*iy,iwv,ihv,iwt,iht);
}

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