#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-10-02 by dominik.matrixstorm.com
//
// First changes:
// 2009-09-30 by dominik.matrixstorm.com

// Based on Source engine SDK:
// Copyright (c) 1996-2005, Valve Corporation, All rights reserved

// Description:
// Interface definitions for accessing the Source engine.

typedef char tier0_char;

typedef void (*Tier0MsgFn)( const tier0_char* pMsg, ... );
typedef void (*Tier0DMsgFn)( const tier0_char *pGroupName, int level, const tier0_char *pMsg, ... );

// debug and message fns, available after tier0.dll has been loaded:
extern Tier0MsgFn Tier0_Msg;
extern Tier0DMsgFn Tier0_DMsg;
extern Tier0MsgFn Tier0_Warning;
extern Tier0DMsgFn Tier0_DWarning;
extern Tier0MsgFn Tier0_Log;
extern Tier0DMsgFn Tier0_DLog;
extern Tier0MsgFn Tier0_Error;

extern Tier0MsgFn Tier0_ConMsg;
extern Tier0MsgFn Tier0_ConWarning;
extern Tier0MsgFn Tier0_ConLog;
extern Tier0MsgFn Tier0_ConDMsg;
extern Tier0MsgFn Tier0_ConDWarning;
extern Tier0MsgFn Tier0_ConDLog;


#define FORCEINLINE __forceinline
#define FORCEINLINE_CVAR FORCEINLINE



#define FCVAR_NONE				0 
#define FCVAR_UNREGISTERED		(1<<0)
#define FCVAR_DEVELOPMENTONLY	(1<<1)
#define FCVAR_GAMEDLL			(1<<2)
#define FCVAR_CLIENTDLL			(1<<3)
#define FCVAR_HIDDEN			(1<<4)

#define CREATEINTERFACE_PROCNAME	"CreateInterface"


class CSysModule;

typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

CreateInterfaceFn Sys_GetFactory( CSysModule *pModule );

typedef float vec_t;


// Vector //////////////////////////////////////////////////////////////////////

class MdtVector;
typedef MdtVector Vector;

class MdtVector				
{
public:
	// Members
	vec_t x, y, z;

	// shortened.
};


// QAngle //////////////////////////////////////////////////////////////////////

class MdtQAngle;
typedef MdtQAngle QAngle;

class MdtQAngle				
{
public:
	// Members
	vec_t x, y, z;

	// shortened.
};


// Rendering related

class CGlobalVarsBase;

// Command / Cvar related:

// we hack around a bit here:
class IWrpCommandArgs;
typedef void (*WrpCommandCallback)(IWrpCommandArgs * args);

class ConVar_003;

typedef void ( *FnChangeCallback_003 )( ConVar_003 *var, char const *pOldString );
typedef void ( *FnCommandCallback_003 )( void );

#define COMMAND_COMPLETION_MAXITEMS_004 64
#define COMMAND_COMPLETION_ITEM_LENGTH_004 64

class CCommand_004;
class ConVar_004;
class IConVar_004;

typedef void ( *FnChangeCallback_t_004 )( IConVar_004 *var, const char *pOldValue, float flOldValue );
typedef void ( *FnCommandCallbackV1_t_004 )( void );
typedef void ( *FnCommandCallback_t_004 )( const CCommand_004 &command );

typedef int CVarDLLIdentifier_t_004;

class ICommandCallback_004
{
public:
	virtual void CommandCallback( const CCommand_004 &command ) = 0;
};


typedef int CVarDLLIdentifier_t_007;



// IConCommandBaseAccessor_003 /////////////////////////////////////////////////////

class ConCommandBase_003;

class IConCommandBaseAccessor_003
{
public:
	virtual bool RegisterConCommandBase( ConCommandBase_003 *pVar )=0;
};


// ConCommandBase_003 /////////////////////////////////////////////////////////////

class WrpConCommands;

/// <remarks> DO NOT CHANGE WITHOUT KNOWING WHAT YOU DO, DIRECTLY ACCESSED BY SOURCE ENGINE! </remarks>
/// <comments> I guess if Valve used a non determisitic C++ compiler they would
///		be screwed when sharing such classes among various compile units.
///		this also means we are screwed too easily when using a different compiler.
///		</comments>
class ConCommandBase_003
{
	friend WrpConCommands; // ugly hack, just like Valve did

public:
	ConCommandBase_003( void );
	ConCommandBase_003( char const *pName, char const *pHelpString = 0, int flags = 0 );

	virtual						~ConCommandBase_003( void );

	virtual	bool				IsCommand( void ) const;

	// Check flag
	virtual bool				IsBitSet( int flag ) const;
	// Set flag
	virtual void				AddFlags( int flags );

	// Return name of cvar
	virtual char const			*GetName( void ) const;

	// Return help text for cvar
	virtual char const			*GetHelpText( void ) const;

	// Deal with next pointer
	const ConCommandBase_003		*GetNext( void ) const;
	void						SetNext( ConCommandBase_003 *next );
	
	virtual bool				IsRegistered( void ) const;

	// Global methods
	static void _NOT_IMPLEMENTED_GetCommands( void );
	static void	_NOT_IMPLEMENTED_AddToList( void );
	static void	_NOT_IMPLEMENTED_RemoveFlaggedCommands( void );
	static void	_NOT_IMPLEMENTED_RevertFlaggedCvars( void );
	static void _NOT_IMPLEMENTED_FindCommand( void );

protected:
	virtual void Create(char const *pName, char const *pHelpString = 0, int flags = 0 );

	virtual void Init();

	void _NOT_IMPLEMENTED_CopyString( void );

	ConCommandBase_003 * m_Next;

	static ConCommandBase_003		*s_pConCommandBases;
	static IConCommandBaseAccessor_003	*s_pAccessor;

private:
	bool m_IsRegistered;
	char * m_Name;
	char * m_HelpText;
	int m_Flags;

};


// ConCommand_003 //////////////////////////////////////////////////////////////

/// <remarks> DO NOT CHANGE WITHOUT KNOWING WHAT YOU DO, DIRECTLY ACCESSED BY SOURCE ENGINE! </remarks>
/// <comments> I guess if Valve used a non determisitic C++ compiler they would
///		be screwed when sharing such classes among various compile units.
///		this also means we are screwed too easily when using a different compiler.
///		</comments>
class ConCommand_003 : public ConCommandBase_003
{
public:
	typedef ConCommandBase_003 BaseClass;

	ConCommand_003( void );

	/// <remarks> tweaked since we don't support completition and use a callback wrapper </remarks>
	ConCommand_003( char const *pName, WrpCommandCallback callback, char const *pHelpString = 0, int flags = 0);

	virtual						~ConCommand_003( void );

	virtual	bool				IsCommand( void ) const;

	/// <remarks> we don't support autocompletition, thus we always return 0 </remarks>
	virtual int					AutoCompleteSuggest(void * dummy1, void * dummy2);

	/// <remarks> we don't support autocompletition, thus we always return false </remarks>
	virtual bool				CanAutoComplete( void );

	// Invoke the function
	virtual void				Dispatch( void );

private:
	WrpCommandCallback			m_Callback;

	virtual void				Create( char const *pName, WrpCommandCallback callback, char const *pHelpString = 0, int flags = 0);

};


// IConCommandBaseAccessor_004 /////////////////////////////////////////////////

class ConCommandBase_004;

class IConCommandBaseAccessor_004
{
public:
	virtual bool RegisterConCommandBase( ConCommandBase_004 *pVar ) = 0;
};

// ConCommandBase_004 //////////////////////////////////////////////////////////

/// <remarks> DO NOT CHANGE WITHOUT KNOWING WHAT YOU DO, DIRECTLY ACCESSED BY SOURCE ENGINE! </remarks>
/// <comments> I guess if Valve used a non determisitic C++ compiler they would
///		be screwed when sharing such classes among various compile units.
///		this also means we are screwed too easily when using a different compiler.
///		</comments>
class ConCommandBase_004
{
	friend WrpConCommands; // ugly hack, just like Valve did

public:
	ConCommandBase_004( void );
	ConCommandBase_004( const char *pName, const char *pHelpString = 0, int flags = 0 );

	virtual						~ConCommandBase_004( void );

	virtual	bool				IsCommand( void ) const;

	// Check flag
	virtual bool				IsFlagSet( int flag ) const;
	// Set flag
	virtual void				AddFlags( int flags );

	// Return name of cvar
	virtual const char			*GetName( void ) const;

	// Return help text for cvar
	virtual const char			*GetHelpText( void ) const;

	// Deal with next pointer
	const ConCommandBase_004		*GetNext( void ) const;
	ConCommandBase_004				*GetNext( void );
	
	virtual bool				IsRegistered( void ) const;

	// Returns the DLL identifier
	virtual CVarDLLIdentifier_t_004	GetDLLIdentifier() const;

protected:
	virtual void Create(const char *pName, const char *pHelpString = 0, int flags = 0);

	// Used internally by OneTimeInit to initialize/shutdown
	virtual void Init();
	void _NOT_IMPLEMENTED_Shutdown();

	void _NOT_IMPLEMENTED_CopyString( void );

	static ConCommandBase_004 *s_pConCommandBases;
	static IConCommandBaseAccessor_004 *s_pAccessor;

private:
	ConCommandBase_004 * m_Next;
	bool m_IsRegistered;
	char * m_Name;
	char * m_HelpText;
	int m_Flags;
};

// ConCommand_004 //////////////////////////////////////////////////////////////

class ConCommand_004 : public ConCommandBase_004
{
public:
	typedef ConCommandBase_004 BaseClass;

