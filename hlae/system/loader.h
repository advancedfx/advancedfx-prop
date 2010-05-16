#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-09-29 by dominik.matrixstorm.com
//
// First changes:
// 2008-03-19 by dominik.matrixstorm.com
// 2007 by Gavin Bramhill

// Description:
// For launching the game and the hook.

#include <system/config.h>

using namespace hlae;

bool CustomLoader(System::String ^ strHookPath, System::String ^ strProgramPath, System::String ^ strCmdLine);
