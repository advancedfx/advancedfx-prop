#include <windows.h>

#include <stdio.h>

// BEGIN HLSDK includes
//
// HACK: prevent cldll_int.h from messing the HSPRITE definition,
// HLSDK's HSPRITE --> MDTHACKED_HSPRITE
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <hlsdk/multiplayer/cl_dll/wrect.h>
#include <hlsdk/multiplayer/cl_dll/cl_dll.h>
#include <hlsdk/multiplayer/engine/cdll_int.h>
#include <hlsdk/multiplayer/common/cvardef.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes
	
#include "cmdregister.h"

extern cl_enginefuncs_s* pEngfuncs;

#include "supportrender.h"

#include <shared/com/basecom.h>

#include "basecomClient.h"

extern CHlaeSupportRender *g_pSupportRender;

// forward definition:
HGLRC Init_Support_Renderer(HWND hMainWindow, HDC hMainWindowDC, int iWidth, int iHeight);


HWND g_hwHlaeBcCltWindow = NULL; // Hlae BaseCom client reciever window

HWND  g_hHlaeServerWND=NULL; // handle to the sever's Game Window Parent

HWND g_HL_MainWindow = NULL; // created by create window
WNDCLASSA *g_HL_WndClassA = NULL; // original class that H-L wanted to register

struct
{
	int iX;
	int iY;
	int nWidth;
	int nHeight;

	HDC hDc;

	struct {
		// some values are cached so they last longer than the render target:
		CHlaeSupportRender::ERenderTarget oldTarget;
		HDC oldOwnDC;
	} SupportRenderInfo;
	
	struct {
		int iX;
		int iY;
	} MouseTarget;

	bool bUndockOnFilming;
} g_HL_MainWindow_info;

bool g_bIsUndocked = false; // undocked i.e. due to filming in Undock capture mode?

bool g_bHoldingActivate = false; // activation event that still needs to be delivered after the window is docked again
bool g_bHoldedActivate; // last value on hold

bool g_bFullScreenCheatMode = false;

bool g_bIsActive = false;

struct
{
	unsigned int ui_rx_packets;
	unsigned int ui_tx_packets;

} g_message_stats = {0, 0};

REGISTER_DEBUGCVAR(debug_blockkeys, "0", 0);

LRESULT CALLBACK Hooking_WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	//
	// one could filter window unspecific messages here
	//

	if (hWnd==NULL || hWnd != g_HL_MainWindow)
		// this is not the MainWindow we want to control
		return g_HL_WndClassA->lpfnWndProc(hWnd,uMsg,wParam,lParam);

	// filter MainWindow specific messages:

	// blah
	switch (uMsg)
	{
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
			g_bIsActive = false;
		break;
	case WM_MOVE:
		g_HL_MainWindow_info.MouseTarget.iX = (int)(short) LOWORD(lParam); 
		g_HL_MainWindow_info.MouseTarget.iY = (int)(short) HIWORD(lParam);
		break;
	case WM_MOUSEACTIVATE:
		pEngfuncs->Con_DPrintf("WM_MOUSEACTIAVTE\n");
		if( !g_bFullScreenCheatMode && !g_bIsUndocked && !g_bIsActive)
		{
			// Client Windows won't recieve window activation events, so we will fake them
			g_bIsActive = true;
			LRESULT lr = g_HL_WndClassA->lpfnWndProc(hWnd,uMsg,wParam,lParam); // pass on WM_MOUSEACTIVATE
			g_HL_WndClassA->lpfnWndProc(g_HL_MainWindow,WM_ACTIVATEAPP, TRUE, NULL);
			g_HL_WndClassA->lpfnWndProc(g_HL_MainWindow,WM_ACTIVATE, WA_ACTIVE, NULL);//lParam);
			// Don't let strange mods like Natural Selection mess with us:
			ShowCursor(TRUE);
			return lr;
		}
		// Don't let strange mods like Natural Selection mess with us:
		ShowCursor(TRUE);
		break;
	case WM_SETFOCUS:
		pEngfuncs->Con_DPrintf("WM_SETFOCUS\n");
		break;
	case WM_KILLFOCUS:
		pEngfuncs->Con_DPrintf("WM_KILLFOCUS\n");
		if( !g_bFullScreenCheatMode && !g_bIsUndocked && g_bIsActive)
		{
			g_bIsActive = false;
			g_HL_WndClassA->lpfnWndProc(g_HL_MainWindow,WM_ACTIVATE, WA_INACTIVE, NULL);//lParam);
			g_HL_WndClassA->lpfnWndProc(g_HL_MainWindow,WM_ACTIVATEAPP, FALSE, NULL);
			return g_HL_WndClassA->lpfnWndProc(hWnd,uMsg,wParam,lParam); // PASS ON WM_KILLFOCUS
		}
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		if( debug_blockkeys->value != 0 ) return NULL;
	}

	return g_HL_WndClassA->lpfnWndProc(hWnd,uMsg,wParam,lParam);
}