	/// <remarks> tweaked since we don't support completition </remarks>
	ConCommand_004(const char *pName, FnCommandCallbackV1_t_004 callback,  const char *pHelpString = 0, int flags = 0);

	/// <remarks> tweaked since we don't support completition and use a callback wrapper </remarks>
	ConCommand_004(const char *pName, WrpCommandCallback callback, const char *pHelpString = 0, int flags = 0);

	/// <remarks> tweaked since we don't support completition </remarks>
	ConCommand_004(const char *pName, ICommandCallback_004 *pCallback, const char *pHelpString = 0, int flags = 0);

	virtual ~ConCommand_004(void);

	virtual	bool IsCommand(void) const;

	/// <remarks> we don't support autocompletition, thus we always return 0 </remarks>
	virtual int AutoCompleteSuggest(void * dummy1, void * dummy2);

	/// <remarks> we don't support autocompletition, thus we always return false </remarks>
	virtual bool CanAutoComplete( void );

	virtual void Dispatch( const CCommand_004 &command );

private:
	union
	{
		FnCommandCallbackV1_t_004 m_fnCommandCallbackV1;
		WrpCommandCallback m_fnCommandCallback;
		ICommandCallback_004 *m_pCommandCallback; 
	};

	bool m_bUsingNewCommandCallback : 1;
	bool m_bUsingCommandCallbackInterface : 1;
};


/// <comments> This is really not my fault, this is the way Valve did it!
///		If you ever wondered who passes s.th. depeding on compiler
///		optimization etc. among DLLs and different compile units
///		- well the Source SDK does - and this time not even
///		anything virtual here - hahahahahha just gr8.
///		</comments>
///	<remarks> Do not implement this or s.th. we just use this to access
///		the class memory in the source engine. </remarks>
class CCommand_004 abstract
{
public:
	/// <remarks> NOT_IMPLEMENTED </remarks>
	CCommand_004(void);

	/// <remarks> NOT_IMPLEMENTED </remarks>
	CCommand_004(void *, void *);

	void _NOT_IMPLEMENTED_Tokenize(void);
	void _NOT_IMPLEMENTED_Reset(void);

	int ArgC() const;
	const char **ArgV() const;
	const char *ArgS() const;					// All args that occur after the 0th arg, in string form
	const char *GetCommandString() const;		// The entire command in string form, including the 0th arg
	const char *operator[]( int nIndex ) const;	// Gets at arguments
	const char *Arg( int nIndex ) const;		// Gets at arguments
	
	void _NOT_IMPLEMENTED_FindArg(void) const;
	void _NOT_IMPLEMENTED_FindArgInt(void) const;

	static int MaxCommandLength();
	static void _NOT_IMPLEMENTED_DefaultBreakSet(void);

private:
	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

	int		m_nArgc;
	int		m_nArgv0Size;
	char	m_pArgSBuffer[ COMMAND_MAX_LENGTH ];
	char	m_pArgvBuffer[ COMMAND_MAX_LENGTH ];
	const char*	m_ppArgv[ COMMAND_MAX_ARGC ];
};

inline int CCommand_004::MaxCommandLength()
{
	return COMMAND_MAX_LENGTH - 1;
}

inline int CCommand_004::ArgC() const
{
	return m_nArgc;
}

inline const char **CCommand_004::ArgV() const
{
	return m_nArgc ? (const char**)m_ppArgv : 0;
}

inline const char *CCommand_004::ArgS() const
{
	return m_nArgv0Size ? &m_pArgSBuffer[m_nArgv0Size] : "";
}

inline const char *CCommand_004::GetCommandString() const
{
	return m_nArgc ? m_pArgSBuffer : "";
}

inline const char *CCommand_004::Arg( int nIndex ) const
{
	// FIXME: Many command handlers appear to not be particularly careful
	// about checking for valid argc range. For now, we're going to
	// do the extra check and return an empty string if it's out of range
	if ( nIndex < 0 || nIndex >= m_nArgc )
		return "";
	return m_ppArgv[nIndex];
}

inline const char *CCommand_004::operator[]( int nIndex ) const
{
	return Arg( nIndex );
}


// IAppSystem //////////////////////////////////////////////////////////////////

enum InitReturnVal_t
{
	INIT_FAILED = 0,
	INIT_OK,

	INIT_LAST_VAL,
};


class IAppSystem abstract
{
public:
	// Here's where the app systems get to learn about each other 
	virtual bool Connect( CreateInterfaceFn factory ) = 0;
	virtual void Disconnect() = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface( const char *pInterfaceName ) = 0;

	// Init, shutdown
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;
};

// IAppSystem_swarm ////////////////////////////////////////////////////////////

struct AppSystemInfo_t
{
	const char *m_pModuleName;
	const char *m_pInterfaceName;
};

enum AppSystemTier_t
{
	APP_SYSTEM_TIER0 = 0,
	APP_SYSTEM_TIER1,
	APP_SYSTEM_TIER2,
	APP_SYSTEM_TIER3,

	APP_SYSTEM_TIER_OTHER,
};

class IAppSystem_swarm abstract
{
public:
	// Here's where the app systems get to learn about each other 
	virtual bool Connect( CreateInterfaceFn factory ) = 0;
	virtual void Disconnect() = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface( const char *pInterfaceName ) = 0;

	// Init, shutdown
	virtual InitReturnVal_t Init() = 0;
	virtual void Shutdown() = 0;

	// Returns all dependent libraries
	virtual const AppSystemInfo_t* GetDependencies() = 0;

	// Returns the tier
	virtual AppSystemTier_t GetTier() = 0;

	// Reconnect to a particular interface
	virtual void Reconnect( CreateInterfaceFn factory, const char *pInterfaceName ) = 0;
};



// ICvar_003 ///////////////////////////////////////////////////////////////////

#define VENGINE_CVAR_INTERFACE_VERSION_003 "VEngineCvar003"

/// <comments>
///		Supported by: Counter-Strike Source
///		</comments>
class ICvar_003 abstract : public IAppSystem
{
public:
	// Try to register cvar
	virtual void			RegisterConCommandBase ( ConCommandBase_003 *variable ) = 0;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual char const*		GetCommandLineValue( char const *pVariableName ) = 0;

	// Try to find the cvar pointer by name
	virtual ConVar_003			*FindVar ( const char *var_name ) = 0;
	virtual const ConVar_003	*FindVar ( const char *var_name ) const = 0;

	// Get first ConCommandBase to allow iteration
	virtual ConCommandBase_003	*GetCommands( void ) = 0;

	// Removes all cvars with flag bit set
	virtual void			UnlinkVariables( int flag ) = 0;

	// Install a global change callback (to be called when any convar changes) 
	virtual void			InstallGlobalChangeCallback( FnChangeCallback_003 callback ) = 0;
	virtual void			CallGlobalChangeCallback( ConVar_003 *var, char const *pOldString ) = 0;
};

// ICvar_004 ///////////////////////////////////////////////////////////////////

#define VENGINE_CVAR_INTERFACE_VERSION_004 "VEngineCvar004"

/// <comments>
///		Supported by: Portal First Slice
///		</comments>
class ICvar_004 abstract : public IAppSystem
{
public:
/*	virtual void _Unknown_001(void);
	virtual void _Unknown_002(void);
	virtual void _Unknown_003(void);
*/
	// Allocate a unique DLL identifier
	virtual CVarDLLIdentifier_t_004 AllocateDLLIdentifier() = 0;

	// Register, unregister commands
	virtual void			RegisterConCommand( ConCommandBase_004 *pCommandBase ) = 0;
	virtual void			UnregisterConCommand( ConCommandBase_004 *pCommandBase ) = 0;
	virtual void			UnregisterConCommands( CVarDLLIdentifier_t_004 id ) = 0;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual const char*		GetCommandLineValue( const char *pVariableName ) = 0;

	// Try to find the cvar pointer by name
	virtual ConCommandBase_004 *FindCommandBase( const char *name ) = 0;
	virtual const ConCommandBase_004 *FindCommandBase( const char *name ) const = 0;
	virtual ConVar_004			*FindVar ( const char *var_name ) = 0;
	virtual const ConVar_004	*FindVar ( const char *var_name ) const = 0;
	virtual ConCommand_004		*FindCommand( const char *name ) = 0;
	virtual const ConCommand_004 *FindCommand( const char *name ) const = 0;

	// Get first ConCommandBase to allow iteration
	virtual ConCommandBase_004	*GetCommands( void ) = 0;
	virtual const ConCommandBase_004 *GetCommands( void ) const = 0;

	// Install a global change callback (to be called when any convar changes) 
	virtual void			InstallGlobalChangeCallback( FnChangeCallback_t_004 callback ) = 0;
	virtual void			RemoveGlobalChangeCallback( FnChangeCallback_t_004 callback ) = 0;
	virtual void			CallGlobalChangeCallbacks( ConVar_004 *var, const char *pOldString, float flOldValue ) = 0;

	virtual void _UNUSED_InstallConsoleDisplayFunc(void)=0;
	virtual void _UNUSED_RemoveConsoleDisplayFunc(void)=0;
	virtual void _UNUSED_ConsoleColorPrintf(void)=0;
	virtual void _UNUSED_ConsolePrintf(void)=0;
	virtual void _UNUSED_ConsoleDPrintf(void)=0;
	virtual void _UNUSED_RevertFlaggedConVars(void)=0;
	virtual void _UNUSED_InstallCVarQuery(void)=0;

#if defined( _X360 )
	virtual void _UNUSED_PublishToVXConsole(void)=0;
#endif
};

