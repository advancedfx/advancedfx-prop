#include <windows.h>
#include <gl/gl.h>

#include <wx/dcclient.h>
#include <wx/string.h>

#include <shared/com/basecom.h>

#include "layout.h"
#include "gamewindow.h"

#include "debug.h"


#include "basecomserver.h"


#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib,"GLu32.lib")
#pragma comment(lib,"GLaux.lib")

// typedef bool (* OnRecieve_t)(class *lpClassPointer,unsigned long dwData,unsigned long cbData,void *lpData);


//
// CBCServerInternal defintion
//

class CBCServerInternal
{
public:
	CBCServerInternal();
	~CBCServerInternal();

	bool HlaeBcSrvStart(CHlaeBcServer *pBase);
	bool HlaeBcSrvStop();

	LRESULT DispatchToClientProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // outdated, not supported anymore
	LRESULT DispatchStruct(DWORD dwDataCode,DWORD cbDataSize,PVOID lpDataPtr);

private:
	CHlaeBcServer *_pBase; // coordinator class
	HWND _hwClient;
	static LRESULT CALLBACK _HlaeBcSrvWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE	_cl_hInstance;		// filled by RegisterClassA, used for relaying window messages to the H-L windowproc
	WNDPROC		_cl_lpfnWndProc;	// .

	BOOL _MyOnRecieve(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	BOOL _ReturnMessage(HWND hWnd,HWND hwTarget,ULONG dwData,DWORD cbData,PVOID lpData);

	// wrappers:
	BOOL _Wrapper_AquireGlWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	BOOL _Wrapper_ReleaseGlWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	BOOL _Wrapper_UpdateWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
};


//
// winapi related globals
//

HWND g_hwHlaeBcSrvWindow = NULL;
BOOL (CBCServerInternal::*g_OnRecieve)(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS) = NULL;
CBCServerInternal *g_pClass = NULL;
	
// USE INTERLOCED ACCES ONLY:
LONG g_lInstancesActive = 0; // this variable is accessed interlocked by HlaeBcSrvStart

//
// Internal class for the internal global
//
// Purpose is to wrap between WinAPI into an abstract view that is suitable for using in a wxWidgets targeted class
// It also does some basic low level handling and processing of the messages and events.
//

CBCServerInternal::CBCServerInternal()
{
	_cl_hInstance = NULL;
	_cl_lpfnWndProc = NULL;
}

CBCServerInternal::~CBCServerInternal()
{
	if (!HlaeBcSrvStop()) throw "Error: Could not stop BaseCom.";
}

bool CBCServerInternal::HlaeBcSrvStart(CHlaeBcServer *pBase)
{
	if (InterlockedIncrement(&g_lInstancesActive)>1)
	{
		 // if already running quit
		InterlockedDecrement(&g_lInstancesActive);
		return false;
	}

	HINSTANCE hInstance = (HINSTANCE)GetCurrentProcessId();

	static bool bRegistered=false;
	static WNDCLASS wc;

	if (!bRegistered)
	{
 
		// Register the main window class. 
		wc.style = NULL; 
		wc.lpfnWndProc = (WNDPROC) _HlaeBcSrvWndProc; 
		wc.cbClsExtra = 0; 
		wc.cbWndExtra = 0; 
		wc.hInstance = hInstance; 
		wc.hIcon = NULL; 
		wc.hCursor = NULL; 
		wc.hbrBackground = NULL; 
		wc.lpszMenuName =  NULL; 
		wc.lpszClassName = HLAE_BASECOM_CLASSNAME;

		if (!RegisterClass(&wc))
		{
			InterlockedDecrement(&g_lInstancesActive);
			return false;
		}

		bRegistered = true;
	 }

	g_OnRecieve = &CBCServerInternal::_MyOnRecieve;
	g_pClass = this;

	_pBase = pBase; // connect to coordinator class

	if (!(g_hwHlaeBcSrvWindow = CreateWindow(wc.lpszClassName,HLAE_BASECOM_SERVER_ID,WS_DISABLED|WS_POPUP,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInstance,NULL)))
	{
		g_OnRecieve = NULL;
		g_pClass = NULL;
		_pBase = NULL;
		InterlockedDecrement(&g_lInstancesActive);
		return false;
	}

	return true;
}

bool CBCServerInternal::HlaeBcSrvStop()
{
	if (g_lInstancesActive==0) // according to MSDN 32 bit reads (or writes) are guaranteed to be atomic
		return true;

	if (!DestroyWindow(g_hwHlaeBcSrvWindow)) return false;

	g_hwHlaeBcSrvWindow = NULL;
	_pBase = NULL;
	InterlockedDecrement(&g_lInstancesActive);
	return true;
}

LRESULT CBCServerInternal::DispatchToClientProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
	/*if (!_hwClient) return FALSE;

	static HLAE_BASECOM_CallWndProc_s mycws;

	mycws.hwnd = hwnd;
	mycws.uMsg = uMsg;
	mycws.wParam = wParam;
	mycws.lParam = lParam;

	return DispatchStruct(HLAE_BASECOM_MSGCL_CallWndProc_s,sizeof(HLAE_BASECOM_CallWndProc_s),&mycws);*/
}

