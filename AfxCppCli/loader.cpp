#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-05-26 by dominik.matrixstorm.com
//
// First changes:
// 2007 by dominik.matrixstorm.com
// 2007 by Gavin Bramhill

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
#include <stdlib.h>
#include <stdio.h>

#pragma comment(lib, "version.lib") // GEtFileVersion...
#pragma comment(lib, "User32.lib") // MessageBox, remove later

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)


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
		_stprintf_s(szVersion, _T("%d.%d.%d.%d"),
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


class AfxHook
{
public:
	AfxHook(LPCTSTR imageFileName)
	{
		m_BootImage = 0;
		m_BootImageSize = 0;

		LoadImage(imageFileName);
	}

	~AfxHook()
	{
		FreeImage();
	}


	bool Inject(
		LPCTSTR programPath, LPCTSTR programDirectory, LPTSTR programOptions,
		LPCTSTR dllDirectory, LPCTSTR dllFileName
	)
	{
		PROCESS_INFORMATION processInfo;
		STARTUPINFO startupInfo;

		ZeroMemory( &processInfo, sizeof(processInfo) );

		ZeroMemory( &startupInfo, sizeof(startupInfo) );
		startupInfo.cb = sizeof(processInfo);

		if(!CreateProcess(
			programPath,
			programOptions,
			NULL,
			NULL,
			TRUE, // inherit handles
				//CREATE_DEFAULT_ERROR_MODE|
				CREATE_NEW_PROCESS_GROUP|
				DETACHED_PROCESS|
				CREATE_SUSPENDED
				//DEBUG_ONLY_THIS_PROCESS|
				//DEBUG_PROCESS				// we want to catch debug event's (sadly also of childs)
				,
			NULL,
			programDirectory,
			&startupInfo,
			&processInfo
		))
		{
			TCHAR strErr[33];
			_itot_s(GetLastError(), strErr, 10);
			MessageBox( 0, _T("Failed to launch the program."), (LPCTSTR)strErr, MB_OK|MB_ICONERROR );
			return false;
		}

		bool imageInjected = InjectImage(
			processInfo.dwProcessId, dllDirectory, dllFileName
		);

		if (!imageInjected)
		{
			MessageBox( 0, _T("Image injection failed."), _T("Error"), MB_OK|MB_ICONERROR );
		}

		Sleep(2000);

		ResumeThread(processInfo.hThread);

		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);

		return imageInjected;
	}

private:
	unsigned char * m_BootImage;
	size_t m_BootImageSize;

	void LoadImage(LPCTSTR imageFileName)
	{
		FILE * file;

		_tfopen_s(&file, imageFileName, _T("rb"));

		bool bOk =
			0 != file
			&& 0 == fseek(file, 0, SEEK_END)
			&& -1 != (m_BootImageSize = ftell(file))
			&& 0 == fseek(file, 0, SEEK_SET)
			&& 0 != (m_BootImage = (unsigned char *)malloc(m_BootImageSize))
			&& 1 == fread(m_BootImage, m_BootImageSize, 1, file)
		;

		if(file) fclose(file);

		if(!bOk)
		{
			FreeImage();
		}

	}

	void FreeImage()
	{
		if(m_BootImage) free(m_BootImage);
		m_BootImage = 0;
		m_BootImageSize = 0;
	}

	bool InjectImage(DWORD processId, LPCTSTR dllDirectory, LPCTSTR dllFileName)
	{
		LPVOID argDllDir = 0;
		LPVOID argDllName = 0;
		size_t dllDirectorySz = sizeof( _TCHAR ) * (_tcsclen(dllDirectory) + 1);
		size_t dllFileNameSz = sizeof( _TCHAR ) * (_tcsclen(dllFileName) + 1);
		HMODULE hKernel32 = GetModuleHandle( _T("kernel32.dll") );
		HANDLE hProc = OpenProcess(CREATE_THREAD_ACCESS, FALSE, processId);
		HANDLE hThread = 0;
		LPVOID imageAfxHook = 0;
		LPVOID pLoadLibrary = 0;
		LPVOID pSetDllDirectory = 0;

		bool bOk =
			m_BootImage && hKernel32 && hProc

			&& (pLoadLibrary = (LPVOID) GetProcAddress(hKernel32,
#ifdef _UNICODE
				"LoadLibraryW"
#elif
				"LoadLibraryA"
#endif
			))
			&& (pSetDllDirectory = (LPVOID) GetProcAddress(hKernel32,
#ifdef _UNICODE
				"SetDllDirectoryW"
#elif
				"SetDllDirectoryA"
#endif
			))

			&& (argDllDir  = VirtualAllocEx(hProc, NULL, dllDirectorySz, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE))
			&& (argDllName = VirtualAllocEx(hProc, NULL, dllFileNameSz, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE))
			&& (imageAfxHook = VirtualAllocEx(hProc, NULL, m_BootImageSize, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE))

			&& UpdateBootImage(pLoadLibrary, pSetDllDirectory, argDllDir, argDllName)

			&& WriteProcessMemory(hProc, argDllDir, dllDirectory, dllDirectorySz, NULL)
			&& WriteProcessMemory(hProc, argDllName, dllFileName, dllFileNameSz, NULL)
			&& WriteProcessMemory(hProc, imageAfxHook, m_BootImage, m_BootImageSize, NULL)

			&& (hThread = CreateRemoteThread(
				hProc, NULL, 0, (LPTHREAD_START_ROUTINE)imageAfxHook, NULL, 0, NULL
			))
		;

		if(bOk)
		{
			bOk = false;
			bool bWait;

			do {
				bWait = false;

				for(int i=0; i < 60; i++)
				{
					if(WAIT_OBJECT_0 == WaitForSingleObject(hThread, 1000))
					{
						bOk = true;
						break;
					}
				}

				if(!bOk)
				{
					bWait = IDYES == MessageBox( 0, _T("Image injection problem.\nContinue waiting?"), _T("Warning"), MB_YESNO|MB_ICONWARNING );	
				}

			} while(bWait);

			if(!bOk)
			{
				TerminateThread(hThread, 1);
			}
			else
			{
				DWORD exitCode;

				bOk =
					0 != GetExitCodeThread(hThread, &exitCode)
					&& 0 == exitCode
				;
			}
		}

		if(hThread) CloseHandle(hThread);
		if(imageAfxHook) VirtualFreeEx(hProc, imageAfxHook, 0, MEM_RELEASE);
		if(argDllDir) VirtualFreeEx(hProc, argDllName, 0, MEM_RELEASE);
		if(argDllName) VirtualFreeEx(hProc, argDllDir, 0, MEM_RELEASE);

		if(hProc) CloseHandle(hProc);

		return bOk;
	}

	bool UpdateBootImage(LPVOID loadLibrary, LPVOID setDllDirectory, LPVOID dllDir, LPVOID dllName)
	{
		if(!(m_BootImage && loadLibrary && setDllDirectory && dllDir && dllName))
			return false;

		unsigned __int32 * imageArgs = (unsigned __int32 *)(m_BootImage +32);

		imageArgs[0] = (unsigned __int32)loadLibrary;
		imageArgs[1] = (unsigned __int32)setDllDirectory;
		imageArgs[2] = (unsigned __int32)dllDir;
		imageArgs[3] = (unsigned __int32)dllName;

		return true;
	}
};


