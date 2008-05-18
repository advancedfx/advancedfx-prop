#include <stdafx.h>
#include "basecom.h"

//
#pragma unmanaged
//

#include <windows.h>
#include "../../shared/com/basecom.h"


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
	BOOL _Wrapper_OnCreateWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	BOOL _Wrapper_OnDestroyWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	BOOL _Wrapper_OnFilmingStart(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	BOOL _Wrapper_OnFilmingStop(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
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
	_hwClient = NULL;
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
	_hwClient = NULL;
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

	case HLAE_BASECOM_QRYSV_OnCreateWindow:
		return _Wrapper_OnCreateWindow(hWnd,hwSender,pMyCDS);
	case HLAE_BASECOM_MSGSV_OnDestroyWindow:
		return _Wrapper_OnDestroyWindow(hWnd,hwSender,pMyCDS);
	case HLAE_BASECOM_MSGSV_OnFilmingStart:
		return _Wrapper_OnFilmingStart(hWnd,hwSender,pMyCDS);
	case HLAE_BASECOM_MSGSV_OnFilmingStop:
		return _Wrapper_OnFilmingStop(hWnd,hwSender,pMyCDS);
	case HLAE_BASECOM_MSGSV_UpdateWindow:
		return _Wrapper_UpdateWindow(hWnd,hwSender,pMyCDS);

	default:
		;
		//g_debug.SendMessage(_T("CBCServerInternal::_MyOnRecieve: Recieved unkown message."), hlaeDEBUG_ERROR);
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

BOOL CBCServerInternal::_Wrapper_OnCreateWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	BOOL bRes;
	if (!_hwClient) _hwClient = hwSender; // allow messages to client

	HLAE_BASECOM_RET_OnCreateWindow_s *pRet = new HLAE_BASECOM_RET_OnCreateWindow_s;

	HLAE_BASECOM_OnCreateWindow_s * pdata = (HLAE_BASECOM_OnCreateWindow_s *)pMyCDS->lpData;

	pRet->parentWindow = (HWND)( _pBase->_OnCreateWindow( pdata->nWidth,pdata->nHeight ) );

	bRes=_ReturnMessage(hWnd,hwSender,HLAE_BASECOM_RETCL_OnCreateWindow,sizeof(HLAE_BASECOM_RET_OnCreateWindow_s),pRet);

	delete pRet;

	return bRes;
}

BOOL CBCServerInternal:: _Wrapper_OnDestroyWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	BOOL bRes;

	HLAE_BASECOM_OnDestroyWindow_s * pdata = (HLAE_BASECOM_OnDestroyWindow_s *)pMyCDS->lpData;

	bRes = _pBase->_OnDestroyWindow() ? TRUE : FALSE;

	_hwClient = NULL; // no messages allowed afterwards
	return bRes;
}

BOOL CBCServerInternal:: _Wrapper_OnFilmingStart(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	BOOL bRes;

	HLAE_BASECOM_OnFilmingStart_s * pdata = (HLAE_BASECOM_OnFilmingStart_s *)pMyCDS->lpData;

	bRes = _pBase->_OnFilmingStart() ? TRUE : FALSE;

	return bRes;
}

BOOL CBCServerInternal:: _Wrapper_OnFilmingStop(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	BOOL bRes;

	HLAE_BASECOM_OnFilmingStop_s * pdata = (HLAE_BASECOM_OnFilmingStop_s *)pMyCDS->lpData;

	bRes = _pBase->_OnFilmingStop() ? TRUE : FALSE;

	return bRes;
}

BOOL CBCServerInternal:: _Wrapper_UpdateWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	BOOL bRes;

	HLAE_BASECOM_UpdateWindows_s * pdata = (HLAE_BASECOM_UpdateWindows_s *)pMyCDS->lpData;

	bRes = _pBase->_UpdateWindow(pdata->nWidth,pdata->nHeight) ? TRUE : FALSE;

	return bRes;
}


///////////////////////////////////////////////////////////////////////////////

//
// CHlaeBcServer
//

//
#pragma managed
//

