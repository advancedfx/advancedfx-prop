#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-07-27 by dominik.matrixstorm.com
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


// VMatrix /////////////////////////////////////////////////////////////////////

class MdtMatrix;
typedef MdtMatrix VMatrix;

class MdtMatrix
{
public:
	MdtMatrix();
	MdtMatrix(const MdtMatrix & mdtMatrix);

	// The matrix.
	vec_t		m[4][4];
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

// IMemAlloc ///////////////////////////////////////////////////////////////////////

class IMemAlloc_csgo
{
public:
	virtual void _UNUSED_000(void) = 0;
	virtual void _UNUSED_001(void) = 0;
	virtual void _UNUSED_002(void) = 0;
	virtual void _UNUSED_003(void) = 0;
	virtual void _UNUSED_004(void) = 0;
	virtual void Free( void *pMem ) = 0;

	// There is more, but we don't need it at the moment
	// [....]
};

IMemAlloc_csgo * Get_g_pMemAlloc(void);

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

// IAppSystem_csgo ////////////////////////////////////////////////////////////

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

class IAppSystem_csgo abstract
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

	// Returns whether or not the app system is a singleton
	virtual bool IsSingleton() = 0;
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

class Color;

class IConsoleDisplayFunc abstract
{
public:
	virtual void ColorPrint( const Color& clr, const char *pMessage ) = 0;
	virtual void Print( const char *pMessage ) = 0;
	virtual void DPrint( const char *pMessage ) = 0;

	virtual void GetConsoleText( char *pchText, size_t bufSize ) const = 0;
};


class ICvar_007 abstract : public IAppSystem_csgo
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

	virtual void			InstallConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
	virtual void			RemoveConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
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

inline ICvar_007::Iterator::Iterator(ICvar_007 *icvar)
{
	m_pIter = icvar->FactoryInternalIterator();
}

inline ICvar_007::Iterator::~Iterator( void )
{
	Get_g_pMemAlloc()->Free(m_pIter);
}

inline void ICvar_007::Iterator::SetFirst( void )
{
	m_pIter->SetFirst();
}

inline void ICvar_007::Iterator::Next( void )
{
	m_pIter->Next();
}

inline bool ICvar_007::Iterator::IsValid( void )
{
	return m_pIter->IsValid();
}

inline ConCommandBase_007 * ICvar_007::Iterator::Get( void )
{
	return m_pIter->Get();
}

//-----------------------------------------------------------------------------
// Abstract interface for ConVars
//-----------------------------------------------------------------------------
class IConVar_007 abstract
{
public:
	// Value set
	virtual void SetValue( const char *pValue ) = 0;
	virtual void SetValue( float flValue ) = 0;
	virtual void SetValue( int nValue ) = 0;
	virtual void SetValue( Color value ) = 0;

	// Return name of command
	virtual const char *GetName( void ) const = 0;

	// Return name of command (usually == GetName(), except in case of FCVAR_SS_ADDED vars
	virtual const char *GetBaseName( void ) const = 0;

	// Accessors.. not as efficient as using GetState()/GetInfo()
	// if you call these methods multiple times on the same IConVar
	virtual bool IsFlagSet( int nFlag ) const = 0;

	virtual int GetSplitScreenPlayerSlot() const = 0;
};

template< class T, class I = int >
/// <remarks>Warning, only required elements declared and defined!</remarks>
class CUtlMemory_007
{
public:
	//
	// We don't need this
};

template< class T, class A = CUtlMemory_007<T> >
/// <remarks>Warning, only required elements declared and defined!</remarks>
class CUtlVector_007
{
	typedef A CAllocator;
public:
	typedef T ElemType_t;

protected:
	CAllocator m_Memory;
	int m_Size;

#ifndef _X360
	// For easier access to the elements through the debugger
	// it's in release builds so this can be used in libraries correctly
	T *m_pElements;

#else
#endif
};

//-----------------------------------------------------------------------------
// Called when a ConVar changes value
// NOTE: For FCVAR_NEVER_AS_STRING ConVars, pOldValue == NULL
//-----------------------------------------------------------------------------
typedef void ( *FnChangeCallback_t_007 )( IConVar_007 *var, const char *pOldValue, float flOldValue );

//-----------------------------------------------------------------------------
// Purpose: A console variable
//-----------------------------------------------------------------------------
/// <remarks>Warning, only required elements declared and defined!</remarks>
class ConVar_007 abstract : public ConCommandBase_007, public IConVar_007
{
friend class CCvar_007;
friend class ConVarRef_007;
friend class SplitScreenConVarRef_007;

friend class WrpConVarRef;

public:
	typedef ConCommandBase_007 BaseClass;

	/// <remarks>not implemented</remarks>
	virtual						~ConVar_007( void ) = 0;

	/// <remarks>not implemented</remarks>
	virtual bool				IsFlagSet( int flag ) const = 0;

	/// <remarks>not implemented</remarks>
	virtual const char*			GetHelpText( void ) const = 0;

	/// <remarks>not implemented</remarks>
	virtual bool				IsRegistered( void ) const = 0;

	/// <remarks>not implemented</remarks>
	virtual const char			*GetName( void ) const = 0;

	// Return name of command (usually == GetName(), except in case of FCVAR_SS_ADDED vars
	/// <remarks>not implemented</remarks>
	virtual const char			*GetBaseName( void ) const = 0;

	/// <remarks>not implemented</remarks>
	virtual int					GetSplitScreenPlayerSlot() const = 0;

	/// <remarks>not implemented</remarks>
	virtual void				AddFlags( int flags ) = 0;

	/// <remarks>not implemented</remarks>
	virtual int					GetFlags() const = 0;

	/// <remarks>not implemented</remarks>
	virtual	bool				IsCommand( void ) const = 0;

	// These just call into the IConCommandBaseAccessor to check flags and set the var (which ends up calling InternalSetValue).
	virtual void				SetValue( const char *value ) = 0;
	virtual void				SetValue( float value ) = 0;
	virtual void				SetValue( int value ) = 0;
	virtual void				SetValue( Color value ) = 0;
	
	// Value
	struct CVValue_t
	{
		char						*m_pszString;
		int							m_StringLength;

		// Values
		float						m_fValue;
		int							m_nValue;
	};

	FORCEINLINE_CVAR CVValue_t &GetRawValue()
	{
		return m_Value;
	}
	FORCEINLINE_CVAR const CVValue_t &GetRawValue() const
	{
		return m_Value;
	}

private:
	// Called by CCvar when the value of a var is changing.
	virtual void				InternalSetValue(const char *value) = 0;

	// For CVARs marked FCVAR_NEVER_AS_STRING
	virtual void				InternalSetFloatValue( float fNewValue ) = 0;
	virtual void				InternalSetIntValue( int nValue ) = 0;
	virtual void				InternalSetColorValue( Color value ) = 0;

	virtual bool				ClampValue( float& value ) = 0;
	virtual void				ChangeStringValue( const char *tempVal, float flOldValue ) = 0;

	virtual void				Create( const char *pName, const char *pDefaultValue, int flags = 0,
									const char *pHelpString = 0, bool bMin = false, float fMin = 0.0,
									bool bMax = false, float fMax = false, FnChangeCallback_t_007 callback = 0 ) = 0;

	// Used internally by OneTimeInit to initialize.
	virtual void				Init() = 0;



protected:

	// This either points to "this" or it points to the original declaration of a ConVar.
	// This allows ConVars to exist in separate modules, and they all use the first one to be declared.
	// m_pParent->m_pParent must equal m_pParent (ie: m_pParent must be the root, or original, ConVar).
	ConVar_007						*m_pParent;

	// Static data
	const char					*m_pszDefaultValue;
	
	CVValue_t					m_Value;

	// Min/Max values
	bool						m_bHasMin;
	float						m_fMinVal;
	bool						m_bHasMax;
	float						m_fMaxVal;
	
	// Call this function when ConVar changes
	CUtlVector_007<FnChangeCallback_t_007> m_fnChangeCallbacks;
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

	// Get access to the world to screen transformation matrix
	virtual const VMatrix& 		WorldToScreenMatrix() = 0;
	
	// Get the matrix to move a point from world space into view space
	// (translate and rotate so the camera is at the origin looking down X).
	virtual const VMatrix& 		WorldToViewMatrix() = 0;

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