//
// Handlers for events from the Hlae GUI server:
//

/*
LRESULT HlaeBcCl_OnServerActivate(PVOID lpData)
{
	HLAE_BASECOM_OnServerActivate_s *myps = (HLAE_BASECOM_OnServerActivate_s *)lpData;

	g_bHoldedActivate = myps->bActive;

	if (g_bIsUndocked)
	{
		// cannot deliver now, put last message on hold:
		g_bHoldingActivate = true;
		pEngfuncs->Con_DPrintf("HlaeBcCl_OnServerActivate: %i (on hold)\n",myps->bActive);
		return TRUE;
	}
	else
		pEngfuncs->Con_DPrintf("HlaeBcCl_OnServerActivate: %i\n",myps->bActive);

	if(g_bHoldedActivate)
	{
		SendMessage(g_HL_MainWindow,WM_ACTIVATEAPP, TRUE ,NULL);
		SendMessage(g_HL_MainWindow,WM_ACTIVATE, WA_ACTIVE ,NULL);
	} else {
		SendMessage(g_HL_MainWindow,WM_ACTIVATE, WA_INACTIVE ,NULL);
		SendMessage(g_HL_MainWindow,WM_ACTIVATEAPP, FALSE ,NULL);
	}

	return TRUE;
}

LRESULT HlaeBcCl_OnGameWindowFocus(PVOID lpData)
{
	HLAE_BASECOM_OnGameWindowFocus_s *myps = (HLAE_BASECOM_OnGameWindowFocus_s *)lpData;

	pEngfuncs->Con_DPrintf("External force focus request ...\n");

	return SetFocus(g_HL_MainWindow) ? TRUE : FALSE;
}
*/

LRESULT HlaeBcCl_OnServerClose(PVOID lpData)
{
	HLAE_BASECOM_OnServerClose_s *myps = (HLAE_BASECOM_OnServerClose_s *)lpData;

	return SendMessage(g_HL_MainWindow,WM_CLOSE,NULL,NULL);
}

/*
LRESULT HlaeBcCl_MouseEvent(PVOID lpData)
{
	HLAE_BASECOM_MSGCL_MouseEvent_s *myps = (HLAE_BASECOM_MSGCL_MouseEvent_s *)lpData;

	g_HL_MainWindow_info.MouseTarget.iX = myps->iX;
	g_HL_MainWindow_info.MouseTarget.iY = myps->iY;

	pEngfuncs->Con_Printf("HlaeBcCl_MouseEvent: (%i,%i)\n",myps->iX,myps->iY);

	return g_HL_WndClassA->lpfnWndProc(g_HL_MainWindow,(UINT)(myps->uMsg),(WPARAM)(myps->wParam),(LPARAM)(((myps->iY) << 16) + myps->iX));
}

// not used anymore:
LRESULT HlaeBcCl_KeyBoardEvent(PVOID lpData)
{
	HLAE_BASECOM_MSGCL_KeyBoardEvent_s *myps = (HLAE_BASECOM_MSGCL_KeyBoardEvent_s *)lpData;

	pEngfuncs->Con_Printf("HlaeBcCl_KeyEvent: %i %i 0x%08x\n",myps->uMsg,myps->uKeyCode,myps->uKeyFlags);

	return g_HL_WndClassA->lpfnWndProc(g_HL_MainWindow,(UINT)(myps->uMsg),(WPARAM)(myps->uKeyCode),(LPARAM)(myps->uKeyFlags));

}
*/


