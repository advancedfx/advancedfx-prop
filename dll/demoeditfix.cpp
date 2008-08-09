
#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "com_model.h"
#include "r_studioint.h"
#include "pm_defs.h"
#include "cvardef.h"
#include "entity_types.h"

#include <map>

#include "detours.h"
#include "cmdregister.h"

#include "hl_addresses.h"

extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s *pEngStudio;
extern playermove_s *ppmove;

std::map <int, int> g_DemoEditMappings;

// Addresses for TFC
#define DM_ADDR	HL_ADDR_HUDSPECTATOR_FUNC_TFC
#define A0_ADDR HL_ADDR_HUDSPECTATOR_CMPA0_TFC

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
	DWORD dwProt;
	BYTE *bCheck = (BYTE *) bounds_address /*A0_ADDR*/;

	VirtualProtect(bCheck, 1, PAGE_READWRITE, &dwProt);
	*bCheck = 0x10;
	VirtualProtect(bCheck, 1, dwProt, 0);
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

//
// TFC HLTV menu removal
//

/* Hints:
void TeamFortressViewport::UpdateSpectatorPanel()

0:010> s -a hl L4000000 "#Spec_Mode%d"
0197802c  23 53 70 65 63 5f 4d 6f-64 65 25 64 00 00 00 00  #Spec_Mode%d....

....

*/

REGISTER_CMD_FUNC(disable_specmenu)
{
	const char *gamedir = pEngfuncs->pfnGetGameDirectory();
	unsigned short *usCheck = NULL;

	if( !strcmp("ag",gamedir) )
	{
		usCheck = (unsigned short *)((unsigned char *)(GetModuleHandle("client.dll")) + HL_ADDR_UpdateSpectatorPanel_checkjmp_ag_clofs);
	}
	else if( !strcmp("tfc",gamedir) )
	{
		usCheck = (unsigned short *) HL_ADDR_UpdateSpectatorPanel_checkjmp_tfc;
	}
	else if( !strcmp("ns",gamedir) )
	{
		usCheck = (unsigned short *)((unsigned char *)(GetModuleHandle("client.dll")) + HL_ADDR_UpdateSpectatorPanel_checkjmp_ns_clofs);
	}
	else if( !strcmp("valve",gamedir) )
	{
		usCheck = (unsigned short *) HL_ADDR_UpdateSpectatorPanel_checkjmp_valve;
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

		DWORD dwProt;
		VirtualProtect(usCheck, 2, PAGE_READWRITE, &dwProt);
		if (!iOn)
		{
			*usCheck = 0x840f; // original JE code
		}
		else
		{
			*usCheck = 0xE990; // NOP + JMP opcode
		}
		VirtualProtect(usCheck, 2, dwProt, 0);

	}
	else
		pEngfuncs->Con_Printf("Usage: " PREFIX "tfc_specmenu 0 (disable) / 1 (enable)\n");

}