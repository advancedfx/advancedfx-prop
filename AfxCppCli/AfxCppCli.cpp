#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-12-22 dominik.matrixstorm.com
//
// First changes:
// 2010-05-26 dominik.matrixstorm.com

#include "AfxCppCli.h"

#include "loader.h"

using namespace AfxCppCli;


// AfxHook /////////////////////////////////////////////////////////////////////

bool AfxHook::LauchAndHook(String ^ programPath, String ^ cmdLine, String ^ hookPath)
{
	return CustomLoader(hookPath, programPath, cmdLine, nullptr);
}

bool AfxHook::LauchAndHook(String ^ programPath, String ^ cmdLine, String ^ hookPath, String ^ environment)
{
	return CustomLoader(hookPath, programPath, cmdLine, environment);
}

