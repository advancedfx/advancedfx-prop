#include "stdafx.h"

#include <hlsdk.h>

#include <map>

#include <shared/detours.h>
#include "cmdregister.h"

#include "hl_addresses.h"

extern cl_enginefuncs_s *pEngfuncs;

std::map <int, int> g_DemoEditMappings;

// Addresses for TFC
#define DM_ADDR	HL_ADDR_GET(HudSpectator_tfc)
#define A0_ADDR HL_ADDR_GET(HudSpectator_cmp_tfc)

typedef bool (__stdcall *DirectorMessage_t)(DWORD *this_ptr, int iSize, void *pbuf);
DirectorMessage_t orig_DirectorMessage;

void __stdcall hook_DirectorMessage(DWORD *this_ptr, int iSize, void *pbuf)
{
	int msg = *((unsigned char *) pbuf);

	// Translation:
	//if (msg == 15)
	//	*((unsigned char *) pbuf) = 11;

	// Convert to our new id
	if (g_DemoEditMappings.find(msg) != g_DemoEditMappings.end())
	{
		int new_msg = g_DemoEditMappings[msg];
		*((unsigned char *) pbuf) = new_msg;
		
		pEngfuncs->Con_DPrintf("[DirectorMessage] Mapped message %d to %d\n", msg, new_msg);

		// Modify the flag too... this is becoming quite specific now
		// Anyway the 14th byte is normally the flag but if we've mapped it to
		// 4 then it is actually the target, so make sure it is zero'd
		if (new_msg == 4)
			*((unsigned char *) pbuf+14) = 0;
	}
	// If we're not converting this one and it's out of the original range (0x0A)
	// then don't call the hooked function and give the normal warning
	else if (msg > 0x0A)
	{
		pEngfuncs->Con_DPrintf("CHudSpectator :: DirectorMessage: unknown command %i.\n", msg);
		return;
	}

	orig_DirectorMessage(this_ptr, iSize, pbuf);
}

void PatchDMFunction(int dm_address, int dm_size, int bounds_address)
{
	// Redirect to hook
	orig_DirectorMessage = (DirectorMessage_t) DetourClassFunc((BYTE *) dm_address /*DM_ADDR*/, (BYTE *) hook_DirectorMessage, dm_size /*8*/);

	// TODO Change size check from 0A to whatever it shold be
	MdtMemBlockInfos mbis;
	BYTE *bCheck = (BYTE *) bounds_address /*A0_ADDR*/;

	MdtMemAccessBegin(bCheck, 1, &mbis);
	*bCheck = 0x10;
	MdtMemAccessEnd(&mbis);
}

REGISTER_DEBUGCMD_FUNC(demoedit_addmapping)
{
	if (pEngfuncs->Cmd_Argc() != 3)
	{
		pEngfuncs->Con_Printf("Usage: " DEBUG_PREFIX "demoedit_addmapping <orig> <new>\n");
		return;
	}

	int o = atoi(pEngfuncs->Cmd_Argv(1));
	int n = atoi(pEngfuncs->Cmd_Argv(2));

	g_DemoEditMappings[o] = n;
}

REGISTER_DEBUGCMD_FUNC(demoedit_delmapping)
{
	if (pEngfuncs->Cmd_Argc() != 2)
	{
		pEngfuncs->Con_Printf("Usage: " DEBUG_PREFIX "demoedit_delmapping <n>\n");
		return;
	}

	int i = atoi(pEngfuncs->Cmd_Argv(1));

	g_DemoEditMappings.erase(i);
}

REGISTER_DEBUGCMD_FUNC(demoedit_viewmapping)
{
	pEngfuncs->Con_Printf("Message mappings:\n");

	// Loop through
	for (std::map<int, int>::iterator iter = g_DemoEditMappings.begin(); iter != g_DemoEditMappings.end(); iter++)
	{
		pEngfuncs->Con_Printf("  %d -> %d\n", *iter, (*iter).first);
	}
}

REGISTER_CMD_FUNC(demoedit_fix)
{
	static bool bFixedMapping = false;

	if (bFixedMapping)
	{
		pEngfuncs->Con_Printf("You will need to restart before you can apply another fix\n");
		return;
	}

	if (pEngfuncs->Cmd_Argc() == 2)
	{
		if (strcmp(pEngfuncs->Cmd_Argv(1), "tfc") == 0)
		{
			g_DemoEditMappings.clear();
			g_DemoEditMappings[14] = 4;
			g_DemoEditMappings[15] = 11;
			PatchDMFunction((int)DM_ADDR, 8, (int)A0_ADDR);
			bFixedMapping = true;
			pEngfuncs->Con_Printf("Fixed TFC demoedit\n");
		}
		else
		{
			pEngfuncs->Con_Printf("Only tfc can be fixed currently\n");
		}
		return;
	}
	else if (pEngfuncs->Cmd_Argc() == 4)
	{
		int dm_addr = atoi(pEngfuncs->Cmd_Argv(1));
		int dm_size = atoi(pEngfuncs->Cmd_Argv(2));
		int bounds_addr = atoi(pEngfuncs->Cmd_Argv(3));

		if (dm_addr == 0 || dm_size == 0 || bounds_addr == 0)
			return;

		PatchDMFunction(dm_addr, dm_size, bounds_addr);
		bFixedMapping = true;
	}
	else
		pEngfuncs->Con_Printf("Usage: " PREFIX "demoedit_fix tfc\n");
}

