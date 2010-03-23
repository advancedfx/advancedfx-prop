#include "stdafx.h"

#include "user32Hooks.h"

#include "../AfxGoldSrcComClient.h"
#include "../supportrender.h"

HWND g_GameWindow = NULL;
bool g_GameWindowActive = false;
WNDCLASSA * g_GameWindowClass = NULL;
WNDPROC g_GameWindowProc = NULL;
bool g_GameWindowUndocked = false;
int g_Height = 0;
int g_Width = 0;


LRESULT CALLBACK NewGameWindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	// one could filter window unspecific messages here.

	if (hWnd==NULL || hWnd != g_GameWindow)
		// this is not the GameWindow we want to control
		return g_GameWindowProc(hWnd,uMsg,wParam,lParam);

	// filter GameWindow specific messages:

	switch (uMsg)
	{
	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE )
			g_GameWindowActive = false;
		break;
	case WM_MOUSEACTIVATE:
		if( !g_AfxGoldSrcComClient.GetFullScreen() && !g_GameWindowUndocked && !g_GameWindowActive )
		{
			// Client Windows won't recieve window activation events,
			// so we will fake them:
			g_GameWindowActive = true;

			LRESULT lr = g_GameWindowProc(hWnd,uMsg,wParam,lParam); // pass on WM_MOUSEACTIVATE
			g_GameWindowProc(hWnd, WM_ACTIVATEAPP, TRUE, NULL);
			g_GameWindowProc(hWnd, WM_ACTIVATE, WA_ACTIVE, NULL);//lParam);

			// Don't let strange mods like Natural Selection mess with us:
			ShowCursor(TRUE);
			return lr;
		}
		// Don't let strange mods like Natural Selection mess with us:
		ShowCursor(TRUE);
		break;
	case WM_SETFOCUS:
		break;
	case WM_KILLFOCUS:
		if( !g_AfxGoldSrcComClient.GetFullScreen() && !g_GameWindowUndocked && g_GameWindowActive )
		{
			g_GameWindowActive = false;
			g_GameWindowProc(hWnd, WM_ACTIVATE, WA_INACTIVE, NULL);//lParam);
			g_GameWindowProc(hWnd, WM_ACTIVATEAPP, FALSE, NULL);
			return g_GameWindowProc(hWnd, uMsg,wParam,lParam); // PASS ON WM_KILLFOCUS
		}
		break;
	}

	return g_GameWindowProc(hWnd, uMsg, wParam, lParam);
}


HWND APIENTRY NewCreateWindowExA(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam)
{
	if (NULL != hWndParent)
		// it's not the window we want.
		return CreateWindowExA(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);

	// it's the window we want.
	
	g_Height = nHeight;
	g_Width = nWidth;
	
	g_AfxGoldSrcComClient.UpdateWindowSize(nHeight, nWidth);
	
	if(!g_AfxGoldSrcComClient.GetFullScreen())
	{
		// modifiy some properities to our needs:
		dwStyle = WS_CHILD; // | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
		dwExStyle = WS_EX_NOPARENTNOTIFY;
		hWndParent = g_AfxGoldSrcComClient.GetParentWindow();
		x = 0;
		y = 0;
	}
	
	return g_GameWindow = CreateWindowExA( dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam );
}


BOOL APIENTRY NewDestroyWindow(HWND hWnd)
{
	if (hWnd != NULL && hWnd == g_GameWindow)
	{
		// H-L main game window being destroyed

		// Close Communication with AfxServer:
		g_AfxGoldSrcComClient.Close();

		g_GameWindow = NULL;

		if (g_pSupportRender) delete g_pSupportRender;
	}

	return DestroyWindow(hWnd);
}


ATOM APIENTRY NewRegisterClassA(CONST WNDCLASSA *lpWndClass)
{
	// quit if it's not the class we want:
	if (!HIWORD(lpWndClass->lpszClassName) || lstrcmp(lpWndClass->lpszClassName,"Valve001"))
		return RegisterClassA(lpWndClass);

	// it's the class we want.

	g_GameWindowClass = const_cast<WNDCLASSA *>(lpWndClass);
	g_GameWindowProc = g_GameWindowClass->lpfnWndProc;

	// modify it to meet our needs:
	g_GameWindowClass->lpfnWndProc = NewGameWindowProc;

	if(!g_AfxGoldSrcComClient.GetFullScreen())
		g_GameWindowClass->style = CS_OWNDC;

	return RegisterClassA(g_GameWindowClass);
}


BOOL WINAPI NewSetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
{
	if( NULL == hWnd
		|| hWnd != g_GameWindow
		|| g_AfxGoldSrcComClient.GetFullScreen() )
		return SetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);

	if ( !(uFlags & SWP_NOSIZE) && (cx != g_Width || cy != g_Height ) )
	{
		g_Width = cx;
		g_Height = cy;

		g_AfxGoldSrcComClient.UpdateWindowSize(cx, cy);
	}

	return SetWindowPos(hWnd,HWND_TOP,0,0,cx,cy,uFlags);//SWP_SHOWWINDOW);
}


void CloseGameWindow()
{
	if(0 != g_GameWindow)
	{
		SendMessage(g_GameWindow, WM_CLOSE, 0, 0);
	}
}


void RedockGameWindow()
{
	if(!g_AfxGoldSrcComClient.GetFullScreen())
	{
		// restore old style and parent (see SetParent() on MSDN2, why we do it in this order):
		SetWindowLongPtr( g_GameWindow, GWL_STYLE, WS_CHILD ); // WS_CHILD has to be set first
		SetParent( g_GameWindow, g_AfxGoldSrcComClient.GetParentWindow() );
		SetWindowPos( g_GameWindow, HWND_TOP, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_SHOWWINDOW);

		g_GameWindowUndocked = false;
	}
}


void UndockGameWindowForCapture()
{
	if(!g_AfxGoldSrcComClient.GetFullScreen())
	{
		g_GameWindowUndocked = true;

		// set new parent and style (see SetParent() on MSDN2, why we do it in this order):
		SetParent( g_GameWindow, NULL );
		SetWindowLongPtr( g_GameWindow, GWL_STYLE, WS_POPUP );
		SetWindowPos( g_GameWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_SHOWWINDOW);
	}
}
