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

	void Do_DoPepareDC(); // called by gamewindow to inform us, that we should adjust the device origin again.

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
	wxClientDC *_pHlaeGameWindowDC;

	void * _DoCreateWindowExA(char *lpClassNameA,char *lpWindowNameA,int x, int y, int nHeight, int nWidth); // outdated
	bool _DoDestroyWindow(WXHWND wxhWnd); // outdated

	bool _Do_GameWndPrepare(int nWidth, int nHeight);
	WXHWND CHlaeBcServer::_Do_GameWndGetDC(); // returns the DC's HWND, not the DC (because the HDC is not allowed to be shared among threads)
	bool _Do_GameWndRelease();
};

#endif