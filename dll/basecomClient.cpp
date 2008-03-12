#include <windows.h>
//#include <winuser.h> // KEYBDINPUT Structure, ...

#if 1//#ifdef MDT_DEBUG
	#include <stdio.h>

	#include "wrect.h"
	#include "cl_dll.h"
	#include "cdll_int.h"

	#include "cmdregister.h"

	extern cl_enginefuncs_s* pEngfuncs;
#endif

#include "../shared/com/basecom.h"

#include "basecomClient.h"


HWND g_hwHlaeBcCltWindow = NULL; // Hlae BaseCom client reciever window

HLAE_BASECOM_WndRectUpdate_s g_HlaeWindowRect; // Warning: in the current implementation those values may not represent the actual server's size, since HlaeBc_AdjustViewPort will do some security clamping on them when needed.

HGLRC g_hHlaeServerGL=NULL;
HWND  g_hHlaeServerWND=NULL;

HWND g_HL_MainWindow = NULL;
WNDCLASSA *g_HL_WndClassA = NULL;

struct
{
	int iX;
	int iY;
	int nWidth;
	int nHeight;

	HDC hDC;

	int iPixelFormat;
	PIXELFORMATDESCRIPTOR pfd;
	
	struct {
		int iX;
		int iY;
	} MouseTarget;
} g_HL_MainWindow_info;



LRESULT CALLBACK Hooking_WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	// filter window unspecific messages:

	switch(uMsg)
	{
	case WM_ACTIVATE:
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		// we won't override those messages, since this would give us problems with the mousecapture if we won't allow it to deactivate
		break;
	}

	if (hWnd==NULL || hWnd != g_HL_MainWindow)
		// this is not the MainWindow we want to control
		return g_HL_WndClassA->lpfnWndProc(hWnd,uMsg,wParam,lParam);

	// filter MainWindow specific messages:

	// blah
	switch (uMsg)
	{
	case WM_MOVE:
		g_HL_MainWindow_info.iX = (int)(short) LOWORD(lParam); 
		g_HL_MainWindow_info.iY = (int)(short) HIWORD(lParam);
		break;
	}

	return g_HL_WndClassA->lpfnWndProc(hWnd,uMsg,wParam,lParam);
}

LRESULT HlaeBcCallWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
// this function is not supported atm / it will reject all requests
// the server will inform us about specific events regarding the game window,
// this is our chance to react to them:
{
	// make sure we have the WindowProc and the H-L MainWindow handle, otherwise quit:
	if (!(g_HL_WndClassA && g_HL_WndClassA->lpfnWndProc && g_HL_MainWindow)) return FALSE;

	//
	// filtering of specific message codes:

	return FALSE; // this function is not supported atm

	// in any case we will replace the hwnd param, since the H-L winproc shall belive it originated from it's own window:


	//
	// after filtering we pass them on to the real proc (in case we didn't quit already):

	LRESULT lrRet = FALSE;

	lrRet = g_HL_WndClassA->lpfnWndProc(hwnd,uMsg,wParam,lParam);

	//
	// we could examine or modify the result here:

	// blah

	//
	// and finally we return:
	return lrRet;
}

// Handlers for events from Server's GameWindow:

LRESULT HlaeBcCl_WndRectUpdate(PVOID lpData)
{
	HLAE_BASECOM_WndRectUpdate_s *myps = (HLAE_BASECOM_WndRectUpdate_s *)lpData;
	//pEngfuncs->Con_Printf("MoveEvent: %i,%i\n",myps->iLeftGlobal,myps->iTopGlobal);
	g_HlaeWindowRect = *myps;

	return TRUE;
}

LRESULT HlaeBcCl_MouseEvent(PVOID lpData)
{
	HLAE_BASECOM_MSGCL_MouseEvent_s *myps = (HLAE_BASECOM_MSGCL_MouseEvent_s *)lpData;

	g_HL_MainWindow_info.MouseTarget.iX = myps->iX;
	g_HL_MainWindow_info.MouseTarget.iY = myps->iY;

	//pEngfuncs->Con_Printf("HlaeBcCl_MouseEvent: (%i,%i)\n",myps->iX,myps->iY);

	pEngfuncs->Con_Printf("OutPorc: 0x%08x - IsProc: 0x%08x\n",(void *)Hooking_WndProc,(void *)(GetWindowLong( g_HL_MainWindow, GWL_WNDPROC )));

	LRESULT tr = FALSE;//g_HL_WndClassA->lpfnWndProc(g_HL_MainWindow,(UINT)(myps->uMsg),(WPARAM)(myps->wParam),(LPARAM)(((myps->iY) << 16) + myps->iX));

	/*int ix,iy;
	pEngfuncs->GetMousePosition(&ix,&iy);
	int dx=g_HL_MainWindow_info.iX+myps->iX;
	int dy=g_HL_MainWindow_info.iY+myps->iX;
	pEngfuncs->Con_Printf("MouseEvent: inx=%i,iny=%i,dx=%i,dy=%i (%i,%i)\n",myps->iX,myps->iY,dx,dy,ix,iy);*/

	return tr;
}

