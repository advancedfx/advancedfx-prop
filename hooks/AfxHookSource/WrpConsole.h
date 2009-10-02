#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-10-02 by dominik.matrixstorm.com
//
// First changes:
// 2009-10-02 by dominik.matrixstorm.com

// Description:
// Wrapper(s) for Source engine ConCommands and ConVars.

#include "SourceInterfaces.h"


// WrpConCommand ///////////////////////////////////////////////////////////////

typedef void ( *WrpCommandCallback )( void );

class WrpConCommand
{
public:
	WrpConCommand(char const * name, WrpCommandCallback callback, char const * helpString = 0);
	virtual ~WrpConCommand();

	WrpCommandCallback GetCallback();
	char const * GetHelpString();
	char const * GetName();

private:
	WrpCommandCallback m_Callback;
	char * m_HelpString;
	char * m_Name;
};


// WrpConCommandsRegistrar_003 /////////////////////////////////////////////////

class WrpConCommandsRegistrar_003 :
	public IConCommandBaseAccessor_003
{
public:
	virtual bool RegisterConCommandBase( ConCommandBase_003 *pVar );
};

// WrpConCommandsRegistrar_004 /////////////////////////////////////////////////

class WrpConCommandsRegistrar_004 :
	public IConCommandBaseAccessor_004
{
public:
	virtual bool RegisterConCommandBase( ConCommandBase_004 *pVar );
};


// WrpConCommands //////////////////////////////////////////////////////////////

struct WrpConCommandsListEntry {
	WrpConCommand * Command;
	WrpConCommandsListEntry * Next;
};

class WrpConCommands
{
public:
	static void RegisterCommands(ICvar_003 * cvarIface);
	static void RegisterCommands(ICvar_004 * cvarIface);

	static void WrpConCommand_Register(WrpConCommand * cmd);
	static void WrpConCommand_Unregister(WrpConCommand * cmd);

	static bool WrpConCommandsRegistrar_003_Register( ConCommandBase_003 *pVar );
	static bool WrpConCommandsRegistrar_004_Register( ConCommandBase_004 *pVar );

private:
	static ICvar_003 * m_CvarIface_003;
	static ICvar_004 * m_CvarIface_004;
	static WrpConCommandsListEntry * m_CommandListRoot;
};