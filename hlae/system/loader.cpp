#include "stdafx.h"

// Project :  Half-Life Advanced Effects
// File    :  hlae/loader.cpp
// Changed :  2008-05-15
// Created :  2008-03-19

// Authors : last change / first change / name
// 2008-05-15 / 2007 / Dominik Tugend
// 2007 / / Gavin Bramhill

// Comment:
//
//   Microsoft "Unicode and MBCS":
//   http://msdn2.microsoft.com/en-us/library/aa296564(VS.60).aspx
//
//   Use the _T macro to conditionally code literal strings to be portable to Unicode.
//   When you pass strings, pay attention to whether function arguments require a length in characters or a length in bytes. The difference is important if you’re using Unicode strings.
//   Use portable versions of the C run-time string-handling functions.
//   TCHAR   Where you would use char.
//   LPTSTR   Where you would use char*.
//   LPCTSTR   Where you would use const char*. CString provides the operator LPCTSTR to convert between CString and LPCTSTR.

#include "loader.h"

#pragma unmanaged

#define WIN32_LEAN_AND_MEAN
#include <tchar.h>
#include <windows.h>
#include <stdlib.h> // remove later, for _itot

#pragma comment(lib, "version.lib") // GEtFileVersion...
#pragma comment(lib, "User32.lib") // MessageBox, remove later

PROCESS_INFORMATION g_HLpi;		// global proccessinfo for HL, only valid when HL has been started by LoaderThread and is still runing
STARTUPINFO g_HLsi;				// I don't know why I made this global, used in LoaderThread

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)

HRESULT InjectDll(DWORD pID, LPCTSTR dllName)
{
	HANDLE hProc = OpenProcess(CREATE_THREAD_ACCESS, FALSE, pID);

	if (!hProc)
		return FALSE;

	LPVOID lLoadLibraryAddr = (LPVOID) GetProcAddress(
		GetModuleHandle( _T("kernel32.dll") ),
#ifdef _UNICODE
		"LoadLibraryW"
#elif
		"LoadLibraryA"
#endif
		);	

	if (!lLoadLibraryAddr)
	{
		CloseHandle(hProc);
		return FALSE;
	}

	LPVOID lArgAddress = VirtualAllocEx(hProc, NULL, sizeof( _TCHAR ) * (_tcsclen(dllName) + 1), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE); 

	if (!lArgAddress)
	{
		CloseHandle(hProc);
		return FALSE;
	}

	BOOL bResult = WriteProcessMemory(hProc, lArgAddress, dllName, sizeof( _TCHAR ) * (_tcsclen(dllName) + 1), NULL);

	if (!bResult)
	{
		VirtualFreeEx(hProc, lArgAddress, sizeof( _TCHAR ) * (_tcsclen(dllName) + 1), MEM_RELEASE|MEM_DECOMMIT);
		CloseHandle(hProc);
		return FALSE;
	}
	
	HANDLE hThread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE) lLoadLibraryAddr, lArgAddress, NULL, NULL);

	if (!hThread)
	{
		VirtualFreeEx(hProc, lArgAddress, sizeof( _TCHAR ) * (_tcsclen(dllName) + 1), MEM_RELEASE|MEM_DECOMMIT);
		CloseHandle(hProc);
		return FALSE;
	}

	VirtualFreeEx(hProc, lArgAddress, sizeof( _TCHAR ) * (_tcsclen(dllName) + 1), MEM_RELEASE|MEM_DECOMMIT);
	CloseHandle(hProc);

	return TRUE;
}

LPCTSTR GetPluginVersion(LPCTSTR pszName)
// this function supports MBCS and Unicode
// (hopefully ...)
{
	static TCHAR szVersion[128] = { 0 };

	VS_FIXEDFILEINFO *lpFFI;
	DWORD dwBufSize;
	char *lpVI;

	DWORD dwLen = GetFileVersionInfoSize(pszName, &dwBufSize);

	if (dwLen == 0)
		return NULL;

	lpVI = (char *) GlobalAlloc(GPTR, dwLen);

	GetFileVersionInfo(pszName, NULL, dwLen, lpVI);

	if (VerQueryValue(lpVI, _T("\\"), (LPVOID *) &lpFFI, (UINT *) &dwBufSize))
	{
		_stprintf(szVersion, _T("%d.%d.%d.%d"),
			HIWORD(lpFFI->dwFileVersionMS),
			LOWORD(lpFFI->dwFileVersionMS),
			HIWORD(lpFFI->dwFileVersionLS),
			LOWORD(lpFFI->dwFileVersionLS)
		);
	}
	else
		return NULL;

	GlobalFree((HGLOBAL) lpVI);

	return szVersion;
}
LPTSTR g_path_dll;
LPTSTR g_path_exe;
LPTSTR g_dir_exe;
LPTSTR g_opts_exe;

bool g_bSignalDone=false;