	// new in csgo:
	virtual void				HideLoadingPlaque( void ) = 0;

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

	// Get access to the world to screen transformation matrix
	virtual const VMatrix& 		WorldToScreenMatrix() = 0;
	
	// Get the matrix to move a point from world space into view space
	// (translate and rotate so the camera is at the origin looking down X).
	virtual const VMatrix& 		WorldToViewMatrix() = 0;

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
	virtual int GetLevelVersion( void ) = 0;

#if !defined( NO_VOICE )
	virtual void _UNUSED_GetVoiceTweakAPI(void)=0;
#endif
	// Tell engine stats gathering system that the rendering frame is beginning/ending
	virtual void		EngineStats_BeginFrame( void ) = 0;
	virtual void		EngineStats_EndFrame( void ) = 0;
	
	virtual void _UNUSED_FireEvents(void)=0;
	virtual void _UNUSED_GetLeavesArea(void)=0;
	virtual void _UNUSED_DoesBoxTouchAreaFrustum(void)=0;
	
	// new in csgo:
	virtual void _UNUSED_GetFrustumList(void) = 0;

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

	// new in csgo:
	virtual int			GetDemoRecordingTick( void ) = 0;
	virtual int			GetDemoPlaybackTick( void ) = 0;
	virtual int			GetDemoPlaybackStartTick( void ) = 0;
	virtual float		GetDemoPlaybackTimeScale( void ) = 0;
	virtual int			GetDemoPlaybackTotalTicks( void ) = 0;

	// Is the game paused?
	virtual bool		IsPaused( void ) = 0;
	// Is the game currently taking a screenshot?

	// new in csgo:
	// What is the game timescale multiplied with the host_timescale?
	virtual float GetTimescale( void ) const = 0;

	virtual bool		IsTakingScreenshot( void ) = 0;
	// Is this a HLTV broadcast ?
	virtual bool		IsHLTV( void ) = 0;
	
	// new in csgo:
	// Is this a Replay demo?
	virtual bool		IsReplay( void ) = 0;

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

	virtual void _UNUSED_WildGuess( void ) = 0;
	virtual void _UNUSED_WildGuess2( void ) = 0;
	virtual void _UNUSED_WildGuess3( void ) = 0;
//	virtual void _UNUSED_WildGuess4( void ) = 0;

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

	// This version does NOT check against FCVAR_CLIENTCMD_CAN_EXECUTE.
	virtual void ClientCmd_Unrestricted( const char *szCmdString ) = 0;

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

	// .... might be more in some games (i.e. source-sdk-2013)
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
#define CLIENT_DLL_INTERFACE_VERSION_CSGO_017 "VClient017"

// IMaterial_csgo //////////////////////////////////////////////////////////////

class IMaterial_csgo abstract
{
public:
	// 000:
	virtual const char *	GetName() const = 0;

	// 001:
	virtual const char *	GetTextureGroupName() const = 0;

	virtual void _UNKNOWN_002(void) = 0;
	virtual void _UNKNOWN_003(void) = 0;
	virtual void _UNKNOWN_004(void) = 0;
	virtual void _UNKNOWN_005(void) = 0;
	virtual void _UNKNOWN_006(void) = 0;
	virtual void _UNKNOWN_007(void) = 0;
	virtual void _UNKNOWN_008(void) = 0;
	virtual void _UNKNOWN_009(void) = 0;
	virtual void _UNKNOWN_010(void) = 0;
	virtual void _UNKNOWN_011(void) = 0;

	// 012:
	virtual void IncrementReferenceCount( void ) = 0;

	// 013:
	virtual void DecrementReferenceCount( void ) = 0;

	virtual void _UNKNOWN_014(void) = 0;
	virtual void _UNKNOWN_015(void) = 0;
	virtual void _UNKNOWN_016(void) = 0;
	virtual void _UNKNOWN_017(void) = 0;
	virtual void _UNKNOWN_018(void) = 0;
	virtual void _UNKNOWN_019(void) = 0;

	virtual void _UNKNOWN_020(void) = 0;
	virtual void _UNKNOWN_021(void) = 0;
	virtual void _UNKNOWN_022(void) = 0;
	virtual void _UNKNOWN_023(void) = 0;
	virtual void _UNKNOWN_024(void) = 0;
	virtual void _UNKNOWN_025(void) = 0;
	virtual void _UNKNOWN_026(void) = 0;
	virtual void _UNKNOWN_027(void) = 0;
	virtual void _UNKNOWN_028(void) = 0;
	virtual void _UNKNOWN_029(void) = 0;

	virtual void _UNKNOWN_030(void) = 0;
	virtual void _UNKNOWN_031(void) = 0;
	virtual void _UNKNOWN_032(void) = 0;
	virtual void _UNKNOWN_033(void) = 0;
	virtual void _UNKNOWN_034(void) = 0;
	virtual void _UNKNOWN_035(void) = 0;
	virtual void _UNKNOWN_036(void) = 0;
	virtual void _UNKNOWN_037(void) = 0;
	virtual void _UNKNOWN_038(void) = 0;
	virtual void _UNKNOWN_039(void) = 0;

	virtual void _UNKNOWN_040(void) = 0;
	virtual void _UNKNOWN_041(void) = 0;
	virtual void _UNKNOWN_042(void) = 0;
	virtual void _UNKNOWN_043(void) = 0;
	virtual void _UNKNOWN_044(void) = 0;
	virtual void _UNKNOWN_045(void) = 0;
	virtual void _UNKNOWN_046(void) = 0;
	virtual void _UNKNOWN_047(void) = 0;
	virtual void _UNKNOWN_048(void) = 0;

	// 049:
	virtual const char * GetShaderName() const = 0;

	// ...
	// there are more here that we don't care about.
};

// IRefCounted_csgo ////////////////////////////////////////////////////////////

class IRefCounted_csgo abstract
{
public:
	virtual int AddRef() = 0;
	virtual int Release() = 0;
};

// IMesh_csgo //////////////////////////////////////////////////////////////////

typedef unsigned __int8 uint8;
typedef unsigned __int32 uint32;
typedef unsigned __int64 uint64;

typedef uint64 VertexFormat_t_csgo;

typedef void MeshBoneRemap_t_csgo;
typedef void matrix3x4_t_csgo;
typedef void ITexture_csgo;
typedef void MaterialLightingState_t_csgo;

enum MaterialPrimitiveType_t_csgo 
{ 
	MATERIAL_POINTS			= 0x0,
	MATERIAL_LINES,
	MATERIAL_TRIANGLES,
	MATERIAL_TRIANGLE_STRIP,
	MATERIAL_LINE_STRIP,
	MATERIAL_LINE_LOOP,	// a single line loop
	MATERIAL_POLYGON,	// this is a *single* polygon
	MATERIAL_QUADS,
	MATERIAL_SUBD_QUADS_EXTRA, // Extraordinary sub-d quads
	MATERIAL_SUBD_QUADS_REG,   // Regular sub-d quads
	MATERIAL_INSTANCED_QUADS, // (X360) like MATERIAL_QUADS, but uses vertex instancing

	// This is used for static meshes that contain multiple types of
	// primitive types.	When calling draw, you'll need to specify
	// a primitive type.
	MATERIAL_HETEROGENOUS
};


typedef void ShaderStencilState_t_csgo;
typedef int MaterialIndexFormat_t_csgo;


class IMesh_csgo;

// For now, vertex compression is simply "on or off" (for the sake of simplicity
// and MeshBuilder perf.), but later we may support multiple flavours.
enum VertexCompressionType_t_csgo
{
	// This indicates an uninitialized VertexCompressionType_t value
	VERTEX_COMPRESSION_INVALID = 0xFFFFFFFF,

	// 'VERTEX_COMPRESSION_NONE' means that no elements of a vertex are compressed
	VERTEX_COMPRESSION_NONE = 0,

