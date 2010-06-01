#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-05-26 by dominik.matrixstorm.com
//
// First changes:
// 2010-05-26 by dominik.matrixstorm.com

#include "AfxCppCli.h"

#include "loader.h"

using namespace AfxCppCli;


// AfxHook /////////////////////////////////////////////////////////////////////

bool AfxHook::LauchAndHook(String ^ programPath, String ^ cmdLine, String ^ hookPath)
{
	return CustomLoader(hookPath, programPath, cmdLine);
}

