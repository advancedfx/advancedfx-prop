#ifndef HLAE_GAMEWINDOW_H
#define HLAE_GAMEWINDOW_H

#include <wx/wx.h>

#include <system/basecom.h>

class CHlaeGameWindow : public wxScrolledWindow
{
public:
	CHlaeGameWindow( wxWindow *parent,
                     wxWindowID winid = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxScrolledWindowStyle,
                     const wxString& name = wxPanelNameStr)
		: wxScrolledWindow(parent, winid, pos, size, style, name)
		{  CreateInternal(); };

    void CreateInternal();

	CHlaeGameWindow::~CHlaeGameWindow();


	// Events:
	void OnFocus(wxFocusEvent& myevent);
	
private:
	DECLARE_EVENT_TABLE()

	CHlaeBcServer* _pHlaeBcServer;

};

#endif