#pragma managed

using namespace System;
using namespace System::Runtime::InteropServices;


bool CustomLoader(System::String ^ strHookPath, System::String ^ strProgramPath, System::String ^ strCmdLine)
{
	System::String ^ strDllDirectory = System::IO::Path::GetDirectoryName( strHookPath );
	System::String ^ strProgramDirectory = System::IO::Path::GetDirectoryName( strProgramPath );
	System::String ^ strImageFileName = System::AppDomain::CurrentDomain->BaseDirectory + "\\AfxHook.dat";

	System::Text::StringBuilder ^strOptsB = gcnew System::Text::StringBuilder( "\"" );
	strOptsB->Append( strProgramPath );
	strOptsB->Append( "\" " );
	strOptsB->Append( strCmdLine );

	LPCTSTR dllDirectory = 0;
	LPCTSTR dllFileName = 0;
	LPCTSTR programDirectory = 0;
	LPTSTR programOptions = 0;
	LPCTSTR programPath = 0;
	LPCTSTR imageFileName = 0;

#ifdef _UNICODE
	dllDirectory = (LPCTSTR)(int)Marshal::StringToHGlobalUni( strDllDirectory );
	dllFileName = (LPCTSTR)(int)Marshal::StringToHGlobalUni( strHookPath );
	programDirectory = (LPCTSTR)(int)Marshal::StringToHGlobalUni( strProgramDirectory );
	programOptions = (LPTSTR)(int)Marshal::StringToHGlobalUni( strOptsB->ToString() );
	programPath = (LPCTSTR)(int)Marshal::StringToHGlobalUni( strProgramPath );
	imageFileName = (LPCTSTR)(int)Marshal::StringToHGlobalUni( strImageFileName );
#else
	dllDirectory = (LPCTSTR)(int)Marshal::StringToHGlobalAnsi( strDllDirectory );
	dllFileName = (LPCTSTR)(int)Marshal::StringToHGlobalAnsi( strHookPath );
	programDirectory = (LPCTSTR)(int)Marshal::StringToHGlobalAnsi( strProgramDirectory );
	programOptions = (LPTSTR)(int)Marshal::StringToHGlobalAnsi( strOptsB->ToString() );
	programPath = (LPCTSTR)(int)Marshal::StringToHGlobalAnsi( strProgramPath );
	imageFileName = (LPCTSTR)(int)Marshal::StringToHGlobalAnsi( strImageFileName );
#endif

	AfxHook afxHook(imageFileName);

	bool bOk = afxHook.Inject(
		programPath, programDirectory, programOptions,
		dllDirectory, dllFileName
	);

	Marshal::FreeHGlobal( (System::IntPtr)(int)imageFileName );	
	Marshal::FreeHGlobal( (System::IntPtr)(int)programPath );
	Marshal::FreeHGlobal( (System::IntPtr)(int)programOptions );
	Marshal::FreeHGlobal( (System::IntPtr)(int)programDirectory );
	Marshal::FreeHGlobal( (System::IntPtr)(int)dllFileName );
	Marshal::FreeHGlobal( (System::IntPtr)(int)dllDirectory );

	return bOk;
}