LRESULT CBCServerInternal::DispatchStruct(DWORD dwDataCode,DWORD cbDataSize,PVOID lpDataPtr)
{
	if (!_hwClient) return FALSE;

	COPYDATASTRUCT myCopyData;

	myCopyData.dwData=dwDataCode;
	myCopyData.cbData=cbDataSize;
	myCopyData.lpData=lpDataPtr;

	return SendMessageW(
		_hwClient,
		WM_COPYDATA,
		(WPARAM)g_hwHlaeBcSrvWindow, // identify us as sender
		(LPARAM)&myCopyData
	);
}

LRESULT CALLBACK CBCServerInternal::_HlaeBcSrvWndProc(
    HWND hwnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{ 
 
    switch (uMsg) 
    { 
        case WM_CREATE: 
            // Initialize the window.
            return FALSE; 
 
        case WM_PAINT: 
            // Paint the window's client area. 
            return FALSE; 
 
        case WM_SIZE: 
            // Set the size and position of the window. 
            return FALSE; 
 
        case WM_DESTROY: 
            // Clean up window-specific data objects. 
            return FALSE; 

		case WM_COPYDATA:
			if (!(g_OnRecieve && g_pClass)) return FALSE;

			PCOPYDATASTRUCT pMyCDS;
			pMyCDS = (PCOPYDATASTRUCT) lParam;
			
			return (g_pClass->*g_OnRecieve)(hwnd,(HWND)wParam,pMyCDS);
 
        default: 
            return DefWindowProc(hwnd, uMsg, wParam, lParam); 
    } 
    
	return FALSE;
}

BOOL CBCServerInternal::_MyOnRecieve(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
// we could add some pointer security checks here, they miss currently, we asume data is consitent.
{
	switch (pMyCDS->dwData)
	{
	case HLAE_BASECOM_QRYSV_HELLO:
		// no checks performed atm
		return FALSE;

	case HLAE_BASECOM_QRYSV_AquireGlWindow:
		return _Wrapper_AquireGlWindow(hWnd,hwSender,pMyCDS);
	case HLAE_BASECOM_QRYSV_ReleaseGlWindow:
		return _Wrapper_ReleaseGlWindow(hWnd,hwSender,pMyCDS);
	case HLAE_BASECOM_MSGSV_UpdateWindow:
		return _Wrapper_UpdateWindow(hWnd,hwSender,pMyCDS);

	default:
		g_debug.SendMessage(wxT("CBCServerInternal::_MyOnRecieve: Recieved unkown message."), hlaeDEBUG_ERROR);
	}
	return FALSE;
}

BOOL CBCServerInternal::_ReturnMessage(HWND hWnd,HWND hwTarget,ULONG dwData,DWORD cbData,PVOID lpData)
{
	if(!hwTarget) return FALSE;
	
	COPYDATASTRUCT myCopyData;
	
	myCopyData.dwData=dwData;
	myCopyData.cbData=cbData;
	myCopyData.lpData=lpData;

	return SendMessageW(
		hwTarget,
		WM_COPYDATA,
		(WPARAM)hWnd, // identify us as sender
		(LPARAM)&myCopyData
	);
}

BOOL CBCServerInternal::_Wrapper_AquireGlWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	BOOL bRes;

	HLAE_BASECOM_RET_AquireGlWindow_s *pRet = new HLAE_BASECOM_RET_AquireGlWindow_s;

	HLAE_BASECOM_AquireGlWindow_s * pdata = (HLAE_BASECOM_AquireGlWindow_s *)pMyCDS->lpData;

	pRet->hServerGLRC = (HGLRC)( _pBase->_AquireGlWindow( pdata->nWidth,pdata->nHeight,pdata->iPixelFormat,(void *)&(pdata->pfd),(void **)&(pRet->hServerWND),&(pRet->hSavedDC) ) );

	bRes=_ReturnMessage(hWnd,hwSender,HLAE_BASECOM_RETCL_AquireGlWindow,sizeof(HLAE_BASECOM_RET_AquireGlWindow_s),pRet);

	delete pRet;

	return bRes;
}

BOOL CBCServerInternal::_Wrapper_ReleaseGlWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	BOOL bRes;

	HLAE_BASECOM_RET_ReleaseGlWindow_s *pRet = new HLAE_BASECOM_RET_ReleaseGlWindow_s;

	HLAE_BASECOM_ReleaseGlWindow_s * pdata = (HLAE_BASECOM_ReleaseGlWindow_s *)pMyCDS->lpData;

	pRet->retResult = _pBase->_ReleaseGlWindow() ? TRUE : FALSE;

	bRes=_ReturnMessage(hWnd,hwSender,HLAE_BASECOM_RETCL_ReleaseGlWindow,sizeof(HLAE_BASECOM_RET_ReleaseGlWindow_s),pRet);

	delete pRet;

	return bRes;
}

