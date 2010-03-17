#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-17 by dominik.matrixstorm.com
///
// First changes:
// 2010-03-17 by dominik.matrixstorm.com

#include "InitScript.h"

#include "mirv_scripting.h"

#include <string>



#define INIT_SCRIPT_FILE "AfxHookGoldSrc_init.js"
#define SCRIPT_FOLDER "scripts\\"
#define DLL_NAME	"AfxHookGoldSrc.dll"

bool RunInitScript()
{
	char hookPath[1025];
	bool bCfgres = false;
	HMODULE hHookDll = GetModuleHandle(DLL_NAME);

	hookPath[0]=NULL;
	
	if (hHookDll)
	{
		GetModuleFileName(hHookDll, hookPath, sizeof(hookPath)/sizeof(*hookPath) -1);

		std::string strFolder(hookPath);
		size_t fp = strFolder.find_last_of('\\');
		if(std::string::npos != fp)
		{
			strFolder.resize(fp+1);
		}

		strFolder += SCRIPT_FOLDER;

		JsSetScriptFolder(strFolder.c_str());

		bCfgres = JsExecute("load('" INIT_SCRIPT_FILE "');");

		if(!bCfgres) {
			strFolder = "Failed to load:\n\""
				+strFolder +INIT_SCRIPT_FILE "\"."
			;

			MessageBox(0, strFolder.c_str(), "MDT_ERROR",MB_OK|MB_ICONHAND);
		}
	}

	return bCfgres;
}

