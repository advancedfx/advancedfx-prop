
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <tlhelp32.h>

#include "Resource.h"

#pragma comment(lib, "version.lib")

#define DLL_NAME	"Mirv Demo Tool.dll"

HWND hWnd;
HFONT hFont;
HBRUSH hBrush;

typedef char * (*pfnGetMDTVersion_t)();

LRESULT CALLBACK DlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DWORD GetProcessId(LPCTSTR lpName);
VOID InitLoader();
HRESULT InjectDll(DWORD pID, LPCTSTR name);
BOOL DoesFileExist(LPCTSTR pszName);



INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
{
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN),
	          hWnd, reinterpret_cast<DLGPROC> (DlgProc));

	hWnd = 0;
	DeleteObject(hBrush);
	DeleteObject(hFont);

	return FALSE;
}

LRESULT CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		hWnd = hWndDlg;
		InitLoader();
		return TRUE;

	case WM_COMMAND:
		switch(wParam)
		{
		case IDOK: case IDCANCEL:
			EndDialog(hWndDlg, 0);
			return TRUE;
		}
		break;

	case WM_DESTROY:
		EndDialog(hWndDlg, 0);
		return TRUE;

	case WM_CTLCOLORDLG:
        return (LRESULT) hBrush;

	case WM_CTLCOLORSTATIC:
		HWND hWnd = (HWND) lParam;
		HDC hDC = (HDC) wParam;

		if (GetWindowLong(hWnd, GWL_ID) == IDC_DISCLAIMER)
		{
			SetTextColor(hDC, GetSysColor(COLOR_GRAYTEXT));
			SetBkMode(hDC, TRANSPARENT);
			return (LRESULT) hBrush;
		}
	}

	return FALSE;
}

DWORD WINAPI LoaderThread(void *p)
{
	static char szBuffer[1024];
	static char *pszAnim[] = { ".", "..", "...", "...." };
	static int nAnim = sizeof(pszAnim) / sizeof(pszAnim[0]);
	static int iAnim = 0;

	for(; hWnd; Sleep(200))
	{
		DWORD dwProcessId = GetProcessId("hl.exe");

		if (dwProcessId != 0)
		{
			if (hWnd)
				SetDlgItemText(hWnd, IDC_STATUS, "Loading MDT into Half-Life");

			// Injecting too quickly will crash HL
			Sleep(100);

			// Get the full path to the dll
			DWORD nLen = GetCurrentDirectory(1024, szBuffer);
			sprintf(szBuffer + nLen, "\\%s", DLL_NAME);

			if (InjectDll(dwProcessId, szBuffer))
			{
				Sleep(3000);
				SendMessage(hWnd, WM_DESTROY, 0, 0);
				return 1;
			}
		}

		if (hWnd)
		{
			sprintf(szBuffer, "Waiting for Half-Life%s", pszAnim[iAnim++ % nAnim]);
			SetDlgItemText(hWnd, IDC_STATUS, szBuffer);
		}
	}
	return 0;
}

const char *GetPluginVersion(const char *pszName)
{
	static char szVersion[128] = { 0 };

	VS_FIXEDFILEINFO *lpFFI;
	DWORD dwBufSize;
	char *lpVI;

	DWORD dwLen = GetFileVersionInfoSize(pszName, &dwBufSize);

	if (dwLen == 0)
		return NULL;

	lpVI = (LPTSTR) GlobalAlloc(GPTR, dwLen);

	GetFileVersionInfo(pszName, NULL, dwLen, lpVI);

	if (VerQueryValue(lpVI, _T("\\"), (LPVOID *) &lpFFI, (UINT *) &dwBufSize))
	{
		sprintf(szVersion, "%d.%d.%d.%d", HIWORD(lpFFI->dwFileVersionMS),
											LOWORD(lpFFI->dwFileVersionMS),
											HIWORD(lpFFI->dwFileVersionLS),
											LOWORD(lpFFI->dwFileVersionLS));
	}
	else
		return NULL;

	GlobalFree((HGLOBAL) lpVI);

	return szVersion;
}

