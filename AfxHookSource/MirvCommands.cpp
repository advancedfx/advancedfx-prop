#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-10-11 by dominik.matrixstorm.com
//
// First changes:
// 2009-09-30 by dominik.matrixstorm.com


// Hint: for now commands are registered upon the first client.dll CreateInterface() call

#include <shared/StringTools.h>

#include "RenderView.h"
#include "SourceInterfaces.h"
#include "WrpVEngineClient.h"
#include "WrpConsole.h"
#include "WrpVEngineClient.h"
#include "csgo_CHudDeathNotice.h"
#include "csgo_GetPlayerName.h"
#include "csgo_SndMixTimeScalePatch.h"
#include "AfxHookSourceInput.h"
#include <shared/hooks/gameOverlayRenderer.h>
#include "AfxStreams.h"
#include "addresses.h"
#include "CampathDrawer.h"
#include "csgo_S_StartSound.h"
#include "d3d9Hooks.h"

#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <cctype>
#include <sstream>
#include <iomanip>

extern WrpVEngineClient * g_VEngineClient;


CON_COMMAND(__mirv_test4, "")
{
	if(2 <= args->ArgC())
	{
		if(0 != atoi(args->ArgV(1)))
		{
			AfxD3D9OverrideBegin_D3DRS_ZWRITEENABLE(FALSE);
		}
		else
		{
			AfxD3D9OverrideEnd_D3DRS_ZWRITEENABLE();
		}
	}
}



CON_COMMAND(__mirv_test3, "")
{
	Tier0_Msg("CamPathValuePiggyBack::GetInstanceCount() == %i\n", CamPathValuePiggyBack::GetInstanceCount());
}

CON_COMMAND(__mirv_test2, "")
{
	WrpVEngineClientDemoInfoEx * di = g_VEngineClient->GetDemoInfoEx();

	if(di && g_Hook_VClient_RenderView.GetGlobals())
	{
		Tier0_Msg(
			"GetDemoRecordingTick=%i\n"
			"GetDemoPlaybackTick=%i\n"
			"GetDemoPlaybackStartTick=%i\n"
			"GetDemoPlaybackTimeScale=%f\n"
			"GetDemoPlaybackTotalTicks=%i\n",
			di->GetDemoRecordingTick(),
			di->GetDemoPlaybackTick(),
			di->GetDemoPlaybackStartTick(),
			di->GetDemoPlaybackTimeScale(),
			di->GetDemoPlaybackTotalTicks()
		);

		double curTime = g_Hook_VClient_RenderView.GetGlobals()->curtime_get();
		int client_current_tick = di->GetDemoPlaybackTick();
		double tick_interval = g_Hook_VClient_RenderView.GetGlobals()->interval_per_tick_get();
		double interpolation_amount = g_Hook_VClient_RenderView.GetGlobals()->interpolation_amount_get();

		double result = client_current_tick * tick_interval + interpolation_amount* tick_interval;

		Tier0_Msg(
			"curtime(%f) == client_current_tick(%i) * tick_interval(%f) +interpolation_amount(%f) * tick_interval == %f\n",
			curTime,
			client_current_tick,
			tick_interval,
			interpolation_amount,
			result
		);
	}
}

bool g_bD3D9DebugPrint = false;

CON_COMMAND(__mirv_test, "")
{
	g_bD3D9DebugPrint = true;
}

CON_COMMAND(__mirv_skyboxscale, "print skyboxscale in CS:GO")
{
	if(AFXADDR_GET(csgo_pLocalPlayer) && 0 != *(unsigned char **)AFXADDR_GET(csgo_pLocalPlayer))
	{
		int skyBoxScale = *(int *)(*(unsigned char **)AFXADDR_GET(csgo_pLocalPlayer) +AFXADDR_GET(csgo_C_BasePlayer_OFS_m_skybox3d_scale));

		Tier0_Msg("skyBoxScale: %i\n", skyBoxScale);
	}
	else
		Tier0_Msg("skyBoxScale: n/a\n");
}

