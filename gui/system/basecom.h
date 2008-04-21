#ifndef HLAE_BASECOMSERVER_H
#define HLAE_BASECOMSERVER_H

//#include "gamewindow.h"

#include <wx/wx.h>

class CHlaeGameWindow;
class CBCServerInternal;

class CHlaeBcServer
{
public:
	CHlaeBcServer(CHlaeGameWindow * pHlaeGameWindow);
	~CHlaeBcServer();

	bool Destroy();

	bool PassEventPreParsed(unsigned int umsg,unsigned int wParam,unsigned int lParam); // outdated
	bool PassEventPreParsed(WXHWND hwnd,unsigned int umsg,unsigned int wParam,unsigned int lParam); // outdated

	// Functions that may be used to pass events to the client:
	bool Pass_MouseEvent(unsigned int uMsg, unsigned int wParam, unsigned short iX,unsigned short iY);
	bool Pass_KeyBoardEvent(unsigned int uMsg, unsigned int uKeyCode, unsigned int uKeyFlags);

	bool OnGameWindowFocus();
	bool OnHlaeActivate(bool bActive);

	friend class CBCServerInternal;

private:
	CHlaeGameWindow * _pHlaeGameWindow;

	CBCServerInternal * _pBCServerInternal;

	bool _OnGameWindowClose(); // tells the server that it's server game window will be closed to allow it to react and communicate with the client
	
	// also called by CBCServerInternal:
	WXHWND _OnCreateWindow(int nWidth, int nHeight);
	bool _OnDestroyWindow();
	bool _OnFilmingStart();
	bool _OnFilmingStop();
	bool _UpdateWindow(int nWidth, int nHeight);
};


#endif