// ICvar_007 ///////////////////////////////////////////////////////////////////

#define CVAR_INTERFACE_VERSION_007 "VEngineCvar007"

class ConCommandBase_007;
class ConVar_007;
class ConCommand_007;
class IConVar_007;
class CCommand_007;

typedef void ( *FnChangeCallback_t_007 )( IConVar_007 *var, const char *pOldValue, float flOldValue );

class IConCommandBaseAccessor_007
{
public:
	// Flags is a combination of FCVAR flags in cvar.h.
	// hOut is filled in with a handle to the variable.
	virtual bool RegisterConCommandBase( ConCommandBase_007 *pVar ) = 0;
};

typedef void ( *FnCommandCallbackV1_t_007 )( void );
typedef void ( *FnCommandCallback_t_007 )( const CCommand_007 &command );

#define COMMAND_COMPLETION_MAXITEMS_007		64
#define COMMAND_COMPLETION_ITEM_LENGTH_007	64

//-----------------------------------------------------------------------------
// Returns 0 to COMMAND_COMPLETION_MAXITEMS worth of completion strings
//-----------------------------------------------------------------------------
typedef int  ( *FnCommandCompletionCallback_007 )( const char *partial, char commands[ COMMAND_COMPLETION_MAXITEMS_007	 ][ COMMAND_COMPLETION_ITEM_LENGTH_007 ] );

class ICommandCallback_007
{
public:
	virtual void CommandCallback( const CCommand_007 &command ) = 0;
};

class ICommandCompletionCallback_007;


class ConCommandBase_007
{
	friend WrpConCommands; // ugly hack, just like Valve did

public:
								ConCommandBase_007( void );
								ConCommandBase_007( const char *pName, const char *pHelpString = 0, 
									int flags = 0 );

	virtual						~ConCommandBase_007( void );

	virtual	bool				IsCommand( void ) const;

	// Check flag
	virtual bool				IsFlagSet( int flag ) const;
	// Set flag
	virtual void				AddFlags( int flags );
	// Clear flag
	virtual void				RemoveFlags( int flags );

	virtual int					GetFlags() const;

	// Return name of cvar
	virtual const char			*GetName( void ) const;

	// Return help text for cvar
	virtual const char			*GetHelpText( void ) const;

	// Deal with next pointer
	const ConCommandBase_007		*GetNext( void ) const;
	ConCommandBase_007				*GetNext( void );
	
	virtual bool				IsRegistered( void ) const;

	// Returns the DLL identifier
	virtual CVarDLLIdentifier_t_007	GetDLLIdentifier() const;

protected:
	virtual void				Create( const char *pName, const char *pHelpString = 0, 
									int flags = 0 );

	// Used internally by OneTimeInit to initialize/shutdown
	virtual void				Init();
	void _NOT_IMPLEMENTED_Shutdown();

	// Internal copy routine ( uses new operator from correct module )
	void _NOT_IMPLEMENTED_CopyString(void);

private:
	// Next ConVar in chain
	// Prior to register, it points to the next convar in the DLL.
	// Once registered, though, m_pNext is reset to point to the next
	// convar in the global list
	ConCommandBase_007				*m_pNext;

	// Has the cvar been added to the global list?
	bool						m_bRegistered;

	// Static data
	char 					*m_pszName;
	char 					*m_pszHelpString;
	
	// ConVar flags
	int							m_nFlags;

protected:
	// ConVars add themselves to this list for the executable. 
	// Then ConVar_Register runs through  all the console variables 
	// and registers them into a global list stored in vstdlib.dll
	static ConCommandBase_007		*s_pConCommandBases;

	// ConVars in this executable use this 'global' to access values.
	static IConCommandBaseAccessor_007	*s_pAccessor;
};


//-----------------------------------------------------------------------------
// Command tokenizer
//-----------------------------------------------------------------------------
class CCommand_007
{
public:
	CCommand_007();
	CCommand_007( int nArgC, const char **ppArgV );
	bool _NOT_IMPLEMENTED_Tokenize(void);
	void _NOT_IMPLEMENTED_Reset(void);

	int ArgC() const;
	const char **ArgV() const;
	const char *ArgS() const;					// All args that occur after the 0th arg, in string form
	const char *GetCommandString() const;		// The entire command in string form, including the 0th arg
	const char *operator[]( int nIndex ) const;	// Gets at arguments
	const char *Arg( int nIndex ) const;		// Gets at arguments
	
	// Helper functions to parse arguments to commands.
	void _NOT_IMPLEMENTED_FindArg(void) const;
	void _NOT_IMPLEMENTED_FindArgInt(void) const;

	static int MaxCommandLength();
	static void _NOT_IMPLEMENTED_DefaultBreakSet();

private:
	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

	int		m_nArgc;
	int		m_nArgv0Size;
	char	m_pArgSBuffer[ COMMAND_MAX_LENGTH ];
	char	m_pArgvBuffer[ COMMAND_MAX_LENGTH ];
	const char*	m_ppArgv[ COMMAND_MAX_ARGC ];
};

inline int CCommand_007::MaxCommandLength()
{
	return COMMAND_MAX_LENGTH - 1;
}

inline int CCommand_007::ArgC() const
{
	return m_nArgc;
}

inline const char **CCommand_007::ArgV() const
{
	return m_nArgc ? (const char**)m_ppArgv : 0;
}

inline const char *CCommand_007::ArgS() const
{
	return m_nArgv0Size ? &m_pArgSBuffer[m_nArgv0Size] : "";
}

inline const char *CCommand_007::GetCommandString() const
{
	return m_nArgc ? m_pArgSBuffer : "";
}

inline const char *CCommand_007::Arg( int nIndex ) const
{
	// FIXME: Many command handlers appear to not be particularly careful
	// about checking for valid argc range. For now, we're going to
	// do the extra check and return an empty string if it's out of range
	if ( nIndex < 0 || nIndex >= m_nArgc )
		return "";
	return m_ppArgv[nIndex];
}

inline const char *CCommand_007::operator[]( int nIndex ) const
{
	return Arg( nIndex );
}


//-----------------------------------------------------------------------------
// Purpose: The console invoked command
//-----------------------------------------------------------------------------
class ConCommand_007 : public ConCommandBase_007
{
friend class CCvar_007;

public:
	typedef ConCommandBase_007 BaseClass;

	/// <remarks>not implemented</remarks>
	ConCommand_007( const char *pName, FnCommandCallbackV1_t_007 callback, 
		const char *pHelpString = 0, int flags = 0, FnCommandCompletionCallback_007 completionFunc = 0 );
	
	/// <remarks> tweaked since we don't support completition and use a callback wrapper </remarks>
	ConCommand_007( const char *pName, WrpCommandCallback callback, 
		const char *pHelpString = 0, int flags = 0, FnCommandCompletionCallback_007 completionFunc = 0 );
	
	/// <remarks>not implemented</remarks>
	ConCommand_007( const char *pName, ICommandCallback_007 *pCallback, 
		const char *pHelpString = 0, int flags = 0, ICommandCompletionCallback_007 *pCommandCompletionCallback = 0 );

	virtual ~ConCommand_007( void );

	virtual	bool IsCommand( void ) const;

	/// <remarks> we don't support autocompletition, thus we always return 0 </remarks>
	virtual int	AutoCompleteSuggest(void * dummy1, void * dummy2);

	/// <remarks> we don't support autocompletition, thus we always return false </remarks>
	virtual bool CanAutoComplete( void );

	// Invoke the function
	virtual void Dispatch( const CCommand_007 &command );

private:
	// NOTE: To maintain backward compat, we have to be very careful:
	// All public virtual methods must appear in the same order always
	// since engine code will be calling into this code, which *does not match*
	// in the mod code; it's using slightly different, but compatible versions
	// of this class. Also: Be very careful about adding new fields to this class.
	// Those fields will not exist in the version of this class that is instanced
	// in mod code.

	// Call this function when executing the command
	union
	{
		FnCommandCallbackV1_t_007 m_fnCommandCallbackV1;
		WrpCommandCallback m_fnCommandCallback;
		ICommandCallback_007 *m_pCommandCallback; 
	};

	union
	{
		FnCommandCompletionCallback_007	m_fnCompletionCallback;
		ICommandCompletionCallback_007 *m_pCommandCompletionCallback;
	};

	bool m_bHasCompletionCallback : 1;
	bool m_bUsingNewCommandCallback : 1;
	bool m_bUsingCommandCallbackInterface : 1;
};



class ICvar_007 abstract : public IAppSystem_swarm
{
public:
	// Allocate a unique DLL identifier
	virtual CVarDLLIdentifier_t_007 AllocateDLLIdentifier() = 0;

	// Register, unregister commands
	virtual void			RegisterConCommand( ConCommandBase_007 *pCommandBase ) = 0;
	virtual void			UnregisterConCommand( ConCommandBase_007 *pCommandBase ) = 0;
	virtual void			UnregisterConCommands( CVarDLLIdentifier_t_007 id ) = 0;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual const char*		GetCommandLineValue( const char *pVariableName ) = 0;

	// Try to find the cvar pointer by name
	virtual ConCommandBase_007 *FindCommandBase( const char *name ) = 0;
	virtual const ConCommandBase_007 *FindCommandBase( const char *name ) const = 0;
	virtual ConVar_007			*FindVar ( const char *var_name ) = 0;
	virtual const ConVar_007	*FindVar ( const char *var_name ) const = 0;
	virtual ConCommand_007		*FindCommand( const char *name ) = 0;
	virtual const ConCommand_007 *FindCommand( const char *name ) const = 0;