CON_COMMAND(mirv_streams, "Access to streams system.")
{
	int argc = args->ArgC();

	if(2 <= argc)
	{
		char const * cmd1 = args->ArgV(1);

		if(!_stricmp(cmd1, "add"))
		{
			if(3 <= argc)
			{
				char const * cmd2 = args->ArgV(2);

				if(!_stricmp(cmd2, "normal"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						g_AfxStreams.Console_AddStream(cmd3);

						return;
					}

					Tier0_Msg(
						"mirv_streams add normal <name> - Add a normal stream with name <name>.\n"
					);
					return;
				}
				else
				if(!_stricmp(cmd2, "baseFx"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						g_AfxStreams.Console_AddBaseFxStream(cmd3);

						return;
					}

					Tier0_Msg(
						"mirv_streams add baseFx <name> - Add a baseFx stream with name <name>.\n"
					);
					return;
				}
				else
				if(!_stricmp(cmd2, "depth"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						g_AfxStreams.Console_AddDepthStream(cmd3);

						return;
					}

					Tier0_Msg(
						"mirv_streams add depth <name> - Add a depth stream with name <name>.\n"
					);
					return;
				}
				else
				if(!_stricmp(cmd2, "matteWorld"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						g_AfxStreams.Console_AddMatteWorldStream(cmd3);

						return;
					}

					Tier0_Msg(
						"mirv_streams add matteWorld <name> - Add a matte world stream with name <name>.\n"
					);
					return;
				}
				else
				if(!_stricmp(cmd2, "depthWorld"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						g_AfxStreams.Console_AddDepthWorldStream(cmd3);

						return;
					}

					Tier0_Msg(
						"mirv_streams add depthWorld <name> - Add a depth world stream with name <name>.\n"
					);
					return;
				}
				else
				if(!_stricmp(cmd2, "matteEntity"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						g_AfxStreams.Console_AddMatteEntityStream(cmd3);

						return;
					}

					Tier0_Msg(
						"mirv_streams add matteEnitity <name> - Add a matte entity stream with name <name>.\n"
					);
					return;
				}
				else
				if(!_stricmp(cmd2, "depthEntity"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						g_AfxStreams.Console_AddDepthEntityStream(cmd3);

						return;
					}

					Tier0_Msg(
						"mirv_streams add depthEntity <name> - Add a depth entity stream with name <name>.\n"
					);
					return;
				}
				else
				if(!_stricmp(cmd2, "developer"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						g_AfxStreams.Console_AddDeveloperStream(cmd3);

						return;
					}

					Tier0_Msg(
						"mirv_streams add developer <name> - Add a developer stream with name <name>.\n"
					);
					return;
				}				
				else
				if(!_stricmp(cmd2, "alphaMatteEntity"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						g_AfxStreams.Console_AddAlphaMatteEntityStream(cmd3);

						return;
					}

					Tier0_Msg(
						"mirv_streams add alphaMatteEntity <name> - Add a alpha matte entity stream with name <name>.\n"
					);
					return;
				}				
				else
				if(!_stricmp(cmd2, "alphaMatte"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						g_AfxStreams.Console_AddAlphaMatteStream(cmd3);

						return;
					}

					Tier0_Msg(
						"mirv_streams add alphaMatte <name> - Add a alpha matte stream with name <name>.\n"
					);
					return;
				}				
				else
				if(!_stricmp(cmd2, "alphaEntity"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						g_AfxStreams.Console_AddAlphaEntityStream(cmd3);

						return;
					}

					Tier0_Msg(
						"mirv_streams add alphaEntity <name> - Add a alpha entity stream with name <name>.\n"
					);
					return;
				}				
				else
				if(!_stricmp(cmd2, "alphaWorld"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						g_AfxStreams.Console_AddAlphaWorldStream(cmd3);

						return;
					}

					Tier0_Msg(
						"mirv_streams add alphaWorld <name> - Add a alpha world stream with name <name>.\n"
					);
					return;
				}				
			}

			Tier0_Msg(
				"mirv_streams add normal [...] - Add a normal stream.\n"
				"mirv_streams add baseFx [...] - Add a stream that allows effects, but should not look different from normal stream unless you edit its settings.\n"
				"mirv_streams add depth [...] - Add a depth stream. WILL NOT WORK PROPERLY ATM!\n"
				"mirv_streams add matteWorld [...] - Add a matte world stream.\n"
				"mirv_streams add depthWorld [...] - Add a depth world stream. WILL NOT WORK PROPERLY ATM!\n"
				"mirv_streams add matteEntity [...] - Add a matte entity stream.\n"
				"mirv_streams add depthEntity [...] - Add a depth entity stream. WILL NOT WORK PROPERLY ATM!\n"
				"mirv_streams add developer [...] - Add a developer stream.\n"
				"mirv_streams add alphaMatteEntity [...] - Add a entity stream with alpha matte combined into a single stream.\n"
				"mirv_streams add alphaWorld [...] - Add a alpha world stream.\n"
				"mirv_streams add alphaMatte [...] - Add a alpha matte stream (alpha channel of alphaMatteEntity).\n"
				"mirv_streams add alphaEntity [...] - Add a alpha entity stream (color channel of alphaMatteEntity).\n"
			);
			return;
		}
		else
		if(!_stricmp(cmd1, "edit"))
		{
			if(3 <= argc)
			{
				char const * cmd2 = args->ArgV(2);

				std::string prefix("mirv_streams edit ");
				prefix.append(cmd2);
				
				g_AfxStreams.Console_EditStream(cmd2, args, 3, prefix.c_str());
				return;
			}

			Tier0_Msg(
				"mirv_streams edit <streamName> [...] - Edit the stream with name <streamName>, you can get the value from mirv_streams print.\n"
			);
			return;
		}
		else
		if(!_stricmp(cmd1, "remove"))
		{
			if(3 <= argc)
			{
				char const * cmd2 = args->ArgV(2);
				
				g_AfxStreams.Console_RemoveStream(cmd2);
				return;
			}

			Tier0_Msg(
				"mirv_streams remove <streamName> - Remove a stream with name <streamName>, you can get the value from mirv_streams print.\n"
			);
			return;
		}
		else
		if(!_stricmp(cmd1, "preview"))
		{
			if(3 <= argc)
			{
				char const * cmd2 = args->ArgV(2);
				
				g_AfxStreams.Console_PreviewStream(cmd2);
				return;
			}

			Tier0_Msg(
				"mirv_streams preview <streamName> - Preview the stream with name <streamName>, you can get the value from mirv_streams print. To end previewing enter \"\" (empty string) for <streamName>!\n"
			);
			return;
		}
		else
		if(!_stricmp(cmd1, "print"))
		{
			g_AfxStreams.Console_PrintStreams();

			return;
		}
		else
		if(!_stricmp(cmd1, "record"))
		{
			if(3 <= argc)
			{
				char const * cmd2 = args->ArgV(2);

				if(!_stricmp(cmd2, "name"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);
						g_AfxStreams.Console_RecordName_set(cmd3);
						return;
					}

					Tier0_Msg(
						"mirv_streams record name <name> - Set record name to <name>.\n"
						"Current value: %s.\n",
						g_AfxStreams.Console_RecordName_get()
					);
					return;
				}
				else
				if(!_stricmp(cmd2, "start"))
				{
					g_AfxStreams.Console_Record_Start();
					return;
				}
				else
				if(!_stricmp(cmd2, "end"))
				{
					g_AfxStreams.Console_Record_End();
					return;
				}
				else
				if(!_stricmp(cmd2, "format"))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);
						g_AfxStreams.Console_RecordFormat_set(cmd3);
						return;
					}

					Tier0_Msg(
						"mirv_streams record format tga|bmp - Set record format to tga or bmp.\n"
						"Current value: %s.\n",
						g_AfxStreams.Console_RecordFormat_get()
					);
					return;
				}

			}

			Tier0_Msg(
				"mirv_streams record name [...] - Set/get record name.\n"
				"mirv_streams record start - Begin recording.\n"
				"mirv_streams record end - End reocrding.\n"
				"mirv_streams record format [...] - Set/get file format.\n"
			);
			return;
		}
	}

	Tier0_Msg(
		"mirv_streams add [...]- Add a stream.\n"
		"mirv_streams edit [...]- Edit a stream.\n"
		"mirv_streams remove [...] - Remove a stream.\n"
		"mirv_streams preview [...] - Preview a stream.\n"
		"mirv_streams print - Print current streams.\n"
		"mirv_streams record [...] - Recording control.\n"
	);
	return;
}