// In order to understand the source that follows the Microsoft Documentation of WM_COPYDATA might be useful.

void *g_pHlaeBcResultTarget=NULL;

LRESULT CALLBACK HlaeBcCltWndProc(
    HWND hwnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{
	g_message_stats.ui_rx_packets++;

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

			case HLAE_BASECOM_RETCL_OnCreateWindow:
				if (!g_pHlaeBcResultTarget) return FALSE;
				memcpy(g_pHlaeBcResultTarget,pMyCDS->lpData,sizeof(HLAE_BASECOM_RET_OnCreateWindow_s));
				return TRUE;

			case HLAE_BASECOM_MSGCL_OnServerClose:
				return HlaeBcCl_OnServerClose(pMyCDS->lpData);

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
	g_message_stats.ui_tx_packets++;
	
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

HWND HlaeBC_OnCreateWindow(int nWidth, int nHeight)
// informs the server (if present) that we started and are about to create the game window
// returns the handle of the window we shall use as parent on success, NULL otherwise.
{
	HWND retResult=NULL;

	HLAE_BASECOM_RET_OnCreateWindow_s *mycwret = new HLAE_BASECOM_RET_OnCreateWindow_s;
	HLAE_BASECOM_OnCreateWindow_s *mycws = new HLAE_BASECOM_OnCreateWindow_s;

	mycws->nWidth		= nWidth;
	mycws->nHeight		= nHeight;

	if(HlaeBcCltSendMessageRet(HLAE_BASECOM_QRYSV_OnCreateWindow,sizeof(HLAE_BASECOM_OnCreateWindow_s),(PVOID)mycws,mycwret))
	{
		retResult = mycwret->parentWindow;
	}

	delete mycws;
	delete mycwret;

	return retResult;
}

bool HlaeBc_OnDestroyWindow()
// informs the server that we are about to shut down
// returns false on fail, true otherwise
{
	bool retResult = false;

	HLAE_BASECOM_OnDestroyWindow_s *mycws = new HLAE_BASECOM_OnDestroyWindow_s;

	if(HlaeBcCltSendMessage(HLAE_BASECOM_MSGSV_OnDestroyWindow,sizeof(HLAE_BASECOM_OnDestroyWindow_s),(PVOID)mycws))
	{
		retResult = true;
	}

	delete mycws;
	return retResult;
}

struct {
	DWORD dwStyle;
	HWND hwParent;
	bool bUndocked;
} g_HlaeOnFilming_r = { 0, 0, false };

bool HlaeBc_OnFilmingStart()
// returns false on fail, true otherwise
{
	bool retResult = false;

	HLAE_BASECOM_OnFilmingStart_s *mycws = new HLAE_BASECOM_OnFilmingStart_s;

	if(HlaeBcCltSendMessage(HLAE_BASECOM_MSGSV_OnFilmingStart,sizeof(HLAE_BASECOM_OnFilmingStart_s),(PVOID)mycws))
	{
		retResult = true;
	}

	delete mycws;

	if (g_HL_MainWindow_info.bUndockOnFilming)
	{
		g_bIsUndocked = true;
		g_HlaeOnFilming_r.bUndocked = true;

		// get old style and parent:
		g_HlaeOnFilming_r.dwStyle = GetWindowLongPtr( g_HL_MainWindow, GWL_STYLE );
		g_HlaeOnFilming_r.hwParent = GetParent( g_HL_MainWindow );

		// set new parent and style (see SetParent() on MSDN2, why we do it in this order):
		SetParent( g_HL_MainWindow, NULL );
		SetWindowLongPtr( g_HL_MainWindow, GWL_STYLE, WS_POPUP );
		SetWindowPos( g_HL_MainWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_SHOWWINDOW);
	}

	return retResult;
}

bool HlaeBc_OnFilmingStop()
// returns false on fail, true otherwise
{
	bool retResult = false;

	HLAE_BASECOM_OnFilmingStop_s *mycws = new HLAE_BASECOM_OnFilmingStop_s;

	if(HlaeBcCltSendMessage(HLAE_BASECOM_MSGSV_OnFilmingStop,sizeof(HLAE_BASECOM_OnFilmingStop_s),(PVOID)mycws))
	{
		retResult = true;
	}

	delete mycws;

	if (g_HlaeOnFilming_r.bUndocked)
	{
		// restore old style and parent (see SetParent() on MSDN2, why we do it in this order):
		SetWindowLongPtr( g_HL_MainWindow, GWL_STYLE, g_HlaeOnFilming_r.dwStyle ); // WS_CHILD has to be set first
		SetParent( g_HL_MainWindow, g_HlaeOnFilming_r.hwParent );
		SetWindowPos( g_HL_MainWindow, HWND_TOP, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_SHOWWINDOW);

		g_bIsUndocked = false;
		g_HlaeOnFilming_r.bUndocked = false;
		if (g_bHoldingActivate)
		{
			// deliver last activate in case it's on hold:
			g_bHoldingActivate = false;
			SendMessage(g_HL_MainWindow,WM_ACTIVATE,g_bHoldedActivate ? WA_ACTIVE : WA_INACTIVE ,NULL);
		}
	}

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

	// check for fullscreen cheatcode:
	if( pEngfuncs->CheckParm("-full",NULL) )
	{
		g_bFullScreenCheatMode = true;
		return RegisterClassA( lpWndClass );
	}

	ATOM tResult = NULL;
	WNDCLASSA *tWndClass= new WNDCLASSA;

	if(!g_HL_WndClassA) g_HL_WndClassA = new WNDCLASSA;

	memcpy(g_HL_WndClassA,lpWndClass,sizeof(WNDCLASSA)); // get us a global copy of the original class, we will need it to access the original lpfnWindowProc i.e.
	memcpy(tWndClass,lpWndClass,sizeof(WNDCLASSA)); // we want to register a modified copy

	// modifiy the copy to fit our needs:

	tWndClass->lpfnWndProc = (WNDPROC)Hooking_WndProc; // we want to hook the WindowProc, so we can control the message flow

	tWndClass->style = CS_OWNDC;
	//tWndClass->hCursor = NULL;

	// register our modified copy of the class instead:
	tResult = RegisterClassA(tWndClass);
	delete tWndClass;

	if (!tResult)
	{
		char ppp[200];
		ppp[sizeof(ppp)-1]=0;
		_snprintf(ppp,sizeof(ppp)-1,"GetLastError(): %i",GetLastError());
		MessageBoxA(g_hHlaeServerWND,ppp,"HlaeBcClt_RegisterClassA",MB_OK|MB_ICONERROR);
	}


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

	if( g_bFullScreenCheatMode )
		return CreateWindowExA( dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam );

	// quit if it's not the window we want:
	if (hWndParent!=NULL)
		return CreateWindowExA(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);

	// save postion:
	g_HL_MainWindow_info.iX = x;
	g_HL_MainWindow_info.iY = y;

	// and size:
	g_HL_MainWindow_info.nWidth = nWidth;
	g_HL_MainWindow_info.nHeight = nHeight;

	// and inform server:
	HWND useParent = HlaeBC_OnCreateWindow(nWidth,nHeight);
	g_hHlaeServerWND = useParent;

	// modifiy some properities to our needs:
	dwStyle = WS_CHILD; // | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
	dwExStyle = WS_EX_NOPARENTNOTIFY;
	hWndParent = useParent;
	x = 0;
	y = 0;

	// place the call

	g_HL_MainWindow = CreateWindowExA(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);

	if (!g_HL_MainWindow)
	{
		char ppp[200];
		ppp[sizeof(ppp)-1]=0;
		_snprintf(ppp,sizeof(ppp)-1,"GetLastError(): %i\nHWND: 0x%08x",GetLastError(),useParent);
		MessageBoxA(g_hHlaeServerWND,ppp,"HlaeBcClt_CreateWindowExA",MB_OK|MB_ICONERROR);
	}

	return g_HL_MainWindow;

}
#include <gl/gl.h>
BOOL APIENTRY HlaeBcClt_DestroyWindow(HWND hWnd)
{
	if( g_bFullScreenCheatMode )
		return DestroyWindow( hWnd );

	if (hWnd!=NULL && hWnd == g_HL_MainWindow)
	{
		// H-L main game window being destroyed

		// inform GUI server:
		HlaeBc_OnDestroyWindow();

		// halt client server:
		HlaeBcCltStop();

		// clean up globals:
		g_HL_MainWindow = NULL;
		if(g_HL_WndClassA) delete g_HL_WndClassA; // I asume we won't use it anymore, this might be wrong in some rare cases.

		if (g_pSupportRender) delete g_pSupportRender;
	}

	return DestroyWindow(hWnd);
}

BOOL WINAPI HlaeBcClt_SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	if( g_bFullScreenCheatMode )
		return SetWindowPos( hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags );

	if (!g_hHlaeServerWND || hWnd==NULL || hWnd != g_HL_MainWindow ) return SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);

	if ( !(uFlags & SWP_NOSIZE) && (cx != g_HL_MainWindow_info.nWidth || cy != g_HL_MainWindow_info.nHeight ) )
	{
		// contains size info
		pEngfuncs->Con_DPrintf("Updating window size: %ix%i",cx,cy);
		g_HL_MainWindow_info.nWidth=cx;
		g_HL_MainWindow_info.nHeight=cy;
		HlaeBc_UpdateWindow(cx,cy);
	}
	return SetWindowPos(hWnd,HWND_TOP,0,0,cx,cy,uFlags);//SWP_SHOWWINDOW);
}