	// Install a global change callback (to be called when any convar changes) 
	virtual void			InstallGlobalChangeCallback( FnChangeCallback_t_007 callback ) = 0;
	virtual void			RemoveGlobalChangeCallback( FnChangeCallback_t_007 callback ) = 0;
	virtual void			CallGlobalChangeCallbacks( ConVar_007 *var, const char *pOldString, float flOldValue ) = 0;

	virtual void			_UNUSED_InstallConsoleDisplayFunc( void ) = 0;
	virtual void			_UNUSED_RemoveConsoleDisplayFunc( void ) = 0;
	virtual void			_UNUSED_ConsoleColorPrintf( void ) const = 0;
	virtual void			_UNUSED_ConsolePrintf( void ) const = 0;
	virtual void			_UNUSED_ConsoleDPrintf( void ) const = 0;
	virtual void			_UNUSED_RevertFlaggedConVars( void ) = 0;
	virtual void			_UNUSED_InstallCVarQuery( void ) = 0;

#if defined( _X360 )
	virtual void			_UNUSED_PublishToVXConsole( ) = 0;
#endif

	virtual void			_UNUSED_SetMaxSplitScreenSlots( int nSlots ) = 0;
	virtual int				_UNUSED_GetMaxSplitScreenSlots() const = 0;

	virtual void			_UNUSED_AddSplitScreenConVars() = 0;
	virtual void			_UNUSED_RemoveSplitScreenConVars( CVarDLLIdentifier_t_007 id ) = 0;

	virtual int				_UNUSED_GetConsoleDisplayFuncCount() const = 0;
	virtual void			_UNUSED_GetConsoleText( int nDisplayFuncIndex, char *pchText, size_t bufSize ) const = 0;

	virtual bool			_UNUSED_IsMaterialThreadSetAllowed( ) const = 0;
	virtual void			_UNUSED_QueueMaterialThreadSetValue( ConVar_007 *pConVar, const char *pValue ) = 0;
	virtual void			_UNUSED_QueueMaterialThreadSetValue( ConVar_007 *pConVar, int nValue ) = 0;
	virtual void			_UNUSED_QueueMaterialThreadSetValue( ConVar_007 *pConVar, float flValue ) = 0;
	virtual bool			_UNUSED_HasQueuedMaterialThreadConVarSets() const = 0;
	virtual int				_UNUSED_ProcessQueuedMaterialThreadConVarSets() = 0;

protected:	class ICVarIteratorInternal;
public:
	/// Iteration over all cvars. 
	/// (THIS IS A SLOW OPERATION AND YOU SHOULD AVOID IT.)
	/// usage: 
	/// { ICVar::Iterator iter(g_pCVar); 
	///   for ( iter.SetFirst() ; iter.IsValid() ; iter.Next() )
	///   {  
	///       ConCommandBase *cmd = iter.Get();
	///   } 
	/// }
	/// The Iterator class actually wraps the internal factory methods
	/// so you don't need to worry about new/delete -- scope takes care
	//  of it.
	/// We need an iterator like this because we can't simply return a 
	/// pointer to the internal data type that contains the cvars -- 
	/// it's a custom, protected class with unusual semantics and is
	/// prone to change.
	class Iterator
	{
	public:
		inline Iterator(ICvar_007 *icvar);
		inline ~Iterator(void);
		inline void		SetFirst( void );
		inline void		Next( void );
		inline bool		IsValid( void );
		inline ConCommandBase_007 *Get( void );
	private:
		ICVarIteratorInternal *m_pIter;
	};

protected:
	// internals for  ICVarIterator
	class ICVarIteratorInternal
	{
	public:
		virtual void		SetFirst( void ) = 0;
		virtual void		Next( void ) = 0;
		virtual	bool		IsValid( void ) = 0;
		virtual ConCommandBase_007 *Get( void ) = 0;
	};

	virtual ICVarIteratorInternal	*FactoryInternalIterator( void ) = 0;
	friend class Iterator;
};


// IVEngineClient_012 //////////////////////////////////////////////////////////

#define VENGINE_CLIENT_INTERFACE_VERSION_012		"VEngineClient012"

/// <comments>
///		Supported by: Counter-Strike Source
///		</comments>
class IVEngineClient_012 abstract
{
public:
	virtual void _UNUSED_GetIntersectingSurfaces(void)=0;
	virtual void _UNUSED_GetLightForPoint(void)=0;
	virtual void _UNUSED_TraceLineMaterialAndLighting(void)=0;
	virtual void _UNUSED_ParseFile(void)=0;
	virtual void _UNUSED_CopyFile(void)=0;

	// Gets the dimensions of the game window
	virtual void				GetScreenSize( int& width, int& height ) = 0;

	// Forwards szCmdString to the server, sent reliably if bReliable is set
	virtual void				ServerCmd( const char *szCmdString, bool bReliable = true ) = 0;
	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	virtual void				ClientCmd( const char *szCmdString ) = 0;

	virtual void _UNUSED_GetPlayerInfo(void)=0;
	virtual void _UNUSED_GetPlayerForUserID(void)=0;
	virtual void _UNUSED_TextMessageGet(void)=0;

	// Returns true if the console is visible
	virtual bool				Con_IsVisible( void ) = 0;

	virtual void _UNUSED_GetLocalPlayer(void)=0;
	virtual void _UNUSED_LoadModel(void)=0;

	// Get accurate, sub-frame clock ( profiling use )
	virtual float				Time( void ) = 0; 

	// Get the exact server timesstamp ( server time ) from the last message received from the server
	virtual float				GetLastTimeStamp( void ) = 0; 

	virtual void _UNUSED_GetSentence(void)=0;
	virtual void _UNUSED_GetSentenceLength(void)=0;
	virtual void _UNUSED_IsStreaming(void)=0;

	// Copy current view orientation into va
	virtual void				GetViewAngles( QAngle& va ) = 0;
	// Set current view orientation from va
	virtual void				SetViewAngles( QAngle& va ) = 0;
	
	// Retrieve the current game's maxclients setting
	virtual int					GetMaxClients( void ) = 0;

	virtual void _UNUSED_Key_Event(void)=0;
	virtual void _UNUSED_Key_LookupBinding(void)=0;
	virtual void _UNUSED_StartKeyTrapMode(void)=0;
	virtual void _UNUSED_CheckDoneKeyTrapping(void)=0;

	// Returns true if the player is fully connected and active in game (i.e, not still loading)
	virtual bool				IsInGame( void ) = 0;
	// Returns true if the player is connected, but not necessarily active in game (could still be loading)
	virtual bool				IsConnected( void ) = 0;
	// Returns true if the loading plaque should be drawn
	virtual bool				IsDrawingLoadingImage( void ) = 0;

	// Prints the formatted string to the notification area of the screen ( down the right hand edge
	//  numbered lines starting at position 0
	virtual void				Con_NPrintf( int pos, const char *fmt, ... ) = 0;
	
	virtual void _UNUSED_Con_NXPrintf(void)=0;

	// During ConCommand processing functions, use this function to get the total # of tokens passed to the command parser
	virtual int					Cmd_Argc( void ) = 0;	
	// During ConCommand processing, this API is used to access each argument passed to the parser
	virtual const char			*Cmd_Argv( int arg ) = 0;

	virtual void _UNUSED_IsBoxVisible(void)=0;
	virtual void _UNUSED_IsBoxInViewCluster(void)=0;
	virtual void _UNUSED_CullBox(void)=0;
	virtual void _UNUSED_Sound_ExtraUpdate(void)=0;

	// Get the current game directory ( e.g., hl2, tf2, cstrike, hl1 )
	virtual const char			*GetGameDirectory( void ) = 0;

	virtual void _UNUSED_WorldToScreenMatrix(void)=0;
	virtual void _UNUSED_WorldToViewMatrix(void)=0;
	virtual void _UNUSED_GameLumpVersion(void)=0;
	virtual void _UNUSED_GameLumpSize(void)=0;
	virtual void _UNUSED_LoadGameLump(void)=0;
	virtual void _UNUSED_LevelLeafCount(void)=0;
	virtual void _UNUSED_GetBSPTreeQuery(void)=0;
	virtual void _UNUSED_LinearToGamma(void)=0;
	virtual void _UNUSED_LightStyleValue(void)=0;
	virtual void _UNUSED_ComputeDynamicLighting(void)=0;
	virtual void _UNUSED_GetAmbientLightColor(void)=0;
	virtual void _UNUSED_GetDXSupportLevel(void)=0;
	virtual void _UNUSED_SupportsHDR(void)=0;
	virtual void _UNUSED_Mat_Stub(void)=0;

	// Get the name of the current map
	virtual char const	*GetLevelName( void ) = 0;
#ifndef _XBOX
	virtual void _UNUSED_GetVoiceTweakAPI(void)=0;
#endif
	// Tell engine stats gathering system that the rendering frame is beginning/ending
	virtual void		EngineStats_BeginFrame( void ) = 0;
	virtual void		EngineStats_EndFrame( void ) = 0;
	