DWORD WINAPI LoaderThread(void *p)
{
	//MessageBox(0,g_path_dll,_T("g_path_dll"),MB_OK|MB_ICONINFORMATION);

	//
	// Phase 1: Launch Half-Life
	//
	//	EnableDebugPrivilege(true);
	// Note: The CreateProcess call has to be made by the thread that want's to Debug!

	// check if the DLL exists:

	//
	// not implemented
	//

	LPCTSTR pszVersion = GetPluginVersion(g_path_dll);

	if (!pszVersion)
	{
//		MessageBox( 0, _T("Could not retrive mdt dll version info."), _T("Error"), MB_OK|MB_ICONERROR );
		//g_debug.SendMessage(wxString::Format(_T("Could not retrive info from %s."), _T(HLAE_DLLNAME)), hlaeDEBUG_ERROR);
//		g_bSignalDone=true;
//		return FALSE;
	} else {
		//wxString tstr;
		//tstr.Printf(_T("Mirv Demo Tool (DLL v%s)"),wxString(pszVersion));
		//g_debug.SendMessage(tstr,hlaeDEBUG_INFO);
	}

	// there should be some detecton if HL is already running here:
	// and if it can be launched (file exists):
	
	//
	// not implemented yet.
	//

	// Launch Half-Life:
    ZeroMemory( &g_HLsi, sizeof(g_HLsi) );
	//GetStartupInfo(&g_HLsi);
    g_HLsi.cb = sizeof(g_HLsi);
    ZeroMemory( &g_HLpi, sizeof(g_HLpi) );

	//g_debug.SendMessage(_T("Launching Half-Life ..."),hlaeDEBUG_INFO);

	if(!CreateProcess(
		g_path_exe,
		g_opts_exe,
		NULL,
		NULL,
		FALSE,
			//CREATE_DEFAULT_ERROR_MODE|
			CREATE_NEW_PROCESS_GROUP|
			DETACHED_PROCESS|
			CREATE_SUSPENDED
			//DEBUG_ONLY_THIS_PROCESS|
			//DEBUG_PROCESS				// we want to catch debug event's (sadly also of childs)
			,
		NULL,
		g_dir_exe,
		&g_HLsi,
		&g_HLpi)  )
	{
		TCHAR strerr[33];
		_itot(GetLastError(),strerr,10);
		MessageBox( 0, _T("Failed to launch Half-Life."), (LPCTSTR)strerr, MB_OK|MB_ICONERROR );
		//g_debug.SendMessage(_T("ERROR: Failed to launch Half-Life."),hlaeDEBUG_ERROR);
		g_bSignalDone=true;
		return FALSE;
	}

	//MessageBox( 0, _T("Injecting hook ..."), _T("Error"), MB_OK|MB_ICONERROR );

	if (!InjectDll(g_HLpi.dwProcessId, g_path_dll))
		MessageBox( 0, _T("Starting injection failed."), _T("Error"), MB_OK|MB_ICONERROR );
		//;
		//g_debug.SendMessage(_T("ERROR: Starting injection failed!"), hlaeDEBUG_ERROR);

	Sleep(2000);

	//g_debug.SendMessage(_T("Resuming ..."), hlaeDEBUG_VERBOSE_LEVEL3);
	ResumeThread(g_HLpi.hThread);

	// I don't know why, but if we don't send a debug message here, the InitLoader will hang forever:
	//g_debug.SendMessage(_T("Done, LoaderThread is about to die ..."), hlaeDEBUG_VERBOSE_LEVEL3);

	g_bSignalDone=true;
	return TRUE;
}

#pragma managed

using namespace System;
using namespace System::Runtime::InteropServices;


bool CustomLoader(System::String ^ hookPath, System::String ^ programPath, System::String ^ cmdline)
{

	System::String ^strExeDir = gcnew System::String( System::IO::Path::GetDirectoryName( programPath ) );

	System::Text::StringBuilder ^strOptsB = gcnew System::Text::StringBuilder( "\"" );
	strOptsB->Append( programPath );
	strOptsB->Append( "\" " );
	strOptsB->Append( cmdline );

#ifdef _UNICODE
	g_path_dll = (LPTSTR)(int)Marshal::StringToHGlobalUni( hookPath->ToString() );
	g_path_exe = (LPTSTR)(int)Marshal::StringToHGlobalUni( programPath );
	g_opts_exe = (LPTSTR)(int)Marshal::StringToHGlobalUni( strOptsB->ToString() );
	g_dir_exe = (LPTSTR)(int)Marshal::StringToHGlobalUni( strExeDir );
#else
	g_path_dll = (LPTSTR)(int)Marshal::StringToHGlobalAnsi( hookPath->ToString() );
	g_path_exe = (LPTSTR)(int)Marshal::StringToHGlobalAnsi( programPath );
	g_opts_exe = (LPTSTR)(int)Marshal::StringToHGlobalAnsi( strOptsB->ToString() );
	g_dir_exe = (LPTSTR)(int)Marshal::StringToHGlobalAnsi( strExeDir );
#endif

	g_bSignalDone = false;
	HANDLE hThread = CreateThread(0, 0, LoaderThread, 0, 0, 0);

	if (hThread!=NULL)
	{
		while (!g_bSignalDone)
		{
			// g_debug.SendMessage(_T("Waiting for LoaderThread ..."), hlaeDEBUG_DEBUG);
			Sleep(500);
		}
	}

	Marshal::FreeHGlobal( (System::IntPtr)g_dir_exe );
	Marshal::FreeHGlobal( (System::IntPtr)g_opts_exe );
	Marshal::FreeHGlobal( (System::IntPtr)g_path_exe );
	Marshal::FreeHGlobal( (System::IntPtr)g_path_dll );

	return true;
}