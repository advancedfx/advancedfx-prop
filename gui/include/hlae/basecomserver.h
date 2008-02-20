#ifndef HLAE_BASECOMSERVER_H
#define HLAE_BASECOMSERVER_H

#include <hlae/auimanager.h>
#include <hlae/gamewindow.h>

//class haleAuiManager;
class CBCServerInternal;

class CHlaeBcServer
{
public:
	CHlaeBcServer(wxWindow *parent,hlaeAuiManager *pHlaeAuiManager);
	~CHlaeBcServer();

	friend class CBCServerInternal;

private:
	wxWindow *_parent;
	hlaeAuiManager *_pHlaeAuiManager;
	CHlaeGameWindow *_pHlaeGameWindow;

	void * _DoCreateWindowExA(char *lpClassNameA,char *lpWindowNameA,int x, int y, int nHeight, int nWidth);
};

#endif