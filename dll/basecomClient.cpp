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

HWND  g_hHlaeServerWND=NULL; // handle to the sever's Game Window Parent

HWND g_HL_MainWindow = NULL; // created by create window
WNDCLASSA *g_HL_WndClassA = NULL; // original class that H-L wanted to register

struct
{
	int iX;
	int iY;
	int nWidth;
	int nHeight;
	
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
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		// if the user clicks on us we want the friggin focus:
		SetFocus(g_HL_MainWindow);
		break;
	}

	return g_HL_WndClassA->lpfnWndProc(hWnd,uMsg,wParam,lParam);
}

//
// Handlers for events from the Hlae GUI server:
//

LRESULT HlaeBcCl_OnGameWindowFocus(PVOID lpData)
{
	HLAE_BASECOM_OnGameWindowFocus_s *myps = (HLAE_BASECOM_OnGameWindowFocus_s *)lpData;

	pEngfuncs->Con_DPrintf("External force focus request ...\n");

	return SetFocus(g_HL_MainWindow) ? TRUE : FALSE;
}

LRESULT HlaeBcCl_OnServerClose(PVOID lpData)
{
	HLAE_BASECOM_OnServerClose_s *myps = (HLAE_BASECOM_OnServerClose_s *)lpData;

	return SendMessage(g_HL_MainWindow,WM_CLOSE,NULL,NULL);
}

LRESULT HlaeBcCl_MouseEvent(PVOID lpData)
{
	HLAE_BASECOM_MSGCL_MouseEvent_s *myps = (HLAE_BASECOM_MSGCL_MouseEvent_s *)lpData;

	g_HL_MainWindow_info.MouseTarget.iX = myps->iX;
	g_HL_MainWindow_info.MouseTarget.iY = myps->iY;

	pEngfuncs->Con_Printf("HlaeBcCl_MouseEvent: (%i,%i)\n",myps->iX,myps->iY);

	return g_HL_WndClassA->lpfnWndProc(g_HL_MainWindow,(UINT)(myps->uMsg),(WPARAM)(myps->wParam),(LPARAM)(((myps->iY) << 16) + myps->iX));
}

LRESULT HlaeBcCl_KeyBoardEvent(PVOID lpData)
{
	HLAE_BASECOM_MSGCL_KeyBoardEvent_s *myps = (HLAE_BASECOM_MSGCL_KeyBoardEvent_s *)lpData;

	pEngfuncs->Con_Printf("HlaeBcCl_KeyEvent: %i %i 0x%08x\n",myps->uMsg,myps->uKeyCode,myps->uKeyFlags);

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

			case HLAE_BASECOM_RETCL_OnCreateWindow:
				if (!g_pHlaeBcResultTarget) return FALSE;
				memcpy(g_pHlaeBcResultTarget,pMyCDS->lpData,sizeof(HLAE_BASECOM_RET_OnCreateWindow_s));
				return TRUE;

			case HLAE_BASECOM_MSGCL_OnGameWindowFocus:
				return HlaeBcCl_OnGameWindowFocus(pMyCDS->lpData);

			case HLAE_BASECOM_MSGCL_OnServerClose:
				return HlaeBcCl_OnServerClose(pMyCDS->lpData);

			case HLAE_BASECOM_MSGCL_MouseEvent:
				return HlaeBcCl_MouseEvent(pMyCDS->lpData);

			case HLAE_BASECOM_MSGCL_KeyBoardEvent:
				return HlaeBcCl_KeyBoardEvent(pMyCDS->lpData);

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
	dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_CHILD;
	dwExStyle = 0;
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

BOOL APIENTRY HlaeBcClt_DestroyWindow(HWND hWnd)
{
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
	}

	return DestroyWindow(hWnd);
}

BOOL WINAPI HlaeBcClt_SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	if (!g_hHlaeServerWND || hWnd==NULL || hWnd != g_HL_MainWindow ) return SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
	return SetWindowPos(hWnd,HWND_TOP,0,0,cx,cy,uFlags);//SWP_SHOWWINDOW);
}

void HlaeBcCl_AdjustViewPort(int x, int y, int width, int height)
// if we are using the server's gui, then we will use this to inform it about the size
{
	if(!g_hHlaeServerWND) return;

	static int iMyLastWidth=-1;
	static int iMyLastHeight=-1;

	// firt check if we need to inform the server about changed coords,
	// this has to be done carefully to avoid useless traffic:
	if ((iMyLastWidth!=width || iMyLastHeight != height ) && width > 0 && height > 0)
	{
		pEngfuncs->Con_DPrintf("HlaeBcCl_AdjustViewPort: Cached values (%ix%i) outdated, forcing update (%ix%i).\n",iMyLastWidth,iMyLastHeight,width,height);

		iMyLastWidth=width;
		iMyLastHeight=height;

		// update the server:
		HlaeBc_UpdateWindow(width, height);
	}
}

BOOL APIENTRY HlaeBcCl_GetCursorPos(LPPOINT lpPoint)
{
	BOOL bRet = GetCursorPos(lpPoint);
	return bRet;
	//if(!g_hHlaeServerGL) return bRet;

	POINT dp = *lpPoint;

	// translate mouse into H-L's coords:
	lpPoint->x=g_HL_MainWindow_info.iX + g_HL_MainWindow_info.MouseTarget.iX;
	lpPoint->y=g_HL_MainWindow_info.iY + g_HL_MainWindow_info.MouseTarget.iY;

	pEngfuncs->Con_Printf("Mouse (%i%i,) -> (%i,%i)\n",dp.x,dp.y,lpPoint->x,lpPoint->y);

	return bRet;
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