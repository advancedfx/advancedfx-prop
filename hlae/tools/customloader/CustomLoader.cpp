#include <stdafx.h>

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-09-29 by dominik.matrixstorm.com
//
// First changes:
// 2009-09-29 by dominik.matrixstorm.com

#include "CustomLoader.h"

#include <system/loader.h>
#include "CustomLoaderForm.h"

using namespace hlae;

bool RunCustomLoader(System::Windows::Forms::IWin32Window ^ owner) {
	CustomLoaderForm ^ frm = gcnew CustomLoaderForm();

	DialogResult dr = frm->ShowDialog(owner);

	if(DialogResult::OK == dr) {
		if(CustomLoader(frm->HookDll, frm->Program, frm->CmdLine))
			return true;

		MessageBox::Show("CustomLoader failed", "Error", ::MessageBoxButtons::OK, ::MessageBoxIcon::Error);
		return false;
	}
	else
		return true;
}