	// Currently (more stuff may be added as needed), 'VERTEX_COMPRESSION_ON' means:
	//  - if a vertex contains VERTEX_ELEMENT_NORMAL, this is compressed
	//    (see CVertexBuilder::CompressedNormal3f)
	//  - if a vertex contains VERTEX_ELEMENT_USERDATA4 (and a normal - together defining a tangent
	//    frame, with the binormal reconstructed in the vertex shader), this is compressed
	//    (see CVertexBuilder::CompressedUserData)
	//  - if a vertex contains VERTEX_ELEMENT_BONEWEIGHTSx, this is compressed
	//    (see CVertexBuilder::CompressedBoneWeight3fv)
	VERTEX_COMPRESSION_ON = 1
};

enum
{
	VERTEX_MAX_TEXTURE_COORDINATES_csgo = 8,
	BONE_MATRIX_INDEX_INVALID_csgo = 255
};

struct VertexDesc_t_csgo
{
	// These can be set to zero if there are pointers to dummy buffers, when the
	// actual buffer format doesn't contain the data but it needs to be safe to
	// use all the CMeshBuilder functions.
	int	m_VertexSize_Position;
	int m_VertexSize_BoneWeight;
	int m_VertexSize_BoneMatrixIndex;
	int	m_VertexSize_Normal;
	int	m_VertexSize_Color;
	int	m_VertexSize_Specular;
	int m_VertexSize_TexCoord[VERTEX_MAX_TEXTURE_COORDINATES_csgo];
	int m_VertexSize_TangentS;
	int m_VertexSize_TangentT;
	int m_VertexSize_Wrinkle;

	int m_VertexSize_UserData;

	int m_ActualVertexSize;	// Size of the vertices.. Some of the m_VertexSize_ elements above
							// are set to this value and some are set to zero depending on which
							// fields exist in a buffer's vertex format.

	// The type of compression applied to this vertex data
	VertexCompressionType_t_csgo m_CompressionType;

	// Number of bone weights per vertex...
	int m_NumBoneWeights;

	// Pointers to our current vertex data
	float			*m_pPosition;

	float			*m_pBoneWeight;

#ifndef NEW_SKINNING_csgo
	unsigned char	*m_pBoneMatrixIndex;
#else
	float			*m_pBoneMatrixIndex;
#endif

	float			*m_pNormal;

	unsigned char	*m_pColor;
	unsigned char	*m_pSpecular;
	float			*m_pTexCoord[VERTEX_MAX_TEXTURE_COORDINATES_csgo];

	// Tangent space *associated with one particular set of texcoords*
	float			*m_pTangentS;
	float			*m_pTangentT;

	float			*m_pWrinkle;

	// user data
	float			*m_pUserData;

	// The first vertex index (used for buffered vertex buffers, or cards that don't support stream offset)
	int	m_nFirstVertex;

	// The offset in bytes of the memory we're writing into 
	// from the start of the D3D buffer (will be 0 for static meshes)
	unsigned int	m_nOffset;

#ifdef DEBUG_WRITE_COMBINE_csgo
	int m_nLastWrittenField;
	unsigned char* m_pLastWrittenAddress;
#endif
};

struct IndexDesc_t_csgo
{
	// Pointers to the index data
	unsigned short	*m_pIndices;

	// The offset in bytes of the memory we're writing into 
	// from the start of the D3D buffer (will be 0 for static meshes)
	unsigned int	m_nOffset;

	// The first index (used for buffered index buffers, or cards that don't support stream offset)
	unsigned int	m_nFirstIndex;

	// 1 if the device is active, 0 if the device isn't active.
	// Faster than doing if checks for null m_pIndices if someone is
	// trying to write the m_pIndices while the device is inactive.
	unsigned int	m_nIndexSize;
};

struct MeshDesc_t_csgo : public VertexDesc_t_csgo, public IndexDesc_t_csgo
{
};

struct MeshBuffersAllocationSettings_t_csgo
{
	uint32 m_uiIbUsageFlags;
};

class Vector4D_csgo					
{
public:
	// Members
	vec_t x, y, z, w;
};

class IVertexBuffer_csgo abstract
{
public:
	// NOTE: The following two methods are only valid for static vertex buffers
	// Returns the number of vertices and the format of the vertex buffer
	virtual int VertexCount() const = 0;
	virtual VertexFormat_t_csgo GetVertexFormat() const = 0;

	// Is this vertex buffer dynamic?
	virtual bool IsDynamic() const = 0;

	// NOTE: For dynamic vertex buffers only!
	// Casts the memory of the dynamic vertex buffer to the appropriate type
	virtual void BeginCastBuffer( VertexFormat_t_csgo format ) = 0;
	virtual void EndCastBuffer() = 0;

	// Returns the number of vertices that can still be written into the buffer
	virtual int GetRoomRemaining() const = 0;

	virtual bool Lock( int nVertexCount, bool bAppend, VertexDesc_t_csgo &desc ) = 0;
	virtual void Unlock( int nVertexCount, VertexDesc_t_csgo &desc ) = 0;

	// Spews the mesh data
	virtual void Spew( int nVertexCount, const VertexDesc_t_csgo &desc ) = 0;

	// Call this in debug mode to make sure our data is good.
	virtual void ValidateData( int nVertexCount, const VertexDesc_t_csgo & desc ) = 0;
};

class IIndexBuffer_csgo abstract
{
public:
	// NOTE: The following two methods are only valid for static index buffers
	// Returns the number of indices and the format of the index buffer
	virtual int IndexCount() const = 0;
	virtual MaterialIndexFormat_t_csgo IndexFormat() const = 0;

	// Is this index buffer dynamic?
	virtual bool IsDynamic() const = 0;

	// NOTE: For dynamic index buffers only!
	// Casts the memory of the dynamic index buffer to the appropriate type
	virtual void BeginCastBuffer( MaterialIndexFormat_t_csgo format ) = 0;
	virtual void EndCastBuffer() = 0;

	// Returns the number of indices that can still be written into the buffer
	virtual int GetRoomRemaining() const = 0;

	// Locks, unlocks the index buffer
	virtual bool Lock( int nMaxIndexCount, bool bAppend, IndexDesc_t_csgo &desc ) = 0;
	virtual void Unlock( int nWrittenIndexCount, IndexDesc_t_csgo &desc ) = 0;

	// FIXME: Remove this!! Here only for backward compat on IMesh
	// Locks, unlocks the index buffer for modify
	virtual void ModifyBegin( bool bReadOnly, int nFirstIndex, int nIndexCount, IndexDesc_t_csgo& desc ) = 0;
	virtual void ModifyEnd( IndexDesc_t_csgo& desc ) = 0;

	// Spews the mesh data
	virtual void Spew( int nIndexCount, const IndexDesc_t_csgo &desc ) = 0;

	// Ensures the data in the index buffer is valid
	virtual void ValidateData( int nIndexCount, const IndexDesc_t_csgo &desc ) = 0;

	// For backward compat to IMesh
	virtual IMesh_csgo* GetMesh() = 0;
};

class CPrimList_csgo;
class CMeshBuilder_csgo;
class ICachedPerFrameMeshData_csgo;

class IMesh_csgo abstract : public IVertexBuffer_csgo, public IIndexBuffer_csgo
{
public:
	// -----------------------------------

	// Sets/gets the primitive type
	virtual void SetPrimitiveType( MaterialPrimitiveType_t_csgo type ) = 0;

	// Draws the mesh
	virtual void Draw( int firstIndex = -1, int numIndices = 0 ) = 0;

	virtual void SetColorMesh( IMesh_csgo *pColorMesh, int nVertexOffset ) = 0;

	// Draw a list of (lists of) primitives. Batching your lists together that use
	// the same lightmap, material, vertex and index buffers with multipass shaders
	// can drastically reduce state-switching overhead.
	// NOTE: this only works with STATIC meshes.
	virtual void Draw( CPrimList_csgo *pLists, int nLists ) = 0;

	// Copy verts and/or indices to a mesh builder. This only works for temp meshes!
	virtual void CopyToMeshBuilder( 
		int iStartVert,		// Which vertices to copy.
		int nVerts, 
		int iStartIndex,	// Which indices to copy.
		int nIndices, 
		int indexOffset,	// This is added to each index.
		CMeshBuilder_csgo &builder ) = 0;

	// Spews the mesh data
	virtual void Spew( int numVerts, int numIndices, const MeshDesc_t_csgo &desc ) = 0;

	// Call this in debug mode to make sure our data is good.
	virtual void ValidateData( int numVerts, int numIndices, const MeshDesc_t_csgo &desc ) = 0;

