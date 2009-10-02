#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-09-30 by dominik.matrixstorm.com
//
// First changes:
// 2009-09-30 by dominik.matrixstorm.com


// Hint: for now commands are registered upon the first client.dll CreateInterface() call

#include "SourceInterfaces.h"
#include "WrpVEngineClient.h"
#include "WrpConsole.h"

#include <windows.h>


extern WrpVEngineClient * g_VEngineClient;


// mirv_testcommand ////////////////////////////////////////////////////////////

void mirv_testcommand_callback() {
	MessageBox(0, "MirvTestCommand called", "AFX_DEBUG", MB_OK|MB_ICONINFORMATION);
	g_VEngineClient->ClientCmd("echo MirvTestCommand called");
}

WrpConCommand mirv_testcommand("mirv_testcommand", &mirv_testcommand_callback, "Calls the test command");
