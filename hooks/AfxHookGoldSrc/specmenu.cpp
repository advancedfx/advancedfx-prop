#include "stdafx.h"

#include <hlsdk.h>

#include <hooks/shared/detours.h>
#include "cmdregister.h"

#include "hl_addresses.h"

extern cl_enginefuncs_s *pEngfuncs;


REGISTER_CMD_FUNC(disable_specmenu)
{
	const char *gamedir = pEngfuncs->pfnGetGameDirectory();
	unsigned short *usCheck = NULL;

	if( !strcmp("ag",gamedir) )
	{
		usCheck = (unsigned short *) HL_ADDR_GET(UpdateSpectatorPanel_checkjmp_ag);
	}
	else if( !strcmp("tfc",gamedir) )
	{
		usCheck = (unsigned short *) HL_ADDR_GET(UpdateSpectatorPanel_checkjmp_tfc);
	}
	else if( !strcmp("ns",gamedir) )
	{
		usCheck = (unsigned short *) HL_ADDR_GET(UpdateSpectatorPanel_checkjmp_ns);
	}
	else if( !strcmp("valve",gamedir) )
	{
		usCheck = (unsigned short *) HL_ADDR_GET(UpdateSpectatorPanel_checkjmp_valve);
	}

	pEngfuncs->Con_DPrintf("0x%08x\n",usCheck);

	if (!usCheck)
	{
		pEngfuncs->Con_Printf(
			"Sorry, your mod (%s) is not supported for this command.\n"
			"May be you can remove the spec menu by modifing a .res file like for cstrike?\n",
			gamedir
		);
	}
	else if (pEngfuncs->Cmd_Argc() == 2)
	{
		int iOn = atoi(pEngfuncs->Cmd_Argv(1));

		MdtMemBlockInfos mbis;
		MdtMemAccessBegin(usCheck, 2, &mbis);
		if (!iOn)
		{
			*usCheck = 0x840f; // original JE code
		}
		else
		{
			*usCheck = 0xE990; // NOP + JMP opcode
		}
		MdtMemAccessEnd(&mbis);

	}
	else
		pEngfuncs->Con_Printf("Usage: " PREFIX "disable_specmenu 0 (disable) / 1 (enable)\n");

}