	// New version
	// Locks/unlocks the mesh, providing space for numVerts and numIndices.
	// numIndices of -1 means don't lock the index buffer...
	virtual void LockMesh( int numVerts, int numIndices, MeshDesc_t_csgo &desc, MeshBuffersAllocationSettings_t_csgo *pSettings ) = 0;
	virtual void ModifyBegin( int firstVertex, int numVerts, int firstIndex, int numIndices, MeshDesc_t_csgo& desc ) = 0;
	virtual void ModifyEnd( MeshDesc_t_csgo& desc ) = 0;
	virtual void UnlockMesh( int numVerts, int numIndices, MeshDesc_t_csgo &desc ) = 0;

	virtual void ModifyBeginEx( bool bReadOnly, int firstVertex, int numVerts, int firstIndex, int numIndices, MeshDesc_t_csgo &desc ) = 0;

	virtual void SetFlexMesh( IMesh_csgo *pMesh, int nVertexOffset ) = 0;

	virtual void DisableFlexMesh() = 0;

	virtual void MarkAsDrawn() = 0;

	// NOTE: I chose to create this method strictly because it's 2 days to code lock
	// and I could use the DrawInstances technique without a larger code change
	// Draws the mesh w/ modulation.
	virtual void DrawModulated( const Vector4D_csgo &vecDiffuseModulation, int firstIndex = -1, int numIndices = 0 ) = 0;

	virtual unsigned int ComputeMemoryUsed() = 0;

	virtual void *AccessRawHardwareDataStream( uint8 nRawStreamIndex, uint32 numBytes, uint32 uiFlags, void *pvContext ) = 0;

	virtual ICachedPerFrameMeshData_csgo *GetCachedPerFrameMeshData() = 0;
	virtual void ReconstructFromCachedPerFrameMeshData( ICachedPerFrameMeshData_csgo *pData ) = 0;
};

class IMeshMgr_csgo abstract : public IMesh_csgo
{
public:
	virtual void _UNKNOWN_030(void) = 0;
	virtual void _UNKNOWN_031(void) = 0;
	virtual void _UNKNOWN_032(void) = 0;
	virtual void _UNKNOWN_033(void) = 0;
	virtual void _UNKNOWN_034(void) = 0;
	virtual void _UNKNOWN_035(void) = 0;
	virtual void _UNKNOWN_036(void) = 0;
	virtual void _UNKNOWN_037(void) = 0;
	virtual void _UNKNOWN_038(void) = 0;
	virtual void _UNKNOWN_039(void) = 0;
	virtual void _UNKNOWN_040(void) = 0;
	virtual void _UNKNOWN_041(void) = 0;
	virtual void _UNKNOWN_042(void) = 0;
	virtual void _UNKNOWN_043(void) = 0;
	virtual void _UNKNOWN_044(void) = 0;
	virtual void _UNKNOWN_045(void) = 0;
	virtual void _UNKNOWN_046(void) = 0;
	virtual void _UNKNOWN_047(void) = 0;
	virtual void _UNKNOWN_048(void) = 0;
};

// IMatRenderContext_csgo //////////////////////////////////////////////////////

struct MeshInstanceData_t_csgo
{
	int						m_nIndexOffset;
	int						m_nIndexCount;
	int						m_nBoneCount;
	MeshBoneRemap_t_csgo *		m_pBoneRemap;		// there are bone count of these, they index into pose to world
	matrix3x4_t_csgo	*			m_pPoseToWorld;	// transforms for the *entire* model, indexed into by m_pBoneIndex. Potentially more than bone count of these
	const ITexture_csgo *		m_pEnvCubemap;
	MaterialLightingState_t_csgo *m_pLightingState;
	MaterialPrimitiveType_t_csgo m_nPrimType;
	const IVertexBuffer_csgo	*	m_pVertexBuffer;
	int						m_nVertexOffsetInBytes;
	const IIndexBuffer_csgo *	m_pIndexBuffer;
	const IVertexBuffer_csgo	*	m_pColorBuffer;
	int						m_nColorVertexOffsetInBytes;
	ShaderStencilState_t_csgo *	m_pStencilState;
	Vector4D_csgo				m_DiffuseModulation;
	int _UNKNOWN_0x4c;
};

class IMatRenderContext_csgo abstract : public IRefCounted_csgo
{
public:
	// 002:
	virtual void BeginRender() = 0;

	// 003:
	virtual void EndRender() = 0;

	// 004;
	virtual void Flush( bool flushHardware = false ) = 0;

	virtual void _UNKNOWN_005(void) = 0;
	virtual void _UNKNOWN_006(void) = 0;
	virtual void _UNKNOWN_007(void) = 0;
	virtual void _UNUSED_008_GetRenderTargetDimensions(void) = 0;

	// 009:
	virtual void Bind( IMaterial_csgo *material, void *proxyData = 0 ) = 0;

	virtual void _UNKNOWN_010(void) = 0;
	virtual void _UNKNOWN_011(void) = 0;
	virtual void _UNKNOWN_012(void) = 0;
	virtual void _UNKNOWN_013(void) = 0;
	virtual void _UNKNOWN_014(void) = 0;
	virtual void _UNKNOWN_015(void) = 0;
	virtual void _UNKNOWN_016(void) = 0;
	virtual void _UNKNOWN_017(void) = 0;
	virtual void _UNKNOWN_018(void) = 0;
	virtual void _UNKNOWN_019(void) = 0;
	virtual void _UNKNOWN_020(void) = 0;
	virtual void _UNKNOWN_021(void) = 0;
	virtual void _UNKNOWN_022(void) = 0;
	virtual void _UNKNOWN_023(void) = 0;
	virtual void _UNKNOWN_024(void) = 0;
	virtual void _UNKNOWN_025(void) = 0;
	virtual void _UNKNOWN_026(void) = 0;
	virtual void _UNKNOWN_027(void) = 0;
	virtual void _UNKNOWN_028(void) = 0;
	virtual void _UNKNOWN_029(void) = 0;
	virtual void _UNKNOWN_030(void) = 0;
	virtual void _UNKNOWN_031(void) = 0;
	virtual void _UNKNOWN_032(void) = 0;
	virtual void _UNKNOWN_033(void) = 0;
	virtual void _UNKNOWN_034(void) = 0;
	virtual void _UNKNOWN_035(void) = 0;
	virtual void _UNKNOWN_036(void) = 0;
	virtual void _UNKNOWN_037(void) = 0;
	virtual void _UNKNOWN_038(void) = 0;
	virtual void _UNKNOWN_039(void) = 0;
	virtual void _UNKNOWN_040(void) = 0;
	virtual void _UNKNOWN_041(void) = 0;
	virtual void _UNKNOWN_042(void) = 0;
	virtual void _UNKNOWN_043(void) = 0;
	virtual void _UNKNOWN_044(void) = 0;
	virtual void _UNKNOWN_045(void) = 0;
	virtual void _UNKNOWN_046(void) = 0;
	virtual void _UNKNOWN_047(void) = 0;
	virtual void _UNKNOWN_048(void) = 0;
	virtual void _UNKNOWN_049(void) = 0;
	virtual void _UNKNOWN_050(void) = 0;
	virtual void _UNKNOWN_051(void) = 0;
	virtual void _UNKNOWN_052(void) = 0;
	virtual void _UNKNOWN_053(void) = 0;
	virtual void _UNKNOWN_054(void) = 0;
	virtual void _UNKNOWN_055(void) = 0;
	virtual void _UNKNOWN_056(void) = 0;
	virtual void _UNKNOWN_057(void) = 0;
	virtual void _UNKNOWN_058(void) = 0;
	virtual void _UNKNOWN_059(void) = 0;
	virtual void _UNKNOWN_060(void) = 0;
	virtual void _UNKNOWN_061(void) = 0;

	// 062:
	// Actually this returns IMesh_csgo, however Valve seems to internally cast this to a derived class (dunno, but that's the way it is).
	virtual IMeshMgr_csgo* GetDynamicMesh( 
		bool buffered = true, 
		IMesh_csgo* pVertexOverride = 0,	
		IMesh_csgo* pIndexOverride = 0, 
		IMaterial_csgo *pAutoBind = 0 ) = 0;

