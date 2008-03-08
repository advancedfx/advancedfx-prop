#include "layout.h"
#include "gamewindow.h"

#include <wx/dcclient.h>
#include <windows.h>
#include <shared/com/basecom.h>

#include "basecomserver.h"

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

	LRESULT DispatchToClientProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam); // outdated
	LRESULT DispatchStruct(DWORD dwDataCode,DWORD cbDataSize,PVOID lpDataPtr);

private:
	CHlaeBcServer *_pBase; // coordinator class
	HWND _hwClient;
	static LRESULT CALLBACK _HlaeBcSrvWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE	_cl_hInstance;		// filled by RegisterClassA, used for relaying window messages to the H-L windowproc
	WNDPROC		_cl_lpfnWndProc;	// .

	bool _MyOnRecieve(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	bool _ReturnMessage(HWND hWnd,HWND hwTarget,ULONG dwData,DWORD cbData,PVOID lpData);

	// wrappers:
	bool _Wrapper_RegisterClassA(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS); // outdated and unused
	bool _Wrapper_CreateWindowExA(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS); //outdated and unused
	bool _Wrapper_DestroyWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS); // outdated and unused
	bool _Wrapper_GameWndPrepare(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	bool _Wrapper_GameWndGetDC(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	bool _Wrapper_GameWndRelease(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
};


//
// winapi related globals
//

HWND g_hwHlaeBcSrvWindow = NULL;
bool (CBCServerInternal::*g_OnRecieve)(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS) = NULL;
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
	if (!_hwClient) return FALSE;

	static HLAE_BASECOM_CallWndProc_s mycws;

	mycws.hwnd = hwnd;
	mycws.uMsg = uMsg;
	mycws.wParam = wParam;
	mycws.lParam = lParam;

	return DispatchStruct(HLAE_BASECOM_MSGCL_CallWndProc_s,sizeof(HLAE_BASECOM_CallWndProc_s),&mycws);
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
			
			if ((g_pClass->*g_OnRecieve)(hwnd,(HWND)wParam,pMyCDS))
				return TRUE;
			else
				return FALSE;
 
        default: 
            return DefWindowProc(hwnd, uMsg, wParam, lParam); 
    } 
    
	return FALSE;
}

