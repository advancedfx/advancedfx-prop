#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-11-12 dominik.matrixstorm.com
//
// First changes:
// 2009-10-02 dominik.matrixstorm.com

#include "WrpConsole.h"

#include <malloc.h>
#include <string.h>

#include <windows.h>

// WrpConCommand ///////////////////////////////////////////////////////////////

WrpConCommand::WrpConCommand(char const * name, WrpCommandCallback callback, char const * helpString) {
	m_Callback = callback;

	if(!helpString) helpString = "";
	m_HelpString =(char *) malloc((1+strlen(helpString))*sizeof(char));
	strcpy(m_HelpString, helpString);
	
	m_Name = (char *)malloc((1+strlen(name))*sizeof(char));
	strcpy(m_Name, name);

	WrpConCommands::WrpConCommand_Register(this);
}

WrpConCommand::~WrpConCommand() {
	WrpConCommands::WrpConCommand_Unregister(this);

	delete m_Name;

	delete m_HelpString;
}

WrpCommandCallback WrpConCommand::GetCallback() {
	return m_Callback;
}
char const * WrpConCommand::GetHelpString() {
	return m_HelpString;
}

char const * WrpConCommand::GetName() {
	return m_Name;
}


// WrpConCommands //////////////////////////////////////////////////////////////

ICvar_003 * WrpConCommands::m_CvarIface_003 = 0;
ICvar_004 * WrpConCommands::m_CvarIface_004 = 0;
ICvar_007 * WrpConCommands::m_CvarIface_007 = 0;
WrpConCommandsListEntry * WrpConCommands::m_CommandListRoot = 0;
IVEngineClient_012 * WrpConCommands::m_VEngineClient_012 = 0;

IVEngineClient_012 * WrpConCommands::GetVEngineClient_012() {
	return m_VEngineClient_012;
}

ICvar_007 * WrpConCommands::GetVEngineCvar007()
{
	return m_CvarIface_007;
}

void WrpConCommands::RegisterCommands(ICvar_003 * cvarIface, IVEngineClient_012 * vEngineClientInterface) {
	if(m_CvarIface_003)
		// already registered the current list
		return;

	m_CvarIface_003 = cvarIface;
	m_VEngineClient_012 = vEngineClientInterface;
	ConCommandBase_003::s_pAccessor = new WrpConCommandsRegistrar_003();

	for(WrpConCommandsListEntry * entry = m_CommandListRoot; entry; entry = entry->Next) {
		WrpConCommand * cmd = entry->Command;

		// will init themself since s_pAccessor is set:
		new ConCommand_003(cmd->GetName(), cmd->GetCallback(), cmd->GetHelpString(), FCVAR_CLIENTDLL);
	}
}

void WrpConCommands::RegisterCommands(ICvar_004 * cvarIface) {
	if(m_CvarIface_004)
		// already registered the current list
		return;

	m_CvarIface_004 = cvarIface;
	ConCommandBase_004::s_pAccessor = new WrpConCommandsRegistrar_004();

	for(WrpConCommandsListEntry * entry = m_CommandListRoot; entry; entry = entry->Next) {
		WrpConCommand * cmd = entry->Command;

		// will init themself since s_pAccessor is set:
		new ConCommand_004(cmd->GetName(), cmd->GetCallback(), cmd->GetHelpString(), FCVAR_CLIENTDLL);
	}
}

void WrpConCommands::RegisterCommands(ICvar_007 * cvarIface) {
	if(m_CvarIface_007)
		// already registered the current list
		return;

	m_CvarIface_007 = cvarIface;
	ConCommandBase_007::s_pAccessor = new WrpConCommandsRegistrar_007();

	for(WrpConCommandsListEntry * entry = m_CommandListRoot; entry; entry = entry->Next) {
		WrpConCommand * cmd = entry->Command;

		// will init themself since s_pAccessor is set:
		new ConCommand_007(cmd->GetName(), cmd->GetCallback(), cmd->GetHelpString(), FCVAR_CLIENTDLL);
	}
}

void WrpConCommands::WrpConCommand_Register(WrpConCommand * cmd) {
	WrpConCommandsListEntry * entry = new WrpConCommandsListEntry();
	entry->Command = cmd;
	entry->Next = m_CommandListRoot;
	m_CommandListRoot = entry;

	// if the list is already live, create (and thus register) the command instantly
	// in the engine:

	if(m_CvarIface_007)
		new ConCommand_007(cmd->GetName(), cmd->GetCallback(), cmd->GetHelpString());
	else if(m_CvarIface_004)
		new ConCommand_004(cmd->GetName(), cmd->GetCallback(), cmd->GetHelpString());
	else if(m_CvarIface_003)
		new ConCommand_003(cmd->GetName(), cmd->GetCallback(), cmd->GetHelpString());
}

void WrpConCommands::WrpConCommand_Unregister(WrpConCommand * cmd) {
	WrpConCommandsListEntry ** pLastNext = &m_CommandListRoot;
	for(WrpConCommandsListEntry * entry = m_CommandListRoot; entry; entry = entry->Next) {
		if(cmd == entry->Command) {
			*pLastNext = entry->Next;
			delete entry;
			break;
		}
		pLastNext = &(entry->Next);
	}
}

bool WrpConCommands::WrpConCommandsRegistrar_003_Register( ConCommandBase_003 *pVar ) {
	if(!m_CvarIface_003)
		return false;

//	MessageBox(0, "WrpConCommands::WrpConCommandsRegistrar_003_Register", "AFX_DEBUG", MB_OK);

	m_CvarIface_003->RegisterConCommandBase(pVar);
	return true;
}


bool WrpConCommands::WrpConCommandsRegistrar_004_Register( ConCommandBase_004 *pVar ) {
	if(!m_CvarIface_004)
		return false;

//	MessageBox(0, "WrpConCommands::WrpConCommandsRegistrar_004_Register", "AFX_DEBUG", MB_OK);

	m_CvarIface_004->RegisterConCommand(pVar);
	return true;
}

bool WrpConCommands::WrpConCommandsRegistrar_007_Register( ConCommandBase_007 *pVar ) {
	if(!m_CvarIface_007)
		return false;

//	MessageBox(0, "WrpConCommands::WrpConCommandsRegistrar_007_Register", "AFX_DEBUG", MB_OK);

	m_CvarIface_007->RegisterConCommand(pVar);
	return true;
}


// WrpConCommandsRegistrar_003 ////////////////////////////////////////////////////

bool WrpConCommandsRegistrar_003::RegisterConCommandBase( ConCommandBase_003 *pVar ) {
	return WrpConCommands::WrpConCommandsRegistrar_003_Register(pVar);
}

// WrpConCommandsRegistrar_004 ////////////////////////////////////////////////////

bool WrpConCommandsRegistrar_004::RegisterConCommandBase( ConCommandBase_004 *pVar ) {
	return WrpConCommands::WrpConCommandsRegistrar_004_Register(pVar);
}

// WrpConCommandsRegistrar_007 ////////////////////////////////////////////////////

bool WrpConCommandsRegistrar_007::RegisterConCommandBase( ConCommandBase_007 *pVar ) {
	return WrpConCommands::WrpConCommandsRegistrar_007_Register(pVar);
}
