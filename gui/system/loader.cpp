// When time clean LoaderThread etcl up a bit please!
//
// Project :  Half-Life Advanced Effects
// File    :  gui/loader.cpp
// Changed :  2008-03-20
// Created :  2008-03-19

// Authors : last change / first change / name
// 2008-03-20 / 2007 / Dominik Tugend
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

#include <wx/wx.h>
#include <wx/filename.h>

#include <defines.h>
#include <system/debug.h>

#include "loader.h"


PROCESS_INFORMATION g_HLpi;		// global proccessinfo for HL, only valid when HL has been started by LoaderThread and is still runing
STARTUPINFO g_HLsi;				// I don't know why I made this global, used in LoaderThread

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)

HRESULT InjectDll(DWORD pID, const char *dllName)
// This function does not accept unicode.
// You may supply dllName as MBCS though (if you are not coding for win95).
{
	HANDLE hProc = OpenProcess(CREATE_THREAD_ACCESS, FALSE, pID);

	if (!hProc)
		return FALSE;

	LPVOID lLoadLibraryAddr = (LPVOID) GetProcAddress(GetModuleHandleA("kernel32.dll"),"LoadLibraryA");	

	if (!lLoadLibraryAddr)
	{
		CloseHandle(hProc);
		return FALSE;
	}

	LPVOID lArgAddress = VirtualAllocEx(hProc, NULL, strlen(dllName) + 1, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE); 

	if (!lArgAddress)
	{
		CloseHandle(hProc);
		return FALSE;
	}

	BOOL bResult = WriteProcessMemory(hProc, lArgAddress, dllName, strlen(dllName) + 1, NULL);

	if (!bResult)
	{
		VirtualFreeEx(hProc, lArgAddress, strlen(dllName) + 1, MEM_RELEASE|MEM_DECOMMIT);
		CloseHandle(hProc);
		return FALSE;
	}
	
	HANDLE hThread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE) lLoadLibraryAddr, lArgAddress, NULL, NULL);

	if (!hThread)
	{
		VirtualFreeEx(hProc, lArgAddress, strlen(dllName) + 1, MEM_RELEASE|MEM_DECOMMIT);
		CloseHandle(hProc);
		return FALSE;
	}

	VirtualFreeEx(hProc, lArgAddress, strlen(dllName) + 1, MEM_RELEASE|MEM_DECOMMIT);
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
wxString *g_path_dll;
wxString *g_path_exe;
wxString *g_dir_exe;
wxString *g_opts_exe;

bool g_bSignalDone=false;

DWORD WINAPI LoaderThread(void *p)
{
	//
	// Phase 1: Launch Half-Life
	//
	//	EnableDebugPrivilege(true);
	// Note: The CreateProcess call has to be made by the thread that want's to Debug!

	// check if the DLL exists:
	if (!::wxFileExists(*g_path_dll))
	{
		g_debug.SendMessage(wxString::Format(_T("%s not found!"), _T(HLAE_DLLNAME)), hlaeDEBUG_ERROR);
		g_bSignalDone=true;
		return FALSE;
	}

	LPCTSTR pszVersion = GetPluginVersion(g_path_dll->c_str());

	if (pszVersion)
	{
		wxString tstr;
		tstr.Printf(_T("Mirv Demo Tool (DLL v%s)"),wxString(pszVersion));
		g_debug.SendMessage(tstr,hlaeDEBUG_INFO);
	}

	// cunstruct the final launchoptions: "\"PATHWITHEXE\" LAUNCHOPTIONS"
	wxString strHLfinal;
	strHLfinal.Printf(_T("\"%s\" %s"),*g_path_exe,*g_opts_exe);
	//strHLfinal.Printf(_T("%s"),*g_path_exe,*g_opts_exe);

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

	g_debug.SendMessage(_T("Launching Half-Life ..."),hlaeDEBUG_INFO);

	wxString tstr(_T(""));
	const wxChar *ttarget = strHLfinal.c_str();
	wxChar *mybuff = tstr.GetWriteBuf(strHLfinal.Len());
	_tcscpy(mybuff,ttarget);

	if(!CreateProcess(
		g_path_exe->c_str(),
		mybuff,
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
		g_dir_exe->c_str(),
		&g_HLsi,
		&g_HLpi)  )
	{
		g_debug.SendMessage(_T("ERROR: Failed to launch Half-Life."),hlaeDEBUG_ERROR);
		g_bSignalDone=true;
		return FALSE;
	}

	tstr.UngetWriteBuf();

	g_debug.SendMessage(_T("Injecting hook ..."), hlaeDEBUG_VERBOSE_LEVEL3);

	if (!InjectDll(g_HLpi.dwProcessId, g_path_dll->mb_str(wxConvFile)))
		g_debug.SendMessage(_T("ERROR: Starting injection failed!"), hlaeDEBUG_ERROR);

	Sleep(2000);

	g_debug.SendMessage(_T("Resuming ..."), hlaeDEBUG_VERBOSE_LEVEL3);
	ResumeThread(g_HLpi.hThread);

	// I don't know why, but if we don't send a debug message here, the InitLoader will hang forever:
	g_debug.SendMessage(_T("Done, LoaderThread is about to die ..."), hlaeDEBUG_VERBOSE_LEVEL3);

	g_bSignalDone=true;
	return TRUE;
}

bool InitLoader(wxWindow *parent, wxString &m_path, wxString &m_cmdline)
{
	// built dll path:
	wxString m_cwd(::wxGetCwd());
	m_cwd+=_T("\\Mirv Demo Tool.dll");

	// built exe dir:
	wxString m_dir;
	wxFileName::SplitPath(m_path,&m_dir,NULL,NULL);
	
	g_path_dll = &m_cwd;
	g_path_exe = &m_path;
	g_opts_exe = &m_cmdline;
	g_dir_exe = &m_dir;

	g_debug.SendMessage(*g_path_dll, hlaeDEBUG_DEBUG);
	g_debug.SendMessage(*g_path_exe, hlaeDEBUG_DEBUG);
	g_debug.SendMessage(*g_opts_exe, hlaeDEBUG_DEBUG);
	g_debug.SendMessage(*g_dir_exe, hlaeDEBUG_DEBUG);

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

	return true;
}