HWND WINAPI HlaeBcClt_SetCapture( HWND hWnd)
{
//	if(!g_hHlaeServerGL)
		return SetCapture(hWnd);

	return NULL;
}

BOOL WINAPI HlaeBcClt_ReleaseCapture( VOID )
{
//	if(!g_hHlaeServerGL)
		return ReleaseCapture();

	return TRUE;
}

int WINAPI HlaeBcClt_ChoosePixelFormat( HDC hdc, CONST PIXELFORMATDESCRIPTOR *ppfd)
{

#ifdef MDT_DEBUG
	MessageBoxA(NULL,"HlaeBcClt_ChoosePixelFormat","MDT_DEBUG",MB_OK|MB_ICONINFORMATION);
#endif

	int iRet;

	PIXELFORMATDESCRIPTOR *myppfd;
	memcpy(&myppfd,&ppfd,sizeof(PIXELFORMATDESCRIPTOR *)); // we intentionally void the const paradigm here

	if (pEngfuncs->CheckParm("-mdtalpha8", NULL ))
	{
		// we intentionally void the const paradigm here:
		myppfd->cAlphaBits = 8; // request alpha bit planes (generic implementation doesn't support that)
		myppfd->cAlphaShift = 24;
	}

	iRet= ChoosePixelFormat(hdc,myppfd);

	return iRet;
}

