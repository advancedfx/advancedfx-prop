#ifndef HLAE_GAMEWINDOW_H
#define HLAE_GAMEWINDOW_H

#include <wx/scrolwin.h>

class CHlaeGameWindow : public wxScrolledWindow
{
public:
	CHlaeGameWindow() : wxScrolledWindow() {};
	CHlaeGameWindow(wxWindow *parent,
                     wxWindowID winid = wxID_ANY,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxScrolledWindowStyle,
                     const wxString& name = wxPanelNameStr) :
	wxScrolledWindow(parent, winid, pos, size, style, name) {};
};

#endif