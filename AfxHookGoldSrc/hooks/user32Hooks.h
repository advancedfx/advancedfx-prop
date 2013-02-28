#pragma once

extern HWND g_GameWindow;
extern int g_Height;
extern int g_Width;

HWND APIENTRY NewCreateWindowExW(DWORD dwExStyle,LPCWSTR lpClassName,LPCWSTR lpWindowName,DWORD dwStyle,int x,int y,int nWidth,int nHeight,HWND hWndParent,HMENU hMenu,HINSTANCE hInstance,LPVOID lpParam);
BOOL APIENTRY NewDestroyWindow(HWND hWnd);
BOOL WINAPI NewSetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags);

void CloseGameWindow();
void RedockGameWindow();
void UndockGameWindowForCapture();
