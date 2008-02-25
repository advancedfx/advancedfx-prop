#include <windows.h>

//
// misc functions
//

// the following are mandatory:

void HlaeBcCl_AdjustViewPort(int &x, int &y, int width, int height);
// You have to use this one in glViewPort:
// Not only will this update the Sever's DC with the correct size
// but also adjust the viewport's x and y to reflect the Server's scrolling properly.

//
// WinAPI hooks, the Hlae Game Client Dll must install them all!:
//
ATOM APIENTRY HlaeBcClt_RegisterClassA(CONST WNDCLASSA *lpWndClass);
HWND APIENTRY HlaeBcClt_CreateWindowExA(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam);
BOOL APIENTRY HlaeBcClt_DestroyWindow(HWND hWnd);
HDC APIENTRY HlaeBcClt_GetDC( HWND hWnd );
int APIENTRY HlaeBcClt_ReleaseDC( HWND hWnd, HDC hDC );