VOID InitLoader()
{
	hBrush = CreateSolidBrush(GetSysColor(CTLCOLOR_DLG));

	hFont = CreateFont(12,
							 0,
							 0,
							 0,
							 FW_NORMAL,
							 FALSE,
							 FALSE,
							 FALSE,
							 ANSI_CHARSET,
							 OUT_DEFAULT_PRECIS,
							 CLIP_DEFAULT_PRECIS,
							 DEFAULT_QUALITY,
							 DEFAULT_PITCH | FF_DONTCARE,
							 "Tahoma");

	SendMessage(GetDlgItem(hWnd, IDC_DISCLAIMER), WM_SETFONT, (WPARAM) hFont, (LPARAM)FALSE);

	if (DoesFileExist(DLL_NAME) == FALSE)
	{
		SetDlgItemText(hWnd, IDC_STATUS, DLL_NAME " not found!");
		return;
	}

	const char *pszVersion = GetPluginVersion(DLL_NAME);

	if (pszVersion)
	{
		char buf[512];
		sprintf(buf, "Mirv Demo Tool v%s", pszVersion);
		SetWindowText(hWnd, buf);
	}

	HANDLE hThread = CreateThread(0, 0, LoaderThread, 0, 0, 0);
}

DWORD GetProcessId(LPCTSTR lpName)
{
	HANDLE hAllProc = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 hProcEntry;
	hProcEntry.dwSize = sizeof(PROCESSENTRY32);
	hAllProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hAllProc == INVALID_HANDLE_VALUE)
		return 0;

	if (Process32First(hAllProc, &hProcEntry))
	{
		while (1)
		{
			if (hProcEntry.th32ProcessID != 0)
			if (!stricmp(hProcEntry.szExeFile, lpName))
			{
				CloseHandle(hAllProc);
				return hProcEntry.th32ProcessID;
			}

            if (!Process32Next(hAllProc, &hProcEntry))
				break;
		}
	}

	CloseHandle(hAllProc);
	return 0;
}

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)

HRESULT InjectDll(DWORD pID, LPCTSTR dllName)
{
	HANDLE hProc = OpenProcess(CREATE_THREAD_ACCESS, FALSE, pID);

	if (!hProc)
		return false;

	LPVOID lLoadLibraryAddr = (LPVOID) GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");	

	if (!lLoadLibraryAddr)
	{
		CloseHandle(hProc);
		return false;
	}

	LPVOID lArgAddress = VirtualAllocEx(hProc, NULL, strlen(dllName) + 1, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE); 

	if (!lArgAddress)
	{
		CloseHandle(hProc);
		return false;
	}

	BOOL bResult = WriteProcessMemory(hProc, lArgAddress, dllName, strlen(dllName) + 1, NULL);

	if (!bResult)
	{
		VirtualFreeEx(hProc, lArgAddress, strlen(dllName) + 1, MEM_RELEASE|MEM_DECOMMIT);
		CloseHandle(hProc);
		return false;
	}
	
	HANDLE hThread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE) lLoadLibraryAddr, lArgAddress, NULL, NULL);

	if (!hThread)
	{
		VirtualFreeEx(hProc, lArgAddress, strlen(dllName) + 1, MEM_RELEASE|MEM_DECOMMIT);
		CloseHandle(hProc);
		return false;
	}

	VirtualFreeEx(hProc, lArgAddress, strlen(dllName) + 1, MEM_RELEASE|MEM_DECOMMIT);
	CloseHandle(hProc);

	return true;
}

BOOL DoesFileExist(LPCTSTR pszName)
{
	FILE *pFile = fopen(DLL_NAME, "r");
	if (!pFile)
		return FALSE;
	fclose(pFile);
	return TRUE;
}