bool CBCServerInternal::_MyOnRecieve(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
// we could add some pointer security checks here, they miss currently, we asume data is consitent.
{
	switch (pMyCDS->dwData)
	{
	case HLAE_BASECOM_MSG_TESTDUMMY:
		MessageBoxW(hWnd,L"Got empty test data.",HLAE_BASECOM_SERVER_ID,MB_OK);
		return true;
	case HLAE_BASECOM_MSGSV_CreateWindowExA:
		return _Wrapper_CreateWindowExA(hWnd,hwSender,pMyCDS);
	case HLAE_BASECOM_MSGSV_RegisterClassA:
		return _Wrapper_RegisterClassA(hWnd,hwSender,pMyCDS);
	case HLAE_BASECOM_MSGSV_DestroyWindow:
		return _Wrapper_DestroyWindow(hWnd,hwSender,pMyCDS);

	case HLAE_BASECOM_MSGSV_GameWndPrepare:
		return _Wrapper_GameWndPrepare(hWnd,hwSender,pMyCDS);
	case HLAE_BASECOM_MSGSV_GameWndGetDC:
		return _Wrapper_GameWndGetDC(hWnd,hwSender,pMyCDS);
	case HLAE_BASECOM_MSGSV_GameWndRelease:
		return _Wrapper_GameWndRelease(hWnd,hwSender,pMyCDS);

	default:
		MessageBoxW(hWnd,L"Error: Recieved unkown message.",HLAE_BASECOM_SERVER_ID,MB_OK|MB_ICONERROR);
	}
	return false;
}

bool CBCServerInternal::_ReturnMessage(HWND hWnd,HWND hwTarget,ULONG dwData,DWORD cbData,PVOID lpData)
{
	if(!hwTarget) return false;
	
	COPYDATASTRUCT myCopyData;
	
	myCopyData.dwData=dwData;
	myCopyData.cbData=cbData;
	myCopyData.lpData=lpData;

	return TRUE==SendMessageW(
		hwTarget,
		WM_COPYDATA,
		(WPARAM)hWnd, // identify us as sender
		(LPARAM)&myCopyData
	);
}

bool CBCServerInternal::_Wrapper_RegisterClassA(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	//MessageBoxW(hWnd,L"recvied",L"Reg",MB_OK);
	HLAE_BASECOM_RegisterClassA_s * pdata = (HLAE_BASECOM_RegisterClassA_s *)pMyCDS->lpData;

	// adjust pointers for piggy backs:
	if (HIWORD(pdata->lpszClassName))
		pdata->lpszClassName=(LPCSTR)((char *)pdata + (size_t)(pdata->lpszClassName));
	else
		pdata->lpszClassName=NULL;
	if (HIWORD(pdata->lpszMenuName))
		pdata->lpszMenuName=(LPCSTR)((char *)pdata + (size_t)(pdata->lpszMenuName));
	else
		pdata->lpszMenuName=NULL;

	// we will just fetch the window proc properties:
	// although we won't use them.
	_cl_hInstance = pdata->hInstance;
	_cl_lpfnWndProc = pdata->lpfnWndProc;

	//char sztemp[100];
	//_snprintf(sztemp,sizeof(sztemp),"0x%08x",pdata->lpfnWndProc);
	//MessageBoxA(0,sztemp,"SV says:",MB_OK);

	// we also fetch the window, since we will need it for transmitting data that shall be passed to the WindowProc function:
	_hwClient = hwSender;

	//MessageBoxW(hWnd,L"Survived?",L"Reg",MB_OK);
	return false; // we didn't handle it, let the hook handle it
}

bool CBCServerInternal::_Wrapper_CreateWindowExA(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	bool bRes;

	HLAE_BASECOM_RET_CreateWindowExA_s *pRet = new HLAE_BASECOM_RET_CreateWindowExA_s;

	HLAE_BASECOM_CreateWindowExA_s * pdata = (HLAE_BASECOM_CreateWindowExA_s *)pMyCDS->lpData;

	// adjust pointers for piggy backs:
	if(HIWORD(pdata->lpClassName))
		pdata->lpClassName=(LPCTSTR)((char *)pdata + (size_t)(pdata->lpClassName));
	else
		pdata->lpClassName=NULL;
	if(HIWORD(pdata->lpWindowName))
		pdata->lpWindowName=(LPCTSTR)((char *)pdata + (size_t)(pdata->lpWindowName));
	else
		pdata->lpWindowName=NULL;

	pRet->retResult = (HWND)_pBase->_DoCreateWindowExA((char *)pdata->lpClassName,(char *)pdata->lpWindowName,pdata->x,pdata->y,pdata->nHeight,pdata->nWidth);

	bRes=_ReturnMessage(hWnd,hwSender,HLAE_BASECOM_MSGCL_RET_CreateWindowExA,sizeof(HLAE_BASECOM_RET_CreateWindowExA_s),pRet);

	delete pRet;

	return bRes;
}

bool CBCServerInternal::_Wrapper_DestroyWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	bool bRes;

	HLAE_BASECOM_RET_DestroyWindow_s *pRet = new HLAE_BASECOM_RET_DestroyWindow_s;

	HLAE_BASECOM_DestroyWindow_s * pdata = (HLAE_BASECOM_DestroyWindow_s *)pMyCDS->lpData;

	if(_pBase->_DoDestroyWindow((WXHWND)pdata->hWnd))
		pRet->retResult = TRUE;
	else
		pRet->retResult = FALSE;

	bRes=_ReturnMessage(hWnd,hwSender,HLAE_BASECOM_MSGCL_RET_CreateWindowExA,sizeof(HLAE_BASECOM_RET_CreateWindowExA_s),pRet);

	delete pRet;

	return bRes;
}

bool CBCServerInternal::_Wrapper_GameWndPrepare(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	bool bRes;

	HLAE_BASECOM_GameWndPrepare_s * pdata = (HLAE_BASECOM_GameWndPrepare_s *)pMyCDS->lpData;

	bRes = _pBase->_Do_GameWndPrepare(pdata->nWidth,pdata->nHeight);

	// set _hwClient, so Messages will be passed:
	_hwClient = hwSender;

	return bRes;
}

bool CBCServerInternal::_Wrapper_GameWndGetDC(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	bool bRes;

	HLAE_BASECOM_RET_GameWndGetDC_s *pRet = new HLAE_BASECOM_RET_GameWndGetDC_s;

	HLAE_BASECOM_GameWndGetDC_s * pdata = (HLAE_BASECOM_GameWndGetDC_s *)pMyCDS->lpData;

	pRet->retResult = (HWND)(_pBase->_Do_GameWndGetDC());

	bRes=_ReturnMessage(hWnd,hwSender,HLAE_BASECOM_MSGCL_RET_GameWndGetDC,sizeof(HLAE_BASECOM_RET_GameWndGetDC_s),pRet);

	delete pRet;

	return bRes;
}

bool CBCServerInternal::_Wrapper_GameWndRelease(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	bool bRes;

	HLAE_BASECOM_GameWndRelease_s * pdata = (HLAE_BASECOM_GameWndRelease_s *)pMyCDS->lpData;

	bRes=_pBase->_Do_GameWndRelease();

	// reset client handle (so no messages will get passed anymore):
	_hwClient = NULL;

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
	_pHlaeGameWindowDC = NULL;

	if(!g_BCServerInternal.HlaeBcSrvStart(this)) throw "ERROR: HlaeBcSrvStart() failed.";


}