BOOL CBCServerInternal:: _Wrapper_UpdateWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	BOOL bRes;

	HLAE_BASECOM_UpdateWindows_s * pdata = (HLAE_BASECOM_UpdateWindows_s *)pMyCDS->lpData;

	bRes = _pBase->_UpdateWindow(pdata->nWidth,pdata->nHeight) ? TRUE : FALSE;

	return bRes;
}


//
// the CBCServerInternal global:
//

CBCServerInternal g_BCServerInternal;


///////////////////////////////////////////////////////////////////////////////

//
// CHlaeBcServer
//

CHlaeBcServer::CHlaeBcServer(wxWindow *parent)
{
	_parent = parent;
	_pHlaeGameWindow = NULL;
	_hGLRC = NULL;

	if(!g_BCServerInternal.HlaeBcSrvStart(this))
		g_debug.SendMessage(wxT("ERROR: HlaeBcSrvStart() failed."),hlaeDEBUG_FATALERROR);

}

CHlaeBcServer::~CHlaeBcServer()
{
	_ReleaseGlWindow();
	if(_pHlaeGameWindow) delete _pHlaeGameWindow;
	g_BCServerInternal.HlaeBcSrvStop();
}

void CHlaeBcServer::Do_DoPepareDC()
{
    /*if ( _pHlaeGameWindow ) && _pHlaeGameWindowDC)
	{
		_pHlaeGameWindow->DoPrepareDC(*_pHlaeGameWindowDC);


		// draw some shit to for fun, can't hurt:
		_pHlaeGameWindowDC->SetPen(*wxBLACK_PEN);
		_pHlaeGameWindowDC->DrawLine(1, 1, 100, 100);
	}*/
}

bool CHlaeBcServer::PassEventPreParsed(unsigned int umsg,unsigned int wParam,unsigned int lParam)
{
	return TRUE == g_BCServerInternal.DispatchToClientProc((HWND)(_pHlaeGameWindow->GetHWND()),(UINT)umsg,(WPARAM)wParam,(LPARAM)lParam);
}

bool CHlaeBcServer::PassEventPreParsed(WXHWND hwnd,unsigned int umsg,unsigned int wParam,unsigned int lParam)
{
	return TRUE == g_BCServerInternal.DispatchToClientProc((HWND)hwnd,(UINT)umsg,(WPARAM)wParam,(LPARAM)lParam);
}