	virtual void _UNUSED_FireEvents(void)=0;
	virtual void _UNUSED_GetLeavesArea(void)=0;
	virtual void _UNUSED_DoesBoxTouchAreaFrustum(void)=0;
	virtual void _UNUSED_SetHearingOrigin(void)=0;
	virtual void _UNUSED_SentenceGroupPick(void)=0;
	virtual void _UNUSED_SentenceGroupPickSequential(void)=0;
	virtual void _UNUSED_SentenceIndexFromName(void)=0;
	virtual void _UNUSED_SentenceNameFromIndex(void)=0;
	virtual void _UNUSED_SentenceGroupIndexFromName(void)=0;
	virtual void _UNUSED_SentenceGroupNameFromIndex(void)=0;
	virtual void _UNUSED_SentenceLength(void)=0;
	virtual void _UNUSED_ComputeLighting(void)=0;
	virtual void _UNUSED_ActivateOccluder(void)=0;
	virtual void _UNUSED_IsOccluded(void)=0;
	virtual void _UNUSED_SaveAllocMemory(void)=0;
	virtual void _UNUSED_SaveFreeMemory(void)=0;
	virtual void _UNUSED_GetNetChannelInfo(void)=0;
	virtual void _UNUSED_DebugDrawPhysCollide(void)=0;
	virtual void _UNUSED_CheckPoint(void)=0;
	virtual void _UNUSED_DrawPortals(void)=0;

	// Determine whether the client is playing back or recording a demo
	virtual bool		IsPlayingDemo( void ) = 0;
	virtual bool		IsRecordingDemo( void ) = 0;
	virtual bool		IsPlayingTimeDemo( void ) = 0;
	// Is the game paused?
	virtual bool		IsPaused( void ) = 0;
	// Is the game currently taking a screenshot?
	virtual bool		IsTakingScreenshot( void ) = 0;
	// Is this a HLTV broadcast ?
	virtual bool		IsHLTV( void ) = 0;
	// is this level loaded as just the background to the main menu? (active, but unplayable)
	virtual bool		IsLevelMainMenuBackground( void ) = 0;
	// returns the name of the background level
	virtual void		GetMainMenuBackgroundName( char *dest, int destlen ) = 0;

	virtual void _UNUSED_SetOcclusionParameters(void)=0;
	virtual void _UNUSED_GetUILanguage(void)=0;
	virtual void _UNUSED_IsSkyboxVisibleFromPoint(void)=0;
	virtual void _UNUSED_GetMapEntitiesString(void)=0;

	// Is the engine in map edit mode ?
	virtual bool		IsInEditMode( void ) = 0;

	// current screen aspect ratio (eg. 4.0f/3.0f, 16.0f/9.0f)
	virtual float		GetScreenAspectRatio() = 0;

	virtual void _UNUSED_SteamRefreshLogin(void)=0;
	virtual void _UNUSED_SteamProcessCall(void)=0;

	// allow other modules to know about engine versioning (one use is a proxy for network compatability)
	virtual unsigned int	GetEngineBuildNumber() = 0; // engines build
	virtual const char *	GetProductVersionString() = 0; // mods version number (steam.inf)

	virtual void _UNUSED_GetLastPressedEngineKey(void)=0;
	virtual void _UNUSED_GrabPreColorCorrectedFrame(void)=0;

	virtual bool			IsHammerRunning( ) const = 0;

	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// And then executes the command string immediately (vs ClientCmd() which executes in the next frame)
	virtual void			ExecuteClientCmd( const char *szCmdString ) = 0;

	virtual void _UNUSED_MapHasHDRLighting(void)=0;

	virtual int	GetAppID() = 0;

	virtual void _UNUSED_GetLightForPointFast(void)=0;
};


// IVEngineClient_013 //////////////////////////////////////////////////////////

#define VENGINE_CLIENT_INTERFACE_VERSION_013 "VEngineClient013"

/// <comments>
///		Supported by: Portal First Slice
///		</comments>
class IVEngineClient_013 abstract
{
public:
	virtual void _UNUSED_GetIntersectingSurfaces(void)=0;
	virtual void _UNUSED_GetLightForPoint(void)=0;
	virtual void _UNUSED_TraceLineMaterialAndLighting(void)=0;
	virtual void _UNUSED_ParseFile(void)=0;
	virtual void _UNUSED_CopyFile(void)=0;

	// Gets the dimensions of the game window
	virtual void				GetScreenSize( int& width, int& height ) = 0;

	// Forwards szCmdString to the server, sent reliably if bReliable is set
	virtual void				ServerCmd( const char *szCmdString, bool bReliable = true ) = 0;
	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// Note: Calls to this are checked against FCVAR_CLIENTCMD_CAN_EXECUTE (if that bit is not set, then this function can't change it).
	//       Call ClientCmd_Unrestricted to have access to FCVAR_CLIENTCMD_CAN_EXECUTE vars.
	virtual void				ClientCmd( const char *szCmdString ) = 0;

	virtual void _UNUSED_GetPlayerInfo(void)=0;
	virtual void _UNUSED_GetPlayerForUserID(void)=0;
	virtual void _UNUSED_TextMessageGet(void)=0;

	// Returns true if the console is visible
	virtual bool				Con_IsVisible( void ) = 0;

	virtual void _UNUSED_GetLocalPlayer(void)=0;
	virtual void _UNUSED_LoadModel(void)=0;

	// Get accurate, sub-frame clock ( profiling use )
	virtual float				Time( void ) = 0; 

	// Get the exact server timesstamp ( server time ) from the last message received from the server
	virtual float				GetLastTimeStamp( void ) = 0; 

	virtual void _UNUSED_GetSentence(void)=0;
	virtual void _UNUSED_GetSentenceLength(void)=0;
	virtual void _UNUSED_IsStreaming(void)=0;

	// Copy current view orientation into va
	virtual void				GetViewAngles( QAngle& va ) = 0;
	// Set current view orientation from va
	virtual void				SetViewAngles( QAngle& va ) = 0;
	
	// Retrieve the current game's maxclients setting
	virtual int					GetMaxClients( void ) = 0;

	virtual void _UNUSED_Key_LookupBinding(void)=0;
	virtual void _UNUSED_Key_BindingForKey(void)=0;
	virtual void _UNUSED_StartKeyTrapMode(void)=0;
	virtual void _UNUSED_CheckDoneKeyTrapping(void)=0;

	// Returns true if the player is fully connected and active in game (i.e, not still loading)
	virtual bool				IsInGame( void ) = 0;
	// Returns true if the player is connected, but not necessarily active in game (could still be loading)
	virtual bool				IsConnected( void ) = 0;
	// Returns true if the loading plaque should be drawn
	virtual bool				IsDrawingLoadingImage( void ) = 0;

	// Prints the formatted string to the notification area of the screen ( down the right hand edge
	//  numbered lines starting at position 0
	virtual void				Con_NPrintf( int pos, const char *fmt, ... ) = 0;
	
	virtual void _UNUSED_Con_NXPrintf(void)=0;
	virtual void _UNUSED_IsBoxVisible(void)=0;
	virtual void _UNUSED_IsBoxInViewCluster(void)=0;
	virtual void _UNUSED_CullBox(void)=0;
	virtual void _UNUSED_Sound_ExtraUpdate(void)=0;

	// Get the current game directory ( e.g., hl2, tf2, cstrike, hl1 )
	virtual const char			*GetGameDirectory( void ) = 0;

	virtual void _UNUSED_WorldToScreenMatrix(void)=0;
	virtual void _UNUSED_WorldToViewMatrix(void)=0;
	virtual void _UNUSED_GameLumpVersion(void)=0;
	virtual void _UNUSED_GameLumpSize(void)=0;
	virtual void _UNUSED_LoadGameLump(void)=0;
	virtual void _UNUSED_LevelLeafCount(void)=0;
	virtual void _UNUSED_GetBSPTreeQuery(void)=0;
	virtual void _UNUSED_LinearToGamma(void)=0;
	virtual void _UNUSED_LightStyleValue(void)=0;
	virtual void _UNUSED_ComputeDynamicLighting(void)=0;
	virtual void _UNUSED_GetAmbientLightColor(void)=0;
	virtual void _UNUSED_GetDXSupportLevel(void)=0;
	virtual void _UNUSED_SupportsHDR(void)=0;
	virtual void _UNUSED_Mat_Stub(void)=0;
	virtual void _UNUSED_GetChapterName(void)=0;

	virtual char const	*GetLevelName( void ) = 0;

#if !defined( NO_VOICE )
	virtual void _UNUSED_GetVoiceTweakAPI(void)=0;
#endif
	// Tell engine stats gathering system that the rendering frame is beginning/ending
	virtual void		EngineStats_BeginFrame( void ) = 0;
	virtual void		EngineStats_EndFrame( void ) = 0;
	
	virtual void _UNUSED_FireEvents(void)=0;
	virtual void _UNUSED_GetLeavesArea(void)=0;
	virtual void _UNUSED_DoesBoxTouchAreaFrustum(void)=0;
	virtual void _UNUSED_SetAudioState(void)=0;
	virtual void _UNUSED_SentenceGroupPick(void)=0;
	virtual void _UNUSED_SentenceGroupPickSequential(void)=0;
	virtual void _UNUSED_SentenceIndexFromName(void)=0;
	virtual void _UNUSED_SentenceNameFromIndex(void)=0;
	virtual void _UNUSED_SentenceGroupIndexFromName(void)=0;
	virtual void _UNUSED_SentenceGroupNameFromIndex(void)=0;
	virtual void _UNUSED_SentenceLength(void)=0;
	virtual void _UNUSED_ComputeLighting(void)=0;
	virtual void _UNUSED_ActivateOccluder(void)=0;
	virtual void _UNUSED_IsOccluded(void)=0;
	virtual void _UNUSED_SaveAllocMemory(void)=0;
	virtual void _UNUSED_SaveFreeMemory(void)=0;
	virtual void _UNUSED_GetNetChannelInfo(void)=0;
	virtual void _UNUSED_DebugDrawPhysCollide(void)=0;
	virtual void _UNUSED_CheckPoint(void)=0;
	virtual void _UNUSED_DrawPortals(void)=0;

