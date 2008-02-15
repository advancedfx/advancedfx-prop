#include <windows.h>

bool HlaeBcCltStart();
bool HlaeBcCltStop();

HWND HlaeBcClt_CreateWindowExA(DWORD dwExStyle,LPCTSTR lpClassName,LPCTSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam);
ATOM HlaeBcClt_RegisterClassA(CONST WNDCLASSA *lpWndClass);
BOOL HlaeBcClt_DestroyWindow(HWND hWnd);