void ClearMaterialInfoSet();
void PrintMaterialInfoSetToFile(void);
extern bool g_CollectMaterialInfo;

CON_COMMAND(__mirv_materialinfo, "")
{
	int argc = args->ArgC();

	if(2 <= argc)
	{
		char const * cmd1 = args->ArgV(1);

		if(!_stricmp(cmd1, "collect") && 3 <= argc)
		{
			g_CollectMaterialInfo = atoi(args->ArgV(2)) != 0 ? true : false;
			return;
		}
		else
		if(!_stricmp(cmd1, "clear"))
		{
			ClearMaterialInfoSet();
			return;
		}
		else
		if(!_stricmp(cmd1, "write"))
		{
			PrintMaterialInfoSetToFile();
			return;
		}
	}		

	Tier0_Msg(
		"__mirv_materialinfo collect 0|1 - Enable / disable collecting material info.\n"
		"__mirv_materialinfo clear - Clear the collected set.\n"
		"__mirv_materialinfo write - Write collected data to afx_material_info.txt in csgo.exe folder.\n"
	);
	return;

}

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

bool GetCurrentDemoTick(int &outTick)
{
	WrpVEngineClientDemoInfoEx * di = g_VEngineClient->GetDemoInfoEx();

	if(di)
	{
		outTick = di->GetDemoPlaybackTick();
		return true;
	}

	return false;
}

bool GetDemoTickFromTime(double curTime, double time, int &outTick)
{
	WrpVEngineClientDemoInfoEx * di = g_VEngineClient->GetDemoInfoEx();
	WrpGlobals * gl = g_Hook_VClient_RenderView.GetGlobals();

	if(di && gl)
	{
		int client_current_tick = di->GetDemoPlaybackTick();

		double tick_interval = gl->interval_per_tick_get();

		double interpolation_amount = gl->interpolation_amount_get();

		double demoTime = (client_current_tick +interpolation_amount) * tick_interval;

		double deltaTime = curTime -demoTime;

		time -= deltaTime;

		outTick = round(time / tick_interval);

		return true;
	}

	return false;
}

bool GetDemoTimeFromTime(double curTime, double time, double &outDemoTime)
{
	WrpVEngineClientDemoInfoEx * di = g_VEngineClient->GetDemoInfoEx();
	WrpGlobals * gl = g_Hook_VClient_RenderView.GetGlobals();

	if(di && gl)
	{
		int client_current_tick = di->GetDemoPlaybackTick();

		double tick_interval = gl->interval_per_tick_get();

		double interpolation_amount = gl->interpolation_amount_get();

		double demoTime = (client_current_tick +interpolation_amount) * tick_interval;

		double deltaTime = curTime -demoTime;

		time -= deltaTime;

		outDemoTime = time;

		return true;

	}

	return false;
}

bool GetCurrentDemoTime(double &outDemoTime)
{
	WrpVEngineClientDemoInfoEx * di = g_VEngineClient->GetDemoInfoEx();
	WrpGlobals * gl = g_Hook_VClient_RenderView.GetGlobals();

	if(gl)
	{
		int client_current_tick = di->GetDemoPlaybackTick();

		double tick_interval = gl->interval_per_tick_get();

		double interpolation_amount = gl->interpolation_amount_get();

		double demoTime = (client_current_tick +interpolation_amount) * tick_interval;

		outDemoTime = demoTime;

		return true;
	}

	return false;
}

void PrintTimeFormated(double time)
{
	int seconds = (int)time % 60;

	time /= 60;
	int minutes = (int)time % 60;

	time /= 60;
	int hours = (int)time;

	std::ostringstream oss;

	oss << std::setfill('0') << std::setw(2);

	if(hours)
	{
		oss << hours << "h";
	}

	oss << minutes << "m" << seconds << "s";
	
	Tier0_Msg("%s", oss.str().c_str());
}

