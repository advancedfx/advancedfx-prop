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
#include "WrpVEngineClient.h"

#include <windows.h>


extern WrpVEngineClient * g_VEngineClient;


CON_COMMAND(mirv_info, "shows information about mirv") {
	g_VEngineClient->ExecuteClientCmd("echo \"//\"");
	g_VEngineClient->ExecuteClientCmd("echo \"// AfxHookSource " __DATE__ " "__TIME__ "\" by ripieces and msthavoc");
	g_VEngineClient->ExecuteClientCmd("echo \"// Copyright (c) advancedfx.org\"");
	g_VEngineClient->ExecuteClientCmd("echo \"//\"");
}

CON_COMMAND(mirv_test, "test") {
	g_VEngineClient->Con_NPrintf(1,"THIS IS A TEST BITCHES!");
}