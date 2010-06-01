#include <stdafx.h>

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-09-29 by dominik.matrixstorm.com
//
// First changes:
// 2009-09-29 by dominik.matrixstorm.com

#include "CustomLoader.h"

#include "CustomLoaderForm.h"

#include <system/config.h>
#include <system/globals.h>


using namespace hlae;
using namespace hlae::globals;

bool RunCustomLoader(System::Windows::Forms::IWin32Window ^ owner) {
	CfgCustomLoader ^ cfg = HlaeConfig::Config->Settings->CustomLoader;
	CustomLoaderForm ^ frm = gcnew CustomLoaderForm();

	frm->HookDll = cfg->HookDllPath;
	frm->Program = cfg->ProgramPath;
	frm->CmdLine = cfg->CmdLine;


	DialogResult dr = frm->ShowDialog(owner);

	if(DialogResult::OK == dr) {
		cfg->HookDllPath = frm->HookDll;
		cfg->ProgramPath = frm->Program;
		cfg->CmdLine = frm->CmdLine;

		HlaeConfig::BackUp();

		if(AfxCppCli::AfxHook::LauchAndHook(frm->Program, frm->CmdLine, frm->HookDll))
			return true;

		MessageBox::Show("CustomLoader failed", "Error", ::MessageBoxButtons::OK, ::MessageBoxIcon::Error);
		return false;
	}
	else
		return true;
}