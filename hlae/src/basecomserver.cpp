
#include "basecomServer.h"

#include <windows.h>

#include "shared/com/basecom.h"

HWND g_hwHlaeBcSrvWindow = NULL;
HWND g_hwHlaeGameWindow = NULL;

LRESULT CALLBACK HlaeBcSrvWndProc(
    HWND hwnd,        // handle to window
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{ 
 
    switch (uMsg) 
    { 
        case WM_CREATE: 
            // Initialize the window.
			//MessageBox(hwnd,L"WM_CREATE",L"HLAE BaseCom Server",MB_OK|MB_ICONINFORMATION);
            return 0; 
 
        case WM_PAINT: 
            // Paint the window's client area. 
            return 0; 
 
        case WM_SIZE: 
            // Set the size and position of the window. 
            return 0; 
 
        case WM_DESTROY: 
            // Clean up window-specific data objects. 
			//MessageBox(hwnd,L"WM_DESTROY",L"HLAE BaseCom Server",MB_OK|MB_ICONINFORMATION);
            return 0; 

		case WM_COPYDATA:
			PCOPYDATASTRUCT pMyCDS;
			pMyCDS = (PCOPYDATASTRUCT) lParam;

			switch(pMyCDS->dwData)
			{
			case HLAE_BASECOM_MSG_EMPTY:
				MessageBoxW(hwnd,L"Got empty test data.",HLAE_BASECOM_CLIENT_ID,MB_OK);
				break;
			case HLAE_BASECOM_MSGSV_GETGWND:
				// client requests window
				COPYDATASTRUCT myCopyData;

				myCopyData.dwData=HLAE_BASECOM_MSGCL_SENDWND;
				myCopyData.cbData=0;
				myCopyData.lpData=NULL;

				SendMessage((HWND)wParam,WM_COPYDATA,(WPARAM)g_hwHlaeGameWindow,(LPARAM)&myCopyData);
				
				break;
			default:
				MessageBoxW(hwnd,L"Unexpected message.",HLAE_BASECOM_CLIENT_ID,MB_OK|MB_ICONERROR);
			}
			return 0;
 
        default: 
            return DefWindowProc(hwnd, uMsg, wParam, lParam); 
    } 
    return 0; 
}


bool HlaeBcSrvStart(unsigned long ulInstance,void *lpGameWindow)
{
	HINSTANCE hInstance = (HINSTANCE) ulInstance;
	
	g_hwHlaeGameWindow = (HWND) lpGameWindow;

	if (g_hwHlaeBcSrvWindow)
		return false; // if already running quit

	static bool bRegistered=false;
	static WNDCLASS wc;

	if (!bRegistered)
	{
 
		// Register the main window class. 
		wc.style = NULL; 
		wc.lpfnWndProc = (WNDPROC) HlaeBcSrvWndProc; 
		wc.cbClsExtra = 0; 
		wc.cbWndExtra = 0; 
		wc.hInstance = hInstance; 
		wc.hIcon = NULL; 
		wc.hCursor = NULL; 
		wc.hbrBackground = NULL; 
		wc.lpszMenuName =  NULL; 
		wc.lpszClassName = HLAE_BASECOM_CLASSNAME;

		if (!RegisterClass(&wc))
			return false;

		bRegistered = true;
	 }

	if (!(g_hwHlaeBcSrvWindow = CreateWindow(wc.lpszClassName,HLAE_BASECOM_SERVER_ID,WS_CHILD|WS_DISABLED|WS_POPUP,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInstance,NULL)))
		return false;

	return true;
}

bool HlaeBcSrvStop()
{
	if (!g_hwHlaeBcSrvWindow) return true;

	if (!DestroyWindow(g_hwHlaeBcSrvWindow)) return false;

	g_hwHlaeBcSrvWindow = NULL;
	return true;
}