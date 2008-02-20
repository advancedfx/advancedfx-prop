#include "basecomClient.h"

#include "shared/com/basecom.h"

// In order to understand the source that follows the Microsoft Documentation of WM_COPYDATA might be useful.

HWND g_hwHlaeBcCltWindow = NULL;
HWND g_hwHlaeGameWindow = NULL;

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
			case HLAE_BASECOM_MSG_TESTDUMMY:
				MessageBoxW(hwnd,L"Got empty test data.",HLAE_BASECOM_CLIENT_ID,MB_OK);
				return TRUE;
			case HLAE_BASECOM_MSGCL_RET_CreateWindowExA:
				MessageBox(0,"TEST","Hello",MB_OK);
				if (g_pHlaeBcResultTarget) memcpy(g_pHlaeBcResultTarget,pMyCDS->lpData,sizeof(HLAE_BASECOM_RET_CreateWindowExA_s));
				if(pMyCDS->lpData == NULL) MessageBox(0,"NULL","0000",MB_OK);
				return TRUE;
			case HLAE_BASECOM_MSGCL_RET_RegisterClassA:
				if (g_pHlaeBcResultTarget) memcpy(g_pHlaeBcResultTarget,pMyCDS->lpData,sizeof(HLAE_BASECOM_RET_RegisterClassA_s));
				return TRUE;
			case HLAE_BASECOM_MSGCL_RET_DestroyWindow:
				if (g_pHlaeBcResultTarget) memcpy(g_pHlaeBcResultTarget,pMyCDS->lpData,sizeof(HLAE_BASECOM_RET_DestroyWindow_s));
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
{
	if (!g_hwHlaeBcCltWindow) return true;

	if (!DestroyWindow(g_hwHlaeBcCltWindow)) return false;

	g_hwHlaeBcCltWindow = NULL;
	return true;
}

bool HlaeBcCltSendMessage(DWORD dwId,DWORD cbSize,PVOID lpData)
// use this for one-way messages
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
// use this for messages that expect a result
{
	bool bRes;

	if (!g_hwHlaeBcCltWindow) return false;

	g_pHlaeBcResultTarget = pResultTarget;
	bRes=HlaeBcCltSendMessage(dwId,cbSize,lpData);
	g_pHlaeBcResultTarget = NULL;

	return bRes;
}

HWND HlaeBcClt_CreateWindowExA(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam)
{
	HWND hwRetWin=NULL;

	HLAE_BASECOM_RET_CreateWindowExA_s *mycwret = new HLAE_BASECOM_RET_CreateWindowExA_s;
	HLAE_BASECOM_CreateWindowExA_s *mycws;

	size_t cbBase=sizeof(HLAE_BASECOM_CreateWindowExA_s);
	size_t cbClassName=strlen(lpClassName)+1;
	size_t cbWindowName=strlen(lpWindowName)+1;
	size_t cbPiggyBack=cbClassName+cbWindowName;

	mycws=(HLAE_BASECOM_CreateWindowExA_s *)malloc(cbBase+cbPiggyBack);

	mycws->dwExStyle = dwExStyle;
	mycws->lpClassName = (LPCTSTR)cbBase; memcpy((char *)mycws + cbBase,lpClassName,cbClassName);
	mycws->lpWindowName = (LPCTSTR)(cbBase + cbClassName); memcpy((char *)mycws + cbBase + cbClassName,lpWindowName,cbWindowName);
	mycws->dwStyle = dwStyle;
	mycws->x = x;
	mycws->y = y ;
	mycws->nWidth = nWidth;
	mycws->nHeight = nHeight;
	mycws->hWndParent = hWndParent;
	mycws->hMenu = hMenu;
	mycws->hInstance = hInstance;
	mycws->lpParam = lpParam;

	HlaeBcCltSendMessageRet(HLAE_BASECOM_MSGSV_CreateWindowExA,cbBase+cbPiggyBack,(PVOID)mycws,mycwret);
	hwRetWin = mycwret->retResult;

	free(mycws);
	delete mycwret;

	if (hwRetWin)
		return hwRetWin;
	else
		// s.th. failed, create window as desired within engine:
		return CreateWindowExA(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);

}

ATOM HlaeBcClt_RegisterClassA(CONST WNDCLASSA *lpWndClass)
{
	ATOM aRetAtom=NULL;

	HLAE_BASECOM_RET_RegisterClassA_s *mycwret = new HLAE_BASECOM_RET_RegisterClassA_s;
	HLAE_BASECOM_RegisterClassA_s *mycws;
	
	size_t cbBase=sizeof(HLAE_BASECOM_CreateWindowExA_s);
	size_t cbClassName=strlen(lpWndClass->lpszClassName)+1;
	size_t cbMenuName=strlen(lpWndClass->lpszMenuName)+1;
	size_t cbPiggyBack=cbClassName+cbMenuName;	
	
	mycws = (HLAE_BASECOM_RegisterClassA_s *)malloc(cbBase+cbPiggyBack);

	memcpy(mycws,lpWndClass,sizeof(WNDCLASSA));

	mycws->lpszClassName = (LPCTSTR)cbBase; memcpy((char *)mycws + cbBase,lpWndClass->lpszClassName,cbClassName);
	mycws->lpszMenuName = (LPCTSTR)(cbBase + cbClassName); memcpy((char *)mycws + cbBase + cbClassName,lpWndClass->lpszMenuName,cbMenuName);

	HlaeBcCltSendMessageRet(HLAE_BASECOM_MSGSV_RegisterClassA,cbBase+cbPiggyBack,(PVOID)mycws,mycwret);
	aRetAtom = mycwret->retResult;

	free(mycws);
	delete mycwret;

	if (aRetAtom)
		return aRetAtom;
	else
		return RegisterClassA(lpWndClass);
}

BOOL HlaeBcClt_DestroyWindow(HWND hWnd)
{
	BOOL bRes;
	BOOL bHandled;

	HLAE_BASECOM_RET_DestroyWindow_s *mycwret = new HLAE_BASECOM_RET_DestroyWindow_s;
	HLAE_BASECOM_DestroyWindow_s *mycws = new HLAE_BASECOM_DestroyWindow_s;

	mycws->hWnd = hWnd;

	bHandled=HlaeBcCltSendMessageRet(HLAE_BASECOM_MSGSV_DestroyWindow,sizeof(HLAE_BASECOM_DestroyWindow_s),(PVOID)mycws,mycwret);
	bRes = mycwret->retResult;

	delete mycws;
	delete mycwret;

	if (bHandled)
		return bRes;
	else
		return DestroyWindow(hWnd);
}