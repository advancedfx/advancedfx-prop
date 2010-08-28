#include "stdafx.h"

// Mirv Demo Tool

#include "mirv_commands.h"

#include "cmdregister.h"
#include "hooks/HookHw.h"
#include <hlsdk.h>
#include <list>

#define NULLPTR 0

struct AfxCvarEntry
{
	char * Name;
	char * Value;
	int Flags;
	struct cvar_s * * OutCvar;
};

struct AfxCmdEntry
{
	char * Name;
	void (*Function)(void);
};

std::list<AfxCvarEntry> & GetAfxCvarEntries() {
	static std::list<AfxCvarEntry> afxCvarEntries;
	return afxCvarEntries;
}

std::list<AfxCmdEntry> & GetAfxCmdEntries() {
	static std::list<AfxCmdEntry> afxCmdEntries;
	return afxCmdEntries;
}

void AfxRegisterCommands()
{
	// Register the cvars:
	{
		for(
			std::list<AfxCvarEntry>::iterator i = GetAfxCvarEntries().begin();
			i != GetAfxCvarEntries().end();
			i++
		) {
			if(NULLPTR != i->OutCvar)
				*(i->OutCvar) = pEngfuncs->pfnRegisterVariable(i->Name, i->Value, i->Flags);
			else
				pEngfuncs->pfnRegisterVariable(i->Name, i->Value, i->Flags);
		}
	}

	// Register the commands:
	{
		for(
			std::list<AfxCmdEntry>::iterator i = GetAfxCmdEntries().begin();
			i != GetAfxCmdEntries().end();
			i++
		)
			pEngfuncs->pfnAddCommand(i->Name, i->Function);
	}
}


RegisterCvar::RegisterCvar(char * name, char * value, int flags, struct cvar_s * * outCvar)
{
	AfxCvarEntry entry = { name, value, flags, outCvar };
	GetAfxCvarEntries().push_back(entry);
}

RegisterCmd::RegisterCmd(char * name, void (*function)(void))
{
	AfxCmdEntry entry = { name, function };
	GetAfxCmdEntries().push_back(entry);
}


REGISTER_DEBUGCMD_FUNC(listcmds)
{
	int cntCmds = 0;
	int cntVars = 0;

	pEngfuncs->Con_Printf("---- cvars ----\n");

	// Register the cvars:
	{
		for(
			std::list<AfxCvarEntry>::iterator i = GetAfxCvarEntries().begin();
			i != GetAfxCvarEntries().end();
			i++
		) {
			pEngfuncs->Con_Printf("%s, %s, 0x%08x\n", i->Name, i->Value, i->Flags);
			cntVars++;
		}
	}

	pEngfuncs->Con_Printf("---- cmds ----\n");

	// Register the commands:
	{
		for(
			std::list<AfxCmdEntry>::iterator i = GetAfxCmdEntries().begin();
			i != GetAfxCmdEntries().end();
			i++
		) {
			pEngfuncs->Con_Printf("%s\n", i->Name);
			cntCmds++;
		}
	}

	pEngfuncs->Con_Printf("----------------\nCvars:%i, Cmds: %i, Total: %i\n", cntVars, cntCmds, cntVars +cntCmds);
}
