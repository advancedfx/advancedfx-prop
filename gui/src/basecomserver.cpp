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
public:
	CBCServerInternal();
	~CBCServerInternal();

	bool HlaeBcSrvStart(CHlaeBcServer *pBase);
	bool HlaeBcSrvStop();

	LRESULT DispatchToClientProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	CHlaeBcServer *_pBase; // coordinator class
	HWND _hwClient;
	static LRESULT CALLBACK _HlaeBcSrvWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE	_cl_hInstance;		// filled by RegisterClassA, used for relaying window messages to the H-L windowproc
	WNDPROC		_cl_lpfnWndProc;	// .

	bool _MyOnRecieve(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	bool _ReturnMessage(HWND hWnd,HWND hwTarget,ULONG dwData,DWORD cbData,PVOID lpData);

	// wrappers:
	bool _Wrapper_RegisterClassA(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	bool _Wrapper_CreateWindowExA(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
	bool _Wrapper_DestroyWindow(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS);
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
	if (!(_cl_lpfnWndProc && _cl_hInstance)) return FALSE;
	if(!_hwClient) return FALSE;
	
	COPYDATASTRUCT myCopyData;

	static HLAE_BASECOM_CallWndProc_s mycws;

	mycws.hInstance = _cl_hInstance;
	mycws.lpfnWndProc = _cl_lpfnWndProc;
	mycws.hwnd = hwnd;
	mycws.uMsg = uMsg;
	mycws.wParam = wParam;
	mycws.lParam = lParam;
	
	myCopyData.dwData=HLAE_BASECOM_MSGCL_CallWndProc_s;
	myCopyData.cbData=sizeof(HLAE_BASECOM_CallWndProc_s);
	myCopyData.lpData=&mycws;

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
		MessageBoxW(hWnd,L"Got empty test data.",HLAE_BASECOM_CLIENT_ID,MB_OK);
		return true;
	case HLAE_BASECOM_MSGSV_CreateWindowExA:
		return _Wrapper_CreateWindowExA(hWnd,hwSender,pMyCDS);
	case HLAE_BASECOM_MSGSV_RegisterClassA:
		_Wrapper_RegisterClassA(hWnd,hwSender,pMyCDS);
		break;
	case HLAE_BASECOM_MSGSV_DestroyWindow:
		_Wrapper_DestroyWindow(hWnd,hwSender,pMyCDS);
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

bool CBCServerInternal::_Wrapper_RegisterClassA(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	HLAE_BASECOM_RegisterClassA_s * pdata = (HLAE_BASECOM_RegisterClassA_s *)pMyCDS->lpData;

	// adjust pointers for piggy backs:
	pdata->lpszClassName=(LPCSTR)((char *)pdata + (size_t)(pdata->lpszClassName));
	pdata->lpszMenuName=(LPCSTR)((char *)pdata + (size_t)(pdata->lpszMenuName));

	// we will just fetch the window proc properties:
	// although we won't use them.
	_cl_hInstance = pdata->hInstance;
	_cl_lpfnWndProc = pdata->lpfnWndProc;

	// we also fetch the window, since we will need it for transmitting data that shall be passed to the WindowProc function:
	_hwClient = hwSender;

	return false; // we didn't handle it, let the hook handle it
}

bool CBCServerInternal::_Wrapper_CreateWindowExA(HWND hWnd,HWND hwSender,PCOPYDATASTRUCT pMyCDS)
{
	bool bRes;

	HLAE_BASECOM_RET_CreateWindowExA_s *pRet = new HLAE_BASECOM_RET_CreateWindowExA_s;

	HLAE_BASECOM_CreateWindowExA_s * pdata = (HLAE_BASECOM_CreateWindowExA_s *)pMyCDS->lpData;

	// adjust pointers for piggy backs:
	pdata->lpClassName=(LPCTSTR)((char *)pdata + (size_t)(pdata->lpClassName));
	pdata->lpWindowName=(LPCTSTR)((char *)pdata + (size_t)(pdata->lpWindowName));

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

bool CHlaeBcServer::PassEventPreParsed(unsigned int umsg,unsigned int wParam,unsigned int lParam)
{
	return TRUE == g_BCServerInternal.DispatchToClientProc((HWND)(_pHlaeGameWindow->GetHWND()),(UINT)umsg,(WPARAM)wParam,(LPARAM)lParam);
}

bool CHlaeBcServer::PassEventPreParsed(WXHWND hwnd,unsigned int umsg,unsigned int wParam,unsigned int lParam)
{
	return TRUE == g_BCServerInternal.DispatchToClientProc((HWND)hwnd,(UINT)umsg,(WPARAM)wParam,(LPARAM)lParam);
}

bool CHlaeBcServer::PassEventToHook(wxEvent &myevent)
{
	int iId = myevent.GetId();

	UINT uMsg = WM_NULL;
	LPARAM lParam=NULL;
	WPARAM wParam=NULL;

	return false;
}

void * CHlaeBcServer::_DoCreateWindowExA(char *lpClassNameA,char *lpWindowNameA,int x, int y, int nHeight, int nWidth)
{
	if (_pHlaeGameWindow)
		return NULL; // window already present, we only allow one though
	else
	{
		wxString mycaption(lpWindowNameA,wxConvUTF8);

		_pHlaeGameWindow =new CHlaeGameWindow(this,_parent,wxID_ANY,wxDefaultPosition,wxSize(200,150),wxHSCROLL | wxVSCROLL,mycaption);

		_pHlaeGameWindow->SetVirtualSize(nWidth,nHeight);
		_pHlaeGameWindow->SetScrollRate(10,10);
		

		_pHlaeAuiManager->AddPane(_pHlaeGameWindow, wxAuiPaneInfo().RightDockable().Float().Caption(mycaption));
		return _pHlaeGameWindow->GetHWND();
	}
}

bool CHlaeBcServer::_DoDestroyWindow(WXHWND wxhWnd)
{
	if (!_pHlaeGameWindow) return false;

	if (_pHlaeGameWindow->GetHWND()==wxhWnd)
		return _pHlaeGameWindow->Destroy();
	else
		return false;
}