HGLRC WINAPI HlaeBcClt_wglCreateContext(HDC hDc)
{

#ifdef MDT_DEBUG
	MessageBoxA(NULL,"HlaeBcClt_wglCreateContext","MDT_DEBUG",MB_OK|MB_ICONINFORMATION);
#endif

	g_HL_MainWindow_info.hDc = hDc;
	HGLRC tHGLRC = Init_Support_Renderer( g_HL_MainWindow, g_HL_MainWindow_info.hDc, g_HL_MainWindow_info.nWidth, g_HL_MainWindow_info.nHeight );

	if (tHGLRC && g_pSupportRender)
	{
		g_HL_MainWindow_info.SupportRenderInfo.oldTarget = g_pSupportRender->GetRenderTarget();
		if (CHlaeSupportRender::RT_MEMORYDC == g_HL_MainWindow_info.SupportRenderInfo.oldTarget)
			g_HL_MainWindow_info.SupportRenderInfo.oldOwnDC = g_pSupportRender->GetInternalHDC();
		else
			g_HL_MainWindow_info.SupportRenderInfo.oldOwnDC = NULL;
	}

	return tHGLRC;
}

BOOL WINAPI HlaeBcClt_wglMakeCurrent(HDC hDc, HGLRC hGlRc)
{
	if (hGlRc && g_pSupportRender && g_pSupportRender->GetHGLRC() == hGlRc)
		return g_pSupportRender->hlaeMakeCurrent(hDc, hGlRc);

	BOOL bRet = wglMakeCurrent(hDc, hGlRc);

	return bRet;

}

