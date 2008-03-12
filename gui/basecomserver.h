#ifndef HLAE_BASECOMSERVER_H
#define HLAE_BASECOMSERVER_H

//#include <hlae/auimanager.h>
//#include <hlae/gamewindow.h>
#include <wx/dcclient.h>

class hlaeLayoutManager;
class CHlaeGameWindow;
class CBCServerInternal;

class CHlaeBcServer
{
public:
	CHlaeBcServer(wxWindow *parent);
	~CHlaeBcServer();

	bool PassEventPreParsed(unsigned int umsg,unsigned int wParam,unsigned int lParam); // outdated
	bool PassEventPreParsed(WXHWND hwnd,unsigned int umsg,unsigned int wParam,unsigned int lParam); // outdated

	// Functions uses by the gamewindow to pass events to the client:
	bool Pass_WndRectUpdate(int iLeft, int iTop, int iWidthVisible, int iHeightVisible, int iWidthTotal, int iHeightTotal, int iLeftGlobal, int iTopGlobal);
	bool Pass_MouseEvent(unsigned int uMsg, unsigned int wParam, unsigned short iX,unsigned short iY);
	bool Pass_KeyBoardEvent(unsigned int uMsg, unsigned int uKeyCode, unsigned int uKeyFlags);

	friend class CBCServerInternal;

private:
	wxWindow *_parent;
	CHlaeGameWindow *_pHlaeGameWindow;
	void *_hGLRC;


	WXHWND _OnCreateWindow(int nWidth, int nHeight);
	bool _UpdateWindow(int nWidth, int nHeight);
};


#endif