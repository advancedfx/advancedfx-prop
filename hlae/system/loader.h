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

bool AfxGoldSrcLaunch(CfgLauncher ^cfg);

bool CustomLoader(System::String ^ hookPath, System::String ^ programPath, System::String ^ cmdline);
