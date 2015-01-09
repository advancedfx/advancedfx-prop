#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-05-24 by dominik.matrixstorm.com
//
// First changes:
// 2009-09-30 by dominik.matrixstorm.com


// Hint: for now commands are registered upon the first client.dll CreateInterface() call

#include <shared/StringTools.h>

//#include "MirvShader.h"
#include "RenderView.h"
#include "SourceInterfaces.h"
#include "WrpVEngineClient.h"
#include "WrpConsole.h"
#include "WrpVEngineClient.h"
#include "csgo_CHudDeathNotice.h"
#include "csgo_GetPlayerName.h"
//#include "csgo_viewrender.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <cctype>

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

CON_COMMAND(mirv_campath,"easy camera paths")
{
	int argc = args->ArgC();

	if(2 <= argc)
	{
		char const * subcmd = args->ArgV(1);

		if(!_stricmp("add", subcmd) && 2 == argc)
		{
			g_Hook_VClient_RenderView.m_CamPath.Add(
				g_Hook_VClient_RenderView.GetCurTime(),
				CamPathValue(
					g_Hook_VClient_RenderView.LastCameraOrigin[0],
					g_Hook_VClient_RenderView.LastCameraOrigin[1],
					g_Hook_VClient_RenderView.LastCameraOrigin[2],
					g_Hook_VClient_RenderView.LastCameraAngles[0],
					g_Hook_VClient_RenderView.LastCameraAngles[1],
					g_Hook_VClient_RenderView.LastCameraAngles[2]
				)
			);

			return;
		}
		else if(!_stricmp("enable", subcmd) && 3 == argc)
		{
			bool enable = 0 != atoi(args->ArgV(2));
			bool enabled = g_Hook_VClient_RenderView.m_CamPath.Enable(
				enable
			);

			if(enable && !enabled)
				Tier0_Msg(
					"Error: Could not enable CamPath.\n"
					"Did you add enough point already?\n"
				);

			return;
		}
		else if(!_stricmp("clear", subcmd) && 2 == argc)
		{
			g_Hook_VClient_RenderView.m_CamPath.Clear();

			return;
		}
		else if(!_stricmp("print", subcmd) && 2 == argc)
		{
			Tier0_Msg("passed id: time -> (x,y,z) (pitch,yaw,roll)\n");

			double curtime = g_Hook_VClient_RenderView.GetCurTime();
			
			int i=0;
			for(CamPathIterator it = g_Hook_VClient_RenderView.m_CamPath.GetBegin(); it != g_Hook_VClient_RenderView.m_CamPath.GetEnd(); ++it)
			{
				double vieworigin[3];
				double viewangles[3];

				double time = it.GetTime();
				CamPathValue val = it.GetValue();

				vieworigin[0] = val.X;
				vieworigin[1] = val.Y;
				vieworigin[2] = val.Z;
				viewangles[0] = val.Pitch;
				viewangles[1] = val.Yaw;
				viewangles[2] = val.Roll;

				Tier0_Msg(
					"%s %i: %f -> (%f,%f,%f) (%f,%f,%f)\n",
					time <= curtime ? "Y" : "n",
					i, time,
					vieworigin[0],vieworigin[1],vieworigin[2],
					viewangles[0],viewangles[1],viewangles[2]
				);

				i++;
			}
			Tier0_Msg("---- Current time: %f\n", curtime);

			return;
		}
		else if(!_stricmp("remove", subcmd) && 3 == argc)
		{
			int idx = atoi(args->ArgV(2));
			int i=0;
			for(CamPathIterator it = g_Hook_VClient_RenderView.m_CamPath.GetBegin(); it != g_Hook_VClient_RenderView.m_CamPath.GetEnd(); ++it)
			{
				if(i == idx)
				{
					double time = it.GetTime();
					g_Hook_VClient_RenderView.m_CamPath.Remove(time);
					break;
				}
				i++;
			}

			return;
		}
		else if(!_stricmp("load", subcmd) && 3 == argc)
		{	
			std::wstring wideString;
			bool bOk = AnsiStringToWideString(args->ArgV(2), wideString)
				&& g_Hook_VClient_RenderView.m_CamPath.Load(wideString.c_str())
			;

			Tier0_Msg("Loading campath: %s.\n", bOk ? "OK" : "ERROR");

			return;
		}
		else if(!_stricmp("save", subcmd) && 3 == argc)
		{	
			std::wstring wideString;
			bool bOk = AnsiStringToWideString(args->ArgV(2), wideString)
				&& g_Hook_VClient_RenderView.m_CamPath.Save(wideString.c_str())
			;

			Tier0_Msg("Saving campath: %s.\n", bOk ? "OK" : "ERROR");

			return;
		}
	}

	Tier0_Msg(
		"mirv_campath add - adds current demotime and view as keyframe\n"
		"mirv_campath enable 0|1 - set whether the camera splines are active or not. Please note that currently at least 4 Points are required to make it active successfully!\n"
		"mirv_campath clear - removes all keyframes\n"
		"mirv_campath print - prints keyframes\n"
		"mirv_campath remove <id> - removes a keyframe\n"
		"mirv_campath load <fileName> - loads the campath from the file (XML format)\n"
		"mirv_campath save <fileName> - saves the campath to the file (XML format)\n"
	);
	return;
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
		if(0 == _stricmp("stop", arg1))
		{
			g_Hook_VClient_RenderView.ExportEnd();
			return;
		}
		else
		if(0 == _stricmp("start", arg1) && 4 <= argc) {
			char const * fileName = args->ArgV(2);
			float fps = (float)atof(args->ArgV(3));
			if(fps < 0.1f) fps = 0.1f;

			std::wstring wideFileName;
			if(
				!AnsiStringToWideString(fileName, wideFileName)
				|| !g_Hook_VClient_RenderView.ExportBegin(wideFileName.c_str(), 1.0f/fps)
			)
				Tier0_Msg("Error: exporting failed.");

			return;
		}
		if(0 == _stricmp("timeinfo", arg1))
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
		if(0 == _stricmp("stop", arg1)) {
			g_Hook_VClient_RenderView.ImportEnd();
			return;
		}
		else
		if(0 == _stricmp("start", arg1) && 3 <= argc) {
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
		if(0 == _stricmp("basetime", arg1)) {
			if(3 <= argc) {
				char const * newTime = args->ArgV(2);
				if(0 == _stricmp("current", newTime))
					g_Hook_VClient_RenderView.SetImportBaseTime(g_Hook_VClient_RenderView.GetCurTime());
				else
					g_Hook_VClient_RenderView.SetImportBaseTime((float)atof(newTime));
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

CON_COMMAND(mirv_cvar_unhide_all,"(CS:GO only) removes hidden and development only flags from all cvars.")
{
	ICvar_007 * pCvar = WrpConCommands::GetVEngineCvar007();
	if(!pCvar)
	{
		Tier0_Warning("Error: No suitable Cvar interface found.\n");
		return;
	}

	ICvar_007::Iterator iter(pCvar);

	int nUnhidden = 0;

	for(iter.SetFirst(); iter.IsValid(); iter.Next())
	{
		ConCommandBase_007 * cmd = iter.Get();

		if(cmd->IsFlagSet(FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN))
			nUnhidden++;

		cmd->RemoveFlags(FCVAR_DEVELOPMENTONLY | FCVAR_HIDDEN);
	}

	Tier0_Msg("Removed FCVAR_DEVELOPMENTONLY or FCVAR_HIDDEN from %i ConVars.\n", nUnhidden);
}

CON_COMMAND(mirv_deathmsg, "controls death notification options")
{
	if(!csgo_CHudDeathNotice_Install())
	{
		Tier0_Warning("Error: Hook not installed.\n");
		return;
	}

	int argc = args->ArgC();

	if(2 <= argc)
	{
		char const * arg1 = args->ArgV(1);
		if(0 == _stricmp("debug", arg1))
		{
			if(3 <= argc)
			{
				csgo_debug_CHudDeathNotice_FireGameEvent = atoi(args->ArgV(2));
				return;
			}
			Tier0_Msg(
				"Usage:\n"
				"mirv_deathmsg debug 0|1|2 - disbale/enable printing of ids of current death notices.\n"
				"Current setting: %i\n",
				csgo_debug_CHudDeathNotice_FireGameEvent
			);
			return;
		}
		else
		if(0 == _stricmp("block", arg1))
		{
			if(4 <= argc)
			{
				char const * uidAttacker = args->ArgV(2);
				char const * uidVictim = args->ArgV(3);
				char const * uidAssister = 5 <= argc ? args->ArgV(4) : "*";

				csgo_CHudDeathNotice_Block(uidAttacker, uidVictim, uidAssister);

				return;
			}
			else
			if(3 <= argc)
			{
				char const * arg2 = args->ArgV(2);

				if(0 == _stricmp("list", arg2))
				{
					csgo_CHudDeathNotice_Block_List();
					return;
				}
				else
				if(0 == _stricmp("clear", arg2))
				{
					csgo_CHudDeathNotice_Block_Clear();
					return;
				}
				
			}
			Tier0_Msg(
				"Usage:\n"
				"mirv_deathmsg block <uidAttacker> <uidVictim> - block these ids\n"
				"\tRemarks: * to match any uid, use !x to match any uid apart from x.\n"
				"mirv_deathmsg block <uidAttacker> <uidVictim> <uidAssister> - block these ids\n"
				"\tRemarks: * to match any uid, use !x to match any uid apart from x.\n"
				"mirv_deathmsg block list - list current blocks\n"
				"mirv_deathmsg block clear - clear current blocks\n"
				"(Use mirv_deathmsg debug 1 to get the uids.)\n"
			);
			return;
		}
		else
		if(0 == _stricmp("cfg", arg1))
		{
			if(4 <= argc)
			{
				char const * arg2 = args->ArgV(2);
				float arg3 = (float)atof(args->ArgV(3));

				if(0 == _stricmp("scrollInTime", arg2))
				{
					csgo_CHudDeathNotice_nScrollInTime = arg3;
					return;
				}
				else
				if(0 == _stricmp("fadeOutTime", arg2))
				{
					csgo_CHudDeathNotice_nFadeOutTime = arg3;
					return;
				}
				else
				if(0 == _stricmp("noticeLifeTime", arg2))
				{
					csgo_CHudDeathNotice_nNoticeLifeTime = arg3;
					return;
				}
				else
				if(0 == _stricmp("localPlayerLifeTimeMod", arg2))
				{
					csgo_CHudDeathNotice_nLocalPlayerLifeTimeMod = arg3;
					return;
				}
			}
			Tier0_Msg(
				"Usage:\n"
				"mirv_deathmsg cfg scrollInTime f - Current: %f\n"
				"mirv_deathmsg cfg fadeOutTime f - Current: %f\n"
				"mirv_deathmsg cfg noticeLifeTime f - This is what you want. Current: %f\n"
				"mirv_deathmsg cfg localPlayerLifeTimeMod f - Current: %f\n"
				"Where f is a floating point value in seconds. Use -1 (a negative value) to use the orginal value instead.\n",
				csgo_CHudDeathNotice_nScrollInTime,
				csgo_CHudDeathNotice_nFadeOutTime,
				csgo_CHudDeathNotice_nNoticeLifeTime,
				csgo_CHudDeathNotice_nLocalPlayerLifeTimeMod
			);
			return;
		}

	}
	Tier0_Msg(
		"Usage:\n"
		"mirv_deathmsg block\n"
		"mirv_deathmsg cfg\n"
		"mirv_deathmsg debug\n"
	);
}

CON_COMMAND(mirv_replace_name, "allows repalcing playernames")
{
	if(!csgo_GetPlayerName_Install())
	{
		Tier0_Warning("Error: Hook not installed.\n");
		return;
	}

	int argc = args->ArgC();

	if(2 <= argc)
	{
		char const * arg1 = args->ArgV(1);

		if(0 == _stricmp("list", arg1))
		{
			csgo_GetPlayerName_Replace_List();
			return;
		}
		else
		if(0 == _stricmp("clear", arg1))
		{
			csgo_GetPlayerName_Replace_Clear();
			return;
		}

		if(3 <= argc)
		{
			const char * arg2 = args->ArgV(2);

			if(0 == _stricmp("debug", arg1))
			{
				csgo_debug_GetPlayerName = 0 != atoi(arg2);
				return;
			}
			else
			if(0 == _stricmp("delete", arg1))
			{
				csgo_GetPlayerName_Replace_Delete(arg2);
				return;
			}

			if(1 <= strlen(arg1) && isdigit(arg1[0]))
			{
				csgo_GetPlayerName_Replace(arg1, arg2);
				return;
			}
		}
	}

	Tier0_Msg(
		"Usage:\n"
		"mirv_replace_name <playerId> <name> - replace <playerId> with given <name>.\n"
		"mirv_replace_name debug 0|1 - print <playerId> -> <name> paris into console as they get queried by the game.\n"
		"mirv_replace_name delete <playerId> - delete replacement for <playerId>.\n"
		"mirv_replace_name list - list all <playerId> -> <name> replacements currently active.\n"
		"mirv_replace_name clear - clear all replacements.\n"
	);
}

/*
CON_COMMAND(mirv_shader, "shader effects")
{
	if(!csgo_viewrender_Install())
	{
		Tier0_Warning("Error: Hook not installed or modification not supported.\n");
		return;
	}

	int argc = args->ArgC();

	if(2 <= argc)
	{
		char const * arg1 = args->ArgV(1);
		
		if(0 == _stricmp("enabled", arg1))
		{
			if(3 == argc)
			{
				int iEnable = atoi(args->ArgV(2));
				g_MirvShader.Enabled_set(0 != iEnable);
				return;
			}

			Tier0_Msg(
				"mirv_shader enabled: %s.\n",
				g_MirvShader.Enabled_get() ? "1" : "0"
			);
			return;
		}
	}

	Tier0_Msg(
		"Usage:\n"
		"mirv_shader enabled [0|1]\n"
	);
}
*/
