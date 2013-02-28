#include "stdafx.h"

#include "user32Hooks.h"

#include <shared/detours.h>

#include "../AfxGoldSrcComClient.h"
#include "../supportrender.h"

HWND g_GameWindow = NULL;
bool g_GameWindowActive = false;
WNDPROC g_GameWindowProc = NULL;
bool g_GameWindowUndocked = false;
int g_Height = 0;
DWORD g_OldWindowStyle;
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

HWND APIENTRY NewCreateWindowExW(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam)
{
	if (NULL != hWndParent || lstrcmpW(L"",lpWindowName))
		// it's not the window we want.
		return CreateWindowExW(dwExStyle,lpClassName,lpWindowName,dwStyle,x,y,nWidth,nHeight,hWndParent,hMenu,hInstance,lpParam);

	// it's the window we want.
	
	g_Height = nHeight;
	g_Width = nWidth;
	
	g_AfxGoldSrcComClient.UpdateWindowSize(nHeight, nWidth);
	
	if(!g_AfxGoldSrcComClient.GetFullScreen())
	{
		// currently won't work:
		/*// modifiy some properities to our needs:
		dwStyle = WS_CHILD; // | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
		dwExStyle = WS_EX_NOPARENTNOTIFY;
		hWndParent = g_AfxGoldSrcComClient.GetParentWindow();
		x = 0;
		y = 0;*/
	}
	
	g_GameWindow = CreateWindowExW( dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam );

	g_GameWindowProc = (WNDPROC)GetWindowLongPtrW(g_GameWindow, GWLP_WNDPROC);
	//char t[100];
	//sprintf_s(t,"0x%08x",g_GameWindowProc);
	//MessageBox(0,t,"g_GameWIndowProc",MB_OK);

	// We can't set a new windowproc, this will get us an endless loop because SDL
	// is just fucked up, so we hook the SDL one:
	//SetWindowLongPtrW(g_GameWindow, GWLP_WNDPROC, (LONG)NewGameWindowProc);
	//g_GameWindowProc = (WNDPROC)DetourApply((BYTE *)g_GameWindowProc, (BYTE *)NewGameWindowProc, 0x09);

	return g_GameWindow;
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
	if(!g_AfxGoldSrcComClient.GetFullScreen() && g_GameWindowUndocked)
	{
		// restore old style and parent (see SetParent() on MSDN2, why we do it in this order):
		SetWindowLongPtr( g_GameWindow, GWL_STYLE, g_OldWindowStyle);
		//SetParent( g_GameWindow, g_AfxGoldSrcComClient.GetParentWindow() );
		SetWindowPos( g_GameWindow, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_SHOWWINDOW);

		g_GameWindowUndocked = false;
	}
}


void UndockGameWindowForCapture()
{
	if(!g_AfxGoldSrcComClient.GetFullScreen() && !g_GameWindowUndocked)
	{
		g_GameWindowUndocked = true;

		g_OldWindowStyle = GetWindowLongPtr(g_GameWindow, GWL_STYLE);

		// set new parent and style (see SetParent() on MSDN2, why we do it in this order):
		SetParent( g_GameWindow, NULL );
		SetWindowLongPtr( g_GameWindow, GWL_STYLE, WS_POPUP );
		SetWindowPos( g_GameWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_FRAMECHANGED|SWP_NOSIZE|SWP_SHOWWINDOW);
	}
}
