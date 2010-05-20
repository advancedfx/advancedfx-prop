#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-09-30 by dominik.matrixstorm.com
//
// First changes:
// 2009-09-30 by dominik.matrixstorm.com


// Hint: for now commands are registered upon the first client.dll CreateInterface() call

#include <shared/StringTools.h>

//#include "DepthImages.h"
#include "RenderView.h"
#include "SourceInterfaces.h"
#include "WrpVEngineClient.h"
#include "WrpConsole.h"
#include "WrpVEngineClient.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>

extern WrpVEngineClient * g_VEngineClient;


CON_COMMAND(__mirv_exec, "client command execution: __mirv_exec <as you would have typed here>") {
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

CON_COMMAND(mirv_camexport, "controls camera motion data export") {
	if(!g_Hook_VClient_RenderView.IsInstalled())
	{
		Tier0_Warning("Error: Hook not installed.\n");
		return;
	}

	int argc = args->ArgC();

	if(2 <= argc)
	{
		char const * arg1 = args->ArgV(1);
		if(0 == stricmp("stop", arg1))
		{
			g_Hook_VClient_RenderView.ExportEnd();
			return;
		}
		else
		if(0 == stricmp("start", arg1) && 4 <= argc) {
			char const * fileName = args->ArgV(2);
			float fps = atof(args->ArgV(3));
			if(fps < 0.1f) fps = 0.1f;

			std::wstring wideFileName;
			if(
				!AnsiStringToWideString(fileName, wideFileName)
				|| !g_Hook_VClient_RenderView.ExportBegin(wideFileName.c_str(), 1.0f/fps)
			)
				Tier0_Msg("Error: exporting failed.");

			return;
		}
		if(0 == stricmp("timeinfo", arg1))
		{			
			Tier0_Msg("Current (interpolated client) time: %f\n", g_Hook_VClient_RenderView.GetCurTime());
			return;
		}

	}

	Tier0_Msg(
		"Usage:\n"
		"mirv_camexport start <filename> <fps>\n"
		"mirv_camexport stop\n"
		"mirv_camexport timeinfo\n"
	);
}

CON_COMMAND(mirv_camimport, "controls camera motion data import") {
	if(!g_Hook_VClient_RenderView.IsInstalled())
	{
		Tier0_Warning("Error: Hook not installed.\n");
		return;
	}

	int argc = args->ArgC();

	if(2 <= argc) {
		char const * arg1 = args->ArgV(1);
		if(0 == stricmp("stop", arg1)) {
			g_Hook_VClient_RenderView.ImportEnd();
			return;
		}
		else
		if(0 == stricmp("start", arg1) && 3 <= argc) {
			char const * fileName = args->ArgV(2);
			g_Hook_VClient_RenderView.SetImportBaseTime(g_Hook_VClient_RenderView.GetCurTime());

			std::wstring wideFileName;
			if(!AnsiStringToWideString(fileName, wideFileName)
				|| !g_Hook_VClient_RenderView.ImportBegin(wideFileName.c_str())
			)
				Tier0_Msg("Loading failed.");
			return;
		}
		else
		if(0 == stricmp("basetime", arg1)) {
			if(3 <= argc) {
				char const * newTime = args->ArgV(2);
				if(0 == stricmp("current", newTime))
					g_Hook_VClient_RenderView.SetImportBaseTime(g_Hook_VClient_RenderView.GetCurTime());
				else
					g_Hook_VClient_RenderView.SetImportBaseTime(atof(newTime));
				return;
			}

			Tier0_Msg(
				"Usage:\n"
				"mirv_camimport basetime <newTime> | current\n"
				"Current setting: %f\n",
				g_Hook_VClient_RenderView.GetImportBasteTime()
			);
			return;
		}
	}
	Tier0_Msg(
		"Usage:\n"
		"mirv_camimport start <filename>\n"
		"mirv_camimport stop\n"
		"mirv_camimport basetime\n"
	);
}

/*

CON_COMMAND(mirv_depthexport, "depth (z-buffer) image export") {
	int argc = args->ArgC();

	if(2 <= argc)
	{
		char const * arg1 = args->ArgV(1);
		if(0 == stricmp("stop", arg1))
		{
			g_DepthImages.WriteDepth_set(false);
			return;
		}
		else if(0 == stricmp("start", arg1) && 3 <= argc)
		{
			char const * fileName = args->ArgV(2);
			g_DepthImages.FilePrefix_set(fileName);
			g_DepthImages.WriteDepth_set(true);
			return;
		}
	}

	Tier0_Msg(
		"Usage:\n"
		"mirv_depthexport start <filePrefix>\n"
		"mirv_depthexport stop\n"
	);
}
*/
