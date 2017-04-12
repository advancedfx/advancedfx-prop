#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-12-22 dominik.matrixstorm.com
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

	/// <param name="environment">Set null for default environment (recommended), or use custom environment block. Each line of the block must be null terminated, the last line must have two nulls.</param>
	static bool LauchAndHook(String ^ programPath, String ^ cmdLine, String ^ hookPath, String ^ environment);
};

} // namespace AfxCppCli {

extern "C" __declspec(dllexport)
bool __cdecl LaunchAndHook(char* programPath, char* cmdLine, char* hookPath, char* environment)
{
	String^ env = nullptr;
	if (environment != nullptr) {
		env = gcnew String(environment);
	}
	return AfxCppCli::AfxHook::LauchAndHook(gcnew String(programPath), gcnew String(cmdLine), gcnew String(hookPath), env);
};