	virtual void _UNKNOWN_063(void) = 0; // CreateStaticVertexBuffer*?
	virtual void _UNKNOWN_064(void) = 0; // CreateStaticIndexBuffer*?
	virtual void _UNKNOWN_065(void) = 0; // DestroyVertexBuffer*?
	virtual void _UNKNOWN_066(void) = 0; // DestroyIndexBuffer*?
	virtual void _UNKNOWN_067(void) = 0; // GetDynamicVertexBuffer*?
	virtual void _UNKNOWN_068(void) = 0; // GetDynamicIndexBuffer?
	virtual void _UNKNOWN_069(void) = 0; // BindVertexBuffer*?
	virtual void _UNKNOWN_070(void) = 0; // BindIndexBuffer?
	virtual void _UNKNOWN_071(void) = 0; // Draw?
	virtual void _UNKNOWN_072(void) = 0; // SelectionMode
	virtual void _UNKNOWN_073(void) = 0; // SelectionBuffer
	virtual void _UNKNOWN_074(void) = 0; // ClearSelectionNames
	virtual void _UNKNOWN_075(void) = 0; // LoadSelectionName
	virtual void _UNKNOWN_076(void) = 0; // PushSelectionName
	virtual void _UNKNOWN_077(void) = 0; // PopSelectionName
	virtual void _UNKNOWN_078(void) = 0; // ClearColor3ub
	virtual void _UNKNOWN_079(void) = 0; // ClearColor4ub
	virtual void _UNKNOWN_080(void) = 0; // OverrideDepthEnable
	virtual void _UNKNOWN_081(void) = 0;
	virtual void _UNKNOWN_082(void) = 0;
	virtual void _UNKNOWN_083(void) = 0;
	virtual void _UNKNOWN_084(void) = 0;
	virtual void _UNKNOWN_085(void) = 0;
	virtual void _UNKNOWN_086(void) = 0;
	virtual void _UNKNOWN_087(void) = 0;
	virtual void _UNKNOWN_088(void) = 0;
	virtual void _UNKNOWN_089(void) = 0;
	virtual void _UNKNOWN_090(void) = 0;
	virtual void _UNKNOWN_091(void) = 0;
	virtual void _UNKNOWN_092(void) = 0;
	virtual void _UNKNOWN_093(void) = 0;
	virtual void _UNKNOWN_094(void) = 0;
	virtual void _UNKNOWN_095(void) = 0;
	virtual void _UNKNOWN_096(void) = 0;
	virtual void _UNKNOWN_097(void) = 0;
	virtual void _UNKNOWN_098(void) = 0;
	virtual void _UNKNOWN_099(void) = 0;
	virtual void _UNKNOWN_100(void) = 0;
	virtual void _UNKNOWN_101(void) = 0;
	virtual void _UNKNOWN_102(void) = 0;
	virtual void _UNKNOWN_103(void) = 0;
	virtual void _UNKNOWN_104(void) = 0;
	virtual void _UNKNOWN_105(void) = 0;
	virtual void _UNKNOWN_106(void) = 0;
	virtual void _UNKNOWN_107(void) = 0;
	virtual void _UNKNOWN_108(void) = 0;
	virtual void _UNKNOWN_109(void) = 0;
	virtual void _UNKNOWN_110(void) = 0;
	virtual void _UNKNOWN_111(void) = 0;
	virtual void _UNKNOWN_112(void) = 0;
	virtual void _UNKNOWN_113(void) = 0;
	virtual void _UNKNOWN_114(void) = 0;
	virtual void _UNKNOWN_115(void) = 0;
	virtual void _UNKNOWN_116(void) = 0;
	virtual void _UNKNOWN_117(void) = 0;
	virtual void _UNKNOWN_118(void) = 0;
	virtual void _UNKNOWN_119(void) = 0;
	virtual void _UNKNOWN_120(void) = 0;
	virtual void _UNKNOWN_121(void) = 0;
	virtual void _UNKNOWN_122(void) = 0;
	virtual void _UNKNOWN_123(void) = 0;
	virtual void _UNKNOWN_124(void) = 0;
	virtual void _UNKNOWN_125(void) = 0;
	virtual void _UNKNOWN_126(void) = 0;
	virtual void _UNKNOWN_127(void) = 0;
	virtual void _UNKNOWN_128(void) = 0;
	virtual void _UNKNOWN_129(void) = 0;
	virtual void _UNKNOWN_130(void) = 0;
	virtual void _UNKNOWN_131(void) = 0;
	virtual void _UNKNOWN_132(void) = 0; // GetMaxToRender
	virtual void _UNKNOWN_133(void) = 0; // GetMaxVerticesToRender
	virtual void _UNKNOWN_134(void) = 0; // GetMaxIndicesToRender
	virtual void _UNKNOWN_135(void) = 0;
	virtual void _UNKNOWN_136(void) = 0;
	virtual void _UNKNOWN_137(void) = 0;
	virtual void _UNKNOWN_138(void) = 0;
	virtual void _UNKNOWN_139(void) = 0;
	virtual void _UNKNOWN_140(void) = 0;
	virtual void _UNKNOWN_141(void) = 0;
	virtual void _UNKNOWN_142(void) = 0;
	virtual void _UNKNOWN_143(void) = 0;
	virtual void _UNKNOWN_144(void) = 0;
	virtual void _UNKNOWN_145(void) = 0;
	virtual void _UNKNOWN_146(void) = 0;
	virtual void _UNKNOWN_147(void) = 0;
	virtual void _UNKNOWN_148(void) = 0;
	virtual void _UNKNOWN_149(void) = 0;
	virtual void _UNKNOWN_150(void) = 0;
	virtual void _UNKNOWN_151(void) = 0;
	virtual void _UNKNOWN_152(void) = 0;
	virtual void _UNKNOWN_153(void) = 0;
	virtual void _UNKNOWN_154(void) = 0;
	virtual void _UNKNOWN_155(void) = 0;
	virtual void _UNKNOWN_156(void) = 0;
	virtual void _UNKNOWN_157(void) = 0;
	virtual void _UNKNOWN_158(void) = 0;
	virtual void _UNKNOWN_159(void) = 0;
	virtual void _UNKNOWN_160(void) = 0;
	virtual void _UNKNOWN_161(void) = 0;
	virtual void _UNKNOWN_162(void) = 0;
	virtual void _UNKNOWN_163(void) = 0;
	virtual void _UNKNOWN_164(void) = 0;
	virtual void _UNKNOWN_165(void) = 0;
	virtual void _UNKNOWN_166(void) = 0;
	virtual void _UNKNOWN_167(void) = 0;
	virtual void _UNKNOWN_168(void) = 0;
	virtual void _UNKNOWN_169(void) = 0;
	virtual void _UNKNOWN_170(void) = 0;
	virtual void _UNKNOWN_171(void) = 0;
	virtual void _UNKNOWN_172(void) = 0;
	virtual void _UNKNOWN_173(void) = 0;
	virtual void _UNKNOWN_174(void) = 0;
	virtual void _UNKNOWN_175(void) = 0;
	virtual void _UNKNOWN_176(void) = 0;
	virtual void _UNKNOWN_177(void) = 0;
	virtual void _UNKNOWN_178(void) = 0;
	virtual void _UNKNOWN_179(void) = 0;
	virtual void _UNKNOWN_180(void) = 0;
	virtual void _UNKNOWN_181(void) = 0;
	virtual void _UNKNOWN_182(void) = 0;
	virtual void _UNKNOWN_183(void) = 0;
	virtual void _UNKNOWN_184(void) = 0;
	virtual void _UNKNOWN_185(void) = 0;
	virtual void _UNKNOWN_186(void) = 0;
	virtual void _UNKNOWN_187(void) = 0;
	virtual void _UNKNOWN_188(void) = 0;
	virtual void _UNKNOWN_189(void) = 0;
	virtual void _UNKNOWN_190(void) = 0;
	virtual void _UNKNOWN_191(void) = 0;

	// 192:
	virtual void DrawInstances( int nInstanceCount, const MeshInstanceData_t_csgo *pInstance ) = 0;

