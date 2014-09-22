#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2014-04-22 dominik.matrixstorm.com
//
// First changes
// 2014-04-22 dominik.matrixstorm.com

#include <windows.h>

/// <returns>new proc</returns>
FARPROC Hook_ServerGetBlendingInterface(FARPROC oldProc);