	// Determine whether the client is playing back or recording a demo
	virtual bool		IsPlayingDemo( void ) = 0;
	virtual bool		IsRecordingDemo( void ) = 0;
	virtual bool		IsPlayingTimeDemo( void ) = 0;
	// Is the game paused?
	virtual bool		IsPaused( void ) = 0;
	// Is the game currently taking a screenshot?
	virtual bool		IsTakingScreenshot( void ) = 0;
	// Is this a HLTV broadcast ?
	virtual bool		IsHLTV( void ) = 0;
	// is this level loaded as just the background to the main menu? (active, but unplayable)
	virtual bool		IsLevelMainMenuBackground( void ) = 0;
	// returns the name of the background level
	virtual void		GetMainMenuBackgroundName( char *dest, int destlen ) = 0;

	virtual void _UNUSED_SetOcclusionParameters(void)=0;
	virtual void _UNUSED_GetUILanguage(void)=0;
	virtual void _UNUSED_IsSkyboxVisibleFromPoint(void)=0;
	virtual void _UNUSED_GetMapEntitiesString(void)=0;

	// Is the engine in map edit mode ?
	virtual bool		IsInEditMode( void ) = 0;

	// current screen aspect ratio (eg. 4.0f/3.0f, 16.0f/9.0f)
	virtual float		GetScreenAspectRatio() = 0;

	virtual void _UNUSED_REMOVED_SteamRefreshLogin(void)=0;
	virtual void _UNUSED_REMOVED_SteamProcessCall(void)=0;

	// allow other modules to know about engine versioning (one use is a proxy for network compatability)
	virtual unsigned int	GetEngineBuildNumber() = 0; // engines build
	virtual const char *	GetProductVersionString() = 0; // mods version number (steam.inf)

	virtual void _UNUSED_GrabPreColorCorrectedFrame(void)=0;

	virtual bool			IsHammerRunning( ) const = 0;

	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// And then executes the command string immediately (vs ClientCmd() which executes in the next frame)
	//
	// Note: this is NOT checked against the FCVAR_CLIENTCMD_CAN_EXECUTE vars.
	virtual void			ExecuteClientCmd( const char *szCmdString ) = 0;

	virtual void _UNUSED_MapHasHDRLighting(void)=0;

	virtual int	GetAppID() = 0;

	virtual void _UNUSED_GetLightForPointFast(void)=0;
	virtual void _UNUSED_ClientCmd_Unrestricted(void)=0;
	virtual void _UNUSED_SetRestrictServerCommands(void)=0;
	virtual void _UNUSED_SetRestrictClientCommands(void)=0;
	virtual void _UNUSED_SetOverlayBindProxy(void)=0;
	virtual void _UNUSED_CopyFrameBufferToMaterial(void)=0;
	virtual void _UNUSED_ChangeTeam(void)=0;
	virtual void _UNUSED_ReadConfiguration(void)=0;
	virtual void _UNUSED_SetAchievementMgr(void)=0;
	virtual void _UNUSED_GetAchievementMgr(void)=0;
	virtual void _UNUSED_MapLoadFailed(void)=0;
	virtual void _UNUSED_SetMapLoadFailed(void)=0;
	virtual void _UNUSED_IsLowViolence(void)=0;
	virtual void _UNUSED_GetMostRecentSaveGame(void)=0;
	virtual void _UNUSED_SetMostRecentSaveGame(void)=0;
	virtual void _UNUSED_StartXboxExitingProcess(void)=0;
	virtual void _UNUSED_IsSaveInProgress(void)=0;
	virtual void _UNUSED_OnStorageDeviceAttached(void)=0;
	virtual void _UNUSED_OnStorageDeviceDetached(void)=0;
	virtual void _UNUSED_ResetDemoInterpolation(void)=0;
	virtual void _UNUSED_SetGamestatsData(void)=0;
	virtual void _UNUSED_GetGamestatsData(void)=0;
};


// IBaseClientDLL_011 //////////////////////////////////////////////////////////

#define CLIENT_DLL_INTERFACE_VERSION_011		"VClient011"

class IBaseClientDLL_011 abstract
{
public:
	// Called once when the client DLL is loaded
	virtual int				Init( CreateInterfaceFn appSystemFactory, 
									CreateInterfaceFn physicsFactory,
									CGlobalVarsBase *pGlobals ) = 0; // 0

	// Called once when the client DLL is being unloaded
	virtual void			Shutdown( void ) = 0; // 1
	
	virtual void _UNUSED_LevelInitPreEntity(void)=0; // 2
	virtual void _UNUSED_LevelInitPostEntity(void)=0; // 3
	virtual void _UNUSED_LevelShutdown(void)=0; // 4
	virtual void _UNUSED_GetAllClasses(void)=0; // 5
	virtual void _UNUSED_HudVidInit(void)=0; // 6
	virtual void _UNUSED_HudProcessInput(void)=0; // 7
	virtual void _UNUSED_HudUpdate(void)=0; // 8
	virtual void _UNUSED_HudReset(void)=0; // 9
	virtual void _UNUSED_HudText(void)=0; // 10
	virtual void _UNUSED_IN_ActivateMouse(void)=0; // 11
	virtual void _UNUSED_IN_DeactivateMouse(void)=0; // 12
	virtual void _UNUSED_IN_MouseEvent(void)=0; // 13
	virtual void _UNUSED_IN_Accumulate(void)=0; // 14
	virtual void _UNUSED_IN_ClearStates(void)=0; // 15
	virtual void _UNUSED_IN_IsKeyDown(void)=0; // 16
	virtual void _UNUSED_IN_KeyEvent(void)=0; // 17
	virtual void _UNUSED_CreateMove(void)=0; // 18
	virtual void _UNUSED_ExtraMouseSample(void)=0; // 19
	virtual void _UNUSED_WriteUsercmdDeltaToBuffer(void)=0; // 20
	virtual void _UNUSED_EncodeUserCmdToBuffer(void)=0; // 21
	virtual void _UNUSED_DecodeUserCmdFromBuffer(void)=0; // 22
	virtual void _UNUSED_View_Render(void)=0; // 23
	virtual void _UNUSED_RenderView(void)=0; // 24
	virtual void _UNUSED_View_Fade(void)=0;
	virtual void _UNUSED_SetCrosshairAngle(void)=0;
	virtual void _UNUSED_InitSprite(void)=0;
	virtual void _UNUSED_ShutdownSprite(void)=0;
	virtual void _UNUSED_GetSpriteSize(void)=0;
	virtual void _UNUSED_VoiceStatus(void)=0;
	virtual void _UNUSED_InstallStringTableCallback(void)=0;
	virtual void _UNUSED_FrameStageNotify(void)=0;
	virtual void _UNUSED_DispatchUserMessage(void)=0;
	virtual void _UNUSED_SaveInit(void)=0;
	virtual void _UNUSED_SaveWriteFields(void)=0;
	virtual void _UNUSED_SaveReadFields(void)=0;
	virtual void _UNUSED_PreSave(void)=0;
	virtual void _UNUSED_Save(void)=0;
	virtual void _UNUSED_WriteSaveHeaders(void)=0;
	virtual void _UNUSED_ReadRestoreHeaders(void)=0;
	virtual void _UNUSED_Restore(void)=0;
	virtual void _UNUSED_DispatchOnRestore(void)=0;
	virtual void _UNUSED_GetStandardRecvProxies(void)=0;
	virtual void _UNUSED_WriteSaveGameScreenshot(void)=0;
	virtual void _UNUSED_EmitSentenceCloseCaption(void)=0;
	virtual void _UNUSED_EmitCloseCaption(void)=0;
};


// IBaseClientDLL_012 //////////////////////////////////////////////////////////

#define CLIENT_DLL_INTERFACE_VERSION_012		"VClient012"

/// <comments>
///		Supported by:
///		</comments>
class IBaseClientDLL_012 abstract
{
public:
	// Called once when the client DLL is loaded
	virtual int				Init( CreateInterfaceFn appSystemFactory, 
									CreateInterfaceFn physicsFactory,
									CGlobalVarsBase *pGlobals ) = 0;

	// Called once when the client DLL is being unloaded
	virtual void			Shutdown( void ) = 0;
	
