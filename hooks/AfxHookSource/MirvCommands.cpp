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
#include <stdio.h>

extern WrpVEngineClient * g_VEngineClient;


CON_COMMAND(mirv_info, "shows information about mirv") {
	g_VEngineClient->ExecuteClientCmd("echo \"//\"");
	g_VEngineClient->ExecuteClientCmd("echo \"// AfxHookSource " __DATE__ " "__TIME__ "\" by ripieces and msthavoc");
	g_VEngineClient->ExecuteClientCmd("echo \"// Copyright (c) advancedfx.org\"");
	g_VEngineClient->ExecuteClientCmd("echo \"//\"");
	g_VEngineClient->Con_NPrintf(1,"TEST");
}

CON_COMMAND(mirv_xexec, "unrestricted client command execution: mirv_xexec <as you would have typed here>") {
	unsigned int len=0;
	char *ttt, *ct;

	for(int i=0; i<args->ArgC(); i++)
	{
		len += strlen(args->ArgV(i))+1;
	}

	if(len<1) len=1;

	ct = ttt = (char *)malloc(sizeof(char)*len);

	for(int i=1; i<args->ArgC(); i++) {
		char const * cur = args->ArgV(i);
		unsigned int lcur = strlen(cur);
		
		if(1<i) {
			strcpy(ct, " ");
			ct++;
		}

		strcpy(ct, cur);
		ct += lcur;
	}

	*ct = 0;

	g_VEngineClient->ExecuteClientCmd(ttt);

	delete ttt;

}