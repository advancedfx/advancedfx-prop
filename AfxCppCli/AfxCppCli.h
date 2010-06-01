#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-05-26 by dominik.matrixstorm.com
//
// First changes:
// 2010-05-26 by dominik.matrixstorm.com

using namespace System;

namespace AfxCppCli {


// AfxHook /////////////////////////////////////////////////////////////////////

public ref class AfxHook
{
public:
	static bool LauchAndHook(String ^ programPath, String ^ cmdLine, String ^ hookPath);
};

} // namespace AfxCppCli {
