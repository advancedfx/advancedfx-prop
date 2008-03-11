#ifndef HLAE_GAMEWINDOW_H
#define HLAE_GAMEWINDOW_H

#include <wx/scrolwin.h>

class CHlaeBcServer;

class CHlaeGameWindow : public wxScrolledWindow
{
public:
	CHlaeGameWindow(CHlaeBcServer *pHlaeBcServer)
		: wxScrolledWindow()
		{ CreateInternal(pHlaeBcServer); };

	CHlaeGameWindow(CHlaeBcServer *pHlaeBcServer,
					 wxWindow *parent,
                     wxWindowID winid = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxScrolledWindowStyle,
                     const wxString& name = wxPanelNameStr)
		: wxScrolledWindow(parent, winid, pos, size, style, name)
		{ CreateInternal(pHlaeBcServer); };

	void CreateInternal(CHlaeBcServer *pHlaeBcServer);
	
	bool Create(wxWindow *parent, wxWindowID winid, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxScrolledWindowStyle, const wxString& name = wxPanelNameStr);
	
	bool Destroy();

	WXHDC GetDCInternal(); // used by basecomserver.h

	void OnDraw(wxDC &dc);
	//virtual bool ProcessEvent(wxEvent &myevent); // override command handler

private:
	DECLARE_EVENT_TABLE()

	CHlaeBcServer *_pHlaeBcServer;
	bool	_bTransmitAllowed;

	WXHDC _internalHDC;

	// fot the events table:
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