	virtual void _UNKNOWN_193(void) = 0; // OverrideAlphaWriteEnable
	virtual void _UNKNOWN_194(void) = 0; // OverrideColorWriteEnable
	virtual void _UNKNOWN_195(void) = 0;
	virtual void _UNKNOWN_196(void) = 0;
	virtual void _UNKNOWN_197(void) = 0;
	virtual void _UNKNOWN_198(void) = 0;
	virtual void _UNKNOWN_199(void) = 0;
	virtual void _UNKNOWN_200(void) = 0;
	virtual void _UNKNOWN_201(void) = 0;
	virtual void _UNKNOWN_202(void) = 0;
	virtual void _UNKNOWN_203(void) = 0;
	virtual void _UNKNOWN_204(void) = 0;
	virtual void _UNKNOWN_205(void) = 0;
	virtual void _UNKNOWN_206(void) = 0;
	virtual void _UNKNOWN_207(void) = 0;
	virtual void _UNKNOWN_208(void) = 0;
	virtual void _UNKNOWN_209(void) = 0;
	virtual void _UNKNOWN_210(void) = 0;
	virtual void _UNKNOWN_211(void) = 0;
	virtual void _UNKNOWN_212(void) = 0;
	virtual void _UNKNOWN_213(void) = 0;
	virtual void _UNKNOWN_214(void) = 0;
	virtual void _UNKNOWN_215(void) = 0;
	virtual void _UNKNOWN_216(void) = 0;
	virtual void _UNKNOWN_217(void) = 0;
	virtual void _UNKNOWN_218(void) = 0;
	virtual void _UNKNOWN_219(void) = 0;
	virtual void _UNKNOWN_220(void) = 0;
	virtual void _UNKNOWN_221(void) = 0;
	virtual void _UNKNOWN_222(void) = 0;
	virtual void _UNKNOWN_223(void) = 0;
	virtual void _UNKNOWN_224(void) = 0;
	virtual void _UNKNOWN_225(void) = 0;
	virtual void _UNKNOWN_226(void) = 0;
	virtual void _UNKNOWN_227(void) = 0;
	virtual void _UNKNOWN_228(void) = 0;
	virtual void _UNKNOWN_229(void) = 0;
	virtual void _UNKNOWN_230(void) = 0;
	virtual void _UNKNOWN_231(void) = 0;
	virtual void _UNKNOWN_232(void) = 0;
	virtual void _UNKNOWN_233(void) = 0;
	virtual void _UNKNOWN_234(void) = 0;
	virtual void _UNKNOWN_235(void) = 0;
	virtual void _UNKNOWN_236(void) = 0;
	virtual void _UNKNOWN_237(void) = 0;
	virtual void _UNKNOWN_238(void) = 0;
	virtual void _UNKNOWN_239(void) = 0;
	virtual void _UNKNOWN_240(void) = 0;
	virtual void _UNKNOWN_241(void) = 0;
	virtual void _UNKNOWN_242(void) = 0;
	virtual void _UNKNOWN_243(void) = 0;
	virtual void _UNKNOWN_244(void) = 0;
	virtual void _UNKNOWN_245(void) = 0;
	virtual void _UNKNOWN_246(void) = 0;
	virtual void _UNKNOWN_247(void) = 0;
	virtual void _UNKNOWN_248(void) = 0;
	virtual void _UNKNOWN_249(void) = 0;
	virtual void _UNKNOWN_250(void) = 0;
	virtual void _UNKNOWN_251(void) = 0;
	virtual void _UNKNOWN_252(void) = 0;
	virtual void _UNKNOWN_253(void) = 0;
	virtual void _UNKNOWN_254(void) = 0;
};

// IMaterialSystem_csgo ////////////////////////////////////////////////////////

#define MATERIAL_SYSTEM_INTERFACE_VERSION_CSGO_80 "VMaterialSystem080"

typedef void * MaterialLock_t_csgo;

class IMaterialSystem_csgo abstract : public IAppSystem_csgo
{
public:
	virtual void _UNKNOWN_009(void) = 0;
	virtual void _UNKNOWN_010(void) = 0;
	virtual void _UNKNOWN_011(void) = 0;
	virtual void _UNKNOWN_012(void) = 0;
	virtual void _UNKNOWN_013(void) = 0;
	virtual void _UNKNOWN_014(void) = 0;
	virtual void _UNKNOWN_015(void) = 0;
	virtual void _UNKNOWN_016(void) = 0;
	virtual void _UNKNOWN_017(void) = 0;
	virtual void _UNKNOWN_018(void) = 0;
	virtual void _UNKNOWN_019(void) = 0;
	virtual void _UNKNOWN_020(void) = 0;
	virtual void _UNKNOWN_021(void) = 0;
	virtual void _UNKNOWN_022(void) = 0;
	virtual void _UNKNOWN_023(void) = 0;
	virtual void _UNKNOWN_024(void) = 0;
	virtual void _UNKNOWN_025(void) = 0;
	virtual void _UNKNOWN_026(void) = 0;
	virtual void _UNKNOWN_027(void) = 0;
	virtual void _UNKNOWN_028(void) = 0;
	virtual void _UNKNOWN_029(void) = 0;
	virtual void _UNKNOWN_030(void) = 0;
	virtual void _UNKNOWN_031(void) = 0;
	virtual void _UNKNOWN_032(void) = 0;
	virtual void _UNKNOWN_033(void) = 0;
	virtual void _UNKNOWN_034(void) = 0;
	virtual void _UNKNOWN_035(void) = 0;
	virtual void _UNKNOWN_036(void) = 0;
	virtual void _UNKNOWN_037(void) = 0;
	virtual void _UNKNOWN_038(void) = 0;
	virtual void _UNKNOWN_039(void) = 0;
	virtual void _UNKNOWN_040(void) = 0;
	virtual void _UNKNOWN_041(void) = 0;
	virtual void _UNKNOWN_042(void) = 0;
	virtual void _UNKNOWN_043(void) = 0;
	virtual void _UNKNOWN_044(void) = 0;
	virtual void _UNKNOWN_045(void) = 0;

	// 046:
	virtual void SwapBuffers() = 0;

	virtual void _UNKNOWN_047(void) = 0;
	virtual void _UNKNOWN_048(void) = 0;
	virtual void _UNKNOWN_049(void) = 0;
	virtual void _UNKNOWN_050(void) = 0;
	virtual void _UNKNOWN_051(void) = 0;
	virtual void _UNKNOWN_052(void) = 0;
	virtual void _UNKNOWN_053(void) = 0;
	virtual void _UNKNOWN_054(void) = 0;
	virtual void _UNKNOWN_055(void) = 0;
	virtual void _UNKNOWN_056(void) = 0;
	virtual void _UNKNOWN_057(void) = 0;
	virtual void _UNKNOWN_058(void) = 0;
	virtual void _UNKNOWN_059(void) = 0;
	virtual void _UNKNOWN_060(void) = 0;
	virtual void _UNKNOWN_061(void) = 0;
	virtual void _UNKNOWN_062(void) = 0;
	virtual void _UNKNOWN_063(void) = 0;
	virtual void _UNKNOWN_064(void) = 0;
	virtual void _UNKNOWN_065(void) = 0;
	virtual void _UNKNOWN_066(void) = 0;
	virtual void _UNKNOWN_067(void) = 0;
	virtual void _UNKNOWN_068(void) = 0;
	virtual void _UNKNOWN_069(void) = 0;
	virtual void _UNKNOWN_070(void) = 0;
	virtual void _UNKNOWN_071(void) = 0;
	virtual void _UNKNOWN_072(void) = 0;
	virtual void _UNKNOWN_073(void) = 0;
	virtual void _UNKNOWN_074(void) = 0;
	virtual void _UNKNOWN_075(void) = 0;
	virtual void _UNKNOWN_076(void) = 0;
	virtual void _UNKNOWN_077(void) = 0;
	virtual void _UNKNOWN_078(void) = 0;
	virtual void _UNKNOWN_079(void) = 0;
	virtual void _UNKNOWN_080(void) = 0;
	virtual void _UNKNOWN_081(void) = 0;
	virtual void _UNKNOWN_082(void) = 0;
	virtual void _UNKNOWN_083(void) = 0;

	// 084:
	virtual IMaterial_csgo * FindMaterial( char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = 0 ) = 0;
	
