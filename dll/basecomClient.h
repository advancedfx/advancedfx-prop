#include <windows.h>

//
// mandatory functions
//

bool HlaeBc_OnFilmingStart();
// Shall be called i.e. by the Filming system to inform the Server that we are about to start filming
// returns false on fail, true otherwise

bool HlaeBc_OnFilmingStop();
// Shall be called i.e. by the Filming system to inform the Server that we stop filming
// returns false on fail, true otherwise

//
// WinAPI hooks, the Hlae Game Client Dll must install them all!:
//
HWND APIENTRY HlaeBcClt_CreateWindowExA(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam);
BOOL APIENTRY HlaeBcClt_DestroyWindow(HWND hWnd);
BOOL WINAPI HlaeBcClt_SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
ATOM APIENTRY HlaeBcClt_RegisterClassA(CONST WNDCLASSA *lpWndClass);

BOOL WINAPI HlaeBcClt_ReleaseCapture( VOID );
HWND WINAPI HlaeBcClt_SetCapture( HWND hWnd);

BOOL  WINAPI HlaeBcClt_SetPixelFormat(HDC hdc, int format, CONST PIXELFORMATDESCRIPTOR * ppfd);
HGLRC WINAPI HlaeBcClt_wglCreateContext(HDC);
BOOL WINAPI HlaeBcClt_wglDeleteContext(HGLRC hGlRc);
BOOL WINAPI HlaeBcClt_wglMakeCurrent(HDC, HGLRC);
int WINAPI HlaeBcClt_ReleaseDC( HWND hWnd, HDC hDC);

//
// support functions
//

HWND HlaeBc_GetGameWindow(void); // returns the captured GameWindow Handle