BOOL WINAPI HlaeBcClt_wglDeleteContext(HGLRC hGlRc)
{
	if (hGlRc && g_pSupportRender && g_pSupportRender->GetHGLRC() == hGlRc)
		return g_pSupportRender->hlaeDeleteContext(hGlRc);

	return wglDeleteContext(hGlRc);
}

int WINAPI HlaeBcClt_ReleaseDC( HWND hWnd, HDC hDc)
{
	return ReleaseDC(hWnd, hDc);
}

//
// support functions:
//

HGLRC Init_Support_Renderer(HWND hMainWindow, HDC hMainWindowDC, int iWidth, int iHeight)
{
	if(g_pSupportRender)
		return NULL; // already created
	
	// determine desired target renderer:
	CHlaeSupportRender::ERenderTarget eRenderTarget = CHlaeSupportRender::RT_GAMEWINDOW;

	g_HL_MainWindow_info.bUndockOnFilming = false;
	if ( !g_bFullScreenCheatMode && pEngfuncs->CheckParm("-mdtoptvis", NULL ) )
	{
		g_HL_MainWindow_info.bUndockOnFilming = true;
	}

	char *pStart=NULL;

	if (pEngfuncs->CheckParm("-mdtrender", &pStart ))
	{
		if (!lstrcmp(pStart,"memdc"))
		{
			pEngfuncs->Con_DPrintf("RenderTarget: user wants RT_MEMORYDC;\n");
			eRenderTarget = CHlaeSupportRender::RT_MEMORYDC;
		} else if (!lstrcmp(pStart,"fbo"))
		{
			pEngfuncs->Con_DPrintf("RenderTarget: user wants RT_FRAMEBUFFEROBJECT\n");
			eRenderTarget = CHlaeSupportRender::RT_FRAMEBUFFEROBJECT;
		}
	}
	if (eRenderTarget == CHlaeSupportRender::RT_GAMEWINDOW)
		pEngfuncs->Con_DPrintf("RenderTarget: using default RT_GAMEWINDOW\n");

	// Init support renderer:
	g_pSupportRender = new CHlaeSupportRender(hMainWindow, iWidth, iHeight);

	HGLRC tHGLRC;
	tHGLRC = g_pSupportRender->hlaeCreateContext(eRenderTarget,hMainWindowDC);

	if (!tHGLRC)
		MessageBoxA(0,"hlaeCreateContext failed.","Init_Support_Renderer ERROR",MB_OK|MB_ICONERROR);

	return tHGLRC;
}

HWND HlaeBc_GetGameWindow(void)
{
	return g_HL_MainWindow;
}

//
// Debug info
//

