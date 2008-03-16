#include <windows.h>

//
// misc functions
//

// the following are mandatory:

void HlaeBcCl_AdjustViewPort(int x, int y, int width, int height);
// You have to use this one in glViewPort:
// will update the Sever's DC with the correct size

//
// WinAPI hooks, the Hlae Game Client Dll must install them all!:
//
HWND APIENTRY HlaeBcClt_CreateWindowExA(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam);
BOOL APIENTRY HlaeBcClt_DestroyWindow(HWND hWnd);
BOOL WINAPI HlaeBcClt_SetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);
BOOL APIENTRY HlaeBcCl_GetCursorPos(LPPOINT lpPoint); // Call this in a WINAPI SetCursorPos hook
ATOM APIENTRY HlaeBcClt_RegisterClassA(CONST WNDCLASSA *lpWndClass);
BOOL WINAPI HlaeBcClt_ReleaseCapture( VOID );
HWND WINAPI HlaeBcClt_SetCapture( HWND hWnd);
int WINAPI HlaeBcClt_ChoosePixelFormat(HDC hdc, PIXELFORMATDESCRIPTOR *ppfd);