LRESULT HlaeBcCl_KeyBoardEvent(PVOID lpData)
{
	HLAE_BASECOM_MSGCL_KeyBoardEvent_s *myps = (HLAE_BASECOM_MSGCL_KeyBoardEvent_s *)lpData;

	// pEngfuncs->Con_Printf("KeyEvent: %i %i 0x%08x\n",myps->uMsg,myps->uKeyCode,myps->uKeyFlags);

	return g_HL_WndClassA->lpfnWndProc(g_HL_MainWindow,(UINT)(myps->uMsg),(WPARAM)(myps->uKeyCode),(LPARAM)(myps->uKeyFlags));

}


// In order to understand the source that follows the Microsoft Documentation of WM_COPYDATA might be useful.

void *g_pHlaeBcResultTarget=NULL;

LRESULT CALLBACK HlaeBcCltWndProc(
    HWND hwnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{ 
    switch (uMsg) 
    { 
        case WM_CREATE: 
            // Initialize the window.
			//MessageBoxW(hwnd,L"WM_CREATE",HLAE_BASECOM_CLIENT_ID,MB_OK);
            return 0; 
 
        case WM_PAINT: 
            // Paint the window's client area. 
			return 0; 
 
        case WM_SIZE: 
            // Set the size and position of the window. 
            return 0; 
 
        case WM_DESTROY: 
            // Clean up window-specific data objects. 
			//MessageBoxW(hwnd,L"WM_DESTROY",HLAE_BASECOM_CLIENT_ID,MB_OK); 
            return 0; 

		case WM_COPYDATA:
			PCOPYDATASTRUCT pMyCDS;
			pMyCDS = (PCOPYDATASTRUCT) lParam;

			switch(pMyCDS->dwData)
			{
			case HLAE_BASECOM_RETCL_HELLO:
				// no checks performed atm
				return FALSE;

			case HLAE_BASECOM_MSGCL_WndRectUpdate:
				return HlaeBcCl_WndRectUpdate(pMyCDS->lpData);

			case HLAE_BASECOM_MSGCL_MouseEvent:
				return HlaeBcCl_MouseEvent(pMyCDS->lpData);

			case HLAE_BASECOM_MSGCL_KeyBoardEvent:
				return HlaeBcCl_KeyBoardEvent(pMyCDS->lpData);

			case HLAE_BASECOM_RETCL_AquireGlWindow:
				if (!g_pHlaeBcResultTarget) return FALSE;
				memcpy(g_pHlaeBcResultTarget,pMyCDS->lpData,sizeof(HLAE_BASECOM_RET_AquireGlWindow_s));
				return TRUE;
				
			case HLAE_BASECOM_RETCL_ReleaseGlWindow:
				if (!g_pHlaeBcResultTarget) return FALSE;
				memcpy(g_pHlaeBcResultTarget,pMyCDS->lpData,sizeof(HLAE_BASECOM_RET_ReleaseGlWindow_s));
				return TRUE;

			default:
				MessageBoxW(hwnd,L"Unexpected message.",HLAE_BASECOM_CLIENT_ID,MB_OK|MB_ICONERROR);
			}
			return FALSE;
 
        default: 
            return DefWindowProc(hwnd, uMsg, wParam, lParam); 
    } 
    return 0; 
}

bool HlaeBcCltStart()
// this starts the client server for being able to recieve
// return messages and other messages form the server
{
	HINSTANCE hInstance = (HINSTANCE)GetCurrentProcessId();

	if (g_hwHlaeBcCltWindow)
		return false; // if already running quit

	static bool bRegistered=false;
	static WNDCLASSW wc;

	if (!bRegistered)
	{
 
		// Register the main window class. 
		wc.style = NULL; 
		wc.lpfnWndProc = (WNDPROC) HlaeBcCltWndProc; 
		wc.cbClsExtra = 0; 
		wc.cbWndExtra = 0; 
		wc.hInstance = hInstance; 
		wc.hIcon = NULL; 
		wc.hCursor = NULL; 
		wc.hbrBackground = NULL; 
		wc.lpszMenuName =  NULL; 
		wc.lpszClassName = HLAE_BASECOM_CLASSNAME;

		if (!RegisterClassW(&wc))
			return false;

		bRegistered = true;
	 }

	if (!(g_hwHlaeBcCltWindow = CreateWindowW(wc.lpszClassName,HLAE_BASECOM_CLIENT_ID,WS_DISABLED|WS_POPUP,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInstance,NULL)))
		return false;

	return true;
}

bool HlaeBcCltStop()
// this stops the client server
{
	if (!g_hwHlaeBcCltWindow) return true;

	if (!DestroyWindow(g_hwHlaeBcCltWindow)) return false;

	g_hwHlaeBcCltWindow = NULL;
	return true;
}

bool HlaeBcCltSendMessage(DWORD dwId,DWORD cbSize,PVOID lpData)
// use this for one-way messages to the server
// this function could be optimized by reducing expensive FindWindowW calls
{
	HWND hwServerWindow;
	
	// curently wastes time:
	if(!(hwServerWindow=FindWindowW(HLAE_BASECOM_CLASSNAME,HLAE_BASECOM_SERVER_ID)))
		return false;

	COPYDATASTRUCT myCopyData;
	
	myCopyData.dwData=dwId;
	myCopyData.cbData=cbSize;
	myCopyData.lpData=lpData;

	return TRUE==SendMessageW(
		hwServerWindow, // the Server is the target
		WM_COPYDATA,
		(WPARAM)g_hwHlaeBcCltWindow, // if we have a window running (!=NULL), then inform the server, so it can identifiy us
		(LPARAM)&myCopyData
	);
}

bool HlaeBcCltSendMessageRet(DWORD dwId,DWORD cbSize,PVOID lpData,void *pResultTarget)
// use this for messages to the server to which the server will reply with a return (RET) message
{
	bool bRes;

	if (!g_hwHlaeBcCltWindow) return false;

	g_pHlaeBcResultTarget = pResultTarget;
	bRes=HlaeBcCltSendMessage(dwId,cbSize,lpData);
	g_pHlaeBcResultTarget = NULL;

	return bRes;
}

//
// Higher level interaction with the server:
//

// Please be aware that the Recieving Window Proc has to know about the return
// codes and act accordingly!
// So if you modifiy s.th. regarding the returns, you might need to modifiy it
// too.

HGLRC HlaeBc_AquireGlWindow(int nWidth, int nHeight, int iPixelFormat, CONST PIXELFORMATDESCRIPTOR *  ppfd, HWND *phServerWND, int *phSavedDC )
// requests the server to Create an OpenGL Resource context (wglCreateContext)
// returns NULL on fail or the GL Resource Handle on success.
{
	HGLRC retResult=NULL;

	HLAE_BASECOM_RET_AquireGlWindow_s *mycwret = new HLAE_BASECOM_RET_AquireGlWindow_s;
	HLAE_BASECOM_AquireGlWindow_s *mycws = new HLAE_BASECOM_AquireGlWindow_s;

	mycws->nWidth		= nWidth;
	mycws->nHeight		= nHeight;
	mycws->iPixelFormat	= iPixelFormat;
	mycws->pfd			= *ppfd;

	if(HlaeBcCltSendMessageRet(HLAE_BASECOM_QRYSV_AquireGlWindow,sizeof(HLAE_BASECOM_AquireGlWindow_s),(PVOID)mycws,mycwret))
	{
		retResult = mycwret->hServerGLRC;
		if (phServerWND) *phServerWND=mycwret->hServerWND;
		if (phSavedDC) *phSavedDC=mycwret->hSavedDC;
	}

	delete mycws;
	delete mycwret;

	return retResult;
}

BOOL HlaeBc_ReleaseGlWindow()
// requests the server to release the OpenGL Resource Context (wglDeleteContext)
// returns FALSE on fail, TRUE otherwise
{
	BOOL retResult=FALSE;

	HLAE_BASECOM_RET_ReleaseGlWindow_s *mycwret = new HLAE_BASECOM_RET_ReleaseGlWindow_s;
	HLAE_BASECOM_ReleaseGlWindow_s *mycws = new HLAE_BASECOM_ReleaseGlWindow_s;

	if(HlaeBcCltSendMessageRet(HLAE_BASECOM_QRYSV_ReleaseGlWindow,sizeof(HLAE_BASECOM_ReleaseGlWindow_s),(PVOID)mycws,mycwret))
	{
		retResult = mycwret->retResult;
	}

	delete mycws;
	delete mycwret;

	return retResult;
}

bool HlaeBc_UpdateWindow(int nWidth, int nHeight)
// informs the server about necessary game window size changes
// returns false on fail, true otherwise
{
	bool retResult = false;

	HLAE_BASECOM_UpdateWindows_s *mycws = new HLAE_BASECOM_UpdateWindows_s;

	mycws->nWidth=nWidth;
	mycws->nHeight=nHeight;
	
	if(HlaeBcCltSendMessage(HLAE_BASECOM_MSGSV_UpdateWindow,sizeof(HLAE_BASECOM_UpdateWindows_s),(PVOID)mycws))
	{
		retResult = true;
	}

	delete mycws;
	return retResult;
}

//
// WinAPI and other hooks for export in the basecomClient.h:
//

ATOM APIENTRY HlaeBcClt_RegisterClassA(CONST WNDCLASSA *lpWndClass)
{

#ifdef MDT_DEBUG
	char sztemp[1000];
	if (HIWORD(lpWndClass->lpszClassName))
	{
		_snprintf(sztemp,sizeof(sztemp),"ClassName: %s",lpWndClass->lpszClassName);
		MessageBox(0,sztemp,"HlaeBcClt_RegisterClassA",MB_OK);
	}
#endif

	// quit if it's not the class we want:
	if (!HIWORD(lpWndClass->lpszClassName) || lstrcmp(lpWndClass->lpszClassName,"Valve001"))
		return RegisterClassA(lpWndClass);

	ATOM tResult = NULL;
	WNDCLASSA *tWndClass= new WNDCLASSA;

	if(!g_HL_WndClassA) g_HL_WndClassA = new WNDCLASSA;

	memcpy(g_HL_WndClassA,lpWndClass,sizeof(WNDCLASSA)); // get us a global copy of the original class, we will need it to access the original lpfnWindowProc i.e.
	memcpy(tWndClass,lpWndClass,sizeof(WNDCLASSA)); // we want to register a modified copy

	// modifiy the copy to fit our needs:

	tWndClass->lpfnWndProc = (WNDPROC)Hooking_WndProc; // we want to hook the WindowProc, so we can control the message flow

	// register our modified copy of the class instead:
	tResult = RegisterClassA(tWndClass);
	delete tWndClass;

	// start client server (if already started this will be ignored):
	HlaeBcCltStart();

	return tResult;
}

HWND APIENTRY HlaeBcClt_CreateWindowExA(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam)
{

#ifdef MDT_DEBUG
	char sbuff[1000];
	_snprintf(sbuff,sizeof(sbuff),"dwExStyle: 0x%08x\nlpClassName: %s\nlpWindowName: %s\ndwStyle: 0x%08x\nx: %i\ny: %i\nWidth: %i\nnHeight: %i\nhWndParent: %u\nhMenu: %u\nhInstance: %u\nlpParam: 0x%08x",dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);
	MessageBoxA(NULL,sbuff,"MDT CreateWindowEx",MB_OK|MB_ICONINFORMATION);
#endif

	// quit if it's not the window we want:
	if (hWndParent!=NULL)
		return CreateWindowExA(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);

	// save postion:
	g_HL_MainWindow_info.iX = x;
	g_HL_MainWindow_info.iY = y;

	// and size:
	g_HL_MainWindow_info.nWidth = nWidth;
	g_HL_MainWindow_info.nHeight = nHeight;

	// pre init some states:
	g_HL_MainWindow_info.hDC = NULL;

	g_HL_MainWindow = CreateWindowExA(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);

	return g_HL_MainWindow;

}

BOOL APIENTRY HlaeBcClt_DestroyWindow(HWND hWnd)
{
	if (hWnd!=NULL && hWnd != g_HL_MainWindow)
	{
		// H-L main game window being destroyed

		// halt client server:
		HlaeBcCltStop();

		// clean up globals:
		g_HL_MainWindow = NULL;
		if(g_HL_WndClassA) delete g_HL_WndClassA; // I asume we won't use it anymore, this might be wrong in some rare cases.
	}

	return DestroyWindow(hWnd);
}

HDC APIENTRY HlaeBcClt_GetDC( HWND hWnd )
{
	// quit if it's not the window we want:
	if (hWnd==NULL || hWnd != g_HL_MainWindow)
		return GetDC(hWnd);

	g_HL_MainWindow_info.hDC = GetDC(hWnd);
	return g_HL_MainWindow_info.hDC;
}

void HlaeBcCl_AdjustViewPort(int &x, int &y, int width, int height)
// this will adjust the incoming params accroding to the gamewindow (in case it's DC can be asumed to be present)
{
	if(!g_hHlaeServerGL) return;

	static int iMyLastWidth=-1;
	static int iMyLastHeight=-1;

	// firt check if we need to inform the server about changed coords,
	// this has to be done carefully to avoid pingpongs in case the Server's Window is oversized i.e.
	// we also prevent updates for 
	if ((iMyLastWidth!=width || iMyLastHeight != height ) && width > 0 && height > 0)
	{
		pEngfuncs->Con_DPrintf("HlaeBcCl_AdjustViewPort: Cached values (%ix%i) outdated, forcing update (%ix%i).\n",iMyLastWidth,iMyLastHeight,width,height);

		iMyLastWidth=width;
		iMyLastHeight=height;

		// update the server:
		HlaeBc_UpdateWindow(width, height);
	}

	// now we will apply some clamping to avoid problems
	// in case the server's window is oversized for some reason:
	
	if (g_HlaeWindowRect.iHeightTotal >  height) g_HlaeWindowRect.iHeightTotal = height;
	if (g_HlaeWindowRect.iWidthTotal >  width) g_HlaeWindowRect.iWidthTotal = width;

	if (g_HlaeWindowRect.iWidthVisible > g_HlaeWindowRect.iWidthTotal) g_HlaeWindowRect.iWidthVisible =  g_HlaeWindowRect.iWidthTotal;
	if (g_HlaeWindowRect.iHeightVisible >  g_HlaeWindowRect.iWidthTotal) g_HlaeWindowRect.iHeightVisible = g_HlaeWindowRect.iHeightTotal;


	// finally we will offset the x and y values for the glViewPort
	// in order to simulate scrolling:
	x=x-g_HlaeWindowRect.iLeft;
	y=y-g_HlaeWindowRect.iHeightTotal+g_HlaeWindowRect.iHeightVisible+g_HlaeWindowRect.iTop;

}

BOOL APIENTRY HlaeBcCl_GetCursorPos(LPPOINT lpPoint)
{
	BOOL bRet = GetCursorPos(lpPoint);
	if(!g_hHlaeServerGL) return bRet;

	POINT dp = *lpPoint;

	// translate mouse into H-L's coords:
	lpPoint->x=g_HL_MainWindow_info.iX + g_HL_MainWindow_info.MouseTarget.iX;
	lpPoint->y=g_HL_MainWindow_info.iY + g_HL_MainWindow_info.MouseTarget.iY;

	pEngfuncs->Con_Printf("Mouse (%i%i,) -> (%i,%i)\n",dp.x,dp.y,lpPoint->x,lpPoint->y);

	return bRet;
}

HWND WINAPI HlaeBcClt_SetCapture( HWND hWnd)
{
	if(!g_hHlaeServerGL)
		return SetCapture(hWnd);

	return NULL;
}

BOOL WINAPI HlaeBcClt_ReleaseCapture( VOID )
{
	if(!g_hHlaeServerGL)
		return ReleaseCapture();

	return TRUE;
}

BOOL WINAPI HlaeBcClt_SetPixelFormat(  HDC  hdc,  int  iPixelFormat, CONST PIXELFORMATDESCRIPTOR *  ppfd )
{
	// copy data for further use:
	g_HL_MainWindow_info.iPixelFormat = iPixelFormat;
	g_HL_MainWindow_info.pfd = *ppfd;
	
	return SetPixelFormat(hdc, iPixelFormat, ppfd);
}

HDC g_temp_serverDC=NULL;

HGLRC WINAPI HlaeBcClt_wglCreateContext(HDC  hdc)
{
	// return if it's not the window (DC) we want:
	if (!(g_HL_MainWindow_info.hDC) || hdc != g_HL_MainWindow_info.hDC)
		return wglCreateContext(hdc);

	int iSavedDC;

	HGLRC hGLRC = HlaeBc_AquireGlWindow(g_HL_MainWindow_info.nWidth,g_HL_MainWindow_info.nHeight,g_HL_MainWindow_info.iPixelFormat,&(g_HL_MainWindow_info.pfd),&g_hHlaeServerWND,&iSavedDC);

	if (hGLRC)
	{
		GetLastError();
		HDC thdc=GetDC(g_hHlaeServerWND);

		RestoreDC(thdc,iSavedDC);

		char ppp[200];
		ppp[sizeof(ppp)-1]=0;
		_snprintf(ppp,sizeof(ppp)-1,"GetLastError(): %i\nHWND: 0x%08x",GetLastError(),g_hHlaeServerWND);
		MessageBoxA(g_hHlaeServerWND,ppp,"HlaeBcClt_wglCreateContext",MB_OK|MB_ICONERROR);
		_snprintf(ppp,sizeof(ppp)-1,"Format 0%08x",GetPixelFormat(thdc));
		MessageBoxA(g_hHlaeServerWND,ppp,"HlaeBcClt_wglCreateContext",MB_OK|MB_ICONERROR);


	/*	LineTo(thdc,1000,1000);
		
		g_hHlaeServerGL = wglCreateContext(thdc);
		hGLRC = g_hHlaeServerGL;
		if (!hGLRC)
		{
			char ptemp[200];
			ptemp[sizeof(ptemp)-1]=0;
			_snprintf(ptemp,sizeof(ptemp)-1,"wglCreateContext failed:\nGetLastError(): %i\nServerHWND: 0x%08x\nServerHGLRC: 0x%08x",GetLastError(),g_hHlaeServerWND,g_hHlaeServerGL);
			MessageBoxA(0,ptemp,"HlaeBcClt_wglCreateContext",MB_OK|MB_ICONERROR);
		}*/

		ReleaseDC(g_hHlaeServerWND,thdc);
	}
	else
	{
		hGLRC = wglCreateContext(hdc);
	}

	return hGLRC;
}

BOOL WINAPI HlaeBcClt_wglMakeCurrent( HDC  hdc, HGLRC  hglrc )
{
	if (!(g_HL_MainWindow_info.hDC) || hdc != g_HL_MainWindow_info.hDC)
		return wglMakeCurrent(hdc,hglrc);

	HDC thdc=GetDC(g_hHlaeServerWND);

	BOOL bRet=wglMakeCurrent(thdc,g_hHlaeServerGL);
	if (!bRet)
	{
		char ptemp[200];
		ptemp[sizeof(ptemp)-1]=0;
		_snprintf(ptemp,sizeof(ptemp)-1,"wglMakeCurrent failed:\nGetLastError(): %i\nGetPixelFormat(): %i\nServerHWND: 0x%08x\nServerHGLRC: 0x%08x",GetLastError(),GetPixelFormat(thdc),g_hHlaeServerWND,g_hHlaeServerGL);
		MessageBoxA(0,ptemp,"HlaeBcClt_wglMakeCurrent",MB_OK|MB_ICONERROR);
	}

	ReleaseDC(g_hHlaeServerWND,thdc);

	return bRet;
}

BOOL WINAPI HlaeBcClt_wglDeleteContext( HGLRC  hglrc )
{
	if(!g_hHlaeServerGL || hglrc!=g_hHlaeServerGL )
		return wglDeleteContext(hglrc);

	if (g_temp_serverDC)
	{
		ReleaseDC(g_hHlaeServerWND,g_temp_serverDC);
	}

	return HlaeBc_ReleaseGlWindow();
}

//
// debug helper:
//

#if 1
//#ifdef MDT_DEBUG

REGISTER_DEBUGCMD_FUNC(info_devicecontext)
{
	if (!g_hHlaeServerGL)
	{
		HDC hdcResult;
	
		hdcResult=GetDC(g_HL_MainWindow);

		PIXELFORMATDESCRIPTOR *pPfd = new PIXELFORMATDESCRIPTOR;

		char sztemp[2000];
		sztemp[sizeof(sztemp)-1]=0;

		if (DescribePixelFormat( hdcResult, GetPixelFormat(hdcResult),sizeof(PIXELFORMATDESCRIPTOR),pPfd))
		{

			_snprintf(sztemp,sizeof(sztemp),"DescribePixelFormat says:\nnSize:%u\nnVersion:%u\ndwFlags:0x%08x\niPixelType:%i\ncColorBits:%u\ncRedBits:%u\ncRedShift:%u\ncGreenBits:%u\ncGreenShift:%u\ncBlueBits:%u\ncBlueShift:%u\ncAlphaBits:%u\ncAlphaShift:%u\ncAccumBits:%u\ncAccumRedBits:%u\ncAccumGreenBits:%u\ncAccumBlueBits:%u\ncAccumAlphaBits:%u\ncDepthBits:%u\ncStencilBits:%u\ncAuxBuffers:%u\niLayerType:%i\nbReserved:%u\ndwLayerMask:%u\ndwVisibleMask:%u\ndwDamageMask:%u",
				pPfd->nSize, pPfd->nVersion, pPfd->dwFlags, pPfd->iPixelType,
				pPfd->cColorBits, pPfd->cRedBits, pPfd->cRedShift, pPfd->cGreenBits,
				pPfd->cGreenShift, pPfd->cBlueBits, pPfd->cBlueShift, pPfd->cAlphaBits,
				pPfd->cAlphaShift, pPfd->cAccumBits, pPfd->cAccumRedBits, pPfd->cAccumGreenBits,
				pPfd->cAccumBlueBits, pPfd->cAccumAlphaBits, pPfd->cDepthBits, pPfd->cStencilBits,
				pPfd->cAuxBuffers, pPfd->iLayerType, pPfd->bReserved, pPfd->dwLayerMask,
				pPfd->dwVisibleMask, pPfd->dwDamageMask);

			MessageBoxA(NULL,sztemp,"Client - info_pxformat",MB_OK|MB_ICONINFORMATION);

		} else 	MessageBoxA(NULL,"DescribePixelFormat failed","Client - info_pxformat",MB_OK|MB_ICONERROR);

		delete pPfd;

		ReleaseDC(g_HL_MainWindow,hdcResult);
	} else pEngfuncs->Con_Printf("HLAE Server GL context is used.\nInfo not available in this case!\n");
}
#endif


// I'll leave this old function source in here for now (may be for historical reasons, don't know):
// may be leave it in, might be a target for remembering how to do some tricky stuff by design
// (it also shows how to piggy back strings and stuff)
/*
HWND Old_HlaeBcClt_CreateWindowExA(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam)
{
	HWND hwRetWin=NULL;

	HLAE_BASECOM_RET_CreateWindowExA_s *mycwret = new HLAE_BASECOM_RET_CreateWindowExA_s;
	HLAE_BASECOM_CreateWindowExA_s *mycws;

	size_t cbBase=sizeof(HLAE_BASECOM_CreateWindowExA_s);
	size_t cbClassName=HIWORD(lpClassName) ? strlen(lpClassName)+1 : 0;
	size_t cbWindowName=HIWORD(lpWindowName) ? strlen(lpWindowName)+1 : 0;
	size_t cbPiggyBack=cbClassName+cbWindowName;

	mycws=(HLAE_BASECOM_CreateWindowExA_s *)malloc(cbBase+cbPiggyBack);

	mycws->dwExStyle = dwExStyle;

	if(cbClassName>0)
	{
		mycws->lpClassName = (LPCTSTR)cbBase; memcpy((char *)mycws + cbBase,lpClassName,cbClassName);
	} else
		mycws->lpClassName = NULL;

	if (cbWindowName>0)
	{
		mycws->lpWindowName = (LPCTSTR)(cbBase + cbClassName); memcpy((char *)mycws + cbBase + cbClassName,lpWindowName,cbWindowName);
	} else
		mycws->lpWindowName = NULL;

	mycws->dwStyle = dwStyle;
	mycws->x = x;
	mycws->y = y ;
	mycws->nWidth = nWidth;
	mycws->nHeight = nHeight;
	mycws->hWndParent = hWndParent;
	mycws->hMenu = hMenu;
	mycws->hInstance = hInstance;
	mycws->lpParam = lpParam;

	if(HlaeBcCltSendMessageRet(HLAE_BASECOM_MSGSV_CreateWindowExA,cbBase+cbPiggyBack,(PVOID)mycws,mycwret))
		hwRetWin = mycwret->retResult;

	free(mycws);
	delete mycwret;

	if (hwRetWin)
		return hwRetWin;
	else
		// s.th. failed, create window as desired within engine:
		return CreateWindowExA(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);

}
*/
