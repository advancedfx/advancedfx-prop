#include "basecomClient.h"

#include "shared/com/basecom.h"

HWND g_hwHlaeBcCltWindow = NULL;
HWND g_hwHlaeGameWindow = NULL;

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
            return 0; 
 
        case WM_PAINT: 
            // Paint the window's client area. 
            return 0; 
 
        case WM_SIZE: 
            // Set the size and position of the window. 
            return 0; 
 
        case WM_DESTROY: 
            // Clean up window-specific data objects. 
            return 0; 

		case WM_COPYDATA:
			PCOPYDATASTRUCT pMyCDS;
			pMyCDS = (PCOPYDATASTRUCT) lParam;

			switch(pMyCDS->dwData)
			{
			case HLAE_BASECOM_MSG_EMPTY:
				MessageBoxW(hwnd,L"Got empty test data.",HLAE_BASECOM_CLIENT_ID,MB_OK);
				break;
			case HLAE_BASECOM_MSGCL_SENDWND:
				// the server sends us the window handle
				g_hwHlaeGameWindow = (HWND)wParam;
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

	if (!(g_hwHlaeBcCltWindow = CreateWindowW(wc.lpszClassName,HLAE_BASECOM_CLIENT_ID,WS_CHILD|WS_DISABLED|WS_POPUP,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInstance,NULL)))
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


HWND HlaeBcClt_retriveGameWindow()
{
	if(!g_hwHlaeBcCltWindow) return NULL;
	
	HWND hwServerWindow=FindWindowW(HLAE_BASECOM_CLASSNAME,HLAE_BASECOM_SERVER_ID);

	if (hwServerWindow)
	{
		COPYDATASTRUCT myCopyData;

		myCopyData.dwData=HLAE_BASECOM_MSGSV_GETGWND;
		myCopyData.cbData=0;
		myCopyData.lpData=NULL;

		SendMessage(hwServerWindow,WM_COPYDATA,(WPARAM)g_hwHlaeBcCltWindow,(LPARAM)&myCopyData);
		
		return g_hwHlaeGameWindow;
	}

	return false;
}