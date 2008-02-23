#ifndef HLAE_GAMEWINDOW_H
#define HLAE_GAMEWINDOW_H

#include <wx/scrolwin.h>

class CHlaeBcServer;

class CHlaeGameWindow : public wxScrolledWindow
{
public:
	CHlaeGameWindow(CHlaeBcServer *pHlaeBcServer)
		: wxScrolledWindow()
		{_pHlaeBcServer = pHlaeBcServer; };

	CHlaeGameWindow(CHlaeBcServer *pHlaeBcServer,
					 wxWindow *parent,
                     wxWindowID winid = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxScrolledWindowStyle,
                     const wxString& name = wxPanelNameStr)
		: wxScrolledWindow(parent, winid, pos, size, style, name)
		{ _pHlaeBcServer = pHlaeBcServer; };

	//virtual bool ProcessEvent(wxEvent &myevent); // override command handler
private:
	DECLARE_EVENT_TABLE()

	CHlaeBcServer *_pHlaeBcServer;

	// events:
	void OnActivate( wxActivateEvent &myevent );
	void OnClose( wxCloseEvent &myevent );
	void OnEraseBackground( wxEraseEvent &myevent );
	void OnSetFocus( wxFocusEvent &myevent );
	void OnKillFocus( wxFocusEvent &myevent );
	void OnKeyDown( wxKeyEvent &myevent );
	void OnKeyUp( wxKeyEvent &myevent );
	void OnChar( wxKeyEvent &myevent );
	void OnIdle( wxIdleEvent &myevent ); // not handled
	void OnMouseLeftDown( wxMouseEvent &myevent );
	void OnMouseLeftUp( wxMouseEvent &myevent );
	void OnMouseMiddleDown( wxMouseEvent &myevent );
	void OnMouseMiddleUp( wxMouseEvent &myevent );
	void OnMouseRightDown( wxMouseEvent &myevent );
	void OnMouseRightUp( wxMouseEvent &myevent );
	void OnMouseMotion( wxMouseEvent &myevent );
	void OnMouseWheel( wxMouseEvent &myevent );	
	void OnMove( wxMoveEvent &myevent);
	void OnPaint( wxPaintEvent &myevent);
	void OnSize( wxSizeEvent &myevent);
};

#endif