	virtual void _UNKNOWN_085(void) = 0;
	virtual void _UNKNOWN_086(void) = 0;
	virtual void _UNKNOWN_087(void) = 0;
	virtual void _UNKNOWN_088(void) = 0;
	virtual void _UNKNOWN_089(void) = 0;
	virtual void _UNKNOWN_090(void) = 0;
	virtual void _UNKNOWN_091(void) = 0;
	virtual void _UNKNOWN_092(void) = 0;
	virtual void _UNKNOWN_093(void) = 0;
	virtual void _UNKNOWN_094(void) = 0;
	virtual void _UNKNOWN_095(void) = 0;
	virtual void _UNKNOWN_096(void) = 0;
	virtual void _UNKNOWN_097(void) = 0;
	virtual void _UNKNOWN_098(void) = 0;
	virtual void _UNKNOWN_099(void) = 0;
	virtual void _UNKNOWN_100(void) = 0;
	virtual void _UNKNOWN_101(void) = 0;
	virtual void _UNKNOWN_102(void) = 0;
	virtual void _UNKNOWN_103(void) = 0;
	virtual void _UNKNOWN_104(void) = 0;
	virtual void _UNKNOWN_105(void) = 0;
	virtual void _UNKNOWN_106(void) = 0;
	virtual void _UNKNOWN_107(void) = 0;
	virtual void _UNKNOWN_108(void) = 0;
	virtual void _UNKNOWN_109(void) = 0;
	virtual void _UNKNOWN_110(void) = 0;
	virtual void _UNKNOWN_111(void) = 0;
	virtual void _UNKNOWN_112(void) = 0;
	virtual void _UNKNOWN_113(void) = 0;
	virtual void _UNKNOWN_114(void) = 0;
	
	// 115:
	virtual IMatRenderContext_csgo *	GetRenderContext() = 0;

	virtual void _UNKNOWN_116(void) = 0;
	virtual void _UNKNOWN_117(void) = 0;
	virtual void _UNKNOWN_118(void) = 0;
	virtual void _UNKNOWN_119(void) = 0;
	virtual void _UNKNOWN_120(void) = 0;

	// 121:
	virtual IMatRenderContext_csgo *SetRenderContext( IMatRenderContext_csgo * ) = 0;

	// ...
	// more that we don't care about
};

// IBaseClientDLL_csgo /////////////////////////////////////////////////////////

class CViewSetup_csgo;

struct vrect_t_csgo
{
	int				x,y,width,height;
	vrect_t_csgo			*pnext;
};

class IBaseClientDLL_csgo abstract
{
public:
	// 000:
	// Connect appsystem components, get global interfaces, don't run any other init code
	virtual int Connect( CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals ) = 0;

	// 001:
	virtual void Disconnect() = 0;

	// 002:
	// run other init code here
	virtual int Init( CreateInterfaceFn appSystemFactory, CGlobalVarsBase *pGlobals ) = 0;

	// 003:
	virtual void PostInit() = 0;

	// 004:
	// Called once when the client DLL is being unloaded
	virtual void Shutdown( void ) = 0;

	// 005:
	// Called at the start of each level change
	virtual void LevelInitPreEntity( char const* pMapName ) = 0;

	// 006:
	// Called at the start of a new level, after the entities have been received and created
	virtual void LevelInitPostEntity( ) = 0;

	// 007:
	// Called at the end of a level
	virtual void LevelShutdown( void ) = 0;
	
	virtual void _UNKOWN_008(void) = 0;
	virtual void _UNKOWN_009(void) = 0;
	virtual void _UNKOWN_010(void) = 0;
	virtual void _UNKOWN_011(void) = 0;
	virtual void _UNKOWN_012(void) = 0;
	virtual void _UNKOWN_013(void) = 0;
	virtual void _UNKOWN_014(void) = 0;
	virtual void _UNKOWN_015(void) = 0;
	virtual void _UNKOWN_016(void) = 0;
	virtual void _UNKOWN_017(void) = 0;
	virtual void _UNKOWN_018(void) = 0;
	virtual void _UNKOWN_019(void) = 0;
	virtual void _UNKOWN_020(void) = 0;
	virtual void _UNKOWN_021(void) = 0;
	virtual void _UNKOWN_022(void) = 0;
	virtual void _UNKOWN_023(void) = 0;
	virtual void _UNKOWN_024(void) = 0;
	virtual void _UNKOWN_025(void) = 0;

	// 026:
	virtual void View_Render( vrect_t_csgo *rect )= 0;

	// 027:
	virtual void RenderView( const CViewSetup_csgo &view, int nClearFlags, int whatToDraw )= 0;

	virtual void _UNKOWN_028(void)= 0;
	virtual void _UNKOWN_029(void) = 0;
	virtual void _UNKOWN_030(void) = 0;
	virtual void _UNKOWN_031(void) = 0;
	virtual void _UNKOWN_032(void) = 0;
	virtual void _UNKOWN_033(void) = 0;
	virtual void _UNKOWN_034(void) = 0;
	virtual void _UNKOWN_035(void) = 0;
	virtual void _UNKOWN_036(void) = 0;
	virtual void _UNKOWN_037(void) = 0;
	virtual void _UNKOWN_038(void) = 0;
	virtual void _UNKOWN_039(void) = 0;
	virtual void _UNKOWN_040(void) = 0;
	virtual void _UNKOWN_041(void) = 0;
	virtual void _UNKOWN_042(void) = 0;
	virtual void _UNKOWN_043(void) = 0;
	virtual void _UNKOWN_044(void) = 0;
	virtual void _UNKOWN_045(void) = 0;
	virtual void _UNKOWN_046(void) = 0;
	virtual void _UNKOWN_047(void) = 0;
	virtual void _UNKOWN_048(void) = 0;
	virtual void _UNKOWN_049(void) = 0;
	virtual void _UNKOWN_050(void) = 0;
	virtual void _UNKOWN_051(void) = 0;
	virtual void _UNKOWN_052(void) = 0;
	virtual void _UNKOWN_053(void) = 0;
	virtual void _UNKOWN_054(void) = 0;
	virtual void _UNKOWN_055(void) = 0;
	virtual void _UNKOWN_056(void) = 0;
	virtual void _UNKOWN_057(void) = 0;
	virtual void _UNKOWN_058(void) = 0;
	virtual void _UNKOWN_059(void) = 0;
	virtual void _UNKOWN_060(void) = 0;
	virtual void _UNKOWN_061(void) = 0;
	virtual void _UNKOWN_062(void) = 0;

	// 063:
	virtual void _UNUSED_GetScreenWidth(void) = 0;
	
	// 064:
	virtual void _UNUSED_GetScreenHeight(void) = 0;
	
	// 065:
	virtual void WriteSaveGameScreenshotOfSize( const char *pFilename, int width, int height, bool bCreatePowerOf2Padded = false, bool bWriteVTF = false ) = 0;
	
	virtual void _UNKOWN_066(void) = 0;
	virtual void _UNKOWN_067(void) = 0;
	virtual void _UNKOWN_068(void) = 0;
	virtual void _UNKOWN_069(void) = 0;
	virtual void _UNKOWN_070(void) = 0;
	virtual void _UNKOWN_071(void) = 0;
	virtual void _UNKOWN_072(void) = 0;
	virtual void _UNKOWN_073(void) = 0;
	virtual void _UNKOWN_074(void) = 0;
	virtual void _UNKOWN_075(void) = 0;
	virtual void _UNKOWN_076(void) = 0;
	virtual void _UNKOWN_077(void) = 0;
	virtual void _UNKOWN_078(void) = 0;
	virtual void _UNKOWN_079(void) = 0;
	virtual void _UNKOWN_080(void) = 0;
	virtual void _UNKOWN_081(void) = 0;
	virtual void _UNKOWN_082(void) = 0;
	virtual void _UNKOWN_083(void) = 0;
	virtual void _UNKOWN_084(void) = 0;
	virtual void _UNKOWN_085(void) = 0;
	virtual void _UNKOWN_086(void) = 0;
	virtual void _UNKOWN_087(void) = 0;
	virtual void _UNKOWN_088(void) = 0;
	virtual void _UNKOWN_089(void) = 0;
	virtual void _UNKOWN_090(void) = 0;
	virtual void _UNKOWN_091(void) = 0;
	virtual void _UNKOWN_092(void) = 0;
	virtual void _UNKOWN_093(void) = 0;
	virtual void _UNKOWN_094(void) = 0;
	virtual void _UNKOWN_095(void) = 0;
	virtual void _UNKOWN_096(void) = 0;
	virtual void _UNKOWN_097(void) = 0;
	virtual void _UNKOWN_098(void) = 0;
	virtual void _UNKOWN_099(void) = 0;
	virtual void _UNKOWN_100(void) = 0;
	virtual void _UNKOWN_101(void) = 0;
	virtual void _UNKOWN_102(void) = 0;
	virtual void _UNKOWN_103(void) = 0;
	virtual void _UNKOWN_104(void) = 0;
	virtual void _UNKOWN_105(void) = 0;
	virtual void _UNKOWN_106(void) = 0;
	virtual void _UNKOWN_107(void) = 0;
	virtual void _UNKOWN_108(void) = 0;
	virtual void _UNKOWN_109(void) = 0;
	virtual void _UNKOWN_110(void) = 0;
	