	virtual void _UNUSED_LevelInitPreEntity(void)=0;
	virtual void _UNUSED_LevelInitPostEntity(void)=0;
	virtual void _UNUSED_LevelShutdown(void)=0;
	virtual void _UNUSED_GetAllClasses(void)=0;
	virtual void _UNUSED_HudVidInit(void)=0;
	virtual void _UNUSED_HudProcessInput(void)=0;
	virtual void _UNUSED_HudUpdate(void)=0;
	virtual void _UNUSED_HudReset(void)=0;
	virtual void _UNUSED_HudText(void)=0;
	virtual void _UNUSED_IN_ActivateMouse(void)=0;
	virtual void _UNUSED_IN_DeactivateMouse(void)=0;
	virtual void _UNUSED_IN_MouseEvent(void)=0;
	virtual void _UNUSED_IN_Accumulate(void)=0;
	virtual void _UNUSED_IN_ClearStates(void)=0;
	virtual void _UNUSED_IN_IsKeyDown(void)=0;
	virtual void _UNUSED_IN_KeyEvent(void)=0;
	virtual void _UNUSED_CreateMove (void)=0;
	virtual void _UNUSED_ExtraMouseSample(void)=0;
	virtual void _UNUSED_WriteUsercmdDeltaToBuffer(void)=0;
	virtual void _UNUSED_EncodeUserCmdToBuffer(void)=0;
	virtual void _UNUSED_DecodeUserCmdFromBuffer(void)=0;
	virtual void _UNUSED_View_Render(void)=0;
	virtual void _UNUSED_RenderView(void)=0;
	virtual void _UNUSED_View_Fade(void)=0;
	virtual void _UNUSED_InitSprite(void)=0;
	virtual void _UNUSED_ShutdownSprite(void)=0;
	virtual void _UNUSED_GetSpriteSize(void)=0;
	virtual void _UNUSED_VoiceStatus(void)=0;
	virtual void _UNUSED_InstallStringTableCallback(void)=0;
	virtual void _UNUSED_FrameStageNotify(void)=0;
	virtual void _UNUSED_DispatchUserMessage(void)=0;
	virtual void _UNUSED_SaveInit(void)=0;
	virtual void _UNUSED_SaveWriteFields(void)=0;
	virtual void _UNUSED_SaveReadFields(void)=0;
	virtual void _UNUSED_PreSave(void)=0;
	virtual void _UNUSED_Save(void)=0;
	virtual void _UNUSED_WriteSaveHeaders(void)=0;
	virtual void _UNUSED_ReadRestoreHeaders(void)=0;
	virtual void _UNUSED_Restore(void)=0;
	virtual void _UNUSED_DispatchOnRestore(void)=0;
	virtual void _UNUSED_GetStandardRecvProxies(void)=0;
	virtual void _UNUSED_WriteSaveGameScreenshot(void)=0;
	virtual void _UNUSED_EmitSentenceCloseCaption(void)=0;
	virtual void _UNUSED_EmitCloseCaption(void)=0;
	virtual void _UNUSED_CanRecordDemo(void)=0;
};


// IBaseClientDLL_013 //////////////////////////////////////////////////////////

#define CLIENT_DLL_INTERFACE_VERSION_013 "VClient013"

/// <comments>
///		Supported by: Counter-Strike Source
///		</comments>
class IBaseClientDLL_013 abstract
{
public:
	// Called once when the client DLL is loaded
	virtual int				Init( CreateInterfaceFn appSystemFactory, 
									CreateInterfaceFn physicsFactory,
									CGlobalVarsBase *pGlobals ) = 0; // 0

	// Called once when the client DLL is being unloaded
	virtual void			Shutdown( void ) = 0; // 1
	
	virtual void _UNUSED_LevelInitPreEntity(void)=0; // 2
	virtual void _UNUSED_LevelInitPostEntity(void)=0; // 3
	virtual void _UNUSED_LevelShutdown(void)=0; // 4
	virtual void _UNUSED_GetAllClasses(void)=0; // 5
	virtual void _UNUSED_HudVidInit(void)=0; // 6
	virtual void _UNUSED_HudProcessInput(void)=0; // 7
	virtual void _UNUSED_HudUpdate(void)=0; // 8
	virtual void _UNUSED_HudReset(void)=0; // 9
	virtual void _UNUSED_HudText(void)=0; // 10
	virtual void _UNUSED_IN_ActivateMouse(void)=0; // 11
	virtual void _UNUSED_IN_DeactivateMouse(void)=0; // 12
	virtual void _UNUSED_IN_MouseEvent (void)=0; // 13
	virtual void _UNUSED_IN_Accumulate (void)=0; // 14
	virtual void _UNUSED_IN_ClearStates (void)=0; // 15
	virtual void _UNUSED_IN_IsKeyDown(void)=0; // 16
	virtual void _UNUSED_IN_KeyEvent(void)=0; // 17
	virtual void _UNUSED_CreateMove(void)=0; // 18
	virtual void _UNUSED_ExtraMouseSample(void)=0; // 19
	virtual void _UNUSED_WriteUsercmdDeltaToBuffer(void)=0; // 20
	virtual void _UNUSED_EncodeUserCmdToBuffer(void)=0; // 21
	virtual void _UNUSED_DecodeUserCmdFromBuffer(void)=0; // 22
	virtual void _UNUSED_View_Render(void)=0; // 23
	virtual void _UNUSED_RenderView(void)=0; // 24
	virtual void _UNUSED_View_Fade(void)=0; // 25
	virtual void _UNUSED_SetCrosshairAngle(void)=0; // 26
	virtual void _UNUSED_InitSprite(void)=0; // 27
	virtual void _UNUSED_ShutdownSprite(void)=0; // 28
	virtual void _UNUSED_GetSpriteSize(void)=0; // 29
	virtual void _UNUSED_VoiceStatus(void)=0; // 30
	virtual void _UNUSED_InstallStringTableCallback(void)=0; // 31
	virtual void _UNUSED_FrameStageNotify(void)=0; // 32
	virtual void _UNUSED_DispatchUserMessage(void)=0; // 33
	virtual void _UNUSED_SaveInit(void)=0; // 34
	virtual void _UNUSED_SaveWriteFields(void)=0; // 35
	virtual void _UNUSED_SaveReadFields(void)=0; // 36
	virtual void _UNUSED_PreSave(void)=0; // 37
	virtual void _UNUSED_Save(void)=0; // 38
	virtual void _UNUSED_WriteSaveHeaders(void)=0; // 39
	virtual void _UNUSED_ReadRestoreHeaders(void)=0; // 40
	virtual void _UNUSED_Restore(void)=0; // 41
	virtual void _UNUSED_DispatchOnRestore(void)=0; // 42
	virtual void _UNUSED_GetStandardRecvProxies(void)=0; // 43
	virtual void _UNUSED_WriteSaveGameScreenshot(void)=0; // 44
	virtual void _UNUSED_EmitSentenceCloseCaption(void)=0; // 45
	virtual void _UNUSED_EmitCloseCaption(void)=0; // 46
	virtual void _UNUSED_CanRecordDemo(void)=0; // 47
	virtual void _UNUSED_WriteSaveGameScreenshotOfSize(void)=0; // 48
	virtual void _UNUSED_RenderViewEx(void)=0; // 49
	virtual void _UNUSED_GetPlayerView(void)=0; // 50
};


// IBaseClientDll_015 //////////////////////////////////////////////////////////

#define CLIENT_DLL_INTERFACE_VERSION_015		"VClient015"

class CGlobalVarsBase;

/// <comments>
///		Supported by: Portal First Slice
///		</comments>
class IBaseClientDLL_015 abstract
{
public:
	// Called once when the client DLL is loaded
	virtual int				Init( CreateInterfaceFn appSystemFactory, 
									CreateInterfaceFn physicsFactory,
									CGlobalVarsBase *pGlobals ) = 0;

	virtual void			PostInit() = 0;

	// Called once when the client DLL is being unloaded
	virtual void			Shutdown( void ) = 0;
	
	virtual void _UNUSED_LevelInitPreEntity(void)=0;
	virtual void _UNUSED_LevelInitPostEntity(void)=0;
	virtual void _UNUSED_LevelShutdown(void)=0;
	virtual void _UNUSED_GetAllClasses(void)=0;
	virtual void _UNUSED_HudVidInit(void)=0;
	virtual void _UNUSED_HudProcessInput(void)=0;
	virtual void _UNUSED_HudUpdate(void)=0;
	virtual void _UNUSED_HudReset(void)=0;
	virtual void _UNUSED_HudText(void)=0;
	virtual void _UNUSED_IN_ActivateMouse(void)=0;
	virtual void _UNUSED_IN_DeactivateMouse(void)=0;
	virtual void _UNUSED_IN_Accumulate(void)=0;
	virtual void _UNUSED_IN_ClearStates(void)=0;
	virtual void _UNUSED_IN_IsKeyDown(void)=0;
	virtual void _UNUSED_IN_KeyEvent(void)=0;
	virtual void _UNUSED_CreateMove(void)=0;
	virtual void _UNUSED_ExtraMouseSample(void)=0;
	virtual void _UNUSED_WriteUsercmdDeltaToBuffer(void)=0;
	virtual void _UNUSED_EncodeUserCmdToBuffer(void)=0;
	virtual void _UNUSED_DecodeUserCmdFromBuffer(void)=0;
	virtual void _UNUSED_View_Render(void)=0;
	virtual void _UNUSED_RenderView(void)=0;
	virtual void _UNUSED_View_Fade(void)=0;
	virtual void _UNUSED_SetCrosshairAngle(void)=0;
	virtual void _UNUSED_InitSprite(void)=0;
	virtual void _UNUSED_ShutdownSprite(void)=0;
	virtual void _UNUSED_GetSpriteSize(void)=0;
	virtual void _UNUSED_VoiceStatus(void)=0;
	virtual void _UNUSED_InstallStringTableCallback(void)=0;
	virtual void _UNUSED_FrameStageNotify(void)=0;
	virtual void _UNUSED_DispatchUserMessage(void)=0;
	virtual void _UNUSED_SaveInit(void)=0;
	virtual void _UNUSED_SaveWriteFields(void)=0;
	virtual void _UNUSED_SaveReadFields(void)=0;
	virtual void _UNUSED_PreSave(void)=0;
	virtual void _UNUSED_Save(void)=0;
	virtual void _UNUSED_WriteSaveHeaders(void)=0;
	virtual void _UNUSED_ReadRestoreHeaders(void)=0;
	virtual void _UNUSED_Restore(void)=0;
	virtual void _UNUSED_DispatchOnRestore(void)=0;
	virtual void _UNUSED_GetStandardRecvProxies(void)=0;
	virtual void _UNUSED_WriteSaveGameScreenshot(void)=0;
	virtual void _UNUSED_EmitSentenceCloseCaption(void)=0;
	virtual void _UNUSED_EmitCloseCaption(void)=0;
	virtual void _UNUSED_CanRecordDemo(void)=0;
	virtual void _UNUSED_WriteSaveGameScreenshotOfSize(void)=0;
	virtual void _UNUSED_GetPlayerView(void)=0;
	virtual void _UNUSED_SetupGameProperties(void)=0;
	virtual void _UNUSED_GetPresenceID(void)=0;
	virtual void _UNUSED_GetPropertyIdString(void)=0;
	virtual void _UNUSED_GetPropertyDisplayString(void)=0;
#ifdef _WIN32
	virtual void _UNUSED_StartStatsReporting(void)=0;
#endif
	virtual void _UNUSED_InvalidateMdlCache(void)=0;
	virtual void _UNUSED_IN_SetSampleTime(void)=0;
};

