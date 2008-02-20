#include <hlae/auimanager.h>
#include <hlae/gamewindow.h>

#include <windows.h>
#include <shared/com/basecom.h>

#include <hlae/basecomserver.h>

// typedef bool (* OnRecieve_t)(class *lpClassPointer,unsigned long dwData,unsigned long cbData,void *lpData);


//
// CBCServerInternal defintion
//

class CBCServerInternal
{
private:
	CHlaeBcServer *_pBase; // coordinator class
	static HWND _hwClient;
	static LRESULT CALLBACK _HlaeBcSrvWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	bool _MyOnRecieve(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	bool _ReturnMessage(HWND hWnd,HWND hwTarget,ULONG dwData,DWORD cbData,PVOID lpData);

	// wrappers:
	bool _Wrapper_CreateWindowExA(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);

public:
	bool HlaeBcSrvStart(CHlaeBcServer *pBase);
	bool HlaeBcSrvStop();
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
		MessageBoxW(hWnd,L"Got empty test data.",HLAE_BASECOM_CLIENT_ID,MB_OK);
		return true;
	case HLAE_BASECOM_MSGSV_CreateWindowExA:
		return _Wrapper_CreateWindowExA(hWnd,hwSender,pMyCDS);
	case HLAE_BASECOM_MSGSV_RegisterClassA:
		break;
	case HLAE_BASECOM_MSGSV_DestroyWindow:
		break;
	default:
		;
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

bool CBCServerInternal::_Wrapper_CreateWindowExA(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	bool bRes;

	HLAE_BASECOM_RET_CreateWindowExA_s *pRet = new HLAE_BASECOM_RET_CreateWindowExA_s;

	HLAE_BASECOM_CreateWindowExA_s * pdata = (HLAE_BASECOM_CreateWindowExA_s *)pMyCDS->lpData;

	pRet->retResult = (HWND)_pBase->_DoCreateWindowExA((char *)pdata->lpClassName,(char *)pdata->lpWindowName,pdata->x,pdata->y,pdata->nHeight,pdata->nWidth);

	bRes=_ReturnMessage(hWnd,hwSender,HLAE_BASECOM_MSGCL_RET_CreateWindowExA,sizeof(HLAE_BASECOM_RET_CreateWindowExA_s),pRet);

	delete pRet;

	return bRes;
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


//
// the CBCServerInternal global:
//

CBCServerInternal g_BCServerInternal;


///////////////////////////////////////////////////////////////////////////////

//
// CHlaeBcServer
//

CHlaeBcServer::CHlaeBcServer(wxWindow *parent,hlaeAuiManager *pHlaeAuiManager)
{
	_parent = parent;
	_pHlaeAuiManager = pHlaeAuiManager;
	_pHlaeGameWindow = NULL;

	if(!g_BCServerInternal.HlaeBcSrvStart(this)) throw "ERROR: HlaeBcSrvStart() failed.";


}

CHlaeBcServer::~CHlaeBcServer()
{
	g_BCServerInternal.HlaeBcSrvStop();
	if(_pHlaeGameWindow) delete _pHlaeGameWindow;
}

void * CHlaeBcServer::_DoCreateWindowExA(char *lpClassNameA,char *lpWindowNameA,int x, int y, int nHeight, int nWidth)
{
	if (_pHlaeGameWindow)
		return NULL; // still window present, we only allow one window
	else
	{
		_pHlaeGameWindow =new CHlaeGameWindow(_parent,wxID_ANY,wxDefaultPosition,wxDefaultSize,wxHSCROLL | wxVSCROLL,wxT("Game Test Window"));

		_pHlaeGameWindow->SetClientSize(nWidth,nHeight);

		_pHlaeAuiManager->AddPane(_pHlaeGameWindow, wxAuiPaneInfo().TopDockable());
		return _pHlaeGameWindow->GetHWND();
	}
}