	// and a few more to be save from updates:

	virtual void _UNKOWN_111(void) = 0;
	virtual void _UNKOWN_112(void) = 0;
	virtual void _UNKOWN_113(void) = 0;
	virtual void _UNKOWN_114(void) = 0;
	virtual void _UNKOWN_115(void) = 0;
	virtual void _UNKOWN_116(void) = 0;
	virtual void _UNKOWN_117(void) = 0;
	virtual void _UNKOWN_118(void) = 0;
	virtual void _UNKOWN_119(void) = 0;
	virtual void _UNKOWN_120(void) = 0;
	virtual void _UNKOWN_121(void) = 0;
	virtual void _UNKOWN_122(void) = 0;
	virtual void _UNKOWN_123(void) = 0;
	virtual void _UNKOWN_124(void) = 0;
	virtual void _UNKOWN_125(void) = 0;
	virtual void _UNKOWN_126(void) = 0;
	virtual void _UNKOWN_127(void) = 0;
	virtual void _UNKOWN_128(void) = 0;
	virtual void _UNKOWN_129(void) = 0;
	virtual void _UNKOWN_130(void) = 0;
};

// IVRenderView_csgo ///////////////////////////////////////////////////////////

#define VENGINE_RENDERVIEW_INTERFACE_VERSION_CSGO_013 "VEngineRenderView013"

class IVRenderView_csgo abstract
{
public:
	virtual void _UNKOWN_000(void) = 0;
	virtual void _UNKOWN_001(void) = 0;
	virtual void _UNKOWN_002(void) = 0;
	virtual void _UNKOWN_003(void) = 0;
	
	// 004:
	virtual void SetBlend( float blend ) = 0;

	virtual void _UNKOWN_005(void) = 0;

	// 006:
	virtual void SetColorModulation( float const* blend ) = 0;

	virtual void _UNKOWN_007(void) = 0;
	virtual void _UNKOWN_008(void) = 0;
	virtual void _UNKOWN_009(void) = 0;
	virtual void _UNKOWN_010(void) = 0;
	virtual void _UNKOWN_011(void) = 0;
	virtual void _UNKOWN_012(void) = 0;
	virtual void _UNKOWN_013(void) = 0;
	virtual void _UNKOWN_014(void) = 0;
	virtual void _UNKOWN_015(void) = 0;
	virtual void _UNKOWN_016(void) = 0;
	virtual void _UNKOWN_017(void) = 0;
	virtual void _UNKOWN_018(void) = 0;
	virtual void _UNKOWN_019(void) = 0;
	virtual void _UNKOWN_020(void) = 0;
	virtual void _UNKOWN_021(void) = 0;
	virtual void _UNKOWN_022(void) = 0;
	virtual void _UNKOWN_023(void) = 0;
	virtual void _UNKOWN_024(void) = 0;
	virtual void _UNKOWN_025(void) = 0;
	virtual void _UNKOWN_026(void) = 0;
	virtual void _UNKOWN_027(void) = 0;
	virtual void _UNKOWN_028(void) = 0;
	virtual void _UNKOWN_029(void) = 0;
	virtual void _UNKOWN_030(void) = 0;
	virtual void _UNKOWN_031(void) = 0;
	virtual void _UNKOWN_032(void) = 0;
	virtual void _UNKOWN_033(void) = 0;
	virtual void _UNKOWN_034(void) = 0;
	virtual void _UNKOWN_035(void) = 0;
	virtual void _UNKOWN_036(void) = 0;
	virtual void _UNKOWN_037(void) = 0;
	virtual void _UNKOWN_038(void) = 0;
	virtual void _UNKOWN_039(void) = 0;
	virtual void _UNKOWN_040(void) = 0;
	virtual void _UNKOWN_041(void) = 0;
	virtual void _UNKOWN_042(void) = 0;
	virtual void _UNKOWN_043(void) = 0;
	virtual void _UNKOWN_044(void) = 0;
	virtual void _UNKOWN_045(void) = 0;
	virtual void _UNKOWN_046(void) = 0;
	virtual void _UNKOWN_047(void) = 0;
	virtual void _UNKOWN_048(void) = 0;
	virtual void _UNKOWN_049(void) = 0;
	virtual void _UNKOWN_050(void) = 0;
	virtual void _UNKOWN_051(void) = 0;
	virtual void _UNKOWN_052(void) = 0;
	virtual void _UNKOWN_053(void) = 0;
	virtual void _UNKOWN_054(void) = 0;
	virtual void _UNKOWN_055(void) = 0;
	virtual void _UNKOWN_056(void) = 0;
	virtual void _UNKOWN_057(void) = 0;
	virtual void _UNKOWN_058(void) = 0;
	virtual void _UNKOWN_059(void) = 0;
};


// IFileSystem_csgo ////////////////////////////////////////////////////////////

enum FilesystemMountRetval_t_csgo
{
	FILESYSTEM_MOUNT_OK = 0,
	FILESYSTEM_MOUNT_FAILED,
};

enum SearchPathAdd_t_csgo
{
	PATH_ADD_TO_HEAD,			// First path searched
	PATH_ADD_TO_TAIL,			// Last path searched
	PATH_ADD_TO_TAIL_ATINDEX,	// First path searched
};

class IBaseFileSystem_csgo abstract
{
public:
	void _UNKNOWN_IBaseFileSystem_csgo_Dummy(); // just to make sure that we get a vtable ptr reserved for this class.

	// ...
	// more we don't care about.
};

#define FILESYSTEM_INTERFACE_VERSION_CSGO_017 "VFileSystem017"

class IFileSystem_csgo abstract : public IAppSystem_csgo, public IBaseFileSystem_csgo
{
public:
	//--------------------------------------------------------
	// Steam operations
	//--------------------------------------------------------

	virtual bool			IsSteam() const = 0;

	// Supplying an extra app id will mount this app in addition 
	// to the one specified in the environment variable "steamappid"
	// 
	// If nExtraAppId is < -1, then it will mount that app ID only.
	// (Was needed by the dedicated server b/c the "SteamAppId" env var only gets passed to steam.dll
	// at load time, so the dedicated couldn't pass it in that way).
	virtual	FilesystemMountRetval_t_csgo MountSteamContent( int nExtraAppId = -1 ) = 0;

	//--------------------------------------------------------
	// Search path manipulation
	//--------------------------------------------------------

	// Add paths in priority order (mod dir, game dir, ....)
	// If one or more .pak files are in the specified directory, then they are
	//  added after the file system path
	// If the path is the relative path to a .bsp file, then any previous .bsp file 
	//  override is cleared and the current .bsp is searched for an embedded PAK file
	//  and this file becomes the highest priority search path ( i.e., it's looked at first
	//   even before the mod's file system path ).
	virtual void			AddSearchPath( const char *pPath, const char *pathID, SearchPathAdd_t_csgo addType = PATH_ADD_TO_TAIL ) = 0;
	virtual bool			RemoveSearchPath( const char *pPath, const char *pathID = 0 ) = 0;

	// Remove all search paths (including write path?)
	virtual void			RemoveAllSearchPaths( void ) = 0;

	// Remove search paths associated with a given pathID
	virtual void			RemoveSearchPaths( const char *szPathID ) = 0;

	// This is for optimization. If you mark a path ID as "by request only", then files inside it
	// will only be accessed if the path ID is specifically requested. Otherwise, it will be ignored.
	// If there are currently no search paths with the specified path ID, then it will still
	// remember it in case you add search paths with this path ID.
	virtual void			MarkPathIDByRequestOnly( const char *pPathID, bool bRequestOnly ) = 0;

	// ...
	// more we don't care about.
};
