#ifndef HLAE_BASECOMSERVER_H
#define HLAE_BASECOMSERVER_H

//#include <hlae/auimanager.h>
//#include <hlae/gamewindow.h>
#include <wx/dcclient.h>

class hlaeAuiManager;
class CHlaeGameWindow;
class CBCServerInternal;

class CHlaeBcServer
{
public:
	CHlaeBcServer(wxWindow *parent,hlaeAuiManager *pHlaeAuiManager);
	~CHlaeBcServer();

	bool PassEventPreParsed(unsigned int umsg,unsigned int wParam,unsigned int lParam);
	bool PassEventPreParsed(WXHWND hwnd,unsigned int umsg,unsigned int wParam,unsigned int lParam);

	bool PassEventToHook(wxEvent &myevent); // not implemented

	friend class CBCServerInternal;

private:
	wxWindow *_parent;
	hlaeAuiManager *_pHlaeAuiManager;
	CHlaeGameWindow *_pHlaeGameWindow;
	wxClientDC *_pHlaeGameWindowDC;

	void * _DoCreateWindowExA(char *lpClassNameA,char *lpWindowNameA,int x, int y, int nHeight, int nWidth);
	bool _DoDestroyWindow(WXHWND wxhWnd);

	bool _Do_GameWndPrepare(int nWidth, int nHeight);
	WXHWND CHlaeBcServer::_Do_GameWndGetDC(); // returns the DC's HWND, not the DC (because the HDC is not allowed to be shared among threads)
	bool _Do_GameWndRelease();
};

#endif