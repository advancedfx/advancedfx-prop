#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-12-22 dominik.matrixstorm.com
//
// First changes:
// 2008-03-19 dominik.matrixstorm.com
// 2007 Gavin Bramhill

// Description:
// For launching the game and the hook.

/// <param name="strEnvironment">Set null for default environment (recommended), or use custom environment block. Each line of the block must be null terminated, the last line must have two nulls.</param>
bool CustomLoader(System::String ^ strHookPath, System::String ^ strProgramPath, System::String ^ strCmdLine, System::String ^ strEnvironment);