bool CHlaeBcServer::Pass_WndRectUpdate(int iLeft, int iTop, int iWidthVisible, int iHeightVisible, int iWidthTotal, int iHeightTotal, int iLeftGlobal, int iTopGlobal)
{
	HLAE_BASECOM_WndRectUpdate_s mys;

	mys.iLeft = iLeft;
	mys.iTop = iTop;
	mys.iWidthVisible = iWidthVisible;
	mys.iHeightVisible = iHeightVisible;
	mys.iWidthTotal = iWidthTotal;
	mys.iHeightTotal = iHeightTotal;
	mys.iLeftGlobal = iLeftGlobal;
	mys.iTopGlobal = iTopGlobal;

	return TRUE==g_BCServerInternal.DispatchStruct(
		HLAE_BASECOM_MSGCL_WndRectUpdate,
		sizeof(mys),
		&mys
	);
}
bool CHlaeBcServer::Pass_MouseEvent(unsigned int uMsg, unsigned int wParam, unsigned short iX,unsigned short iY)
{
	HLAE_BASECOM_MSGCL_MouseEvent_s mys;

	mys.uMsg = uMsg;
	mys.wParam = wParam;
	mys.iX = iX;
	mys.iY = iY;

	// WARNING: HACK HACK! bad hack, we can't decide if the dispatch failed or if it wasn't processed!
	return FALSE==g_BCServerInternal.DispatchStruct(
		HLAE_BASECOM_MSGCL_MouseEvent,
		sizeof(mys),
		&mys
	);
}
bool CHlaeBcServer::Pass_KeyBoardEvent(unsigned int uMsg, unsigned int uKeyCode, unsigned int uKeyFlags)
{
	HLAE_BASECOM_MSGCL_KeyBoardEvent_s mys;

	mys.uMsg = uMsg;
	mys.uKeyCode = uKeyCode;
	mys.uKeyFlags = uKeyFlags;

	// WARNING: HACK HACK! bad hack, we can't decide if the dispatch failed or if it wasn't processed!
	return FALSE==g_BCServerInternal.DispatchStruct(
		HLAE_BASECOM_MSGCL_KeyBoardEvent,
		sizeof(mys),
		&mys
	);
}