CHlaeBcServer::~CHlaeBcServer()
{
	g_BCServerInternal.HlaeBcSrvStop();
	if(_pHlaeGameWindowDC) delete _pHlaeGameWindowDC;
	if(_pHlaeGameWindow) delete _pHlaeGameWindow;
}

void CHlaeBcServer::Do_DoPepareDC()
{
    if ( _pHlaeGameWindow && _pHlaeGameWindowDC)
	{
		_pHlaeGameWindow->DoPrepareDC(*_pHlaeGameWindowDC);

		// draw some shit to for fun, can't hurt:
		_pHlaeGameWindowDC->SetPen(*wxBLACK_PEN);
		_pHlaeGameWindowDC->DrawLine(1, 1, 100, 100);
	}
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

void * CHlaeBcServer::_DoCreateWindowExA(char *lpClassNameA,char *lpWindowNameA,int x, int y, int nHeight, int nWidth)
{
	return NULL; // code inactive

	if (_pHlaeGameWindow)
		return NULL; // window already present, we only allow one though
	else
	{
		wxString mycaption(lpWindowNameA,wxConvUTF8);

		_pHlaeGameWindow =new CHlaeGameWindow(this,_parent,wxID_ANY,wxDefaultPosition,wxSize(200,150),wxHSCROLL | wxVSCROLL,mycaption);

		_pHlaeGameWindow->SetVirtualSize(nWidth,nHeight);
		_pHlaeGameWindow->SetScrollRate(10,10);
		
		g_layoutmanager.AddPane(_pHlaeGameWindow, wxAuiPaneInfo().RightDockable().Float().Caption(mycaption));

		return _pHlaeGameWindow->GetHWND();
	}
}

bool CHlaeBcServer::_DoDestroyWindow(WXHWND wxhWnd)
{
	return false; // code inactive

	if (!_pHlaeGameWindow) return false;

	if (_pHlaeGameWindow->GetHWND()==wxhWnd)
	{
		return _pHlaeGameWindow->Destroy();
	}
	else
		return false;
}

bool CHlaeBcServer::_Do_GameWndPrepare(int nWidth, int nHeight)
{
	//char sztemp[100];
	//_snprintf(sztemp,sizeof(sztemp),"nWidth: %i, nHeight: %i",nWidth,nHeight);
	//MessageBoxA(0,sztemp,"CHlaeBcServer::_Do_GameWndPrepare",MB_OK);

	if (!_pHlaeGameWindow)
	{
		wxString mycaption("Game Window",wxConvUTF8);

		_pHlaeGameWindow =new CHlaeGameWindow(this,_parent,wxID_ANY,wxDefaultPosition,wxSize(200,150),wxHSCROLL | wxVSCROLL,mycaption);

		_pHlaeGameWindow->SetVirtualSize(nWidth,nHeight);
		//_pHlaeGameWindow->SetScrollRate(10,10);
		_pHlaeGameWindow->SetScrollbars(1,1,nWidth,nHeight);
		_pHlaeGameWindow->SetMaxSize(wxSize(nWidth,nHeight));

		g_layoutmanager.AddPane(_pHlaeGameWindow, wxAuiPaneInfo().RightDockable().Float().Caption(mycaption));

	} else {
		_pHlaeGameWindow->SetVirtualSize(nWidth,nHeight);
		//_pHlaeGameWindow->SetScrollRate(10,10);
		_pHlaeGameWindow->SetScrollbars(1,1,nWidth,nHeight);
		_pHlaeGameWindow->SetMaxSize(wxSize(nWidth,nHeight));
	}

	return true; // may be a bit more safe in the future, but that's it for now
}

WXHWND CHlaeBcServer::_Do_GameWndGetDC()
// returns the DC's HWND, not the DC (because the HDC is not allowed to be shared among threads)
{
	if (!_pHlaeGameWindow) return NULL;

    if (!_pHlaeGameWindowDC) _pHlaeGameWindowDC = new wxClientDC(_pHlaeGameWindow);

	_pHlaeGameWindow->DoPrepareDC(*_pHlaeGameWindowDC);

	// draw some shit to for fun, can't hurt:
	_pHlaeGameWindowDC->SetPen(*wxBLACK_PEN);
	_pHlaeGameWindowDC->DrawLine(1, 1, 100, 100);

	WXHWND pResult=NULL;

	pResult = (_pHlaeGameWindowDC->GetWindow())->GetHWND();

	//char sztemp[200];
	//_snprintf(sztemp,sizeof(sztemp),"DC's WXHWND is: %i (0x%08x)",pResult,pResult);
	///MessageBoxA(NULL,sztemp,"CHlaeBcServer::_Do_GameWndGetDC()",MB_OK|MB_ICONERROR);

    return pResult;
}

bool CHlaeBcServer::_Do_GameWndRelease()
{
	if (!_pHlaeGameWindow) return false;

	return true; // may be a bit more safe in the future, but that's it for now
}