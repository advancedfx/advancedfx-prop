#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2013-05-13 dominik.matrixtstorm.com
//
// First changes:
// 2013-05-13 dominik.matrixtstorm.com

#include "cmdregister.h"
#include "filming.h"
#include "hooks/DemoPlayer/DemoPlayer.h"
#include "AfxGoldSrcComClient.h"
#include "hooks/HookHw.h"

REGISTER_DEBUGCMD_FUNC(tst_camera_broken)
{
	int argc = pEngfuncs->Cmd_Argc();

	if(2 <= argc)
	{
		char * subcmd = pEngfuncs->Cmd_Argv(1);

		if(!stricmp("add", subcmd) && 2 == argc)
		{
			g_AfxGoldSrcComClient.SendMessage(CLM_CameraAdd);
			PipeCom * pc = &g_AfxGoldSrcComClient;
			
			pc->Write((ComDouble)g_DemoPlayer->GetDemoTime());

			pc->Write((ComDouble)g_Filming.LastCameraOrigin[0]);
			pc->Write((ComDouble)g_Filming.LastCameraOrigin[1]);
			pc->Write((ComDouble)g_Filming.LastCameraOrigin[2]);
			pc->Write((ComDouble)g_Filming.LastCameraAngles[PITCH]);
			pc->Write((ComDouble)g_Filming.LastCameraAngles[YAW]);
			pc->Write((ComDouble)g_Filming.LastCameraAngles[ROLL]);

			return;
		}
		else if(!stricmp("active", subcmd) && 3 == argc)
		{
			g_AfxGoldSrcComClient.SendMessage(CLM_CameraActive);
			PipeCom * pc = &g_AfxGoldSrcComClient;

			pc->Write((ComBoolean)0 != atoi(pEngfuncs->Cmd_Argv(2)));

			return;
		}
		else if(!stricmp("clear", subcmd) && 2 == argc)
		{
			g_AfxGoldSrcComClient.SendMessage(CLM_CameraClear);

			return;
		}
		else if(!stricmp("print", subcmd) && 2 == argc)
		{
			g_AfxGoldSrcComClient.SendMessage(CLM_CameraPrint);
			PipeCom * pc = &g_AfxGoldSrcComClient;
			int len = pc->ReadInt32();

			pEngfuncs->Con_Printf("id: time: origin angles\n");
			
			for(int i=0; i<len; i++)
			{
				double vieworigin[3];
				double viewangles[3];

				double time = pc->ReadDouble();
				vieworigin[0] = pc->ReadDouble();
				vieworigin[1] = pc->ReadDouble();
				vieworigin[2] = pc->ReadDouble();
				viewangles[PITCH] = pc->ReadDouble();
				viewangles[YAW] = pc->ReadDouble();
				viewangles[ROLL] = pc->ReadDouble();

				pEngfuncs->Con_Printf(
					"%i: %f: (%f,%f,%f) (%f,%f,%f)\n",
					i, time,
					vieworigin[0],vieworigin[1],vieworigin[2],
					viewangles[PITCH],viewangles[YAW],viewangles[ROLL]
				);
			}
			pEngfuncs->Con_Printf("----\n");

			return;
		}
		else if(!stricmp("remove", subcmd) && 3 == argc)
		{
			g_AfxGoldSrcComClient.SendMessage(CLM_CameraRemove);
			PipeCom * pc = &g_AfxGoldSrcComClient;

			pc->Write((ComInt32)atoi(pEngfuncs->Cmd_Argv(2)));

			return;
		}
	}

	pEngfuncs->Con_Printf(
		"THIS COMMAND IS NOT SUPPORTED AT THE MOMENT, IT DOESN'T WORK PROPERLY!\n"
		PREFIX "camera add - adds current demotime and view as keyframe\n"
		PREFIX "camera active 0|1 - set whether the camera splines are active or not\n"
		PREFIX "camera clear - removes all keyframes\n"
		PREFIX "camera print - prints keyframes\n"
		PREFIX "camera remove <id> - removes a keyframe\n"
	);
	return;
}