REGISTER_DEBUGCMD_FUNC(debug_devicecontext)
{
	HDC hdcResult;

	hdcResult=GetDC(g_HL_MainWindow);

	PIXELFORMATDESCRIPTOR *pPfd = new PIXELFORMATDESCRIPTOR;

	if (DescribePixelFormat( hdcResult, GetPixelFormat(hdcResult),sizeof(PIXELFORMATDESCRIPTOR),pPfd))
	{

		pEngfuncs->Con_Printf("DescribePixelFormat says:\nnSize:%u\nnVersion:%u\ndwFlags:0x%08x\niPixelType:%i\ncColorBits:%u\ncRedBits:%u\ncRedShift:%u\ncGreenBits:%u\ncGreenShift:%u\ncBlueBits:%u\ncBlueShift:%u\ncAlphaBits:%u\ncAlphaShift:%u\ncAccumBits:%u\ncAccumRedBits:%u\ncAccumGreenBits:%u\ncAccumBlueBits:%u\ncAccumAlphaBits:%u\ncDepthBits:%u\ncStencilBits:%u\ncAuxBuffers:%u\niLayerType:%i\nbReserved:%u\ndwLayerMask:%u\ndwVisibleMask:%u\ndwDamageMask:%u",
			pPfd->nSize, pPfd->nVersion, pPfd->dwFlags, pPfd->iPixelType,
			pPfd->cColorBits, pPfd->cRedBits, pPfd->cRedShift, pPfd->cGreenBits,
			pPfd->cGreenShift, pPfd->cBlueBits, pPfd->cBlueShift, pPfd->cAlphaBits,
			pPfd->cAlphaShift, pPfd->cAccumBits, pPfd->cAccumRedBits, pPfd->cAccumGreenBits,
			pPfd->cAccumBlueBits, pPfd->cAccumAlphaBits, pPfd->cDepthBits, pPfd->cStencilBits,
			pPfd->cAuxBuffers, pPfd->iLayerType, pPfd->bReserved, pPfd->dwLayerMask,
			pPfd->dwVisibleMask, pPfd->dwDamageMask);

	} else pEngfuncs->Con_Printf("DescribePixelFormat failed");

	delete pPfd;

	ReleaseDC(g_HL_MainWindow,hdcResult);
}

REGISTER_DEBUGCMD_FUNC(debug_message_stats)
{
	pEngfuncs->Con_Printf("HLAE BaseCom Client (includes failed):\ntx packets: %u\nrx packets: %u\n",g_message_stats.ui_tx_packets,g_message_stats.ui_rx_packets);
}

REGISTER_DEBUGCMD_FUNC(debug_wndproc_isontop)
{
	if( Hooking_WndProc != (WNDPROC)GetWindowLong( (HWND)g_HL_MainWindow, GWL_WNDPROC ))
		pEngfuncs->Con_Printf("S.th. hooked us!\n");
	else
		pEngfuncs->Con_Printf("We seem to be at top.\n");
}

REGISTER_DEBUGCMD_FUNC(debug_wndproc_rehook)
{
	SetWindowLongPtr( (HWND)g_HL_MainWindow, GWL_WNDPROC, (LONG)Hooking_WndProc );
}

REGISTER_DEBUGCMD_FUNC(debug_getcommandline)
{
	pEngfuncs->Con_Printf("GetCommandLine(): %s\n",GetCommandLine());
}

REGISTER_DEBUGCMD_FUNC(debug_activateapp_on)
{
	SendMessage(g_HL_MainWindow,WM_ACTIVATEAPP, TRUE ,NULL);
	SendMessage(g_HL_MainWindow,WM_ACTIVATE, WA_ACTIVE ,NULL);
}

REGISTER_DEBUGCMD_FUNC(debug_activateapp_off)
{
	SendMessage(g_HL_MainWindow,WM_ACTIVATE, WA_INACTIVE ,NULL);
	SendMessage(g_HL_MainWindow,WM_ACTIVATEAPP, FALSE ,NULL);
}