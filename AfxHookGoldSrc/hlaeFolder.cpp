#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2014-02-13 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "hlaeFolder.h"

#include <string>

#define DLL_NAME	"AfxHookGoldSrc.dll"

std::string g_HlaeFolder("");

void CalculateHlaeFolder()
{
	char hookPath[1025];
	HMODULE hHookDll = GetModuleHandle(DLL_NAME);

	hookPath[0]=NULL;
	
	if (hHookDll)
	{
		GetModuleFileName(hHookDll, hookPath, sizeof(hookPath)/sizeof(*hookPath) -1);

		g_HlaeFolder.assign(hookPath);
		size_t fp = g_HlaeFolder.find_last_of('\\');
		if(std::string::npos != fp)
		{
			g_HlaeFolder.resize(fp+1);
		}
	}
}

char const * GetHlaeFolder()
{
	static bool firstRun = true;
	if(firstRun)
	{
		firstRun = false;
		CalculateHlaeFolder();
	}

	return g_HlaeFolder.c_str();
}