void * CHlaeBcServer::_AquireGlWindow(int nWidth, int nHeight, int iPixelFormat,const void *ppfd, void **phServerWnd, int *phSavedDC)
{
	HDC myhdc;

	if (!_pHlaeGameWindow)
	{
		// window not present, create it first:
		g_debug.SendMessage(wxT("Creating game window"), hlaeDEBUG_VERBOSE_LEVEL3);

		wxString mycaption("Game Window",wxConvUTF8);

		_pHlaeGameWindow =new CHlaeGameWindow(this,_parent,wxID_ANY,wxDefaultPosition,wxSize(200,150),wxHSCROLL | wxVSCROLL,mycaption);

		// adjust size:
		_UpdateWindow(nWidth, nHeight);
		
		g_layoutmanager.AddPane(_pHlaeGameWindow, wxAuiPaneInfo().CentrePane().Caption(mycaption));

		// first time, so also set the pixelformat:
		myhdc = ::GetDC( (HWND)(_pHlaeGameWindow->GetHWND()) );

		bool bSucc= TRUE == ::SetPixelFormat(myhdc,iPixelFormat,(PIXELFORMATDESCRIPTOR *)ppfd);

		int iCurFormat = ::GetPixelFormat(myhdc);

		::ReleaseDC((HWND)(_pHlaeGameWindow->GetHWND()),myhdc);

		wxString mystr;

		if (bSucc)
		{
			mystr.Printf(wxT("SetPixelFormat succeeded: Requested: %i, Current: %i"),iPixelFormat,iCurFormat); 
			g_debug.SendMessage(mystr, hlaeDEBUG_VERBOSE_LEVEL3);
		} else {
			mystr.Printf(wxT("SetPixelFormat failed: Requested: %i, Current: %i, GetLastError(): %i"),iPixelFormat,iCurFormat,::GetLastError()); 
			g_debug.SendMessage(mystr, hlaeDEBUG_ERROR);

			return 0; // this is a problem, we can't continue
		}
	}

	void *retResult = 0;

	/*if (!_hGLRC)
	{
		// no OpenGL Resource Context handle, so create new HGLRC:
		_pHlaeGameWindow->DoPrepareDC(wxClientDC(_pHlaeGameWindow));

		wxString mystr;
		HDC myhdc = ::GetDC( (HWND)(_pHlaeGameWindow->GetHWND()) );

		retResult = (void *)(wglCreateContext(myhdc));

		if (!retResult)
		{
	
			mystr.Printf(wxT("wglCreateContext failed: Requested: GetLastError(): %i"),::GetLastError()); 
			g_debug.SendMessage(mystr, hlaeDEBUG_ERROR);
		} else {
			mystr.Printf(wxT("wglCreateContext succeeded: HGLRC: 0x%08x"),retResult); 
			g_debug.SendMessage(mystr, hlaeDEBUG_VERBOSE_LEVEL3);

			// fill in server window:
			if (phServerWnd) *phServerWnd = (void *)(_pHlaeGameWindow->GetHWND());
		}

		::ReleaseDC((HWND)(_pHlaeGameWindow->GetHWND()),myhdc);

		myhdc=::GetDC((HWND)(_pHlaeGameWindow->GetHWND()));
		mystr.Printf(wxT("GetPixelFormat(myhdc):%i"),::GetPixelFormat(myhdc)); 
		::ReleaseDC((HWND)(_pHlaeGameWindow->GetHWND()),myhdc);
		g_debug.SendMessage(mystr, hlaeDEBUG_VERBOSE_LEVEL3);

	}

	wxClientDC *udontfuckwithme = new wxClientDC(_pHlaeGameWindow);
	_pHlaeGameWindow->DoPrepareDC(*udontfuckwithme);
	udontfuckwithme->DrawEllipse(0,0,100,100);
	_pHlaeGameWindow->OnDraw(*udontfuckwithme);
	delete udontfuckwithme;

	//MessageBoxW((HWND)(_pHlaeGameWindow->GetHWND()),L"CHOOCHOO",L"I am crazy11",MB_OK);
*/
	// fill in server window:
	if (phServerWnd) *phServerWnd = (void *)(_pHlaeGameWindow->GetHWND());


	wxString mystr;
	int iSavedDC;
	myhdc=::GetDC((HWND)(_pHlaeGameWindow->GetHWND()));
	
	iSavedDC= ::SaveDC(myhdc);
	::RestoreDC(myhdc,iSavedDC);
	if (phSavedDC) *phSavedDC = iSavedDC;

	mystr.Printf(wxT("GetPixelFormat(myhdc):%i, ServerWND: 0x%08x"),::GetPixelFormat(myhdc),phServerWnd? *phServerWnd : 0); 
	::ReleaseDC((HWND)(_pHlaeGameWindow->GetHWND()),myhdc);
	g_debug.SendMessage(mystr, hlaeDEBUG_VERBOSE_LEVEL3);

	retResult=(void *)1;

	return retResult;
}

bool CHlaeBcServer::_ReleaseGlWindow()
{
	if (!_hGLRC) return false;

	bool retResult = TRUE == wglDeleteContext((HGLRC)_hGLRC);
	if (!retResult)
	{
		wxString mystr;
		mystr.Printf(wxT("wglDeleteContext failed: Requested: GetLastError(): %i"),::GetLastError()); 
		g_debug.SendMessage(mystr, hlaeDEBUG_ERROR);
	} else {
			g_debug.SendMessage(wxT("wglCreateContext succeeded"), hlaeDEBUG_VERBOSE_LEVEL3);
	}
	return retResult;
}

bool CHlaeBcServer::_UpdateWindow(int nWidth, int nHeight)
{
	if (!_pHlaeGameWindow) return false;
	
	_pHlaeGameWindow->SetVirtualSize(nWidth,nHeight);
	//_pHlaeGameWindow->SetScrollRate(10,10);
	_pHlaeGameWindow->SetScrollbars(1,1,nWidth,nHeight);
	_pHlaeGameWindow->SetMaxSize(wxSize(nWidth,nHeight));

	return true;
}