CHlaeBcServer::CHlaeBcServer( System::Windows::Forms::ToolStripContentPanel ^gameWindowParent )
{
	_gameWindowParent = System::Runtime::InteropServices::GCHandle::Alloc( gameWindowParent );
	_hwndGameWindowParent = gameWindowParent->Handle.ToPointer();

	_pBCServerInternal = new CBCServerInternal();

	if(!(_pBCServerInternal->HlaeBcSrvStart(this)))
		System::Windows::Forms::MessageBox::Show( "ERROR: HlaeBcSrvStart() failed.", "Error", System::Windows::Forms::MessageBoxButtons::OK, System::Windows::Forms::MessageBoxIcon::Error );
}

CHlaeBcServer::~CHlaeBcServer()
{
	_OnGameWindowClose();
	_pBCServerInternal->HlaeBcSrvStop();
	delete _pBCServerInternal;
	_gameWindowParent.Free();
}

//
#pragma unmanaged
//

bool CHlaeBcServer::PassEventPreParsed(unsigned int umsg,unsigned int wParam,unsigned int lParam)
{
	return TRUE == _pBCServerInternal->DispatchToClientProc((HWND)(_hwndGameWindowParent),(UINT)umsg,(WPARAM)wParam,(LPARAM)lParam);
}

bool CHlaeBcServer::PassEventPreParsed(unsigned int hwnd,unsigned int umsg,unsigned int wParam,unsigned int lParam)
{
	return TRUE == _pBCServerInternal->DispatchToClientProc((HWND)hwnd,(UINT)umsg,(WPARAM)wParam,(LPARAM)lParam);
}

bool CHlaeBcServer::OnGameWindowFocus()
{
	HLAE_BASECOM_OnGameWindowFocus_s mys;

	return TRUE==_pBCServerInternal->DispatchStruct(
		HLAE_BASECOM_MSGCL_OnGameWindowFocus,
		sizeof(mys),
		&mys
	);
}

bool CHlaeBcServer::OnHlaeActivate(bool bActive)
{
	HLAE_BASECOM_OnServerActivate_s mys;

	mys.bActive = bActive;

	return TRUE==_pBCServerInternal->DispatchStruct(
		HLAE_BASECOM_MSGCL_OnServerActivate,
		sizeof(mys),
		&mys
	);
}

bool CHlaeBcServer::_OnGameWindowClose()
{
	HLAE_BASECOM_OnServerClose_s mys;

	return TRUE==_pBCServerInternal->DispatchStruct(
		HLAE_BASECOM_MSGCL_OnServerClose,
		sizeof(mys),
		&mys
	);
	return true;
}
bool CHlaeBcServer::Pass_MouseEvent(unsigned int uMsg, unsigned int wParam, unsigned short iX,unsigned short iY)
{
	HLAE_BASECOM_MSGCL_MouseEvent_s mys;

	mys.uMsg = uMsg;
	mys.wParam = wParam;
	mys.iX = iX;
	mys.iY = iY;

	// WARNING: HACK HACK! bad hack, we can't decide if the dispatch failed or if it wasn't processed!
	return FALSE==_pBCServerInternal->DispatchStruct(
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
	return FALSE==_pBCServerInternal->DispatchStruct(
		HLAE_BASECOM_MSGCL_KeyBoardEvent,
		sizeof(mys),
		&mys
	);
}

void *CHlaeBcServer::_OnCreateWindow(int nWidth, int nHeight)
{
	// adjust size and prepare for drawing:
	_UpdateWindow(nWidth, nHeight);

	return _hwndGameWindowParent;
}

bool CHlaeBcServer::_OnDestroyWindow()
{
	return true;
}

bool CHlaeBcServer::_OnFilmingStart()
{
	return true;
}
bool CHlaeBcServer:: _OnFilmingStop()
{
	return true;
}

//
#pragma managed
//

bool CHlaeBcServer::_UpdateWindow(int nWidth, int nHeight)
{
	((System::Windows::Forms::ToolStripContentPanel^)_gameWindowParent.Target)->AutoScrollMinSize = System::Drawing::Size( nWidth, nHeight );
	return true;
}

//
#pragma unmanaged
//


//
#pragma managed
//