CON_COMMAND(mirv_campath,"camera paths")
{
	if(!g_Hook_VClient_RenderView.IsInstalled())
	{
		Tier0_Warning("Error: Hook not installed.\n");
		return;
	}

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
					g_Hook_VClient_RenderView.LastCameraAngles[2],
					g_Hook_VClient_RenderView.LastCameraFov
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
					"Did you add enough points already?\n"
				);

			return;
		}
		else if(!_stricmp("draw", subcmd))
		{
			if(3 <= argc)
			{
				char const * cmd2 = args->ArgV(2);

				if(!_stricmp("enabled", cmd2))
				{
					if(4 <= argc)
					{
						char const * cmd3 = args->ArgV(3);

						bool enabled = 0 != atoi(cmd3);

						if(enabled)
						{
							Tier0_Msg("AFXINFO: Forcing mat_queue_mode 0\n");
							g_VEngineClient->ExecuteClientCmd("mat_queue_mode 0");
						}

						g_CampathDrawer.Draw_set(enabled);
						return;
					}

					Tier0_Msg(
						"mirv_campath draw enabled 0|1 - enable (1) / disable (0) drawing.\n"
						"Current value: %s\n",
						g_CampathDrawer.Draw_get() ? "1 (enabled)" : "0 (disabled)"
					);
					return;
				}
			}

			Tier0_Msg("mirv_campath draw enabled [...] - enable / disable drawing.\n");
			return;
		}
		else if(!_stricmp("clear", subcmd) && 2 == argc)
		{
			g_Hook_VClient_RenderView.m_CamPath.Clear();

			return;
		}
		else if(!_stricmp("print", subcmd) && 2 == argc)
		{
			Tier0_Msg("passed? selected? id: tick[approximate!], demoTime[approximate!], gameTime -> (x,y,z) fov (pitch,yaw,roll)\n");

			double curtime = g_Hook_VClient_RenderView.GetCurTime();
			
			int i=0;
			for(CamPathIterator it = g_Hook_VClient_RenderView.m_CamPath.GetBegin(); it != g_Hook_VClient_RenderView.m_CamPath.GetEnd(); ++it)
			{
				double vieworigin[3];
				double viewangles[3];
				double fov;

				double time = it.GetTime();
				CamPathValue val = it.GetValue();
				bool selected = it.IsSelected();

				vieworigin[0] = val.X;
				vieworigin[1] = val.Y;
				vieworigin[2] = val.Z;
				viewangles[0] = val.Pitch;
				viewangles[1] = val.Yaw;
				viewangles[2] = val.Roll;
				fov = val.Fov;

				Tier0_Msg(
					"%s %s %i: ",
					time <= curtime ? "Y" : "n",
					selected ? "Y" : "n",
					i
				);
				
				int myTick;
				if(GetDemoTickFromTime(curtime, time, myTick))
					Tier0_Msg("%i", myTick);
				else
					Tier0_Msg("n/a");

				Tier0_Msg(", ");

				double myDemoTime;
				if(GetDemoTimeFromTime(curtime, time, myDemoTime))
					PrintTimeFormated(myDemoTime);
				else
					Tier0_Msg("n/a");

				Tier0_Msg(", %f -> (%f,%f,%f) %f (%f,%f,%f)\n",
					time,
					vieworigin[0],vieworigin[1],vieworigin[2],
					fov,
					viewangles[0],viewangles[1],viewangles[2]
				);

				i++;
			}
			
			Tier0_Msg("----\n");
			
			Tier0_Msg("Current tick: ");
			int curTick;
			bool hasCurTick;
			if(hasCurTick = GetCurrentDemoTick(curTick))
				Tier0_Msg("%i", curTick);
			else
				Tier0_Msg("n/a");
			Tier0_Msg(", Current demoTime: ");
			double curDemoTime;
			if(hasCurTick && GetCurrentDemoTime(curDemoTime))
				PrintTimeFormated(curDemoTime);
			else
				Tier0_Msg("n/a");
			Tier0_Msg(", Current gameTime: %f\n", curtime);
			
			Tier0_Msg("Current (x,y,z) fov (pitch,yaw,roll): (%f,%f,%f), %f, (%f,%f,%f)\n",
				g_Hook_VClient_RenderView.LastCameraOrigin[0],
				g_Hook_VClient_RenderView.LastCameraOrigin[1],
				g_Hook_VClient_RenderView.LastCameraOrigin[2],
				g_Hook_VClient_RenderView.LastCameraFov,
				g_Hook_VClient_RenderView.LastCameraAngles[0],
				g_Hook_VClient_RenderView.LastCameraAngles[1],
				g_Hook_VClient_RenderView.LastCameraAngles[2]
			);

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
		else if(!_stricmp("edit", subcmd))
		{	
			if(3 <= argc)
			{
				const char * arg2 = args->ArgV(2);
			
				if(!_stricmp("start", arg2))
				{
					if(3 == argc)
					{
						g_Hook_VClient_RenderView.m_CamPath.SetStart(
							g_Hook_VClient_RenderView.GetCurTime()
						);

						return;
					}
					else
					if(3 < argc)
					{
						const char * arg3 = args->ArgV(3);

						if(!_stricmp("abs",arg3) && 5 <= argc)
						{
							char const * arg4 = args->ArgV(4);

							g_Hook_VClient_RenderView.m_CamPath.SetStart(
								atof(arg4)
							);

							return;
						}
						else
						if(StringBeginsWith(arg3, "delta") && 4 == argc)
						{
							if(StringBeginsWith(arg3, "delta+"))
							{
								arg3 += strlen("delta+");

								if(1 <= g_Hook_VClient_RenderView.m_CamPath.GetSize())
									g_Hook_VClient_RenderView.m_CamPath.SetStart(
										g_Hook_VClient_RenderView.m_CamPath.GetLowerBound() + atof(arg3)
									);

								return;
							}
							else
							if(StringBeginsWith(arg3, "delta-"))
							{
								arg3 += strlen("delta-");

								if(1 <= g_Hook_VClient_RenderView.m_CamPath.GetSize())
									g_Hook_VClient_RenderView.m_CamPath.SetStart(
										g_Hook_VClient_RenderView.m_CamPath.GetLowerBound() - atof(arg3)
									);

								return;
							}
						}
					}

				}
				else
				if(!_stricmp("duration", arg2) && 4 <= argc)
				{
					double duration = atof(args->ArgV(3));

					g_Hook_VClient_RenderView.m_CamPath.SetDuration(
						duration
					);

					return;
				}
				else
				if(!_stricmp(arg2, "position") && 4 <= argc)
				{
					char const * arg3 = args->ArgV(3);

					if(!_stricmp("current", arg3))
					{
						g_Hook_VClient_RenderView.m_CamPath.SetPosition(
							g_Hook_VClient_RenderView.LastCameraOrigin[0],
							g_Hook_VClient_RenderView.LastCameraOrigin[1],
							g_Hook_VClient_RenderView.LastCameraOrigin[2]
						);

						return;
					}
					else
					if(6 <= argc)
					{
						char const * arg4 = args->ArgV(4);
						char const * arg5 = args->ArgV(5);
						g_Hook_VClient_RenderView.m_CamPath.SetPosition(
							atof(arg3),
							atof(arg4),
							atof(arg5)
						);

						return;
					}
				}
				else
				if(!_stricmp(arg2, "angles") && 4 <= argc)
				{
					char const * arg3 = args->ArgV(3);

					if(!_stricmp("current", arg3))
					{
						g_Hook_VClient_RenderView.m_CamPath.SetAngles(
							g_Hook_VClient_RenderView.LastCameraAngles[0],
							g_Hook_VClient_RenderView.LastCameraAngles[1],
							g_Hook_VClient_RenderView.LastCameraAngles[2]
						);

						return;
					}
					else
					if(6 <= argc)
					{
						char const * arg4 = args->ArgV(4);
						char const * arg5 = args->ArgV(5);
						g_Hook_VClient_RenderView.m_CamPath.SetAngles(
							atof(arg3),
							atof(arg4),
							atof(arg5)
						);

						return;
					}
				}
				else
				if(!_stricmp(arg2, "fov") && 4 <= argc)
				{
					char const * arg3 = args->ArgV(3);

					if(!_stricmp("current", arg3))
					{
						g_Hook_VClient_RenderView.m_CamPath.SetFov(
							g_Hook_VClient_RenderView.LastCameraFov
						);

						return;
					}
					else
					{
						g_Hook_VClient_RenderView.m_CamPath.SetFov(
							atof(arg3)
						);

						return;
					}
				}
				else
				if(!_stricmp(arg2, "rotate") && 6 <= argc)
				{
					char const * arg3 = args->ArgV(3);
					char const * arg4 = args->ArgV(4);
					char const * arg5 = args->ArgV(5);

					g_Hook_VClient_RenderView.m_CamPath.Rotate(
						atof(arg3),
						atof(arg4),
						atof(arg5)
					);
					return;
				}
			}

			Tier0_Msg(
				"mirv_campath edit start - Sets current demotime as new start time for the path [or selected keyframes].\n"
				"mirv_campath edit start abs <dValue> - Sets an given floating point value as new start time for the path [or selected keyframes].\n"
				"mirv_campath edit start delta(+|-)<dValue> - Offsets the path [or selected keyframes] by the given <dValue> delta value (Example: \"mirv_campah edit start delta-1.5\" moves the path [or selected keyframes] 1.5 seconds back in time).\n"
				"mirv_campath edit duration <dValue> - set floating point value <dValue> as new duration for the path [or selected keyframes] (in seconds). Please see remarks in HLAE manual.\n"
				"mirv_campath edit position current|(<dX> <dY> <dZ>) - Edit position of the path [or selected keyframes]. The position is applied to the center of the bounding box (\"middle\") of all [or the selected] keyframes, meaning the keyframes are moved releative to that. Current uses the current camera position, otherwise you can give the exact position.\n"
				"mirv_campath edit angles current|(<dPitchY> <dYawZ> <dRollX>) - Edit angles of the path [or selected keyframes]. All keyframes are assigned the same angles. Current uses the current camera angles, otherwise you can give the exact angles.\n"
				"mirv_campath edit fov current|<dFov> - Similar to mirv_campath edit angles, except for field of view (fov).\n"
				"mirv_campath edit rotate <dPitchY> <dYawZ> <dRollX>\n - Rotate path [or selected keyframes] around the middle of their bounding box by the given angles in degrees.\n"
			);
			return;
		}
		else if(!_stricmp("select", subcmd))
		{	
			if(3 <= argc)
			{
				const char * cmd2 = args->ArgV(2);

				if(!_stricmp(cmd2, "all"))
				{
					g_Hook_VClient_RenderView.m_CamPath.SelectAll();
					return;
				}
				else
				if(!_stricmp(cmd2, "none"))
				{
					g_Hook_VClient_RenderView.m_CamPath.SelectNone();
					return;
				}
				else
				if(!_stricmp(cmd2, "invert"))
				{
					g_Hook_VClient_RenderView.m_CamPath.SelectInvert();
					return;
				}
				else
				{
					bool bOk = true;

					int idx = 2;
					bool add = false;
					if(!_stricmp(cmd2, "add"))
					{
						add = true;
						++idx;
					}

					bool isFromId = false;
					int fromId = 0;
					bool isFromCurrent = false;
					double fromValue = 0.0;
					if(idx < argc)
					{
						const char * fromArg = args->ArgV(idx);

						if(StringBeginsWith(fromArg, "#"))
						{
							isFromId = true;
							++fromArg;
							fromId = atoi(fromArg);
						}
						else if(!_stricmp(fromArg, "current"))
						{
							fromValue = g_Hook_VClient_RenderView.GetCurTime();
						}
						else if(StringBeginsWith(fromArg, "current+"))
						{
							fromValue = g_Hook_VClient_RenderView.GetCurTime();
							fromArg += strlen("current+");
							fromValue += atof(fromArg);
						}
						else if(StringBeginsWith(fromArg, "current-"))
						{
							fromValue = g_Hook_VClient_RenderView.GetCurTime();
							fromArg += strlen("current-");
							fromValue -= atof(fromArg);
						}
						else
						{
							fromValue = atof(fromArg);
						}

						++idx;
					}
					else
						bOk = false;

					bool isToId = false;
					int toId = 0;
					double toValue = 0.0;
					if(idx < argc)
					{
						const char * toArg = args->ArgV(idx);

						if(StringBeginsWith(toArg, "#"))
						{
							isToId = true;
							++toArg;
							toId = atoi(toArg);
						}
						else if(StringBeginsWith(toArg, "current+"))
						{
							toValue = g_Hook_VClient_RenderView.GetCurTime();
							toArg += strlen("current+");
							toValue += atof(toArg);
						}
						else if(StringBeginsWith(toArg, "current-"))
						{
							toValue = g_Hook_VClient_RenderView.GetCurTime();
							toArg += strlen("current-");
							toValue -= atof(toArg);
						}
						else
						{
							toValue = atof(toArg);
						}
						++idx;
					}
					else
						bOk = false;

					bOk = bOk && idx == argc;

					size_t selected = 0;

					if(bOk)
					{
						if(isFromId && isToId)
						{
							if(!add) g_Hook_VClient_RenderView.m_CamPath.SelectNone();
							selected = g_Hook_VClient_RenderView.m_CamPath.SelectAdd((size_t)fromId,(size_t)toId);
						}
						else
						if(!isFromId && isToId)
						{
							if(!add) g_Hook_VClient_RenderView.m_CamPath.SelectNone();
							selected = g_Hook_VClient_RenderView.m_CamPath.SelectAdd((double)fromValue,(size_t)toId);
						}
						else
						if(!isFromId && !isToId)
						{
							if(!add) g_Hook_VClient_RenderView.m_CamPath.SelectNone();
							selected = g_Hook_VClient_RenderView.m_CamPath.SelectAdd((double)fromValue,(double)toValue);
						}
						else bOk = false;
					}

					if(bOk)
					{
						Tier0_Msg("A total of %u keyframes is selected now.\n", selected);
						if(selected < 1)
							Tier0_Warning("WARNING: You have no keyframes selected, thus most operations like mirv_campath clear will think you mean all keyframes (i.e. clear all)!\n", selected);

						return;
					}
				}

			}

			Tier0_Msg(
				"mirv_campath select all - Select all points.\n"
				"mirv_campath select none - Selects no points.\n"
				"mirv_campath select invert - Invert selection.\n"
				"mirv_campath select [add] #<idBegin> #<idEnd> - Select keyframes starting at id <idBegin> and ending at id <idEnd>. If add is given, then selection is added to the current one.\n"
				"mirv_campath select [add] current[(+|-)<dOfsMin>]|<dMin> #<count> - Select keyframes starting at given time and up to <count> number of keyframes. If add is given, then selection is added to the current one.\n"
				"mirv_campath select [add] current[(+|-)<dOfsMin>]|<dMin> current[(+|-)<dOfsMax>]|<dMax> - Select keyframes betwen given star time and given end time . If add is given, then selection is added to the current one.\n"
				"Examples:\n"
				"mirv_campath select current #2 - Select two keyframes starting from current time.\n"
				"mirv_campath select add current #2 - Add two keyframes starting from current time to the current selection.\n"
				"mirv_campath select 64.5 #2 - Select two keyframes starting from time 64.5 seconds.\n"
				"mirv_campath select current-0.5 current+2.5 - Select keyframes between half a second earlier than now and 2.5 seconds later than now.\n"
				"mirv_campath select 128.0 current - Select keyframes between time 128.0 seconds and current time.\n"
				"mirv_campath select add 128.0 current+2.0 - Add keyframes between time 128.0 seconds and 2 seconds later than now to the current selection.\n"
				"Hint: All time values are in game time (in seconds).\n"
			);
			return;

		}
	}

	Tier0_Msg(
		"mirv_campath add - adds current demotime and view as keyframe\n"
		"mirv_campath enable 0|1 - set whether the camera splines are active or not. Please note that currently at least 4 Points are required to make it active successfully!\n"
		"mirv_campath draw [...] - controls drawing of the camera path.\n"
		"mirv_campath clear - removes all [or all selected] keyframes\n"
		"mirv_campath print - prints keyframes\n"
		"mirv_campath remove <id> - removes a keyframe\n"
		"mirv_campath load <fileName> - loads the campath from the file (XML format)\n"
		"mirv_campath save <fileName> - saves the campath to the file (XML format)\n"
		"mirv_campath edit [...] - edit properties of the path [or selected keyframes]\n"
		"mirv_campath select [...] - keyframe selection.\n"
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
			double fps = atof(args->ArgV(3));
			if(fps < 0.1f) fps = 0.1f;

			std::wstring wideFileName;
			if(
				!AnsiStringToWideString(fileName, wideFileName)
				|| !g_Hook_VClient_RenderView.ExportBegin(wideFileName.c_str(), 1.0/fps)
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

CON_COMMAND(mirv_cvar_hack, "")
{
	int argc = args->ArgC();

	if(3 <= argc)
	{
		char const * cvarName = args->ArgV(1);
		float cvarValue = (float)atof(args->ArgV(2));

		WrpConVarRef * cvar = new WrpConVarRef(cvarName);

		cvar->SetDirectHack(cvarValue);

		delete cvar;

		return;
	}

	Tier0_Msg(
		"mirv_cvar_hack <cvarName> <floatValue> - Force value directly, this will only work for true cvars (not cmds) and only for a subset of those, might have unwanted side effects - use with caution!\n"
	);
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
		if(0 == _stricmp("highLightId", arg1))
		{
			if(3 <= argc)
			{
				csgo_CHudDeathNotice_HighLightId = atoi(args->ArgV(2));
				return;
			}
			Tier0_Msg(
				"Usage:\n"
				"mirv_deathmsg highLightId -1|0|<id> - -1 is default behaviour, 0 is never highlight, otherwise <id> is the ID (you can get it from mirv_deathmsg debug) of the player you want to highlight.\n"
				"Current setting: %i\n",
				csgo_CHudDeathNotice_HighLightId
			);
			return;
		}
		else
		if(0 == _stricmp("highLightAssists", arg1))
		{
			if(3 <= argc)
			{
				csgo_CHudDeathNotice_HighLightAssists = 0 != atoi(args->ArgV(2));
				return;
			}
			Tier0_Msg(
				"Usage:\n"
				"mirv_deathmsg highLightAssists 0|1 - 0 is don't highlight asissts, 1 is highlight assists.\n"
				"Current setting: %s\n",
				csgo_CHudDeathNotice_HighLightAssists ? "1" : "0"
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
		"mirv_deathmsg block [...] - block specific death messages.\n"
		"mirv_deathmsg cfg [...] - configure death message properties, i.e. noticeLifeTime.\n"
		"mirv_deathmsg debug [...] - enable debug message in cosnole (allows finding player IDs).\n"
		"mirv_deathmsg highLightId [...] - control highlighting.\n"
		"mirv_deathmsg highLightAssists [...] - Whether to highlight assists for highLightId.\n"
	);
}

CON_COMMAND(mirv_fov,"allows overriding FOV (Filed Of View) of the camera")
{
	if(!g_Hook_VClient_RenderView.IsInstalled())
	{
		Tier0_Warning("Error: Hook not installed.\n");
		return;
	}

	int argc = args->ArgC();

	if(2 <= argc)
	{
		char const * arg1 = args->ArgV(1);

		if(0 == _stricmp("default", arg1))
		{
			g_Hook_VClient_RenderView.FovDefault();
			return;
		}
		else
		{
			g_Hook_VClient_RenderView.FovOverride(atof(arg1));
			return;
		}
	}

	Tier0_Msg(
		"Usage:\n"
		"mirv_fov f - Override fov with given floating point value (f).\n"
		"mirv_fov default - Revert to the game's default behaviour.\n"
	);
	{
		Tier0_Msg("Current value: ");

		double fovValue;
		if(!g_Hook_VClient_RenderView.GetFovOverride(fovValue))
			Tier0_Msg("default\n");
		else
			Tier0_Msg("%f\n", fovValue);
	}
}

CON_COMMAND(mirv_replace_name, "allows replacing player names")
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
		"mirv_replace_name debug 0|1 - print <playerId> -> <name> pairs into console as they get queried by the game.\n"
		"mirv_replace_name delete <playerId> - delete replacement for <playerId>.\n"
		"mirv_replace_name list - list all <playerId> -> <name> replacements currently active.\n"
		"mirv_replace_name clear - clear all replacements.\n"
	);
}

CON_COMMAND(mirv_input, "Input mode configuration.")
{
	int argc = args->ArgC();

	if(2 <= argc)
	{
		char const * arg1 = args->ArgV(1);

		if(0 == _stricmp("camera", arg1))
		{
			g_AfxHookSourceInput.SetCameraControlMode(true);
			return;
		}
		else
		if(0 == _stricmp("cfg", arg1))
		{
			if(3 <= argc)
			{
				char const * arg2 = args->ArgV(2);

				if(0 == _stricmp("msens", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.SetMouseSensitivity(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.GetMouseSensitivty());
					return;
				}
				else
				if(0 == _stricmp("ksens", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.SetKeyboardSensitivity(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.GetKeyboardSensitivty());
					return;
				}
				else
				if(0 == _stricmp("kForwardSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardForwardSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardForwardSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kBackwardSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardBackwardSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardBackwardSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kLeftSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardLeftSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardLeftSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kRightSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardRightSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardRightSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kUpSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardUpSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardUpSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kDownSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardDownSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardDownSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kPitchPositiveSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardPitchPositiveSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardPitchPositiveSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kPitchNegativeSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardPitchNegativeSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardPitchNegativeSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kYawPositiveSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardYawPositiveSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardYawPositiveSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kYawNegativeSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardYawNegativeSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardYawNegativeSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kRollPositiveSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardRollPositiveSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardRollPositiveSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kRollNegativeSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardRollNegativeSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardRollNegativeSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kFovPositiveSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardFovPositiveSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardFovPositiveSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("kFovNegativeSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.KeyboardFovNegativeSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.KeyboardFovNegativeSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("mYawSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.MouseYawSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.MouseYawSpeed_get());
					return;
				}
				else
				if(0 == _stricmp("mPitchSpeed", arg2))
				{
					if(4 <= argc)
					{
						double value = atof(args->ArgV(3));
						g_AfxHookSourceInput.MousePitchSpeed_set(value);
						return;
					}
					Tier0_Msg("Value: %f", g_AfxHookSourceInput.MousePitchSpeed_get());
					return;
				}
			}

			Tier0_Msg(
				"Usage:\n"
				"mirv_input cfg msens - Get mouse sensitiviy.\n"
				"mirv_input cfg msens <dValue> - Set mouse sensitiviy.\n"
				"mirv_input cfg ksens - Get keyboard sensitivity.\n"
				"mirv_input cfg ksens <dValue> - Set keyboard sensitivity.\n"
				"mirv_input cfg kForwardSpeed - Get value.\n"
				"mirv_input cfg kForwardSpeed <dValue> - Set value.\n"
				"mirv_input cfg kBackwardSpeed - Get value.\n"
				"mirv_input cfg kBackwardSpeed <dValue> - Set value.\n"
				"mirv_input cfg kLeftSpeed - Get value.\n"
				"mirv_input cfg kLeftSpeed <dValue> - Set value.\n"
				"mirv_input cfg kRightSpeed - Get value.\n"
				"mirv_input cfg kRightSpeed <dValue> - Set value.\n"
				"mirv_input cfg kUpSpeed - Get value.\n"
				"mirv_input cfg kUpSpeed <dValue> - Set value.\n"
				"mirv_input cfg kDownSpeed - Get value.\n"
				"mirv_input cfg kDownSpeed <dValue> - Set value.\n"
				"mirv_input cfg kPitchPositiveSpeed - Get value.\n"
				"mirv_input cfg kPitchPositiveSpeed <dValue> - Set value.\n"
				"mirv_input cfg kPitchNegativeSpeed - Get value.\n"
				"mirv_input cfg kPitchNegativeSpeed <dValue> - Set value.\n"
				"mirv_input cfg kYawPositiveSpeed - Get value.\n"
				"mirv_input cfg kYawPositiveSpeed <dValue> - Set value.\n"
				"mirv_input cfg kYawNegativeSpeed - Get value.\n"
				"mirv_input cfg kYawNegativeSpeed <dValue> - Set value.\n"
				"mirv_input cfg kRollPositiveSpeed - Get value.\n"
				"mirv_input cfg kRollPositiveSpeed <dValue> - Set value.\n"
				"mirv_input cfg kRollNegativeSpeed - Get value.\n"
				"mirv_input cfg kRollNegativeSpeed <dValue> - Set value.\n"
				"mirv_input cfg kFovPositiveSpeed - Get value.\n"
				"mirv_input cfg kFovPositiveSpeed <dValue> - Set value.\n"
				"mirv_input cfg kFovNegativeSpeed - Get value.\n"
				"mirv_input cfg kFovNegativeSpeed <dValue> - Set value.\n"
				"mirv_input cfg mYawSpeed - Get value.\n"
				"mirv_input cfg mYawSpeed <dValue> - Set value.\n"
				"mirv_input cfg mPitchSpeed - Get value.\n"
				"mirv_input cfg mPitchSpeed <dValue> - Set value.\n"
			);
			return;
		}
		else
		if(0 == _stricmp("end", arg1))
		{
			g_AfxHookSourceInput.SetCameraControlMode(false);
			return;
		}
		else
		if(0 == _stricmp("position", arg1))
		{
			if(5 == argc)
			{
				char const * arg2 = args->ArgV(2);
				char const * arg3 = args->ArgV(3);
				char const * arg4 = args->ArgV(4);
	
				g_Hook_VClient_RenderView.LastCameraOrigin[0] = atof(arg2);
				g_Hook_VClient_RenderView.LastCameraOrigin[1] = atof(arg3);
				g_Hook_VClient_RenderView.LastCameraOrigin[2] = atof(arg4);
				return;
			}

			Tier0_Msg(
				"mirv_input positon <x> <y> <z> - Set new position (only useful in camera input mode).\n"
				"Current value: %f %f %f\n"
				, g_Hook_VClient_RenderView.LastCameraOrigin[0]
				, g_Hook_VClient_RenderView.LastCameraOrigin[1]
				, g_Hook_VClient_RenderView.LastCameraOrigin[2]
			);
			return;
		}
		else
		if(0 == _stricmp("angles", arg1))
		{
			if(5 == argc)
			{
				char const * arg2 = args->ArgV(2);
				char const * arg3 = args->ArgV(3);
				char const * arg4 = args->ArgV(4);
	
				g_Hook_VClient_RenderView.LastCameraAngles[0] = atof(arg2);
				g_Hook_VClient_RenderView.LastCameraAngles[1] = atof(arg3);
				g_Hook_VClient_RenderView.LastCameraAngles[2] = atof(arg4);
				return;
			}

			Tier0_Msg(
				"mirv_input angles <yPitch> <xRoll> <zYaw> - Set new angles (only useful in camera input mode).\n"
				"Current value: %f %f %f\n"
				, g_Hook_VClient_RenderView.LastCameraAngles[0]
				, g_Hook_VClient_RenderView.LastCameraAngles[1]
				, g_Hook_VClient_RenderView.LastCameraAngles[2]
			);
			return;
		}
		else
		if(0 == _stricmp("fov", arg1))
		{
			if(3 == argc)
			{
				char const * arg2 = args->ArgV(2);
	
				g_Hook_VClient_RenderView.LastCameraFov = atof(arg2);
				return;
			}

			Tier0_Msg(
				"mirv_input fov <fov> - Set new fov (only useful in camera input mode).\n"
				"Current value: %f\n"
				, g_Hook_VClient_RenderView.LastCameraFov
			);
			return;
		}
	}

	Tier0_Msg(
		"Usage:\n"
		"mirv_input camera - Enable camera input mode, see HLAE manual for keys etc.\n"
		"mirv_input cfg [...] - Control input mode configuration.\n"
		"mirv_input end - End input mode(s).\n"
		"mirv_input positon [...]\n"
		"mirv_input angles [...]\n"
		"mirv_input fov [...]\n"
	);
}

CON_COMMAND(mirv_snd_timescale, "(CS:GO only) allows to override host_timescale value for sound system.")
{
	if(!Hook_csgo_SndMixTimeScalePatch())
	{
		Tier0_Warning("Error: Hook not installed.\n");
		return;
	}

	int argc = args->ArgC();

	if(2 <= argc)
	{
		const char * arg1 = args->ArgV(1);

		if(0 == _stricmp("default", arg1))
		{
			csgo_SndMixTimeScalePatch_enable = false;
			return;
		}
		else
		{
			csgo_SndMixTimeScalePatch_enable = true;
			csgo_SndMixTimeScalePatch_value = (float)atof(arg1);
			return;
		}
	}
	Tier0_Msg(
		"Usage:\n"
		"mirv_snd_timescale <fValue> - override sound system host_timescale value with floating point value <fValue>.\n"
		"mirv_snd_timescale default - don't override.\n"
	);
}

CON_COMMAND(mirv_gameoverlay, "GameOverlayRenderer control.")
{
	int argc = args->ArgC();

	if(3 <= argc)
	{
		const char * arg1 = args->ArgV(1);

		if(0 == _stricmp("enable", arg1))
		{
			bool value = 0 != atoi(args->ArgV(2));
			Tier0_Msg(
				"%s %s.\n",
				value ? "Enable" : "Disable",
				GameOverlay_Enable(value) ? "OK" : "FAILED"
			);
			return;
		}
	}

	Tier0_Msg(
		"Usage:\n"
		"mirv_gameoverlay enable 0|1 - Disable/Enable the GameOverlay (will only do s.th. useful when it was enabled initally).\n"
	);
}

CON_COMMAND(mirv_snd_filter, "Sound control (i.e. blocking sounds).")
{
	if(!csgo_S_StartSound_Install())
	{
		Tier0_Warning("Error: Hook not installed.\n");
		return;
	}

	int argc = args->ArgC();

	if(2 <= argc)
	{
		char const * arg1 = args->ArgV(1);

		if(0 == _stricmp("block", arg1))
		{
			if(3 <= argc)
			{
				const char * arg2 = args->ArgV(2);

				csgo_S_StartSound_Block_Add(arg2);
				return;
			}
		}
		else
		if(0 == _stricmp("print", arg1))
		{
			csgo_S_StartSound_Block_Print();
			return;
		}
		else
		if(0 == _stricmp("remove", arg1))
		{
			if(3 <= argc)
			{
				const char * arg2 = args->ArgV(2);

				csgo_S_StartSound_Block_Remove(atoi(arg2));
				return;
			}
		}
		else
		if(0 == _stricmp("clear", arg1))
		{
			csgo_S_StartSound_Block_Clear();
			return;
		}
		else
		if(0 == _stricmp("debug", arg1))
		{
			if(3 <= argc)
			{
				const char * arg2 = args->ArgV(2);

				g_csgo_S_StartSound_Debug = 0 != atoi(arg2);
				return;
			}
			
			Tier0_Msg("Current value: %s\n", g_csgo_S_StartSound_Debug ? "1" : "0");
			return;
		}
	}

	Tier0_Msg(
		"Usage:\n"
		"mirv_snd_filter block <mask> - Blocks given <mask> string (for format see bellow).\n"
		"mirv_snd_filter print - Prints current blocks.\n"
		"mirv_snd_filter remove <index> - Removes the block with index <index> (You can get that from the print sub-command).\n"
		"mirv_snd_filter clear - Clears all blocks.\n"
		"mirv_snd_filter debug 0|1 - Print sounds played into console.\n"
		"<mask> - string to match, where \\* = wildcard and \\\\ = \\\n"
	);
}