// IBaseClientDll_016 //////////////////////////////////////////////////////////

#define CLIENT_DLL_INTERFACE_VERSION_016		"VClient016" 

// IBaseClientDll_017 //////////////////////////////////////////////////////////

#define CLIENT_DLL_INTERFACE_VERSION_017		"VClient017"


// IBaseInterface //////////////////////////////////////////////////////////////

class IBaseInterface
{
public:
	virtual	~IBaseInterface() {}
};

// IClientEngineTools //////////////////////////////////////////////////////////

typedef void * HTOOLHANDLE;
typedef void KeyValues;
typedef struct {} AudioState_t;

class IClientEngineTools_001 : public IBaseInterface
{
public:
	// Level init, shutdown
	virtual void LevelInitPreEntityAllTools() = 0;
	// entities are created / spawned / precached here
	virtual void LevelInitPostEntityAllTools() = 0;

	virtual void LevelShutdownPreEntityAllTools() = 0;
	// Entities are deleted / released here...
	virtual void LevelShutdownPostEntityAllTools() = 0;

	virtual void PreRenderAllTools() = 0;
	virtual void PostRenderAllTools() = 0;

	virtual void PostToolMessage( HTOOLHANDLE hEntity, KeyValues *msg ) = 0;

	virtual void AdjustEngineViewport( int& x, int& y, int& width, int& height ) = 0;
	virtual bool SetupEngineView( Vector &origin, QAngle &angles, float &fov ) = 0;
	virtual bool SetupAudioState( AudioState_t &audioState ) = 0;

	// Paintmode is an enum declared in ienginevgui.h
	virtual void VGui_PreRenderAllTools( int paintMode ) = 0;
	virtual void VGui_PostRenderAllTools( int paintMode ) = 0;

	virtual bool IsThirdPersonCamera( ) = 0;

	virtual bool InToolMode() = 0;
};

#define VCLIENTENGINETOOLS_INTERFACE_VERSION_001 "VCLIENTENGINETOOLS001"


// IBaseClientDll_csgo_016 /////////////////////////////////////////////////////

#define CLIENT_DLL_INTERFACE_VERSION_CSGO_016 "VClient016"

// TODO: this is not the exact size, minium is 89, maybe it's 99
#define CLIENT_DLL_INTERFACE_CSGO_016_VTABLESIZE (4*99)

class IBaseClientDLL_csgo_016 abstract
{
public:
	// Connect appsystem components, get global interfaces, don't run any other init code
	virtual int			Connect( CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals ) = 0;

	virtual void			Disonnect() = 0;

	// run other init code here
	virtual int				Init( CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals ) = 0;

	virtual void			PostInit() = 0;

	// Called once when the client DLL is being unloaded
	virtual void			Shutdown( void ) = 0;
	
	virtual void _UNUSED_LevelInitPreEntity(void) = 0;
	virtual void _UNUSED_LevelInitPostEntity(void) = 0;
	virtual void _UNUSED_LevelShutdown(void) = 0;
	virtual void _UNUSED_GetAllClasses(void) = 0;
	virtual void _UNUSED_HudVidInit(void) = 0;
	virtual void _UNUSED_HudProcessInput(void) = 0;
	virtual void _UNUSED_HudUpdate(void) = 0;
	virtual void _UNUSED_HudReset(void) = 0;
	virtual void _UNUSED_HudText(void) = 0;
	virtual void _UNUSED_ShouldDrawDropdownConsole(void) = 0;
	virtual void _UNUSED_IN_ActivateMouse(void) = 0;
	virtual void _UNUSED_IN_DeactivateMouse(void) = 0;
	virtual void _UNUSED_IN_Accumulate (void) = 0;
	virtual void _UNUSED_IN_ClearStates (void) = 0;
	virtual void _UNUSED_IN_IsKeyDown(void ) = 0;
	virtual void _UNUSED_IN_KeyEvent(void) = 0;
	virtual void _UNUSED_CreateMove (void) = 0;				
	virtual void _UNUSED_ExtraMouseSample(void) = 0;
	virtual void _UNUSED_WriteUsercmdDeltaToBuffer(void) = 0;
	virtual void _UNUSED_EncodeUserCmdToBuffer(void) = 0;
	virtual void _UNUSED_DecodeUserCmdFromBuffer(void) = 0;
	virtual void _UNUSED_View_Render(void) = 0;
	virtual void _UNUSED_RenderView(void) = 0;
	virtual void _UNUSED_View_Fade(void) = 0;
	virtual void _UNUSED_SetCrosshairAngle(void) = 0;
	virtual void _UNUSED_InitSprite(void) = 0;
	virtual void _UNUSED_ShutdownSprite(void) = 0;
	virtual void _UNUSED_GetSpriteSize(void) const = 0;
	virtual void _UNUSED_VoiceStatus(void) = 0;
	virtual void _UNUSED_PlayerAudible(void) = 0;
	virtual void _UNUSED_InstallStringTableCallback(void) = 0;
	virtual void _UNUSED_FrameStageNotify(void) = 0;
	virtual void _UNUSED_DispatchUserMessage(void) = 0;
	virtual void _UNUSED_SaveInit(void) = 0;
	virtual void _UNUSED_SaveWriteFields(void) = 0;
	virtual void _UNUSED_SaveReadFields(void) = 0;
	virtual void _UNUSED_PreSave(void) = 0;
	virtual void _UNUSED_Save(void) = 0;
	virtual void _UNUSED_WriteSaveHeaders(void) = 0;
	virtual void _UNUSED_ReadRestoreHeaders(void) = 0;
	virtual void _UNUSED_Restore(void) = 0;
	virtual void _UNUSED_DispatchOnRestore(void) = 0;
	virtual void _UNUSED_GetStandardRecvProxies(void) = 0;
	virtual void _UNUSED_WriteSaveGameScreenshot(void) = 0;
	virtual void _UNUSED_EmitSentenceCloseCaption(void) = 0;
	virtual void _UNUSED_EmitCloseCaption(void) = 0;
	virtual void _UNUSED_CanRecordDemo(void) const = 0;
	virtual void _UNUSED_OnDemoRecordStart(void) = 0;
	virtual void _UNUSED_OnDemoRecordStop(void) = 0;
	virtual void _UNUSED_OnDemoPlaybackStart(void) = 0;
	virtual void _UNUSED_OnDemoPlaybackStop(void) = 0;
	virtual void _UNUSED_RecordDemoPolishUserInput(void) = 0;
	virtual void _UNUSED_CacheReplayRagdolls(void) = 0;
	virtual void _UNUSED_ReplayUI_SendMessage(void) = 0;
	virtual void _UNUSED_GetReplayFactory(void) = 0;
	virtual void _UNUSED_ClearLocalPlayerReplayPtr(void) = 0;
	virtual void _UNUSED_GetScreenWidth(void) = 0;
	virtual void _UNUSED_GetScreenHeight(void) = 0;
	virtual void _UNUSED_WriteSaveGameScreenshotOfSize(void) = 0;
	virtual void _UNUSED_WriteReplayScreenshotBadParams(void) = 0;
	virtual void _UNUSED_UpdateReplayScreenshotCache(void) = 0;
	virtual void _UNUSED_GetPlayerView(void) = 0;
	virtual void _UNUSED_ShouldHideLoadingPlaque(void) = 0;
	virtual void _UNUSED_InvalidateMdlCache(void) = 0;
	virtual void _UNUSED_IN_SetSampleTime(void) = 0;
	virtual void _UNUSED_OnActiveSplitscreenPlayerChanged(void) = 0;
	virtual void _UNUSED_OnSplitScreenStateChanged(void) = 0;
	virtual void _UNUSED_CenterStringOff(void) = 0;
	virtual void _UNUSED_OnScreenSizeChanged(void) = 0;
	virtual void _UNUSED_InstantiateMaterialProxy(void) = 0;
	virtual void _UNUSED_GetFullscreenClientDLLVPanel(void) = 0;
	virtual void _UNUSED_MarkEntitiesAsTouching(void) = 0;
	virtual void _UNUSED_OnKeyBindingChanged(void) = 0;
	virtual void _UNUSED_SetBlurFade(void) = 0;
	virtual void _UNUSED_ResetHudCloseCaption(void) = 0;
	virtual void _UNUSED_HandleGameUIEvent(void) = 0;
	virtual void _UNUSED_GetSoundSpatializationBadParams(void) = 0;
	virtual void _UNUSED_Hud_SaveStarted(void) = 0;
	virtual void _UNUSED_ShutdownMovies(void) = 0; 
	virtual void _UNUSED_IsChatRaised(void) = 0;
	virtual void _UNUSED_IsRadioPanelRaised(void) = 0;
	virtual void _UNUSED_IsBindMenuRaised(void) = 0;
	virtual void _UNUSED_IsTeamMenuRaised(void) = 0;
	virtual void _UNUSED_IsLoadingScreenRaised(void) = 0;
};
