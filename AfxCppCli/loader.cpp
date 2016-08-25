#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-08-25 dominik.matrixstorm.com
//
// First changes:
// 2007 dominik.matrixstorm.com
// 2007 Gavin Bramhill

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
	AfxHook(LPCWSTR imageFileName)
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
		LPCWSTR programPath, LPCWSTR programDirectory, LPWSTR programOptions,
		LPCWSTR dllDirectory, LPCWSTR dllFilePath, LPVOID environment
	)
	{
		PROCESS_INFORMATION processInfo;
		STARTUPINFOW startupInfo;

		ZeroMemory( &processInfo, sizeof(processInfo) );

		ZeroMemory( &startupInfo, sizeof(startupInfo) );
		startupInfo.cb = sizeof(processInfo);

		if(!CreateProcessW(
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
				|CREATE_UNICODE_ENVIRONMENT
				,
			environment,
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

		//MessageBox(0, _T("Click OK."), _T("Waiting"), MB_OK);

		bool imageInjected = InjectImage(
			processInfo.dwProcessId, dllDirectory, dllFilePath
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

	void LoadImage(LPCWSTR imageFileName)
	{
		FILE * file;

		_wfopen_s(&file, imageFileName, L"rb");

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

	bool InjectImage(DWORD processId, LPCWSTR dllDirectory, LPCWSTR dllFilePath)
	{
		LPVOID argDllDir = 0;
		LPVOID argDllFilePath = 0;
		size_t dllDirectorySz = sizeof( wchar_t ) * (wcslen(dllDirectory) + 1);
		size_t dllFilePathSz = sizeof(wchar_t) * (wcslen(dllFilePath) + 1);
		HMODULE hKernel32 = GetModuleHandle( _T("kernel32.dll") );
		HANDLE hProc = OpenProcess(CREATE_THREAD_ACCESS, FALSE, processId);
		HANDLE hThread = 0;
		LPVOID imageAfxHook = 0;
		LPVOID pGetModuleHandleW = 0;
		LPVOID pGetProcAddress = 0;

		bool bOk =
			m_BootImage && hKernel32 && hProc

			&& (pGetModuleHandleW = (LPVOID) GetProcAddress(hKernel32,
				"GetModuleHandleW"
			))
			&& (pGetProcAddress = (LPVOID) GetProcAddress(hKernel32,
				"GetProcAddress"
			))

			&& (argDllDir  = VirtualAllocEx(hProc, NULL, dllDirectorySz, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE))
			&& (argDllFilePath = VirtualAllocEx(hProc, NULL, dllFilePathSz, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE))
			&& (imageAfxHook = VirtualAllocEx(hProc, NULL, m_BootImageSize, MEM_RESERVE|MEM_COMMIT, PAGE_EXECUTE_READWRITE))

			&& UpdateBootImage(pGetModuleHandleW, pGetProcAddress, argDllDir, argDllFilePath)

			&& WriteProcessMemory(hProc, argDllDir, dllDirectory, dllDirectorySz, NULL)
			&& WriteProcessMemory(hProc, argDllFilePath, dllFilePath, dllFilePathSz, NULL)
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

				bOk = 0 != GetExitCodeThread(hThread, &exitCode);

				if (bOk)
				{
					if (0 != exitCode)
					{
						bOk = false;
						TCHAR strErr[33];
						_itot_s(exitCode, strErr, 10);
						MessageBox(0, (LPCTSTR)strErr, _T("AfxHook exit code"), MB_OK | MB_ICONERROR);
					}
				}
			}
		}

		if(hThread) CloseHandle(hThread);
		if(imageAfxHook) VirtualFreeEx(hProc, imageAfxHook, 0, MEM_RELEASE);
		if(argDllDir) VirtualFreeEx(hProc, argDllFilePath, 0, MEM_RELEASE);
		if(argDllFilePath) VirtualFreeEx(hProc, argDllDir, 0, MEM_RELEASE);

		if(hProc) CloseHandle(hProc);

		return bOk;
	}

	bool UpdateBootImage(LPVOID getModuleHandleW, LPVOID getProcAddress, LPVOID dllDir, LPVOID dllFilePath)
	{
		if(!(m_BootImage && getModuleHandleW && getProcAddress && dllDir && dllFilePath))
			return false;

		unsigned __int32 * imageArgs = (unsigned __int32 *)(m_BootImage +32);

		imageArgs[0] = (unsigned __int32)getModuleHandleW;
		imageArgs[1] = (unsigned __int32)getProcAddress;
		imageArgs[2] = (unsigned __int32)dllDir;
		imageArgs[3] = (unsigned __int32)dllFilePath;

		return true;
	}
};


#pragma managed

using namespace System;
using namespace System::Runtime::InteropServices;


bool CustomLoader(System::String ^ strHookPath, System::String ^ strProgramPath, System::String ^ strCmdLine, System::String ^ strEnvironment)
{
	System::String ^ strDllDirectory = System::IO::Path::GetDirectoryName( strHookPath ); // maybe we should check that strDllDirectory <= MAXPATH-2 here?
	System::String ^ strProgramDirectory = System::IO::Path::GetDirectoryName( strProgramPath );
	System::String ^ strImageFileName = System::AppDomain::CurrentDomain->BaseDirectory + "\\AfxHook.dat";

	System::Text::StringBuilder ^strOptsB = gcnew System::Text::StringBuilder( "\"" );
	strOptsB->Append( strProgramPath );
	strOptsB->Append( "\" " );
	strOptsB->Append( strCmdLine );

	LPCWSTR dllDirectory = 0;
	LPCWSTR dllFilePath = 0;
	LPCWSTR programDirectory = 0;
	LPWSTR programOptions = 0;
	LPCWSTR programPath = 0;
	LPCWSTR imageFileName = 0;
	LPVOID environment = 0;

	dllDirectory = (LPCWSTR)(int)Marshal::StringToHGlobalUni( strDllDirectory );
	dllFilePath = (LPCWSTR)(int)Marshal::StringToHGlobalUni( strHookPath );
	programDirectory = (LPCWSTR)(int)Marshal::StringToHGlobalUni( strProgramDirectory );
	programOptions = (LPWSTR)(int)Marshal::StringToHGlobalUni( strOptsB->ToString() );
	programPath = (LPCWSTR)(int)Marshal::StringToHGlobalUni( strProgramPath );
	imageFileName = (LPCWSTR)(int)Marshal::StringToHGlobalUni( strImageFileName );
	environment = (LPVOID)(int)Marshal::StringToHGlobalUni( strEnvironment );

	AfxHook afxHook(imageFileName);

	bool bOk = afxHook.Inject(
		programPath, programDirectory, programOptions,
		dllDirectory, dllFilePath, environment
	);

	Marshal::FreeHGlobal( (System::IntPtr)(int)environment );
	Marshal::FreeHGlobal( (System::IntPtr)(int)imageFileName );	
	Marshal::FreeHGlobal( (System::IntPtr)(int)programPath );
	Marshal::FreeHGlobal( (System::IntPtr)(int)programOptions );
	Marshal::FreeHGlobal( (System::IntPtr)(int)programDirectory );
	Marshal::FreeHGlobal( (System::IntPtr)(int)dllFilePath );
	Marshal::FreeHGlobal( (System::IntPtr)(int)dllDirectory );

	return bOk;
}
