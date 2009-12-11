#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-12-03 dominik.matrixstorm.com
//
// First changes
// 2009-12-03 dominik.matrixstorm.com

#include "filetools.h"

#include <tchar.h>
#include <windows.h>

bool CreateDirectoryRecursiveA(char const *lpPath) {
	bool bOk = true;
	LPWSTR buf = NULL;
	LPWSTR buf2 = NULL;
	LPWSTR aBuf;
	
	LPCWSTR prefix =
		NULL != strchr(lpPath, ':') ?
		L"\\\\?\\" // only abs paths can be of extended length
		: L""
	;
	int i;

	i = MultiByteToWideChar(
		CP_ACP,
		0,
		lpPath,
		-1,
		NULL,
		0
	);

	bOk = 0 != i;

	if(bOk) {
		buf = (LPWSTR)malloc(sizeof(WCHAR)*(
			wcslen(prefix)+i
		));
		
		aBuf = buf +wcslen(prefix);

		bOk = NULL != buf;
	}

	if(bOk) {
		wcscpy(buf, prefix);

		i =	MultiByteToWideChar(
			CP_ACP,
			0,
			lpPath,
			-1,
			aBuf,
			i
		);

		bOk = 0 != i;
	}

	if(bOk) {
		i = GetFullPathNameW(
			buf,
			0,
			NULL,
			NULL
		);

		bOk = 0 != i;
	}

	if(bOk) {
		buf2 = (LPWSTR)malloc(sizeof(WCHAR)*i);
		bOk = 0 != buf2;
	}

	if(bOk) {
		i = GetFullPathNameW(
			buf,
			i,
			buf2,
			NULL
		);

		bOk = 0 != i;
	}

	free(buf);

	if(bOk) {
		bool bCreated = false;
		LPWSTR findPos = 0;
		int numstacked = 0;

		// try to walk down until first directory is created:
		do {
			if(findPos) {
				*findPos = '\0';
				numstacked++;
			}

			bCreated = 0 != CreateDirectoryW(buf2, NULL);
		} while(!bCreated && 0 != (findPos = wcsrchr(buf2, '\\')));

		// try to walk up until last directory is created:
		if(bCreated) {
			while(bCreated && numstacked) {
				buf2[wcslen(buf2)] = '\\';
				numstacked--;

				bCreated = 0 != CreateDirectoryW(buf2, NULL);
			}
		}

		bOk = bCreated;
	}

	free(buf2);

	return bOk;
}