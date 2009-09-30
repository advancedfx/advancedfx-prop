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


// mirv_testcommand ////////////////////////////////////////////////////////////

void mirv_testcommand_callback() {
	g_VEngineClient->ClientCmd("echo MirvTestCommand called");
}

ConCommand_003 mirv_testcommand("mirv_testcommand", &mirv_testcommand_callback, "Calls the test command");
