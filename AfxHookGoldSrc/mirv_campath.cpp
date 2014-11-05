#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-11-03 dominik.matrixtstorm.com
//
// First changes:
// 2013-05-13 dominik.matrixtstorm.com

#include "cmdregister.h"
#include "filming.h"
#include "hooks/DemoPlayer/DemoPlayer.h"
#include "hooks/HookHw.h"

REGISTER_CMD_FUNC(campath)
{
	int argc = pEngfuncs->Cmd_Argc();

	if(2 <= argc)
	{
		char * subcmd = pEngfuncs->Cmd_Argv(1);

		if(!_stricmp("add", subcmd) && 2 == argc)
		{
			g_Filming.GetCamPath()->Add(
				g_DemoPlayer->GetDemoTime(),
				CamPathValue(
					g_Filming.LastCameraOrigin[0],
					g_Filming.LastCameraOrigin[1],
					g_Filming.LastCameraOrigin[2],
					g_Filming.LastCameraAngles[PITCH],
					g_Filming.LastCameraAngles[YAW],
					g_Filming.LastCameraAngles[ROLL]
				)
			);

			return;
		}
		else if(!_stricmp("enable", subcmd) && 3 == argc)
		{
			bool enable = 0 != atoi(pEngfuncs->Cmd_Argv(2));
			bool enabled = g_Filming.GetCamPath()->Enable(
				enable
			);

			if(enable && !enabled)
				pEngfuncs->Con_Printf(
					"Error: Could not enable CamPath.\n"
					"Did you add enough point already?\n"
				);

			return;
		}
		else if(!_stricmp("clear", subcmd) && 2 == argc)
		{
			g_Filming.GetCamPath()->Clear();

			return;
		}
		else if(!_stricmp("print", subcmd) && 2 == argc)
		{
			pEngfuncs->Con_Printf("passed id: time -> (x,y,z) (pitch,yaw,roll)\n");
			
			int i=0;
			for(CamPathIterator it = g_Filming.GetCamPath()->GetBegin(); it != g_Filming.GetCamPath()->GetEnd(); ++it)
			{
				double vieworigin[3];
				double viewangles[3];

				double curtime = g_DemoPlayer->GetDemoTime();
				double time = it.GetTime();
				CamPathValue val = it.GetValue();

				vieworigin[0] = val.X;
				vieworigin[1] = val.Y;
				vieworigin[2] = val.Z;
				viewangles[PITCH] = val.Pitch;
				viewangles[YAW] = val.Yaw;
				viewangles[ROLL] =  val.Roll;

				pEngfuncs->Con_Printf(
					"%s %i: %f -> (%f,%f,%f) (%f,%f,%f)\n",
					time <= curtime ? "Y" : "n",
					i, time,
					vieworigin[0],vieworigin[1],vieworigin[2],
					viewangles[PITCH],viewangles[YAW],viewangles[ROLL]
				);

				i++;
			}
			pEngfuncs->Con_Printf("---- Current time: %f\n", g_DemoPlayer->GetDemoTime());

			return;
		}
		else if(!_stricmp("remove", subcmd) && 3 == argc)
		{
			int idx = atoi(pEngfuncs->Cmd_Argv(2));
			int i=0;
			for(CamPathIterator it = g_Filming.GetCamPath()->GetBegin(); it != g_Filming.GetCamPath()->GetEnd(); ++it)
			{
				if(i == idx)
				{
					double time = it.GetTime();
					g_Filming.GetCamPath()->Remove(time);
					break;
				}
			}

			return;
		}
	}

	pEngfuncs->Con_Printf(
		"THIS COMMAND IS NOT SUPPORTED AT THE MOMENT, IT DOESN'T WORK PROPERLY!\n"
		PREFIX "campath add - adds current demotime and view as keyframe\n"
		PREFIX "campath enable 0|1 - set whether the camera splines are active or not. Please note that currently at least 4 Points are required to make it active successfully!\n"
		PREFIX "campath clear - removes all keyframes\n"
		PREFIX "campath print - prints keyframes\n"
		PREFIX "campath remove <id> - removes a keyframe\n"
	);
	return;
}