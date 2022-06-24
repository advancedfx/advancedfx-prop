#pragma once

// Based on Source engine SDK:
// Copyright (c) 1996-2005, Valve Corporation, All rights reserved

// Description:
// Interface definitions for accessing the Source engine.

#include <shared/AfxConsole.h>
#include "SourceSdkShared.h"

#include "csgo/sdk_src/public/appframework/IAppSystem.h"

#include <float.h>

typedef advancedfx::ICommandArgs IWrpCommandArgs;
typedef advancedfx::CommandCallback_t WrpCommandCallback;
class WrpConCommands;
class WrpConVarRef;


typedef char tier0_char;

typedef void (*Tier0MsgFn)( const tier0_char* pMsg, ... );
typedef void (*Tier0DevMsgFn)( int level, const tier0_char *pMsg, ... );

// debug and message fns, available after tier0.dll has been loaded:
extern Tier0MsgFn Tier0_Msg;
extern Tier0MsgFn Tier0_Warning;
extern Tier0MsgFn Tier0_Error;

extern Tier0DevMsgFn Tier0_DevMsg;
extern Tier0DevMsgFn Tier0_DevWarning;

namespace SOURCESDK {

#ifndef NULL
#define NULL 0
#endif

#define FORCEINLINE __forceinline
#define FORCEINLINE_CVAR FORCEINLINE

#define Assert(condition)
#define CHECK_VALID( _v)

#define Q_memcpy strncpy

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


// Rendering related

class CGlobalVarsBase;

// Command / Cvar related:

// we hack around a bit here:
typedef void (*WrpCommandCallback)(::IWrpCommandArgs * args);

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

/// <remarks> DO NOT CHANGE WITHOUT KNOWING WHAT YOU DO, DIRECTLY ACCESSED BY SOURCE ENGINE! </remarks>
/// <comments> I guess if Valve used a non determisitic C++ compiler they would
///		be screwed when sharing such classes among various compile units.
///		this also means we are screwed too easily when using a different compiler.
///		</comments>
class ConCommandBase_003
{
	friend ::WrpConCommands; // ugly hack, just like Valve did

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
	friend ::WrpConCommands; // ugly hack, just like Valve did

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


// IVEngineClient_012 //////////////////////////////////////////////////////////

#define VENGINE_CLIENT_INTERFACE_VERSION_012		"VEngineClient012"

/// <comments>
///		Supported by: Counter-Strike Source
///		</comments>
class IVEngineClient_012 abstract
{
public:
	virtual void _UNUSED_GetIntersectingSurfaces(void) = 0;
	virtual void _UNUSED_GetLightForPoint(void) = 0;
	virtual void _UNUSED_TraceLineMaterialAndLighting(void) = 0;
	virtual void _UNUSED_ParseFile(void) = 0;
	virtual void _UNUSED_CopyFile(void) = 0;

	// Gets the dimensions of the game window
	virtual void				GetScreenSize(int& width, int& height) = 0;

	// Forwards szCmdString to the server, sent reliably if bReliable is set
	virtual void				ServerCmd(const char *szCmdString, bool bReliable = true) = 0;
	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	virtual void				ClientCmd(const char *szCmdString) = 0;

	virtual void _UNUSED_GetPlayerInfo(void) = 0;
	virtual void _UNUSED_GetPlayerForUserID(void) = 0;
	virtual void _UNUSED_TextMessageGet(void) = 0;

	// Returns true if the console is visible
	virtual bool				Con_IsVisible(void) = 0;

	// Get the entity index of the local player
	virtual int					GetLocalPlayer(void) = 0;

	virtual void _UNUSED_LoadModel(void) = 0;

	// Get accurate, sub-frame clock ( profiling use )
	virtual float				Time(void) = 0;

	// Get the exact server timesstamp ( server time ) from the last message received from the server
	virtual float				GetLastTimeStamp(void) = 0;

	virtual void _UNUSED_GetSentence(void) = 0;
	virtual void _UNUSED_GetSentenceLength(void) = 0;
	virtual void _UNUSED_IsStreaming(void) = 0;

	// Copy current view orientation into va
	virtual void				GetViewAngles(QAngle& va) = 0;
	// Set current view orientation from va
	virtual void				SetViewAngles(QAngle& va) = 0;

	// Retrieve the current game's maxclients setting
	virtual int					GetMaxClients(void) = 0;

	virtual void _UNUSED_Key_Event(void) = 0;
	virtual void _UNUSED_Key_LookupBinding(void) = 0;
	virtual void _UNUSED_StartKeyTrapMode(void) = 0;
	virtual void _UNUSED_CheckDoneKeyTrapping(void) = 0;

	// Returns true if the player is fully connected and active in game (i.e, not still loading)
	virtual bool				IsInGame(void) = 0;
	// Returns true if the player is connected, but not necessarily active in game (could still be loading)
	virtual bool				IsConnected(void) = 0;
	// Returns true if the loading plaque should be drawn
	virtual bool				IsDrawingLoadingImage(void) = 0;

	// Prints the formatted string to the notification area of the screen ( down the right hand edge
	//  numbered lines starting at position 0
	virtual void				Con_NPrintf(int pos, const char *fmt, ...) = 0;

	virtual void _UNUSED_Con_NXPrintf(void) = 0;

	// During ConCommand processing functions, use this function to get the total # of tokens passed to the command parser
	virtual int					Cmd_Argc(void) = 0;
	// During ConCommand processing, this API is used to access each argument passed to the parser
	virtual const char			*Cmd_Argv(int arg) = 0;

	virtual void _UNUSED_IsBoxVisible(void) = 0;
	virtual void _UNUSED_IsBoxInViewCluster(void) = 0;
	virtual void _UNUSED_CullBox(void) = 0;
	virtual void _UNUSED_Sound_ExtraUpdate(void) = 0;

	// Get the current game directory ( e.g., hl2, tf2, cstrike, hl1 )
	virtual const char			*GetGameDirectory(void) = 0;

	// Get access to the world to screen transformation matrix
	virtual const VMatrix& 		WorldToScreenMatrix() = 0;

	// Get the matrix to move a point from world space into view space
	// (translate and rotate so the camera is at the origin looking down X).
	virtual const VMatrix& 		WorldToViewMatrix() = 0;

	virtual void _UNUSED_GameLumpVersion(void) = 0;
	virtual void _UNUSED_GameLumpSize(void) = 0;
	virtual void _UNUSED_LoadGameLump(void) = 0;
	virtual void _UNUSED_LevelLeafCount(void) = 0;
	virtual void _UNUSED_GetBSPTreeQuery(void) = 0;
	virtual void _UNUSED_LinearToGamma(void) = 0;
	virtual void _UNUSED_LightStyleValue(void) = 0;
	virtual void _UNUSED_ComputeDynamicLighting(void) = 0;
	virtual void _UNUSED_GetAmbientLightColor(void) = 0;
	virtual void _UNUSED_GetDXSupportLevel(void) = 0;
	virtual void _UNUSED_SupportsHDR(void) = 0;
	virtual void _UNUSED_Mat_Stub(void) = 0;

	// Get the name of the current map
	virtual char const	*GetLevelName(void) = 0;
#ifndef _XBOX
	virtual void _UNUSED_GetVoiceTweakAPI(void) = 0;
#endif
	// Tell engine stats gathering system that the rendering frame is beginning/ending
	virtual void		EngineStats_BeginFrame(void) = 0;
	virtual void		EngineStats_EndFrame(void) = 0;

	virtual void _UNUSED_FireEvents(void) = 0;
	virtual void _UNUSED_GetLeavesArea(void) = 0;
	virtual void _UNUSED_DoesBoxTouchAreaFrustum(void) = 0;
	virtual void _UNUSED_SetHearingOrigin(void) = 0;
	virtual void _UNUSED_SentenceGroupPick(void) = 0;
	virtual void _UNUSED_SentenceGroupPickSequential(void) = 0;
	virtual void _UNUSED_SentenceIndexFromName(void) = 0;
	virtual void _UNUSED_SentenceNameFromIndex(void) = 0;
	virtual void _UNUSED_SentenceGroupIndexFromName(void) = 0;
	virtual void _UNUSED_SentenceGroupNameFromIndex(void) = 0;
	virtual void _UNUSED_SentenceLength(void) = 0;
	virtual void _UNUSED_ComputeLighting(void) = 0;
	virtual void _UNUSED_ActivateOccluder(void) = 0;
	virtual void _UNUSED_IsOccluded(void) = 0;
	virtual void _UNUSED_SaveAllocMemory(void) = 0;
	virtual void _UNUSED_SaveFreeMemory(void) = 0;
	virtual void _UNUSED_GetNetChannelInfo(void) = 0;
	virtual void _UNUSED_DebugDrawPhysCollide(void) = 0;
	virtual void _UNUSED_CheckPoint(void) = 0;
	virtual void _UNUSED_DrawPortals(void) = 0;

	// Determine whether the client is playing back or recording a demo
	virtual bool		IsPlayingDemo(void) = 0;
	virtual bool		IsRecordingDemo(void) = 0;
	virtual bool		IsPlayingTimeDemo(void) = 0;
	// Is the game paused?
	virtual bool		IsPaused(void) = 0;
	// Is the game currently taking a screenshot?
	virtual bool		IsTakingScreenshot(void) = 0;
	// Is this a HLTV broadcast ?
	virtual bool		IsHLTV(void) = 0;
	// is this level loaded as just the background to the main menu? (active, but unplayable)
	virtual bool		IsLevelMainMenuBackground(void) = 0;
	// returns the name of the background level
	virtual void		GetMainMenuBackgroundName(char *dest, int destlen) = 0;

	virtual void _UNUSED_SetOcclusionParameters(void) = 0;
	virtual void _UNUSED_GetUILanguage(void) = 0;
	virtual void _UNUSED_IsSkyboxVisibleFromPoint(void) = 0;
	virtual void _UNUSED_GetMapEntitiesString(void) = 0;

	// Is the engine in map edit mode ?
	virtual bool		IsInEditMode(void) = 0;

	// current screen aspect ratio (eg. 4.0f/3.0f, 16.0f/9.0f)
	virtual float		GetScreenAspectRatio() = 0;

	virtual void _UNUSED_SteamRefreshLogin(void) = 0;
	virtual void _UNUSED_SteamProcessCall(void) = 0;

	// allow other modules to know about engine versioning (one use is a proxy for network compatability)
	virtual unsigned int	GetEngineBuildNumber() = 0; // engines build
	virtual const char *	GetProductVersionString() = 0; // mods version number (steam.inf)

	virtual void _UNUSED_GetLastPressedEngineKey(void) = 0;
	virtual void _UNUSED_GrabPreColorCorrectedFrame(void) = 0;

	virtual bool			IsHammerRunning() const = 0;

	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// And then executes the command string immediately (vs ClientCmd() which executes in the next frame)
	virtual void			ExecuteClientCmd(const char *szCmdString) = 0;

	virtual void _UNUSED_MapHasHDRLighting(void) = 0;

	virtual int	GetAppID() = 0;

	virtual void _UNUSED_GetLightForPointFast(void) = 0;
};

// IVEngineClient_013 //////////////////////////////////////////////////////////

#define VENGINE_CLIENT_INTERFACE_VERSION_013 "VEngineClient013"
#define VENGINE_CLIENT_INTERFACE_VERSION_014 "VEngineClient014"
#define VENGINE_CLIENT_INTERFACE_VERSION_015 "VEngineClient015"

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

	// Get the entity index of the local player
	virtual int					GetLocalPlayer(void) = 0;

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
	virtual int	GetLevelVersion( void ) = 0;

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

	// new in Source SDK 2013:
	virtual int			GetDemoRecordingTick( void ) = 0; 

	// new in Source SDK 2013:
 	virtual int			GetDemoPlaybackTick( void ) = 0; 

	// new in Source SDK 2013:
 	virtual int			GetDemoPlaybackStartTick( void ) = 0; 

	// new in Source SDK 2013:
 	virtual float		GetDemoPlaybackTimeScale( void ) = 0; 

	// new in Source SDK 2013:
 	virtual int			GetDemoPlaybackTotalTicks( void ) = 0; 


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

	// new in Source SDK 2013:
	// Get video modes 
	virtual void _UNUSED_GetVideoModes(void) = 0; 

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

// IVEngineClient_014_csgo /////////////////////////////////////////////////////

#define SOURCESDK_CSGO_MAX_PLAYER_NAME_LENGTH	128
#define SOURCESDK_CSGO_SIGNED_GUID_LEN 32
#define SOURCESDK_CSGO_MAX_CUSTOM_FILES	4

typedef unsigned long CRC32_t;
typedef unsigned __int64 uint64;

typedef struct player_info_s_csgo
{
	//DECLARE_BYTESWAP_DATADESC();
	// version for future compatibility
	uint64			version;
	// network xuid
	uint64			xuid;
	// scoreboard information
	char			name[SOURCESDK_CSGO_MAX_PLAYER_NAME_LENGTH];
	// local server user ID, unique while server is running
	int				userID;
	// global unique player identifer
	char			guid[SOURCESDK_CSGO_SIGNED_GUID_LEN + 1];
	// friends identification number
	uint32			friendsID;
	// friends name
	char			friendsName[SOURCESDK_CSGO_MAX_PLAYER_NAME_LENGTH];
	// true, if player is a bot controlled by game.dll
	bool			fakeplayer;
	// true if player is the HLTV proxy
	bool			ishltv;
	// custom files CRC for this player
	CRC32_t			customFiles[SOURCESDK_CSGO_MAX_CUSTOM_FILES];
	// this counter increases each time the server downloaded a new file
	unsigned char	filesDownloaded;
} player_info_t_csgo;


#define VENGINE_CLIENT_INTERFACE_VERSION_014_CSGO "VEngineClient014"

/// <comments>
///		Supported by: CS:GO
///		</comments>
class IVEngineClient_014_csgo abstract
{
public:
	virtual void _UNUSED_GetIntersectingSurfaces(void)=0; // :0
	virtual void _UNUSED_GetLightForPoint(void)=0; // :1
	virtual void _UNUSED_TraceLineMaterialAndLighting(void)=0; // :2
	virtual void _UNUSED_ParseFile(void)=0; // :3
	virtual void _UNUSED_CopyFile(void)=0; // :4

	// Gets the dimensions of the game window
	virtual void				GetScreenSize( int& width, int& height ) = 0; // :5

	// Forwards szCmdString to the server, sent reliably if bReliable is set
	virtual void				ServerCmd( const char *szCmdString, bool bReliable = true ) = 0; // :6
	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// Note: Calls to this are checked against FCVAR_CLIENTCMD_CAN_EXECUTE (if that bit is not set, then this function can't change it).
	//       Call ClientCmd_Unrestricted to have access to FCVAR_CLIENTCMD_CAN_EXECUTE vars.
	virtual void				ClientCmd( const char *szCmdString ) = 0; // :7

	// Fill in the player info structure for the specified player index (name, model, etc.)
	virtual bool				GetPlayerInfo(int ent_num, player_info_t_csgo *pinfo) = 0; //:8

	// Retrieve the player entity number for a specified userID
	virtual int					GetPlayerForUserID(int userID) = 0; //:009

	virtual void _UNUSED_TextMessageGet(void)=0; // :10

	// Returns true if the console is visible
	virtual bool				Con_IsVisible( void ) = 0; // :11

	// Get the entity index of the local player
	virtual int					GetLocalPlayer(void) = 0;

	virtual void _UNUSED_LoadModel(void)=0; // :13

	// Get the exact server timesstamp ( server time ) from the last message received from the server
	virtual float				GetLastTimeStamp( void ) = 0;  // :14

	virtual void _UNUSED_GetSentence(void)=0; // :15
	virtual void _UNUSED_GetSentenceLength(void)=0; // :16
	virtual void _UNUSED_IsStreaming(void)=0; // :17

	// Copy current view orientation into va
	virtual void				GetViewAngles( QAngle& va ) = 0; // :18
	// Set current view orientation from va
	virtual void				SetViewAngles( QAngle& va ) = 0; // :19
	
	// Retrieve the current game's maxclients setting
	virtual int					GetMaxClients( void ) = 0; // :20

	virtual void _UNUSED_Key_LookupBinding(void)=0; // :21
	virtual void _UNUSED_Key_BindingForKey(void)=0; // :22

	virtual void _UNKOWN_023(void) = 0; // :23

	virtual void _UNUSED_StartKeyTrapMode(void)=0; // :24
	virtual void _UNUSED_CheckDoneKeyTrapping(void)=0; // :25

	// Returns true if the player is fully connected and active in game (i.e, not still loading)
	virtual bool				IsInGame( void ) = 0; // :26
	// Returns true if the player is connected, but not necessarily active in game (could still be loading)
	virtual bool				IsConnected( void ) = 0; // :27
	// Returns true if the loading plaque should be drawn
	virtual bool				IsDrawingLoadingImage( void ) = 0; // :28

	// new in csgo:
	virtual void				HideLoadingPlaque( void ) = 0; // :29

	// Prints the formatted string to the notification area of the screen ( down the right hand edge
	//  numbered lines starting at position 0
	virtual void				Con_NPrintf( int pos, const char *fmt, ... ) = 0; // :30
	
	virtual void _UNUSED_Con_NXPrintf(void)=0; // :31
	virtual void _UNUSED_IsBoxVisible(void)=0; // :32
	virtual void _UNUSED_IsBoxInViewCluster(void)=0; // :33
	virtual void _UNUSED_CullBox(void)=0; // :34
	virtual void _UNUSED_Sound_ExtraUpdate(void)=0; // :35

	// Get the current game directory ( e.g., hl2, tf2, cstrike, hl1 )
	virtual const char			*GetGameDirectory( void ) = 0; // :36

	// Get access to the world to screen transformation matrix
	virtual const VMatrix& 		WorldToScreenMatrix() = 0; // :37
	
	// Get the matrix to move a point from world space into view space
	// (translate and rotate so the camera is at the origin looking down X).
	virtual const VMatrix& 		WorldToViewMatrix() = 0; // :38

	virtual void _UNUSED_GameLumpVersion(void)=0; // :39
	virtual void _UNUSED_GameLumpSize(void)=0; // :40
	virtual void _UNUSED_LoadGameLump(void)=0; // :41
	virtual void _UNUSED_LevelLeafCount(void)=0; // :42
	virtual void _UNUSED_GetBSPTreeQuery(void)=0; // :43
	virtual void _UNUSED_LinearToGamma(void)=0; // :44
	virtual void _UNUSED_LightStyleValue(void)=0; // :45
	virtual void _UNUSED_ComputeDynamicLighting(void)=0; // :46
	virtual void _UNUSED_GetAmbientLightColor(void)=0; // :47
	virtual void _UNUSED_GetDXSupportLevel(void)=0; // :48
	virtual void _UNUSED_SupportsHDR(void)=0; // :49
	virtual void _UNUSED_Mat_Stub(void)=0; // :50
	virtual void _UNUSED_GetChapterName(void)=0; // :51

	virtual char const	*GetLevelName( void ) = 0; // :52

	virtual char const	*GetLevelNameShort(void) = 0; // :53

	// New in source SDK 2013
	virtual int GetLevelVersion( void ) = 0; // :54

#if !defined( NO_VOICE )
	virtual void _UNUSED_GetVoiceTweakAPI(void)=0; // :55
#endif
	// Tell engine stats gathering system that the rendering frame is beginning/ending
	virtual void		EngineStats_BeginFrame( void ) = 0; // :56
	virtual void		EngineStats_EndFrame( void ) = 0; // :57

	virtual void _UNKOWN_058(void); // :58
	
	virtual void _UNUSED_FireEvents(void)=0; // :59
	virtual void _UNUSED_GetLeavesArea(void)=0; // :60
	virtual void _UNUSED_DoesBoxTouchAreaFrustum(void)=0; // :61
	
	virtual void _UNKOWN_062(void) = 0; // :62

	virtual void _UNKOWN_063(void) = 0; // :63
	virtual void _UNKOWN_064(void) = 0; // :64

	virtual void _UNUSED_SentenceGroupPick(void)=0; // :65
	virtual void _UNUSED_SentenceGroupPickSequential(void)=0; // :66
	virtual void _UNUSED_SentenceIndexFromName(void)=0; // :67
	virtual void _UNUSED_SentenceNameFromIndex(void)=0; // :68
	virtual void _UNUSED_SentenceGroupIndexFromName(void)=0; // :69
	virtual void _UNUSED_SentenceGroupNameFromIndex(void)=0; // :70
	virtual void _UNUSED_SentenceLength(void)=0; // :71
	virtual void _UNUSED_ComputeLighting(void)=0; // :72

	virtual void _UNKOWN_073(void); // :73
	virtual void _UNKOWN_074(void); // :74
	virtual void _UNKOWN_075(void); // :75

	virtual void _UNUSED_SaveAllocMemory(void)=0; // :76
	virtual void _UNUSED_SaveFreeMemory(void)=0; // :77
	virtual void _UNUSED_GetNetChannelInfo(void)=0; // :78
	virtual void _UNUSED_DebugDrawPhysCollide(void)=0; // :79
	virtual void _UNUSED_CheckPoint(void)=0; // :80
	virtual void _UNUSED_DrawPortals(void)=0; // :81

	// Determine whether the client is playing back or recording a demo
	virtual bool		IsPlayingDemo( void ) = 0; // :82
	virtual bool		IsRecordingDemo( void ) = 0; // :83
	virtual bool		IsPlayingTimeDemo( void ) = 0; // :84

	// new in csgo:
	virtual int			GetDemoRecordingTick( void ) = 0; // :85
	virtual int			GetDemoPlaybackTick( void ) = 0; // :86
	virtual int			GetDemoPlaybackStartTick( void ) = 0; // :87
	virtual float		GetDemoPlaybackTimeScale( void ) = 0; // :88
	virtual int			GetDemoPlaybackTotalTicks( void ) = 0; // :89

	// Is the game paused?
	virtual bool		IsPaused( void ) = 0; // :90

	// What is the game timescale multiplied with the host_timescale?
	virtual float GetTimescale( void ) const = 0; // :91

	// Is the game currently taking a screenshot?
	virtual bool		IsTakingScreenshot( void ) = 0; // :92
	
	// Is this a HLTV broadcast ?
	virtual bool		IsHLTV( void ) = 0; // :93
	
	// is this level loaded as just the background to the main menu? (active, but unplayable)
	virtual bool		IsLevelMainMenuBackground( void ) = 0; // :94
	
	// returns the name of the background level
	virtual void		GetMainMenuBackgroundName( char *dest, int destlen ) = 0; // :95

	virtual void _UNUSED_SetOcclusionParameters(void)=0; // :96

	virtual void _UNUSED_GetUILanguage(void)=0; // :97


	virtual void _UNUSED_IsSkyboxVisibleFromPoint(void)=0; // :98
	virtual void _UNUSED_GetMapEntitiesString(void)=0; // :99

	// Is the engine in map edit mode ?
	virtual bool		IsInEditMode( void ) = 0; // :100

	// current screen aspect ratio (eg. 4.0f/3.0f, 16.0f/9.0f)
	virtual float		GetScreenAspectRatio(int viewportWidth, int viewportHeight) = 0; // :101

	virtual void _UNUSED_REMOVED_SteamRefreshLogin(void)=0; // :102
	virtual void _UNUSED_REMOVED_SteamProcessCall(void)=0; // :103

	// allow other modules to know about engine versioning (one use is a proxy for network compatability)
	virtual unsigned int	GetEngineBuildNumber() = 0; // engines build // :104
	virtual const char *	GetProductVersionString() = 0; // mods version number (steam.inf) // :105

	virtual void _UNUSED_GrabPreColorCorrectedFrame(void)=0; // :106

	virtual bool			IsHammerRunning( ) const = 0; // :107

	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// And then executes the command string immediately (vs ClientCmd() which executes in the next frame)
	//
	// Note: this is NOT checked against the FCVAR_CLIENTCMD_CAN_EXECUTE vars.
	virtual void			ExecuteClientCmd( const char *szCmdString ) = 0; // :108


	virtual void _UNKOWN_109(void); // :109
	virtual void _UNKOWN_110(void); // :110

	virtual int	GetAppID() = 0; // :111

	virtual void _UNKOWN_112(void); // :112
	virtual void _UNKOWN_113(void); // :113

	// This version does NOT check against FCVAR_CLIENTCMD_CAN_EXECUTE.
	virtual void ClientCmd_Unrestricted( const char *szCmdString, int unknown = 1 ) = 0; // :114

	//
	// might be in-accurate:

	virtual void _UNUSED_SetRestrictServerCommands(void)=0;
	virtual void _UNUSED_SetRestrictClientCommands(void)=0;
	virtual void _UNUSED_SetOverlayBindProxy(void)=0;
	virtual void _UNUSED_CopyFrameBufferToMaterial(void)=0;
	virtual void _UNUSED_ReadConfiguration(void)=0;
	virtual void _UNUSED_SetAchievementMgr(void)=0;
	virtual void _UNUSED_GetAchievementMgr(void)=0;
	virtual void _UNUSED_MapLoadFailed(void)=0;
	virtual void _UNUSED_SetMapLoadFailed(void)=0;
	virtual void _UNUSED_IsLowViolence(void)=0;

	//
	// more we don't care about ...

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

// IBaseClientDll_018 //////////////////////////////////////////////////////////

#define CLIENT_DLL_INTERFACE_VERSION_018		"VClient018"

// IClientEngineTools //////////////////////////////////////////////////////////

typedef void * HTOOLHANDLE;
typedef void KeyValues_something;
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

	virtual void PostToolMessage( HTOOLHANDLE hEntity, KeyValues_something *msg ) = 0;

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

// IMaterial_csgo //////////////////////////////////////////////////////////////

class IMaterialVar_csgo;

class IMaterial_csgo;

typedef IMaterial_csgo IMaterialInternal_csgo;

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

	// 040:
	virtual int ShaderParamCount() const = 0;

	// 041:
	virtual IMaterialVar_csgo **GetShaderParams( void ) = 0;

	
	// 042:
	// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
	// the material can't be found.
	virtual bool IsErrorMaterial() const = 0;

	virtual void _UNKNOWN_043(void) = 0;
	virtual void _UNKNOWN_044(void) = 0;
	virtual void _UNKNOWN_045(void) = 0;
	virtual void _UNKNOWN_046(void) = 0;
	virtual void _UNKNOWN_047(void) = 0;
	virtual void _UNKNOWN_048(void) = 0;

	// 049:
	virtual const char * GetShaderName() const = 0;

	// 050:
	virtual void			DeleteIfUnreferenced() = 0;

	virtual void _UNKNOWN_051(void) = 0;
	virtual void _UNKNOWN_052(void) = 0;
	virtual void _UNKNOWN_053(void) = 0;
	virtual void _UNKNOWN_054(void) = 0;
	virtual void _UNKNOWN_055(void) = 0;

	// 056:
	virtual int Probably_GetReferenceCount(void) = 0;

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

	virtual IMaterialInternal_csgo * GetRealTimeVersion(void) = 0; //:098 // Internal only

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
typedef void MaterialLightingState_t_csgo;

class ITexture_csgo
{
public:
	virtual void _UNKNOWN_000(void) = 0;
	virtual void _UNKNOWN_001(void) = 0;
	virtual void _UNKNOWN_002(void) = 0;
	virtual void _UNKNOWN_003(void) = 0;
	virtual void _UNKNOWN_004(void) = 0;
	virtual void _UNKNOWN_005(void) = 0;
	virtual void _UNKNOWN_006(void) = 0;
	virtual void _UNKNOWN_007(void) = 0;
	virtual void _UNKNOWN_008(void) = 0;
	virtual void _UNKNOWN_009(void) = 0;

	// Methods associated with reference count
	virtual void IncrementReferenceCount( void ) = 0;
	virtual void DecrementReferenceCount( void ) = 0;

	// ...
	// more we don't care about;
};

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
	virtual int VertexCount() const = 0; // :000
	virtual VertexFormat_t_csgo GetVertexFormat() const = 0; // :001

	// Is this vertex buffer dynamic?
	virtual bool IsDynamic() const = 0; // :002

	// NOTE: For dynamic vertex buffers only!
	// Casts the memory of the dynamic vertex buffer to the appropriate type
	virtual void BeginCastBuffer( VertexFormat_t_csgo format ) = 0; // :003
	virtual void EndCastBuffer() = 0; // :004

	// Returns the number of vertices that can still be written into the buffer
	virtual int GetRoomRemaining() const = 0; // :005

	virtual bool Lock( int nVertexCount, bool bAppend, VertexDesc_t_csgo &desc ) = 0; // :006*
	virtual void Unlock( int nVertexCount, VertexDesc_t_csgo &desc ) = 0; // :007*

	// Spews the mesh data
	virtual void Spew( int nVertexCount, const VertexDesc_t_csgo &desc ) = 0;  // :008

	// Call this in debug mode to make sure our data is good.
	virtual void ValidateData( int nVertexCount, const VertexDesc_t_csgo & desc ) = 0; // :009

	virtual void _Unknown_10_IVertexBuffer_csgo(void) = 0; // :010
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

	//
	// Those are probably not required anymore, but better safe than sorry:

	virtual void _Unknown_13_IIndexBuffer_csgo(void * arg0) = 0;

	virtual bool _Unknown_14_IIndexBuffer_csgo(void) = 0;
};

class CPrimList_csgo;
class CMeshBuilder_csgo;
class ICachedPerFrameMeshData_csgo;

class IMesh_csgo abstract : public IVertexBuffer_csgo, public IIndexBuffer_csgo
{
public:
	// -----------------------------------

	// Draws the mesh
	virtual void Draw( int firstIndex = -1, int numIndices = 0 ) = 0; // :012

	virtual void SetColorMesh( IMesh_csgo *pColorMesh, int nVertexOffset ) = 0; // :013

	// Draw a list of (lists of) primitives. Batching your lists together that use
	// the same lightmap, material, vertex and index buffers with multipass shaders
	// can drastically reduce state-switching overhead.
	// NOTE: this only works with STATIC meshes.
	virtual void Draw( CPrimList_csgo *pLists, int nLists ) = 0; // :011

	// Copy verts and/or indices to a mesh builder. This only works for temp meshes!
	virtual void CopyToMeshBuilder( 
		int iStartVert,		// Which vertices to copy.
		int nVerts, 
		int iStartIndex,	// Which indices to copy.
		int nIndices, 
		int indexOffset,	// This is added to each index.
		CMeshBuilder_csgo &builder ) = 0; // :014

	// Spews the mesh data
	virtual void Spew( int numVerts, int numIndices, const MeshDesc_t_csgo &desc ) = 0; // :015

	// Call this in debug mode to make sure our data is good.
	virtual void ValidateData( int numVerts, int numIndices, const MeshDesc_t_csgo &desc ) = 0; // :016

	// New version
	// Locks/unlocks the mesh, providing space for numVerts and numIndices.
	// numIndices of -1 means don't lock the index buffer...
	virtual void LockMesh( int numVerts, int numIndices, MeshDesc_t_csgo &desc, MeshBuffersAllocationSettings_t_csgo *pSettings ) = 0; // :017
	virtual void ModifyBegin( int firstVertex, int numVerts, int firstIndex, int numIndices, MeshDesc_t_csgo& desc ) = 0; // :018
	virtual void ModifyEnd( MeshDesc_t_csgo& desc ) = 0; // :019
	virtual void UnlockMesh( int numVerts, int numIndices, MeshDesc_t_csgo &desc ) = 0; // :020

	virtual void ModifyBeginEx( bool bReadOnly, int firstVertex, int numVerts, int firstIndex, int numIndices, MeshDesc_t_csgo &desc ) = 0; // :021

	virtual void SetFlexMesh( IMesh_csgo *pMesh, int nVertexOffset ) = 0; //:022

	virtual void DisableFlexMesh() = 0; // :023

	virtual void MarkAsDrawn() = 0; // :024 // Not sure if to use it's safe, in some classes this does nothing actually.

	// NOTE: I chose to create this method strictly because it's 2 days to code lock
	// and I could use the DrawInstances technique without a larger code change
	// Draws the mesh w/ modulation.
	virtual void DrawModulated( const Vector4D_csgo &vecDiffuseModulation, int firstIndex = -1, int numIndices = 0 ) = 0; // :025

	virtual unsigned int ComputeMemoryUsed() = 0; // :026

	virtual void *AccessRawHardwareDataStream( uint8 nRawStreamIndex, uint32 numBytes, uint32 uiFlags, void *pvContext ) = 0; // :027

	virtual ICachedPerFrameMeshData_csgo *GetCachedPerFrameMeshData() = 0; // :028
	virtual void ReconstructFromCachedPerFrameMeshData( ICachedPerFrameMeshData_csgo *pData ) = 0; // :029
};

class IMeshEx_csgo abstract : public IMesh_csgo
{
public:
	//
	// Those are probably not required anymore, but better safe than sorry:

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

namespace CSGO {

class ICallQueue;

} // namespace CSGO {

struct MeshInstanceData_t_csgo
{
	int						m_nIndexOffset;
	int						m_nIndexCount;
	int						m_nBoneCount;
	MeshBoneRemap_t_csgo *		m_pBoneRemap;		// there are bone count of these, they index into pose to world
	matrix3x4_t	*			m_pPoseToWorld;	// transforms for the *entire* model, indexed into by m_pBoneIndex. Potentially more than bone count of these
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
	void * _UNKNOWN_0x50;
};

enum ImageFormat_csgo 
{ 
	IMAGE_FORMAT_UNKNOWN  = -1,
	IMAGE_FORMAT_RGBA8888 = 0, 
	IMAGE_FORMAT_ABGR8888, 
	IMAGE_FORMAT_RGB888, 
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565, 
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,	// NOTE: D3D9 does not have this format
	IMAGE_FORMAT_RGBA32323232F,
	IMAGE_FORMAT_RG1616F,
	IMAGE_FORMAT_RG3232F,
	IMAGE_FORMAT_RGBX8888,

	IMAGE_FORMAT_NULL,			// Dummy format which takes no video memory

	// Compressed normal map formats
	IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
	IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format

	IMAGE_FORMAT_RGBA1010102,	// 10 bit-per component render targets
	IMAGE_FORMAT_BGRA1010102,
	IMAGE_FORMAT_R16F,			// 16 bit FP format

	// Depth-stencil texture formats
	IMAGE_FORMAT_D16,
	IMAGE_FORMAT_D15S1,
	IMAGE_FORMAT_D32,
	IMAGE_FORMAT_D24S8,
	IMAGE_FORMAT_LINEAR_D24S8,
	IMAGE_FORMAT_D24X8,
	IMAGE_FORMAT_D24X4S4,
	IMAGE_FORMAT_D24FS8,
	IMAGE_FORMAT_D16_SHADOW,	// Specific formats for shadow mapping
	IMAGE_FORMAT_D24X8_SHADOW,	// Specific formats for shadow mapping

	// supporting these specific formats as non-tiled for procedural cpu access (360-specific)
	IMAGE_FORMAT_LINEAR_BGRX8888,
	IMAGE_FORMAT_LINEAR_RGBA8888,
	IMAGE_FORMAT_LINEAR_ABGR8888,
	IMAGE_FORMAT_LINEAR_ARGB8888,
	IMAGE_FORMAT_LINEAR_BGRA8888,
	IMAGE_FORMAT_LINEAR_RGB888,
	IMAGE_FORMAT_LINEAR_BGR888,
	IMAGE_FORMAT_LINEAR_BGRX5551,
	IMAGE_FORMAT_LINEAR_I8,
	IMAGE_FORMAT_LINEAR_RGBA16161616,

	IMAGE_FORMAT_LE_BGRX8888,
	IMAGE_FORMAT_LE_BGRA8888,

	NUM_IMAGE_FORMATS
};

enum MaterialMatrixMode_t_csgo
{
	MATERIAL_VIEW = 0,
	MATERIAL_PROJECTION,

	MATERIAL_MATRIX_UNUSED0,
	MATERIAL_MATRIX_UNUSED1,
	MATERIAL_MATRIX_UNUSED2,
	MATERIAL_MATRIX_UNUSED3,
	MATERIAL_MATRIX_UNUSED4,
	MATERIAL_MATRIX_UNUSED5,
	MATERIAL_MATRIX_UNUSED6,
	MATERIAL_MATRIX_UNUSED7,

	MATERIAL_MODEL,

	// Total number of matrices
	NUM_MATRIX_MODES = MATERIAL_MODEL + 1,
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
	
	// 012:
	virtual void ClearBuffers( bool bClearColor, bool bClearDepth, bool bClearStencil = false ) = 0;
	
	// 013:
	// read to a unsigned char rgb image. 
	virtual void ReadPixels( int x, int y, int width, int height, unsigned char *data, ImageFormat_csgo dstFormat, unsigned __int32 _unknown7 = 0) = 0; 

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

	virtual void				GetMatrix(MaterialMatrixMode_t_csgo matrixMode, VMatrix *matrix) = 0; //:032
	virtual void				LoadIdentity(void) = 0; //:033
	virtual void				Ortho(double left, double top, double right, double bottom, double zNear, double zFar) = 0; //:034
	virtual void				PerspectiveX(double fovx, double aspect, double zNear, double zFar) = 0; //:035
	virtual void				PickMatrix(int x, int y, int width, int height) = 0; //:036
	virtual void				Rotate(float angle, float x, float y, float z) = 0; //:037
	virtual void				Translate(float x, float y, float z) = 0; //:038
	virtual void				Scale(float x, float y, float z) = 0; //:039
	
	// Sets/gets the viewport
	virtual void				Viewport(int x, int y, int width, int height) = 0; //:040
	virtual void				GetViewport(int& x, int& y, int& width, int& height) const = 0;  //:041
	
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
	virtual IMeshEx_csgo* GetDynamicMesh( 
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
	virtual void _UNKNOWN_072(void) = 0; // SelectionMode?
	virtual void _UNKNOWN_073(void) = 0; // SelectionBuffer?
	virtual void _UNKNOWN_074(void) = 0; // ClearSelectionNames?
	virtual void _UNKNOWN_075(void) = 0; // LoadSelectionName?
	virtual void _UNKNOWN_076(void) = 0; // PushSelectionName?
	virtual void _UNKNOWN_077(void) = 0; // PopSelectionName?
	virtual void _UNKNOWN_078(void) = 0; // ClearColor3ub
	
	// 079:
	virtual void ClearColor4ub( unsigned char r, unsigned char g, unsigned char b, unsigned char a ) = 0;

	virtual void _UNKNOWN_080(void) = 0; // OverrideDepthEnable?
	
	// 081:
	virtual void DrawScreenSpaceQuad(IMaterial_csgo * pMaterial) = 0;

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
	virtual void _UNKNOWN_098(void) = 0; // GetFlashlightMode?
	virtual void _UNKNOWN_099(void) = 0; // IsCullingEnabledForSinglePassFlashlight?
	virtual void _UNKNOWN_100(void) = 0; // EnableCullingForSinglePassFlashlight?
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

	// 114:
	// This function performs a texture map from one texture map to the render destination, doing
	// all the necessary pixel/texel coordinate fix ups. fractional values can be used for the
	// src_texture coordinates to get linear sampling - integer values should produce 1:1 mappings
	// for non-scaled operations.
	virtual void DrawScreenSpaceRectangle( 
		IMaterial_csgo *pMaterial,
		int destx, int desty,
		int width, int height,
		float src_texture_x0, float src_texture_y0,			// which texel you want to appear at
		// destx/y
		float src_texture_x1, float src_texture_y1,			// which texel you want to appear at
		// destx+width-1, desty+height-1
		int src_texture_width, int src_texture_height,		// needed for fixup
		void *pClientRenderable = 0,
		int nXDice = 1,
		int nYDice = 1 )=0;

	virtual void _UNKNOWN_115(void) = 0;
	
	// 119:
	// This version will push the current rendertarget + current viewport onto the stack
	virtual void PushRenderTargetAndViewport( ) = 0;

	// 118:
	// This version will push a new rendertarget + a maximal viewport for that rendertarget onto the stack
	virtual void PushRenderTargetAndViewport( ITexture_csgo *pTexture ) = 0;

	// 117:
	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport( ITexture_csgo *pTexture, int nViewX, int nViewY, int nViewW, int nViewH ) = 0;

	// 116:
	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void PushRenderTargetAndViewport( ITexture_csgo *pTexture, ITexture_csgo *pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH ) = 0;

	// 120:
	// This will pop a rendertarget + viewport
	virtual void PopRenderTargetAndViewport( void ) = 0;

	virtual void _UNKNOWN_121(void) = 0; // BindLightmapTexture?
	virtual void _UNKNOWN_122(void) = 0; // CopyRenderTargetToTextureEx?
	virtual void _UNKNOWN_123(void) = 0;
	virtual void _UNKNOWN_124(void) = 0;
	virtual void _UNKNOWN_125(void) = 0;
	virtual void _UNKNOWN_126(void) = 0;
	virtual void _UNKNOWN_127(void) = 0;
	virtual void _UNKNOWN_128(void) = 0;
	virtual void _UNKNOWN_129(void) = 0;
	virtual void _UNKNOWN_130(void) = 0; 
	virtual void _UNKNOWN_131(void) = 0; // PushCustomClipPlane?
	virtual void _UNKNOWN_132(void) = 0; // PopCustomClipPlane?
	virtual void _UNKNOWN_133(void) = 0; // GetMaxToRender?
	virtual void _UNKNOWN_134(void) = 0; // GetMaxVerticesToRender?
	virtual void _UNKNOWN_135(void) = 0; // GetMaxIndicesToRender?
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

	// Raw access to the call queue, which can be NULL if not in a queued mode
	virtual SOURCESDK::CSGO::ICallQueue *GetCallQueue() = 0; //:151

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
	virtual void _UNKNOWN_167(void) = 0; // GetMorphAccumulatorTexCoord?
	virtual void _UNKNOWN_168(void) = 0;

	// 169:
	virtual IMeshEx_csgo* GetDynamicMeshEx(
		VertexFormat_t_csgo vertexFormat,
		bool buffered = true, 
		IMesh_csgo* pVertexOverride = 0,	
		IMesh_csgo* pIndexOverride = 0, 
		IMaterial_csgo *pAutoBind = 0
	) = 0;

	virtual void _UNKNOWN_170(void) = 0; // FogMaxDensity?
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
	virtual void _UNKNOWN_192(void) = 0;
	virtual void _UNKNOWN_193(void) = 0;

	// 194:
	virtual void DrawInstances( int nInstanceCount, const MeshInstanceData_t_csgo *pInstance ) = 0;

	virtual void _UNKNOWN_195(void) = 0; // OverrideAlphaWriteEnable?
	virtual void _UNKNOWN_196(void) = 0; // OverrideColorWriteEnable?
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
	
	virtual void UnkDrawVguiA(bool notInRenderView) = 0; // :213
	virtual void UnkDrawVguiB(bool notInRenderView) = 0; // :214

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
	virtual void _UNKNOWN_255(void) = 0;
	virtual void _UNKNOWN_256(void) = 0;
	virtual void _UNKNOWN_257(void) = 0;
	virtual void _UNKNOWN_258(void) = 0;
	virtual void _UNKNOWN_259(void) = 0;
};

// IMaterialSystem_csgo ////////////////////////////////////////////////////////

#define MATERIAL_SYSTEM_INTERFACE_VERSION_CSGO_80 "VMaterialSystem080"

typedef void * MaterialLock_t_csgo;

// NOTE: All size modes will force the render target to be smaller than or equal to
// the size of the framebuffer.
enum RenderTargetSizeMode_t_csgo
{
	RT_SIZE_NO_CHANGE=0,			// Only allowed for render targets that don't want a depth buffer
	// (because if they have a depth buffer, the render target must be less than or equal to the size of the framebuffer).
	RT_SIZE_DEFAULT=1,				// Don't play with the specified width and height other than making sure it fits in the framebuffer.
	RT_SIZE_PICMIP=2,				// Apply picmip to the render target's width and height.
	RT_SIZE_HDR=3,					// frame_buffer_width / 4
	RT_SIZE_FULL_FRAME_BUFFER=4,	// Same size as frame buffer, or next lower power of 2 if we can't do that.
	RT_SIZE_OFFSCREEN=5,			// Target of specified size, don't mess with dimensions
	RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP=6 // Same size as the frame buffer, rounded up if necessary for systems that can't do non-power of two textures.
};

enum MaterialRenderTargetDepth_t_csgo
{
	MATERIAL_RT_DEPTH_SHARED   = 0x0,
	MATERIAL_RT_DEPTH_SEPARATE = 0x1,
	MATERIAL_RT_DEPTH_NONE     = 0x2,
	MATERIAL_RT_DEPTH_ONLY	   = 0x3,
};

namespace CSGO
{
enum MaterialContextType_t
{
	MATERIAL_HARDWARE_CONTEXT,
	MATERIAL_QUEUED_CONTEXT,
	MATERIAL_NULL_CONTEXT
};

enum MaterialThreadMode_t
{
	MATERIAL_SINGLE_THREADED,
	MATERIAL_QUEUED_SINGLE_THREADED,
	MATERIAL_QUEUED_THREADED
};
}

class IMaterialSystem_csgo abstract : public SOURCESDK::CSGO::IAppSystem
{
public:
	virtual void _UNKNOWN_009(void) = 0; // Init
	virtual void _UNKNOWN_010(void) = 0; // SetShaderAPI
	virtual void _UNKNOWN_011(void) = 0; // SetAdapter
	virtual void _UNKNOWN_012(void) = 0; // ModInit
	virtual void _UNKNOWN_013(void) = 0; // ModShutdown
	virtual void _UNKNOWN_014(void) = 0;
	virtual SOURCESDK::CSGO::MaterialThreadMode_t GetThreadMode() = 0; //:015
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
	
	virtual void				BeginFrame(float frameTime) = 0; //:042
	virtual void				EndFrame() = 0; //:043

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
	virtual void _UNKNOWN_082(void) = 0; // ? ReloadMaterials
	virtual void _UNKNOWN_083(void) = 0; // CreateMaterial

	// 084:
	virtual IMaterial_csgo * FindMaterial( char const* pMaterialName, const char *pTextureGroupName, bool complain = true, const char *pComplainPrefix = 0 ) = 0;
	
	virtual void _UNKNOWN_085(void) = 0; // ?
	virtual void _UNKNOWN_086(void) = 0; // FirstMaterial
	virtual void _UNKNOWN_087(void) = 0; // NextMaterial
	virtual void _UNKNOWN_088(void) = 0; // InvalidMaterial
	virtual void _UNKNOWN_089(void) = 0; // GetMaterial
	virtual void _UNKNOWN_090(void) = 0; // GetNumMaterials
	virtual void _UNKNOWN_091(void) = 0; // FindTexture
	virtual void _UNKNOWN_092(void) = 0; // IsTextureLoaded
	virtual void _UNKNOWN_093(void) = 0; // CreateProceduralTexture

	//
	// Render targets
	//

	// 094:
	virtual void BeginRenderTargetAllocation() = 0;

	// 095:
	virtual void EndRenderTargetAllocation() = 0; // Simulate an Alt-Tab in here, which causes a release/restore of all resources

	// 096:
	// Creates a render target
	// If depth == true, a depth buffer is also allocated. If not, then
	// the screen's depth buffer is used.
	// Creates a texture for use as a render target
	virtual ITexture_csgo *	CreateRenderTargetTexture( int w, 
		int h, 
		RenderTargetSizeMode_t_csgo sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		ImageFormat_csgo	format, 
		MaterialRenderTargetDepth_t_csgo depth = MATERIAL_RT_DEPTH_SHARED ) = 0;

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
	/// <remarks>GetRenderContext calls AddRef on returned!</remarks>
	virtual IMatRenderContext_csgo *	GetRenderContext() = 0;

	virtual void _UNKNOWN_116(void) = 0;
	virtual void _UNKNOWN_117(void) = 0;
	virtual void _UNKNOWN_118(void) = 0;
	virtual void _UNKNOWN_119(void) = 0;

	// Create a custom render context. Cannot be used to create MATERIAL_HARDWARE_CONTEXT
	virtual IMatRenderContext_csgo *CreateRenderContext(CSGO::MaterialContextType_t type) = 0; //:120

	// 121:
	/// <remarks>SetRenderContext calls AddRef on param!</remarks>
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

enum ClearFlags_t_csgo
{ 
	VIEW_CLEAR_COLOR = 0x1,
	VIEW_CLEAR_DEPTH = 0x2,
	VIEW_CLEAR_FULL_TARGET = 0x4,
	VIEW_NO_DRAW = 0x8,
	VIEW_CLEAR_OBEY_STENCIL = 0x10, // Draws a quad allowing stencil test to clear through portals
	VIEW_CLEAR_STENCIL = 0x20,
};

// Used by RenderView 
enum RenderViewInfo_t_csgo
{ 
	RENDERVIEW_UNSPECIFIED	 = 0,
	RENDERVIEW_DRAWVIEWMODEL = (1<<0), 
	RENDERVIEW_DRAWHUD		 = (1<<1),
	RENDERVIEW_SUPPRESSMONITORRENDERING = (1<<2),
};

#define CLIENT_DLL_INTERFACE_VERSION_CSGO_018 "VClient018"

namespace CSGO
{
//-----------------------------------------------------------------------------
// Purpose: The engine reports to the client DLL what stage it's entering so the DLL can latch events
//  and make sure that certain operations only happen during the right stages.
// The value for each stage goes up as you move through the frame so you can check ranges of values
//  and if new stages get added in-between, the range is still valid.
//-----------------------------------------------------------------------------
enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};
}

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

	// Mouse Input Interfaces
	// Activate the mouse (hides the cursor and locks it to the center of the screen)
	virtual void			IN_ActivateMouse(void) = 0; //:016
	// Deactivates the mouse (shows the cursor and unlocks it)
	virtual void			IN_DeactivateMouse(void) = 0; //:017
	
	virtual void _UNKOWN_018(void) = 0;
	virtual void _UNKOWN_019(void) = 0;
	virtual void _UNKOWN_020(void) = 0;
	virtual void _UNKOWN_021(void) = 0;
	virtual void _UNKOWN_022(void) = 0;
	virtual void _UNKOWN_023(void) = 0;
	virtual void _UNKOWN_024(void) = 0;
	virtual void _UNKOWN_025(void) = 0;
	virtual void _UNKOWN_026(void) = 0;

	// 027:
	virtual void View_Render( vrect_t_csgo *rect )= 0;

	// 028:
	virtual void RenderView( const CViewSetup_csgo &view, int nClearFlags, int whatToDraw )= 0;

	virtual void _UNKOWN_029(void) = 0;
	virtual void _UNKOWN_030(void) = 0;
	virtual void _UNKOWN_031(void) = 0;
	virtual void _UNKOWN_032(void) = 0;
	virtual void _UNKOWN_033(void) = 0;
	virtual void _UNKOWN_034(void) = 0;
	virtual void _UNKOWN_035(void) = 0;
	virtual void _UNKOWN_036(void) = 0;

	// 037:
	virtual void FrameStageNotify(SOURCESDK::CSGO::ClientFrameStage_t curStage) = 0;

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

	virtual void			OnDemoPlaybackStart(char const* pDemoBaseName) = 0; //:055

	virtual void _UNKOWN_056(void) = 0;

	virtual void			OnDemoPlaybackStop() = 0; //:057

	virtual void _UNKOWN_058(void) = 0;
	virtual void _UNKOWN_059(void) = 0;
	virtual void _UNKOWN_060(void) = 0;
	virtual void _UNKOWN_061(void) = 0;
	virtual void _UNKOWN_062(void) = 0;
	virtual void _UNKOWN_063(void) = 0;
	virtual void _UNKOWN_064(void) = 0;
	virtual void _UNKOWN_065(void) = 0;
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
	virtual void _UNKOWN_131(void) = 0;

	// and a few more to be save from updates:

	virtual void _UNKOWN_132(void) = 0;
	virtual void _UNKOWN_133(void) = 0;
	virtual void _UNKOWN_134(void) = 0;
	virtual void _UNKOWN_135(void) = 0;
	virtual void _UNKOWN_136(void) = 0;
	virtual void _UNKOWN_137(void) = 0;
	virtual void _UNKOWN_138(void) = 0;
	virtual void _UNKOWN_139(void) = 0;
};

// IVRenderView_csgo ///////////////////////////////////////////////////////////

namespace CSGO
{
	// In-game panels are cropped to the current engine viewport size
	enum PaintMode_t
	{
		PAINT_UIPANELS = (1 << 0),
		PAINT_INGAMEPANELS = (1 << 1),
	};

}

#define VENGINE_RENDERVIEW_INTERFACE_VERSION_CSGO "VEngineRenderView014"

class VPlane_csgo;
typedef VPlane_csgo  * Frustum_csgo;

class IVRenderView_csgo abstract
{
public:
	virtual void _UNKOWN_000(void) = 0;
	virtual void _UNKOWN_001(void) = 0;
	virtual void _UNKOWN_002(void) = 0;
	virtual void _UNKOWN_003(void) = 0;
	
	// 004:
	virtual void SetBlend( float blend ) = 0;

	// 005
	virtual float GetBlend( void ) = 0;

	// 006:
	virtual void SetColorModulation( float const* blend ) = 0;

	// 007:
	virtual void GetColorModulation( float* blend ) = 0;

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

	// See i
	virtual void			VGui_Paint(int mode) = 0; //:042

	virtual void _UNKOWN_043(void) = 0;
	virtual void _UNKOWN_044(void) = 0;
	
	virtual void			Push2DView(const CViewSetup_csgo &view, int nFlags, ITexture_csgo* pRenderTarget, Frustum_csgo frustumPlanes) = 0; //:045
	virtual void			PopView(Frustum_csgo frustumPlanes) = 0; //:046

	virtual void _UNKOWN_047(void) = 0;
	virtual void _UNKOWN_048(void) = 0;
	virtual void _UNKOWN_049(void) = 0;
	virtual void _UNKOWN_050(void) = 0;
	virtual void _UNKOWN_051(void) = 0;
	virtual void _UNKOWN_052(void) = 0;
	virtual void _UNKOWN_053(void) = 0;
	virtual void _UNKOWN_054(void) = 0;
	virtual void _UNKOWN_055(void) = 0;

	virtual void			GetMatricesForView(const CViewSetup_csgo &view, VMatrix *pWorldToView, VMatrix *pViewToProjection, VMatrix *pWorldToProjection, VMatrix *pWorldToPixels) = 0; // :056

	virtual void _UNKOWN_057(void) = 0;


	virtual void _UNKOWN_058(void) = 0;
	virtual void _UNKOWN_059(void) = 0;
};

// IViewRender_csgo ////////////////////////////////////////////////////////////

class C_BaseEntity_csgo;

class IViewRender_csgo abstract
{
public:
	virtual void _UNKOWN_000(void) = 0;
	virtual void _UNKOWN_001(void) = 0;
	virtual void _UNKOWN_002(void) = 0;
	virtual void _UNKOWN_003(void) = 0;
	virtual void _UNKOWN_004(void) = 0;
	virtual void _UNKOWN_005(void) = 0;
	
	// 006:
	// Called to render just a particular setup ( for timerefresh and envmap creation )
	// First argument is 3d view setup, second is for the HUD (in most cases these are ==, but in split screen the client .dll handles this differently)
	virtual void RenderView( const CViewSetup_csgo &view, const CViewSetup_csgo &hudViewSetup, int nClearFlags, int whatToDraw ) = 0;

	virtual void _UNKOWN_007(void) = 0;
	virtual void _UNKOWN_008(void) = 0;
	virtual void _UNKOWN_009(void) = 0;

	// This can only be called during rendering (while within RenderView).
	virtual VPlane_csgo*		GetFrustum() = 0; //:010

	virtual void _UNKOWN_011(void) = 0;

	// 012:
	virtual const CViewSetup_csgo *GetPlayerViewSetup( int nSlot = -1 ) const = 0;

	// 013:
	virtual const CViewSetup_csgo *GetViewSetup( void ) const = 0;

	virtual void _UNKOWN_014(void) = 0; // DisableVis 
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

	virtual C_BaseEntity_csgo *GetCurrentlyDrawingEntity() = 0; //:028

	virtual void		SetCurrentlyDrawingEntity(C_BaseEntity_csgo *pEnt) = 0; //:029

	// 030:
	virtual bool UpdateShadowDepthTexture( ITexture_csgo *pRenderTarget, ITexture_csgo *pDepthTexture, const CViewSetup_csgo &shadowView, unsigned char _unkArg4 = 1, unsigned char _unkArg5 = 0 ) = 0;

	// ...
	// more we don't care about.
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

class IFileSystem_csgo abstract : public SOURCESDK::CSGO::IAppSystem, public IBaseFileSystem_csgo
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

// CViewSetup_csgo /////////////////////////////////////////////////////////////

class CViewSetup_csgo
{
public:

// shared by 2D & 3D views

	// left side of view window
	int			x;	 
	int			m_nUnscaledX;
	// top side of view window
	int			y;
	int			m_nUnscaledY;
	// width of view window
	int			width;
	int			m_nUnscaledWidth;
	// height of view window
	int			height;
	int			m_nUnscaledHeight;

	char _unknown_20_b0[0x90];

	// horizontal FOV in degrees
	float		fov;
	// horizontal FOV in degrees for in-view model
	float		fovViewmodel;

	// 3D origin of camera
	Vector		origin;

	// heading of camera (pitch, yaw, roll)
	QAngle		angles;
	// local Z coordinate of near plane of camera
	float		zNear;
	// local Z coordinate of far plane of camera
	float		zFar;

	// local Z coordinate of near plane of camera ( when rendering view model )
	float		zNearViewmodel;
	// local Z coordinate of far plane of camera ( when rendering view model )
	float		zFarViewmodel;

	char _unknown_eo_148[0x58];

	// Controls for off-center projection (needed for poster rendering)
	float		m_flOffCenterTop;
	float		m_flOffCenterBottom;
	float		m_flOffCenterLeft;
	float		m_flOffCenterRight;
	bool		m_bOffCenter : 1;

	// set to true if this is to draw into a subrect of the larger screen
	// this really is a hack, but no more than the rest of the way this class is used
	bool		m_bRenderToSubrectOfLargerScreen : 1;

	// Controls that the SFM needs to tell the engine when to do certain post-processing steps
	bool		m_bDoBloomAndToneMapping : 1;
	bool		m_bDoDepthOfField : 1;
	bool		m_bHDRTarget : 1;
	bool		m_bDrawWorldNormal : 1;
	bool		m_bCullFrontFaces : 1;

	// Cached mode for certain full-scene per-frame varying state such as sun entity coverage
	bool		m_bCacheFullSceneState : 1;

	bool		m_bRenderFlashlightDepthTranslucents : 1;

	char _unknown_14a_198[0x4e];
};

////////////////////////////////////////////////////////////////////////////////

enum ShaderBlendFactor_t_csgo
{
	SHADER_BLEND_ZERO,
	SHADER_BLEND_ONE,
	SHADER_BLEND_DST_COLOR,
	SHADER_BLEND_ONE_MINUS_DST_COLOR,
	SHADER_BLEND_SRC_ALPHA,
	SHADER_BLEND_ONE_MINUS_SRC_ALPHA,
	SHADER_BLEND_DST_ALPHA,
	SHADER_BLEND_ONE_MINUS_DST_ALPHA,
	SHADER_BLEND_SRC_ALPHA_SATURATE,
	SHADER_BLEND_SRC_COLOR,
	SHADER_BLEND_ONE_MINUS_SRC_COLOR
};

// IShaderShadow_csgo //////////////////////////////////////////////////////////

#define SHADERSHADOW_INTERFACE_VERSION_CSGO "ShaderShadow010"

class IShaderShadow_csgo
{
public:
	virtual void _UNKOWN_000(void) = 0;
	virtual void _UNKOWN_001(void) = 0;
	
	// 002:
	virtual void EnableDepthWrites( bool bEnable ) = 0;
	
	virtual void _UNKOWN_003(void) = 0;
	virtual void _UNKOWN_004(void) = 0;

	// 005:
	virtual void EnableColorWrites( bool bEnable ) = 0;

	// 006:
	virtual void EnableAlphaWrites( bool bEnable ) = 0;

	// 007:
	virtual void EnableBlending( bool bEnable ) = 0;

	virtual void _UNKOWN_008(void) = 0;
	
	// 009:
	virtual void BlendFunc( ShaderBlendFactor_t_csgo srcFactor, ShaderBlendFactor_t_csgo dstFactor ) = 0;

	virtual void _UNKOWN_010(void) = 0;
	virtual void _UNKOWN_011(void) = 0;
	virtual void _UNKOWN_012(void) = 0;
	virtual void _UNKOWN_013(void) = 0;
	virtual void _UNKOWN_014(void) = 0;
	virtual void _UNKOWN_015(void) = 0;
	virtual void _UNKOWN_016(void) = 0; // VertexShaderVertexFormat

	// 017:
	virtual void SetVertexShader( const char* pFileName, int nStaticVshIndex ) = 0;

	// 018:
	virtual	void SetPixelShader( const char* pFileName, int nStaticPshIndex = 0 ) = 0;

	// 019:
	// Convert from linear to gamma color space on writes to frame buffer.
	virtual void EnableSRGBWrite( bool bEnable ) = 0;

	virtual void _UNKOWN_020(void) = 0; // EnableSRGBRead, but unimplemented.
	virtual void _UNKOWN_021(void) = 0; // EnableTexture

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
};


// IShaderDynamicAPI_csgo //////////////////////////////////////////////////////

class IShaderDynamicAPI_csgo
{
public:
	virtual void _UNKOWN_000(void) = 0;
	virtual void _UNKOWN_001(void) = 0;
	virtual void _UNKOWN_002(void) = 0;
	virtual void _UNKOWN_003(void) = 0;
	virtual void _UNKOWN_004(void) = 0;
	virtual void _UNKOWN_005(void) = 0;
	virtual void _UNKOWN_006(void) = 0;
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

	// 019:
	virtual void SetVertexShaderIndex( int vshIndex = -1 ) = 0;

	// 020:
	virtual void SetPixelShaderIndex( int pshIndex = 0 ) = 0;

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

	// 058:
	virtual void ExecuteCommandBuffer( uint8 *pCmdBuffer ) = 0;

	virtual void _UNKOWN_059(void) = 0;
	virtual void _UNKOWN_060(void) = 0;
	virtual void _UNKOWN_061(void) = 0;
	virtual void _UNKOWN_062(void) = 0;
	virtual void _UNKOWN_063(void) = 0;
	virtual void _UNKOWN_064(void) = 0;
	virtual void _UNKOWN_065(void) = 0;
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
	virtual void _UNKOWN_131(void) = 0;
	virtual void _UNKOWN_132(void) = 0;
	virtual void _UNKOWN_133(void) = 0;
	virtual void _UNKOWN_134(void) = 0;
	virtual void _UNKOWN_135(void) = 0;
	virtual void _UNKOWN_136(void) = 0;
	virtual void _UNKOWN_137(void) = 0;
	virtual void _UNKOWN_138(void) = 0;
	virtual void _UNKOWN_139(void) = 0;
	virtual void _UNKOWN_140(void) = 0;
	virtual void _UNKOWN_141(void) = 0;
	virtual void _UNKOWN_142(void) = 0;
	virtual void _UNKOWN_143(void) = 0;
	virtual void _UNKOWN_144(void) = 0;
	virtual void _UNKOWN_145(void) = 0;
	virtual void _UNKOWN_146(void) = 0;
};

// IMaterialVar_csgo ///////////////////////////////////////////////////////////

enum MaterialVarSym_t_csgo
{
};

enum ShaderMaterialVars_t_csgo
{
	FLAGS = 0,
	FLAGS_DEFINED,	// mask indicating if the flag was specified
	FLAGS2,
	FLAGS_DEFINED2,
	COLOR,
	ALPHA,
	BASETEXTURE,
	FRAME,
	BASETEXTURETRANSFORM,
	FLASHLIGHTTEXTURE,
	FLASHLIGHTTEXTUREFRAME,
	COLOR2,
	SRGBTINT,

	NUM_SHADER_MATERIAL_VARS
};

enum MaterialVarFlags_t_csgo
{
	MATERIAL_VAR_DEBUG					  = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE		  = (1 << 1),
	MATERIAL_VAR_NO_DRAW				  = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE	  = (1 << 3),

	MATERIAL_VAR_VERTEXCOLOR			  = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA			  = (1 << 5),
	MATERIAL_VAR_SELFILLUM				  = (1 << 6),
	MATERIAL_VAR_ADDITIVE				  = (1 << 7),
	MATERIAL_VAR_ALPHATEST				  = (1 << 8),
//	MATERIAL_VAR_UNUSED					  = (1 << 9),
	MATERIAL_VAR_ZNEARER				  = (1 << 10),
	MATERIAL_VAR_MODEL					  = (1 << 11),
	MATERIAL_VAR_FLAT					  = (1 << 12),
	MATERIAL_VAR_NOCULL					  = (1 << 13),
	MATERIAL_VAR_NOFOG					  = (1 << 14),
	MATERIAL_VAR_IGNOREZ				  = (1 << 15),
	MATERIAL_VAR_DECAL					  = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE			  = (1 << 17), // OBSOLETE
//	MATERIAL_VAR_UNUSED					  = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE	      = (1 << 19), // OBSOLETE
	MATERIAL_VAR_BASEALPHAENVMAPMASK	  = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT              = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING  = (1 << 23), // OBSOLETE
	MATERIAL_VAR_OPAQUETEXTURE			  = (1 << 24),
	MATERIAL_VAR_ENVMAPMODE				  = (1 << 25), // OBSOLETE
	MATERIAL_VAR_SUPPRESS_DECALS		  = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT			  = (1 << 27),
	MATERIAL_VAR_WIREFRAME                = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE     = (1 << 29),
	MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY  = (1 << 30),
	MATERIAL_VAR_VERTEXFOG				  = (1 << 31),

	// NOTE: Only add flags here that either should be read from
	// .vmts or can be set directly from client code. Other, internal
	// flags should to into the flag enum in IMaterialInternal.h
};

enum MaterialVarType_t_csgo 
{ 
	MATERIAL_VAR_TYPE_FLOAT = 0,
	MATERIAL_VAR_TYPE_STRING,
	MATERIAL_VAR_TYPE_VECTOR,
	MATERIAL_VAR_TYPE_TEXTURE,
	MATERIAL_VAR_TYPE_INT,
	MATERIAL_VAR_TYPE_FOURCC,
	MATERIAL_VAR_TYPE_UNDEFINED,
	MATERIAL_VAR_TYPE_MATRIX,
	MATERIAL_VAR_TYPE_MATERIAL,
};

class IMaterialVar_csgo
{
protected:
	// base data and accessors
	char* m_pStringVal;
	int m_intVal;
	Vector4D_csgo m_VecVal;

	// member data. total = 4 bytes
	uint8 m_Type : 4;

	//
	// more we don't carea about atm.

public:
	//
	// Virtuals:

	// 000:
	virtual ITexture_csgo *GetTextureValue( void ) = 0;

	virtual void _UNKOWN_001(void) = 0;

	// 002:
	virtual char const *	GetName( void ) const = 0;

	// 003:
	virtual MaterialVarSym_t_csgo	GetNameAsSymbol() const = 0;

	// 004:
	virtual void			SetFloatValue( float val ) = 0;
	
	// 005:
	virtual void			SetIntValue( int val ) = 0;
	
	// 006:
	virtual void			SetStringValue( char const *val ) = 0;

	// 007:
	virtual char const *	GetStringValue( void ) const = 0;

	virtual void _UNKOWN_008(void) = 0;
	virtual void _UNKOWN_009(void) = 0;
	virtual void _UNKOWN_010(void) = 0;
	virtual void _UNKOWN_011(void) = 0;
	virtual void _UNKOWN_012(void) = 0;
	virtual void _UNKOWN_013(void) = 0;
	virtual void _UNKOWN_014(void) = 0;
	virtual void _UNKOWN_015(void) = 0;

	// 016:
	virtual IMaterial_csgo * GetMaterialValue( void ) = 0;
	
	// 017:
	virtual void SetMaterialValue( IMaterial_csgo * ) = 0;

	// 018:
	virtual bool IsDefined() const = 0;

	// 019:
	virtual void SetUndefined() = 0;

	virtual void _UNKOWN_020(void) = 0;
	virtual void _UNKOWN_021(void) = 0;
	virtual void _UNKOWN_022(void) = 0;
	virtual void _UNKOWN_023(void) = 0;
	virtual void _UNKOWN_024(void) = 0;
	
	// 025:
	virtual IMaterial_csgo * GetOwningMaterial() = 0;

	virtual void _UNKOWN_026(void) = 0; // SetVecComponentValue

protected:

	// 027:
	virtual int				GetIntValueInternal( void ) const = 0;

	// 028:
	virtual float			GetFloatValueInternal( void ) const = 0;

	// 030:
	virtual float const*	GetVecValueInternal( ) const = 0;

	// 029:
	virtual void			GetVecValueInternal( float *val, int numcomps ) const = 0;

	// 030:
	virtual int				VectorSizeInternal() const = 0;

	//
	// more we don't carea about atm.

public:
	FORCEINLINE MaterialVarType_t_csgo GetType( void ) const
	{
		return ( MaterialVarType_t_csgo )m_Type;
	}

	FORCEINLINE bool IsTexture() const
	{
		return m_Type == MATERIAL_VAR_TYPE_TEXTURE;
	}

#ifndef FAST_MATERIALVAR_ACCESS
	FORCEINLINE int GetIntValue( void ) const
	{
		return GetIntValueInternal();
	}

	FORCEINLINE float GetFloatValue( void ) const
	{
		return GetFloatValueInternal();
	}

	FORCEINLINE float const* GetVecValue( ) const
	{
		return GetVecValueInternal();
	}

	FORCEINLINE void GetVecValue( float *val, int numcomps ) const 
	{
		return GetVecValueInternal( val, numcomps );
	}

	FORCEINLINE int VectorSize() const
	{
		return VectorSizeInternal();
	}
#endif

};

// CBasePerMaterialContextData_csgo ////////////////////////////////////////////

class CBasePerMaterialContextData_csgo								
{
public:
	uint32 m_nVarChangeID;
	bool m_bMaterialVarsChanged;							// set by mat system when material vars change. shader should rehtink and then clear the var

	FORCEINLINE CBasePerMaterialContextData_csgo( void )
	{
		m_bMaterialVarsChanged = true;
		m_nVarChangeID = 0xffffffff;
	}

	// virtual destructor so that derived classes can have their own data to be cleaned up on
	// delete of material
	virtual ~CBasePerMaterialContextData_csgo( void )
	{
	}
};

// CommandBufferCommand_t_csgo /////////////////////////////////////////////////

enum CommandBufferCommand_t_csgo
{
	// flow control commands.
	CBCMD_END = 0,									// end of stream
	CBCMD_JUMP,										// int cmd, void *adr. jump to another
													// stream. Can be used to implement
													// non-sequentially allocated storage
	CBCMD_JSR,										// int cmd, void *adr. subroutine call to another stream.

	// constant setting commands
	CBCMD_SET_PIXEL_SHADER_FLOAT_CONST,				// int cmd,int first_reg, int nregs, float values[nregs*4]


	CBCMD_SET_VERTEX_SHADER_FLOAT_CONST,			// int cmd,int first_reg, int nregs, float values[nregs*4]
	CBCMD_SET_VERTEX_SHADER_FLOAT_CONST_REF,		// int cmd,int first_reg, int nregs, &float values[nregs*4]
	CBCMD_SETPIXELSHADERFOGPARAMS,					// int cmd, int regdest
	CBCMD_STORE_EYE_POS_IN_PSCONST,					// int cmd, int regdest
	CBCMD_SET_DEPTH_FEATHERING_CONST,				// int cmd, int constant register, float blend scale

	// texture binding
	CBCMD_BIND_STANDARD_TEXTURE,					// cmd, sampler, texture id
	CBCMD_BIND_SHADERAPI_TEXTURE_HANDLE,			// cmd, sampler, texture handle

	// shaders
	CBCMD_SET_PSHINDEX,								// cmd, idx
	CBCMD_SET_VSHINDEX,								// cmd, idx

	CBCMD_SET_VERTEX_SHADER_FLASHLIGHT_STATE,		// cmd, int first_reg (for worldToTexture matrix)
	CBCMD_SET_PIXEL_SHADER_FLASHLIGHT_STATE,		// cmd, int color reg, int atten reg, int origin reg, sampler (for flashlight texture)

	CBCMD_SET_PIXEL_SHADER_UBERLIGHT_STATE,			// cmd

	CBCMD_SET_VERTEX_SHADER_NEARZFARZ_STATE,		// cmd
};

// CBaseShader_csgo ////////////////////////////////////////////////////////////

struct ShaderParamInfo_t_csgo;
class IShaderInit_csgo;
class CBasePerInstanceContextData_csgo;

class IBaseShader_csgo abstract
{
	// Returns the shader name
	virtual char const* GetName( ) const = 0;

	// returns the shader fallbacks
	virtual char const* GetFallbackShader( IMaterialVar_csgo** params ) const = 0;

	// Shader parameters
	virtual int GetParamCount( ) const = 0;
	virtual const ShaderParamInfo_t_csgo & GetParamInfo( int paramIndex ) const = 0;

	virtual void InitShaderParams( IMaterialVar_csgo** ppParams, const char *pMaterialName ) = 0;

	virtual void InitShaderInstance( IMaterialVar_csgo ** ppParams, IShaderInit_csgo *pShaderInit, const char *pMaterialName, const char *pTextureGroupName ) = 0;

	virtual void DrawElements( IMaterialVar_csgo **params, int nModulationFlags, IShaderShadow_csgo* pShaderShadow, IShaderDynamicAPI_csgo* pShaderAPI,
								VertexCompressionType_t_csgo vertexCompression, CBasePerMaterialContextData_csgo **pContext, CBasePerInstanceContextData_csgo** pInstanceDataPtr ) = 0;

	virtual void _Unknown_007_SomewhatDrawElements(
		unsigned __int32 * unkDataPtr1,
		unsigned __int32 * unkDataPtr2,
		void * unkClass1, // related to VertexBuffer
		IShaderDynamicAPI_csgo* pShaderAPI,
		unsigned __int32 unkData2,
		CBasePerMaterialContextData_csgo **pContext,
		unsigned __int32 unkData3
		) = 0;

	virtual int ComputeModulationFlags( IMaterialVar_csgo** params, IShaderDynamicAPI_csgo* pShaderAPI ) = 0;

	virtual bool NeedsPowerOfTwoFrameBufferTexture( IMaterialVar_csgo **params, bool bCheckSpecificToThisFrame = true ) const;

	virtual bool NeedsFullFrameBufferTexture( IMaterialVar_csgo	 **params, bool bCheckSpecificToThisFrame = true ) const;

	virtual bool IsTranslucent( IMaterialVar_csgo **params ) const;

	virtual int GetFlags() const = 0;

	virtual void _Unknown_013_SetTexturGroupName( const char *pTextureGroupName );

	virtual void _Unknown_014_SetModulationFlags( int nModulationFlags );
	
	virtual void OnInitShaderParams( IMaterialVar_csgo** ppParams, const char *pMaterialName ) {}
	
	virtual void OnInitShaderInstance( IMaterialVar_csgo** ppParams, IShaderInit_csgo *pShaderInit, const char *pMaterialName ) = 0;
	
	virtual void OnDrawElements( IMaterialVar_csgo **params, IShaderShadow_csgo* pShaderShadow, IShaderDynamicAPI_csgo* pShaderAPI, VertexCompressionType_t_csgo vertexCompression, CBasePerMaterialContextData_csgo **pContextDataPtr ) = 0;

};


////////////////////////////////////////////////////////////////////////////////

namespace CSGO {
	class CBaseHandle;
}

class IClientEntity_csgo;
class ClientClass_csgo;
class IClientNetworkable_csgo;
class IClientUnknown_csgo;
struct EntityCacheInfo_t_csgo;

//-----------------------------------------------------------------------------
// Purpose: Exposes IClientEntity's to engine
//-----------------------------------------------------------------------------
class IClientEntityList_csgo abstract
{
public:
	// Get IClientNetworkable interface for specified entity
	virtual IClientNetworkable_csgo*	GetClientNetworkable( int entnum ) = 0;
	virtual IClientNetworkable_csgo*	GetClientNetworkableFromHandle( CSGO::CBaseHandle hEnt ) = 0;
	virtual IClientUnknown_csgo*		GetClientUnknownFromHandle( CSGO::CBaseHandle hEnt ) = 0;

	// NOTE: This function is only a convenience wrapper.
	// It returns GetClientNetworkable( entnum )->GetIClientEntity().
	virtual IClientEntity_csgo*		GetClientEntity( int entnum ) = 0;
	virtual IClientEntity_csgo*		GetClientEntityFromHandle( CSGO::CBaseHandle hEnt ) = 0;

	// Returns number of entities currently in use
	virtual int					NumberOfEntities( bool bIncludeNonNetworkable ) = 0;

	// Returns highest index actually used
	virtual int					GetHighestEntityIndex( void ) = 0;

	// Sizes entity list to specified size
	virtual void				SetMaxEntities( int maxents ) = 0;
	virtual int					GetMaxEntities( ) = 0;
	virtual EntityCacheInfo_t_csgo	*GetClientNetworkableArray() = 0;
};

extern IClientEntityList_csgo * g_Entitylist_csgo;

#define VCLIENTENTITYLIST_INTERFACE_VERSION_CSGO "VClientEntityList003"

// An IHandleEntity-derived class can go into an entity list and use ehandles.
class IHandleEntity_csgo
{
public:
	virtual ~IHandleEntity_csgo() {}
	virtual void SetRefEHandle( const CSGO::CBaseHandle &handle ) = 0;
	virtual const CSGO::CBaseHandle& GetRefEHandle() const = 0;
#ifdef _X360
	IHandleEntity() :
		m_bIsStaticProp( false )
	{
	}

	bool m_bIsStaticProp;
#endif
};

class C_BaseEntity_csgo;
class IClientRenderable_csgo;
class ICollideable_csgo;
class IClientThinkable_csgo;
class IClientModelRenderable_csgo;
class IClientAlphaProperty_csgo;

// This is the client's version of IUnknown. We may want to use a QueryInterface-like
// mechanism if this gets big.
class IClientUnknown_csgo : public IHandleEntity_csgo
{
public:
	virtual ICollideable_csgo*		GetCollideable() = 0;
	virtual IClientNetworkable_csgo*	GetClientNetworkable() = 0;
	virtual IClientRenderable_csgo*	GetClientRenderable() = 0;
	virtual IClientEntity_csgo*		GetIClientEntity() = 0;
	virtual C_BaseEntity_csgo*		GetBaseEntity() = 0;
	virtual IClientThinkable_csgo*	GetClientThinkable() = 0;
	virtual IClientModelRenderable_csgo*	GetClientModelRenderable() = 0;
	// not in csgo // virtual IClientAlphaProperty_csgo*	GetClientAlphaProperty() = 0;
};

namespace CSGO
{
	typedef unsigned short ModelInstanceHandle_t;

	enum
	{
		MODEL_INSTANCE_INVALID = (ModelInstanceHandle_t)~0
	};

	typedef void model_t;

	class IVModelInfoClient
	{
	public:
		virtual					~IVModelInfoClient() {};
		virtual const model_t*	GetModel(int modelindex) const = 0;
		virtual int				GetModelIndex(const char* name) const = 0;
		virtual const char*		GetModelName(const model_t* model) const = 0;
	};

#define SOURCESDK_CSGO_VMODELINFO_CLIENT_INTERFACE_VERSION "VModelInfoClient004"
}

class IClientRenderable_csgo abstract
{
public:
	// Gets at the containing class...
	virtual IClientUnknown_csgo*	GetIClientUnknown() = 0; //:000

	// Data accessors
	virtual Vector const&			GetRenderOrigin(void) = 0; //:001
	virtual QAngle const&			GetRenderAngles(void) = 0; //:002

	virtual bool					ShouldDraw(void) = 0; //:003

	virtual void _UNKNOWN_IClientRenderable_csgo_004(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_005(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_006(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_007(void);

	virtual const SOURCESDK::CSGO::model_t* GetModel(void) const = 0; //:008

	virtual void _UNKNOWN_IClientRenderable_csgo_009(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_010(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_011(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_012(void);

	// virtual bool	SetupBones( matrix3x4a_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime ) = 0;
	virtual void _UNKNOWN_IClientRenderable_csgo_013(void);

	virtual void _UNKNOWN_IClientRenderable_csgo_014(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_015(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_016(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_017(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_018(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_019(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_020(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_021(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_022(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_023(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_024(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_025(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_026(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_027(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_028(void);
	virtual void _UNKNOWN_IClientRenderable_csgo_029(void);

	// Create/get/destroy model instance
	virtual void CreateModelInstance() = 0; //:030
	virtual SOURCESDK::CSGO::ModelInstanceHandle_t GetModelInstance() = 0; //:031
	
	// Returns the transform from RenderOrigin/RenderAngles to world
	virtual const matrix3x4_t &RenderableToWorldTransform() = 0; //:032

	// Attachments
	virtual int LookupAttachment(const char *pAttachmentName) = 0; //:33
	virtual	bool GetAttachment(int number, Vector &origin, QAngle &angles) = 0; //:34
	virtual bool GetAttachment(int number, matrix3x4_t &matrix) = 0; //:35

	// More functions, but we don't care about this.
};


class IClientModelRenderable_csgo
{
public:
	// More functions, but we don't care about this.
};


namespace CSGO {
	enum DataUpdateType_t
	{
		DATA_UPDATE_CREATED = 0,
		DATA_UPDATE_DATATABLE_CHANGED,
		DATA_UPDATE_POST_UPDATE,
	};
}


class IClientNetworkable_csgo abstract
{
public:
	virtual void _UNKNOWN_IClientNetworkable_000(void);
	virtual void _UNKNOWN_IClientNetworkable_001(void);
	virtual void _UNKNOWN_IClientNetworkable_002(void);
	virtual void _UNKNOWN_IClientNetworkable_003(void);
	virtual void _UNKNOWN_IClientNetworkable_004(void);
	virtual void _UNKNOWN_IClientNetworkable_005(void);

	virtual void			PreDataUpdate(CSGO::DataUpdateType_t updateType) = 0;
	virtual void			PostDataUpdate(CSGO::DataUpdateType_t updateType) = 0;

	virtual void _UNKNOWN_IClientNetworkable_008(void);

	// Objects become dormant on the client if they leave the PVS on the server.
	virtual bool			IsDormant(void) = 0;

	// Ent Index is the server handle used to reference this entity.
	// If the index is < 0, that indicates the entity is not known to the server
	virtual int				entindex(void) const = 0; //:10

	// More functions, but we don't care about this.
};

class IClientThinkable_csgo abstract
{
public:
	// More functions, but we don't care about this.

};


class CMouthInfo_csgo;
struct SpatializationInfo_t_csgo;

//-----------------------------------------------------------------------------
// Purpose: All client entities must implement this interface.
//-----------------------------------------------------------------------------
class IClientEntity_csgo abstract : public IClientUnknown_csgo, public IClientRenderable_csgo, public IClientNetworkable_csgo, public IClientThinkable_csgo
{
public:
	// Delete yourself.
	// not in csgo // virtual void			Release( void ) = 0;
	
	// Network origin + angles
	virtual const Vector&	GetAbsOrigin( void ) const = 0;
	virtual const QAngle&	GetAbsAngles( void ) const = 0;

	virtual CMouthInfo_csgo		*GetMouth( void ) = 0;

	// Retrieve sound spatialization info for the specified sound on this entity
	// Return false to indicate sound is not audible
	virtual bool			GetSoundSpatialization( SpatializationInfo_t_csgo& info ) = 0;

	virtual bool			IsBlurred( void ) = 0;
};

class C_BaseCombatCharacter_csgo;
class C_BaseCombatWeapon_csgo;

namespace CSGO {

typedef ::SOURCESDK::IHandleEntity_csgo IHandleEntity;

// How many bits to use to encode an edict.
#define	SOURCESDK_CSGO_MAX_EDICT_BITS				11			// # of bits needed to represent max edicts
// Max # of edicts in a level
#define	SOURCESDK_CSGO_MAX_EDICTS					(1<<MAX_EDICT_BITS)

// Used for networking ehandles.
#define SOURCESDK_CSGO_NUM_ENT_ENTRY_BITS		(SOURCESDK_CSGO_MAX_EDICT_BITS + 2)
#define SOURCESDK_CSGO_NUM_ENT_ENTRIES			(1 << SOURCESDK_CSGO_NUM_ENT_ENTRY_BITS)
#define SOURCESDK_CSGO_INVALID_EHANDLE_INDEX	0xFFFFFFFF

#define SOURCESDK_CSGO_NUM_SERIAL_NUM_BITS		16 // (32 - NUM_ENT_ENTRY_BITS)
#define SOURCESDK_CSGO_NUM_SERIAL_NUM_SHIFT_BITS (32 - SOURCESDK_CSGO_NUM_SERIAL_NUM_BITS)
#define SOURCESDK_CSGO_ENT_ENTRY_MASK	(( 1 << SOURCESDK_CSGO_NUM_SERIAL_NUM_BITS) - 1)


// -------------------------------------------------------------------------------------------------- //
// CBaseHandle.
// -------------------------------------------------------------------------------------------------- //

class CBaseHandle
{
	friend class CBaseEntityList;

public:

	CBaseHandle();
	CBaseHandle(const CBaseHandle &other);
	CBaseHandle(unsigned long value);
	CBaseHandle(int iEntry, int iSerialNumber);

	void Init(int iEntry, int iSerialNumber);
	void Term();

	// Even if this returns true, Get() still can return return a non-null value.
	// This just tells if the handle has been initted with any values.
	bool IsValid() const;

	int GetEntryIndex() const;
	int GetSerialNumber() const;

	int ToInt() const;
	bool operator !=(const CBaseHandle &other) const;
	bool operator ==(const CBaseHandle &other) const;
	bool operator ==(const IHandleEntity* pEnt) const;
	bool operator !=(const IHandleEntity* pEnt) const;
	bool operator <(const CBaseHandle &other) const;
	bool operator <(const IHandleEntity* pEnt) const;

	// Assign a value to the handle.
	const CBaseHandle& operator=(const IHandleEntity *pEntity);
	const CBaseHandle& Set(const IHandleEntity *pEntity);

	// Use this to dereference the handle.
	// Note: this is implemented in game code (ehandle.h)
	IHandleEntity* Get() const;

	void AfxAssign(const CBaseHandle &other)
	{
		m_Index = other.m_Index;
	}

protected:
	// The low NUM_SERIAL_BITS hold the index. If this value is less than MAX_EDICTS, then the entity is networkable.
	// The high NUM_SERIAL_NUM_BITS bits are the serial number.
	unsigned long	m_Index;
};

inline CBaseHandle::CBaseHandle()
{
	m_Index = SOURCESDK_CSGO_INVALID_EHANDLE_INDEX;
}

inline CBaseHandle::CBaseHandle(const CBaseHandle &other)
{
	m_Index = other.m_Index;
}

inline CBaseHandle::CBaseHandle(unsigned long value)
{
	m_Index = value;
}

inline CBaseHandle::CBaseHandle(int iEntry, int iSerialNumber)
{
	Init(iEntry, iSerialNumber);
}

inline void CBaseHandle::Init(int iEntry, int iSerialNumber)
{
	Assert(iEntry >= 0 && (iEntry & SOURCESDK_CSGO_ENT_ENTRY_MASK) == iEntry);
	Assert(iSerialNumber >= 0 && iSerialNumber < (1 << SOURCESDK_CSGO_NUM_SERIAL_NUM_BITS));

	m_Index = iEntry | (iSerialNumber << SOURCESDK_CSGO_NUM_SERIAL_NUM_SHIFT_BITS);
}

inline void CBaseHandle::Term()
{
	m_Index = SOURCESDK_CSGO_INVALID_EHANDLE_INDEX;
}

inline bool CBaseHandle::IsValid() const
{
	return m_Index != SOURCESDK_CSGO_INVALID_EHANDLE_INDEX;
}

inline int CBaseHandle::GetEntryIndex() const
{
	// There is a hack here: due to a bug in the original implementation of the 
	// entity handle system, an attempt to look up an invalid entity index in 
	// certain cirumstances might fall through to the the mask operation below.
	// This would mask an invalid index to be in fact a lookup of entity number
	// NUM_ENT_ENTRIES, so invalid ent indexes end up actually looking up the
	// last slot in the entities array. Since this slot is always empty, the 
	// lookup returns NULL and the expected behavior occurs through this unexpected
	// route.
	// A lot of code actually depends on this behavior, and the bug was only exposed
	// after a change to NUM_SERIAL_NUM_BITS increased the number of allowable
	// static props in the world. So the if-stanza below detects this case and 
	// retains the prior (bug-submarining) behavior.
	if (!IsValid())
		return SOURCESDK_CSGO_NUM_ENT_ENTRIES - 1;
	return m_Index & SOURCESDK_CSGO_ENT_ENTRY_MASK;
}

inline int CBaseHandle::GetSerialNumber() const
{
	return m_Index >> SOURCESDK_CSGO_NUM_SERIAL_NUM_SHIFT_BITS;
}

inline int CBaseHandle::ToInt() const
{
	return (int)m_Index;
}

inline bool CBaseHandle::operator !=(const CBaseHandle &other) const
{
	return m_Index != other.m_Index;
}

inline bool CBaseHandle::operator ==(const CBaseHandle &other) const
{
	return m_Index == other.m_Index;
}

inline bool CBaseHandle::operator <(const CBaseHandle &other) const
{
	return m_Index < other.m_Index;
}

template<class T> class CHandle : public CBaseHandle
{
	// TODO: Implement actually xD
};


} // namespace CSGO {

class C_BasePlayer_csgo;

class C_BaseEntity_csgo : public IClientEntity_csgo
{
public:
	// (0..14 is from IClientEntity_csgo)

	virtual void _UNKNOWN_C_BaseEntity_015(void);
	virtual void _UNKNOWN_C_BaseEntity_016(void);
	virtual void _UNKNOWN_C_BaseEntity_017(void);
	virtual void _UNKNOWN_C_BaseEntity_018(void);
	virtual void _UNKNOWN_C_BaseEntity_019(void);
	virtual void _UNKNOWN_C_BaseEntity_020(void);
	virtual void _UNKNOWN_C_BaseEntity_021(void);
	virtual void _UNKNOWN_C_BaseEntity_022(void);
	virtual void _UNKNOWN_C_BaseEntity_023(void);
	virtual void _UNKNOWN_C_BaseEntity_024(void);
	virtual void _UNKNOWN_C_BaseEntity_025(void);
	virtual void _UNKNOWN_C_BaseEntity_026(void);
	virtual void _UNKNOWN_C_BaseEntity_027(void);
	virtual void _UNKNOWN_C_BaseEntity_028(void);
	virtual void _UNKNOWN_C_BaseEntity_029(void);
	virtual void _UNKNOWN_C_BaseEntity_030(void);
	virtual void _UNKNOWN_C_BaseEntity_031(void);
	virtual void _UNKNOWN_C_BaseEntity_032(void);
	virtual void _UNKNOWN_C_BaseEntity_033(void);
	virtual void _UNKNOWN_C_BaseEntity_034(void);
	virtual void _UNKNOWN_C_BaseEntity_035(void);
	virtual void _UNKNOWN_C_BaseEntity_036(void);
	virtual void _UNKNOWN_C_BaseEntity_037(void);
	virtual void _UNKNOWN_C_BaseEntity_038(void);
	virtual void _UNKNOWN_C_BaseEntity_039(void);
	virtual void _UNKNOWN_C_BaseEntity_040(void);
	virtual void _UNKNOWN_C_BaseEntity_041(void);
	virtual void _UNKNOWN_C_BaseEntity_042(void);
	virtual void _UNKNOWN_C_BaseEntity_043(void);
	virtual void _UNKNOWN_C_BaseEntity_044(void);
	virtual void _UNKNOWN_C_BaseEntity_045(void);
	virtual void _UNKNOWN_C_BaseEntity_046(void);
	virtual void _UNKNOWN_C_BaseEntity_047(void);
	virtual void _UNKNOWN_C_BaseEntity_048(void);
	virtual void _UNKNOWN_C_BaseEntity_049(void);
	virtual void _UNKNOWN_C_BaseEntity_050(void);
	virtual void _UNKNOWN_C_BaseEntity_051(void);
	virtual void _UNKNOWN_C_BaseEntity_052(void);

	virtual void _UNKNOWN_C_BaseEntity_053(void); // GetAttackDamageScale
	
	virtual void _UNKNOWN_C_BaseEntity_054(void); // ValidateModelIndex

	virtual void _UNKNOWN_C_BaseEntity_055(void);
	virtual void _UNKNOWN_C_BaseEntity_056(void);
	virtual void _UNKNOWN_C_BaseEntity_057(void);
	virtual void _UNKNOWN_C_BaseEntity_058(void);
	virtual void _UNKNOWN_C_BaseEntity_059(void);
	virtual void _UNKNOWN_C_BaseEntity_060(void);
	virtual void _UNKNOWN_C_BaseEntity_061(void);
	virtual void _UNKNOWN_C_BaseEntity_062(void);
	virtual void _UNKNOWN_C_BaseEntity_063(void);
	virtual void _UNKNOWN_C_BaseEntity_064(void);
	virtual void _UNKNOWN_C_BaseEntity_065(void);
	virtual void _UNKNOWN_C_BaseEntity_066(void);
	virtual void _UNKNOWN_C_BaseEntity_067(void);
	virtual void _UNKNOWN_C_BaseEntity_068(void);
	virtual void _UNKNOWN_C_BaseEntity_069(void);
	virtual void _UNKNOWN_C_BaseEntity_070(void);
	virtual void _UNKNOWN_C_BaseEntity_071(void);
	virtual void _UNKNOWN_C_BaseEntity_072(void);
	virtual void _UNKNOWN_C_BaseEntity_073(void);
	virtual void _UNKNOWN_C_BaseEntity_074(void);
	virtual void _UNKNOWN_C_BaseEntity_075(void);
	virtual void _UNKNOWN_C_BaseEntity_076(void);
	virtual void _UNKNOWN_C_BaseEntity_077(void);
	virtual void _UNKNOWN_C_BaseEntity_078(void);
	virtual void _UNKNOWN_C_BaseEntity_079(void);
	virtual void _UNKNOWN_C_BaseEntity_080(void);
	virtual void _UNKNOWN_C_BaseEntity_081(void);
	virtual void _UNKNOWN_C_BaseEntity_082(void);
	virtual void _UNKNOWN_C_BaseEntity_083(void);
	virtual void _UNKNOWN_C_BaseEntity_084(void);
	virtual void _UNKNOWN_C_BaseEntity_085(void);
	virtual void _UNKNOWN_C_BaseEntity_086(void);
	virtual void _UNKNOWN_C_BaseEntity_087(void); // GetTeam
	
	virtual int						GetTeamNumber(void) const; //:088

	virtual void _UNKNOWN_C_BaseEntity_089(void);
	virtual void _UNKNOWN_C_BaseEntity_090(void);
	virtual void _UNKNOWN_C_BaseEntity_091(void);
	virtual void _UNKNOWN_C_BaseEntity_092(void);
	virtual void _UNKNOWN_C_BaseEntity_093(void);
	virtual void _UNKNOWN_C_BaseEntity_094(void);
	virtual void _UNKNOWN_C_BaseEntity_095(void);
	virtual void _UNKNOWN_C_BaseEntity_096(void);
	virtual void _UNKNOWN_C_BaseEntity_097(void);
	virtual void _UNKNOWN_C_BaseEntity_098(void);
	virtual void _UNKNOWN_C_BaseEntity_099(void);
	virtual void _UNKNOWN_C_BaseEntity_100(void);
	virtual void _UNKNOWN_C_BaseEntity_101(void);
	virtual void _UNKNOWN_C_BaseEntity_102(void);
	virtual void _UNKNOWN_C_BaseEntity_103(void); // GetToolRecordingState, If This changes needs update in ClientToolsCS:GO
	virtual void _UNKNOWN_C_BaseEntity_104(void);
	virtual void _UNKNOWN_C_BaseEntity_105(void);
	virtual void _UNKNOWN_C_BaseEntity_106(void);
	virtual void _UNKNOWN_C_BaseEntity_107(void);
	virtual void _UNKNOWN_C_BaseEntity_108(void);
	virtual void _UNKNOWN_C_BaseEntity_109(void);
	virtual void _UNKNOWN_C_BaseEntity_110(void);
	virtual void _UNKNOWN_C_BaseEntity_111(void);
	virtual void _UNKNOWN_C_BaseEntity_112(void);

	virtual float					GetInterpolationAmount(int flags); //:113

	virtual void _UNKNOWN_C_BaseEntity_114(void);
	virtual void _UNKNOWN_C_BaseEntity_115(void);
	virtual void _UNKNOWN_C_BaseEntity_116(void);
	virtual void _UNKNOWN_C_BaseEntity_117(void);
	virtual void _UNKNOWN_C_BaseEntity_118(void);
	virtual void _UNKNOWN_C_BaseEntity_119(void);
	virtual void _UNKNOWN_C_BaseEntity_120(void);
	virtual void _UNKNOWN_C_BaseEntity_121(void);
	virtual void _UNKNOWN_C_BaseEntity_122(void);
	virtual void _UNKNOWN_C_BaseEntity_123(void);
	virtual void _UNKNOWN_C_BaseEntity_124(void);
	virtual void _UNKNOWN_C_BaseEntity_125(void);
	
	virtual bool IsClientCreated( void ) const; //:126
	
	virtual void UpdateOnRemove(void); //:127

	virtual void _UNKNOWN_C_BaseEntity_128(void);

	virtual SOURCESDK::C_BasePlayer_csgo *GetPredictionOwner( void ) = 0; //:129

	virtual void _UNKNOWN_C_BaseEntity_130(void);
	virtual void _UNKNOWN_C_BaseEntity_131(void);
	virtual void _UNKNOWN_C_BaseEntity_132(void);
	virtual void _UNKNOWN_C_BaseEntity_133(void);
	virtual void _UNKNOWN_C_BaseEntity_134(void);
	virtual void _UNKNOWN_C_BaseEntity_135(void);
	virtual void _UNKNOWN_C_BaseEntity_136(void);
	virtual void _UNKNOWN_C_BaseEntity_137(void);

	virtual bool ShouldPredict( void ) = 0; //:138

	virtual void _UNKNOWN_C_BaseEntity_139(void);
	virtual void _UNKNOWN_C_BaseEntity_140(void);
	virtual void _UNKNOWN_C_BaseEntity_141(void);
	virtual void _UNKNOWN_C_BaseEntity_142(void); //:142


	virtual char const				*GetClassname( void ); //:143
	
	virtual char const				*GetDebugName( void ); //:144
	
	virtual const char				*GetPlayerName() const { return 0; } //:145

	virtual void _UNKNOWN_C_BaseEntity_146(void);
	virtual void _UNKNOWN_C_BaseEntity_147(void);
	virtual void _UNKNOWN_C_BaseEntity_148(void);
	virtual void _UNKNOWN_C_BaseEntity_149(void);
	virtual void _UNKNOWN_C_BaseEntity_150(void);
	virtual void _UNKNOWN_C_BaseEntity_151(void);
	virtual void _UNKNOWN_C_BaseEntity_152(void);
	virtual void _UNKNOWN_C_BaseEntity_153(void);
	virtual void _UNKNOWN_C_BaseEntity_154(void);
	virtual void _UNKNOWN_C_BaseEntity_155(void);

	virtual bool					IsAlive(void); //:156

	virtual void _UNKNOWN_C_BaseEntity_157(void);
	
	virtual bool					IsPlayer(void) const { return false; }; //:158

	virtual void _UNKNOWN_C_BaseEntity_159(void);
	virtual void _UNKNOWN_C_BaseEntity_160(void);

	virtual C_BaseCombatCharacter_csgo	*MyCombatCharacterPointer(void) { return NULL; } //:161

	virtual void _UNKNOWN_C_BaseEntity_162(void);
	virtual void _UNKNOWN_C_BaseEntity_163(void);
	virtual void _UNKNOWN_C_BaseEntity_164(void);
	virtual void _UNKNOWN_C_BaseEntity_165(void);
	virtual void _UNKNOWN_C_BaseEntity_166(void);

	virtual C_BaseCombatWeapon_csgo * MyCombatWeaponPointer(void) { return NULL; } //:167
	
	virtual void _UNKNOWN_C_BaseEntity_168(void);

	// Returns the eye point + angles (used for viewing + shooting)
	virtual Vector			EyePosition( void ); // :169
	virtual const QAngle&	EyeAngles( void );		// Direction of eyes // :170
	virtual const QAngle&	LocalEyeAngles( void );	// Direction of eyes in local space (pl.v_angle) // :171

	virtual void _UNKNOWN_C_BaseEntity_172(void);
	virtual void _UNKNOWN_C_BaseEntity_173(void);
	virtual void _UNKNOWN_C_BaseEntity_174(void);
	virtual void _UNKNOWN_C_BaseEntity_175(void);
	virtual void _UNKNOWN_C_BaseEntity_176(void);
	virtual void _UNKNOWN_C_BaseEntity_177(void);
	virtual void _UNKNOWN_C_BaseEntity_178(void);
	virtual void _UNKNOWN_C_BaseEntity_179(void);
	virtual void _UNKNOWN_C_BaseEntity_180(void);
	virtual void _UNKNOWN_C_BaseEntity_181(void);
	virtual void _UNKNOWN_C_BaseEntity_182(void);
	virtual void _UNKNOWN_C_BaseEntity_183(void);
	virtual void _UNKNOWN_C_BaseEntity_184(void);
	virtual void _UNKNOWN_C_BaseEntity_185(void);
	virtual void _UNKNOWN_C_BaseEntity_186(void);
	virtual void _UNKNOWN_C_BaseEntity_187(void);

public:
	const char	*GetEntityName();

	SOURCESDK::CSGO::CBaseHandle AfxGetMoveParentHandle() const;

private:
	char _pad_h00c[0x154 -0x0c];
	char m_iName[260]; //:0x154
	char _pad_h258[0x0b0];
	SOURCESDK::CSGO::CHandle<C_BaseEntity_csgo> m_pMoveParent; //:0x308
};

inline const char *C_BaseEntity_csgo::GetEntityName() 
{ 
	return m_iName; 
}

inline SOURCESDK::CSGO::CBaseHandle C_BaseEntity_csgo::AfxGetMoveParentHandle() const
{
	return m_pMoveParent;
}

class C_BaseAnimating_csgo : public C_BaseEntity_csgo, public IClientModelRenderable_csgo
{
public:
	virtual void _UNKNOWN_C_BaseAnimating_188(void);
	virtual void _UNKNOWN_C_BaseAnimating_189(void);
	virtual void _UNKNOWN_C_BaseAnimating_190(void);
	virtual void _UNKNOWN_C_BaseAnimating_191(void);
	virtual void _UNKNOWN_C_BaseAnimating_192(void);
	virtual void _UNKNOWN_C_BaseAnimating_193(void);
	virtual void _UNKNOWN_C_BaseAnimating_194(void);
	virtual void _UNKNOWN_C_BaseAnimating_195(void);
	virtual void _UNKNOWN_C_BaseAnimating_196(void);
	virtual void _UNKNOWN_C_BaseAnimating_197(void);
	virtual void _UNKNOWN_C_BaseAnimating_198(void);
	virtual void _UNKNOWN_C_BaseAnimating_199(void);

	virtual void FireEvent( const Vector& origin, const QAngle& angles, int event, const char *options ) = 0; //:200

	virtual void _UNKNOWN_C_BaseAnimating_201(void);
	virtual void _UNKNOWN_C_BaseAnimating_202(void);
	virtual void _UNKNOWN_C_BaseAnimating_203(void);
	virtual void _UNKNOWN_C_BaseAnimating_204(void);
	virtual void _UNKNOWN_C_BaseAnimating_205(void);
	virtual void _UNKNOWN_C_BaseAnimating_206(void);
	virtual void _UNKNOWN_C_BaseAnimating_207(void);
	virtual void _UNKNOWN_C_BaseAnimating_208(void);
	virtual void _UNKNOWN_C_BaseAnimating_209(void);
	virtual void _UNKNOWN_C_BaseAnimating_210(void);
	virtual void _UNKNOWN_C_BaseAnimating_211(void);
	virtual void _UNKNOWN_C_BaseAnimating_212(void);
	virtual void _UNKNOWN_C_BaseAnimating_213(void);
	virtual void _UNKNOWN_C_BaseAnimating_214(void);
	virtual void _UNKNOWN_C_BaseAnimating_215(void);
	virtual void _UNKNOWN_C_BaseAnimating_216(void);
	virtual void _UNKNOWN_C_BaseAnimating_217(void);
	virtual void _UNKNOWN_C_BaseAnimating_218(void);
	virtual void _UNKNOWN_C_BaseAnimating_219(void);
	virtual void _UNKNOWN_C_BaseAnimating_220(void);
	virtual void _UNKNOWN_C_BaseAnimating_221(void);
	virtual void _UNKNOWN_C_BaseAnimating_222(void);
	virtual void _UNKNOWN_C_BaseAnimating_223(void);
	virtual void _UNKNOWN_C_BaseAnimating_224(void);
	virtual void _UNKNOWN_C_BaseAnimating_225(void);
	virtual void _UNKNOWN_C_BaseAnimating_226(void);
	virtual void _UNKNOWN_C_BaseAnimating_227(void);
	virtual void _UNKNOWN_C_BaseAnimating_228(void);
	virtual void _UNKNOWN_C_BaseAnimating_229(void);
	virtual void _UNKNOWN_C_BaseAnimating_230(void);
	virtual void _UNKNOWN_C_BaseAnimating_231(void);
	virtual void _UNKNOWN_C_BaseAnimating_232(void);
	virtual void _UNKNOWN_C_BaseAnimating_233(void);
	virtual void _UNKNOWN_C_BaseAnimating_234(void);
	virtual void _UNKNOWN_C_BaseAnimating_235(void);
	virtual void _UNKNOWN_C_BaseAnimating_236(void);
	virtual void _UNKNOWN_C_BaseAnimating_237(void);
	virtual void _UNKNOWN_C_BaseAnimating_238(void);
	virtual void _UNKNOWN_C_BaseAnimating_239(void);
	virtual void _UNKNOWN_C_BaseAnimating_240(void);
	virtual void _UNKNOWN_C_BaseAnimating_241(void);
	virtual void _UNKNOWN_C_BaseAnimating_242(void);
	virtual void _UNKNOWN_C_BaseAnimating_243(void);
	virtual void _UNKNOWN_C_BaseAnimating_244(void);
	virtual void _UNKNOWN_C_BaseAnimating_245(void);
	virtual void _UNKNOWN_C_BaseAnimating_246(void);
	virtual void _UNKNOWN_C_BaseAnimating_247(void);
};

class C_BaseAnimatingOverlay_csgo : public C_BaseAnimating_csgo
{
public:
	virtual void _UNKNOWN_C_BaseAnimatingOverlay_248(void);
	virtual void _UNKNOWN_C_BaseAnimatingOverlay_249(void);
	virtual void _UNKNOWN_C_BaseAnimatingOverlay_250(void);
	virtual void _UNKNOWN_C_BaseAnimatingOverlay_251(void);
};

class C_BaseFlex_csgo : public C_BaseAnimatingOverlay_csgo
{
public:
	virtual void _UNKNOWN_C_BaseFlex_252(void);
	virtual void _UNKNOWN_C_BaseFlex_253(void);
	virtual void _UNKNOWN_C_BaseFlex_254(void);
	virtual void _UNKNOWN_C_BaseFlex_255(void);
	virtual void _UNKNOWN_C_BaseFlex_256(void);
};

class C_BaseCombatWeapon_csgo;

class C_BaseCombatCharacter_csgo : public C_BaseFlex_csgo
{
public:
	virtual void _UNKNOWN_C_BaseCombatChracter_257(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_258(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_259(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_260(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_261(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_262(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_263(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_264(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_265(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_266(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_267(void);

	virtual C_BaseCombatWeapon_csgo* GetActiveWeapon(void) const; //:268

	virtual void _UNKNOWN_C_BaseCombatChracter_269(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_270(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_271(void);
	virtual void _UNKNOWN_C_BaseCombatChracter_272(void);
};

class C_BasePlayer_csgo : public C_BaseCombatCharacter_csgo
{
public:
	virtual void _UNKNOWN_C_BasePlayer_csgo_273(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_274(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_275(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_276(void);
	
	virtual void		CalcView(Vector& eyeOrigin, QAngle& eyeAngles, float& zNear, float& zFar, float& fov) = 0; // :277
	virtual void		CalcViewModelView(const Vector& eyeOrigin, const QAngle& eyeAngles); // :278

	virtual void _UNKNOWN_C_BasePlayer_csgo_279(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_280(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_281(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_282(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_283(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_284(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_285(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_286(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_287(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_288(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_289(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_290(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_291(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_292(void);
	virtual void _UNKNOWN_C_BasePlayer_csgo_293(void);

	virtual int			GetObserverMode() const; //:294
	virtual C_BaseEntity_csgo	*GetObserverTarget() const; //:295

	// ... more we don't care about
};

// Weapon m_iState
#define SOURCESDK_CSGO_WEAPON_NOT_CARRIED				0	// Weapon is on the ground
#define SOURCESDK_CSGO_WEAPON_IS_CARRIED_BY_PLAYER		1	// This client is carrying this weapon.
#define SOURCESDK_CSGO_WEAPON_IS_ACTIVE				2	// This client is carrying this weapon and it's the currently held weapon

class C_BaseCombatWeapon_csgo : public C_BaseAnimating_csgo
{
public:
	typedef SOURCESDK::CSGO::CHandle< C_BaseCombatCharacter_csgo > CBaseCombatCharacterHandle;

	SOURCESDK::CSGO::CBaseHandle AfxGet_m_hOwner()
	{
		CBaseCombatCharacterHandle * pHandle = (CBaseCombatCharacterHandle *)((char *)this + 0x3240);
		return *pHandle;
	}
};

class C_BaseViewModel_csgo : public C_BaseAnimating_csgo
{
public:
	virtual void _UNKNOWN_C_BaseViewModel_csgo_248(void);
	virtual void _UNKNOWN_C_BaseViewModel_csgo_249(void);
	virtual void _UNKNOWN_C_BaseViewModel_csgo_250(void);
	virtual void _UNKNOWN_C_BaseViewModel_csgo_251(void);

	virtual C_BaseEntity_csgo *GetOwner( void ) = 0; //:252
};

namespace CSGO {

#define IClientRenderable IClientRenderable_csgo
#define IBaseFileSystem IBaseFileSystem_csgo

class CUtlBuffer;

// callback to evaluate a $<symbol> during evaluation, return true or false
typedef bool(*GetSymbolProc_t)(const char *pKey);

class IKeyValuesDumpContext;
class IKeyValuesSystem;

//-----------------------------------------------------------------------------
// Purpose: Simple recursive data access class
//			Used in vgui for message parameters and resource files
//			Destructor deletes all child KeyValues nodes
//			Data is stored in key (string names) - (string/int/float)value pairs called nodes.
//
//	About KeyValues Text File Format:

//	It has 3 control characters '{', '}' and '"'. Names and values may be quoted or
//	not. The quote '"' charater must not be used within name or values, only for
//	quoting whole tokens. You may use escape sequences wile parsing and add within a
//	quoted token a \" to add quotes within your name or token. When using Escape
//	Sequence the parser must now that by setting KeyValues::UsesEscapeSequences( true ),
//	which it's off by default. Non-quoted tokens ends with a whitespace, '{', '}' and '"'.
//	So you may use '{' and '}' within quoted tokens, but not for non-quoted tokens.
//  An open bracket '{' after a key name indicates a list of subkeys which is finished
//  with a closing bracket '}'. Subkeys use the same definitions recursively.
//  Whitespaces are space, return, newline and tabulator. Allowed Escape sequences
//	are \n, \t, \\, \n and \". The number character '#' is used for macro purposes 
//	(eg #include), don't use it as first charater in key names.
//-----------------------------------------------------------------------------
class KeyValues
{
public:
/*	KeyValues(const char *setName);

	//
	// AutoDelete class to automatically free the keyvalues.
	// Simply construct it with the keyvalues you allocated and it will free them when falls out of scope.
	// When you decide that keyvalues shouldn't be deleted call Assign(NULL) on it.
	// If you constructed AutoDelete(NULL) you can later assign the keyvalues to be deleted with Assign(pKeyValues).
	//
	class AutoDelete
	{
	public:
		explicit inline AutoDelete(KeyValues *pKeyValues) : m_pKeyValues(pKeyValues) {}
		explicit inline AutoDelete(const char *pchKVName) : m_pKeyValues(new KeyValues(pchKVName)) {}
		inline ~AutoDelete(void) { if (m_pKeyValues) m_pKeyValues->deleteThis(); }
		inline void Assign(KeyValues *pKeyValues) { m_pKeyValues = pKeyValues; }
		KeyValues *operator->() { return m_pKeyValues; }
		operator KeyValues *() { return m_pKeyValues; }
	private:
		AutoDelete(AutoDelete const &x); // forbid
		AutoDelete & operator= (AutoDelete const &x); // forbid
	protected:
		KeyValues *m_pKeyValues;
	};

	//
	// AutoDeleteInline is useful when you want to hold your keyvalues object inside
	// and delete it right after using.
	// You can also pass temporary KeyValues object as an argument to a function by wrapping it into KeyValues::AutoDeleteInline
	// instance:   call_my_function( KeyValues::AutoDeleteInline( new KeyValues( "test" ) ) )
	//
	class AutoDeleteInline : public AutoDelete
	{
	public:
		explicit inline AutoDeleteInline(KeyValues *pKeyValues) : AutoDelete(pKeyValues) {}
		inline operator KeyValues *() const { return m_pKeyValues; }
		inline KeyValues * Get() const { return m_pKeyValues; }
	};

	// Quick setup constructors
	KeyValues(const char *setName, const char *firstKey, const char *firstValue);
	KeyValues(const char *setName, const char *firstKey, const wchar_t *firstValue);
	KeyValues(const char *setName, const char *firstKey, int firstValue);
	KeyValues(const char *setName, const char *firstKey, const char *firstValue, const char *secondKey, const char *secondValue);
	KeyValues(const char *setName, const char *firstKey, int firstValue, const char *secondKey, int secondValue);
*/
	// Section name
	const char *GetName() const;
	void SetName(const char *setName);

	// gets the name as a unique int
	int GetNameSymbol() const;
	int GetNameSymbolCaseSensitive() const;

	// File access. Set UsesEscapeSequences true, if resource file/buffer uses Escape Sequences (eg \n, \t)
	void UsesEscapeSequences(bool state); // default false
	bool LoadFromFile(IBaseFileSystem *filesystem, const char *resourceName, const char *pathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL);
	bool SaveToFile(IBaseFileSystem *filesystem, const char *resourceName, const char *pathID = NULL);

	// Read from a buffer...  Note that the buffer must be null terminated
	bool LoadFromBuffer(char const *resourceName, const char *pBuffer, IBaseFileSystem* pFileSystem = NULL, const char *pPathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL);

	// Read from a utlbuffer...
	bool LoadFromBuffer(char const *resourceName, CUtlBuffer &buf, IBaseFileSystem* pFileSystem = NULL, const char *pPathID = NULL, GetSymbolProc_t pfnEvaluateSymbolProc = NULL);

	// Find a keyValue, create it if it is not found.
	// Set bCreate to true to create the key if it doesn't already exist (which ensures a valid pointer will be returned)
	KeyValues *FindKey(const char *keyName, bool bCreate = false);
	KeyValues *FindKey(int keySymbol) const;
	KeyValues *CreateNewKey();		// creates a new key, with an autogenerated name.  name is guaranteed to be an integer, of value 1 higher than the highest other integer key name
	void AddSubKey(KeyValues *pSubkey);	// Adds a subkey. Make sure the subkey isn't a child of some other keyvalues
	void RemoveSubKey(KeyValues *subKey);	// removes a subkey from the list, DOES NOT DELETE IT
	void InsertSubKey(int nIndex, KeyValues *pSubKey); // Inserts the given sub-key before the Nth child location
	bool ContainsSubKey(KeyValues *pSubKey); // Returns true if this key values contains the specified sub key, false otherwise.
	void SwapSubKey(KeyValues *pExistingSubKey, KeyValues *pNewSubKey);	// Swaps an existing subkey for a new one, DOES NOT DELETE THE OLD ONE but takes ownership of the new one
	void ElideSubKey(KeyValues *pSubKey);	// Removes a subkey but inserts all of its children in its place, in-order (flattens a tree, like firing a manager!)

											// Key iteration.
											//
											// NOTE: GetFirstSubKey/GetNextKey will iterate keys AND values. Use the functions 
											// below if you want to iterate over just the keys or just the values.
											//
	KeyValues *GetFirstSubKey();	// returns the first subkey in the list
	KeyValues *GetNextKey();		// returns the next subkey
	void SetNextKey(KeyValues * pDat);

	//
	// These functions can be used to treat it like a true key/values tree instead of 
	// confusing values with keys.
	//
	// So if you wanted to iterate all subkeys, then all values, it would look like this:
	//     for ( KeyValues *pKey = pRoot->GetFirstTrueSubKey(); pKey; pKey = pKey->GetNextTrueSubKey() )
	//     {
	//		   Msg( "Key name: %s\n", pKey->GetName() );
	//     }
	//     for ( KeyValues *pValue = pRoot->GetFirstValue(); pKey; pKey = pKey->GetNextValue() )
	//     {
	//         Msg( "Int value: %d\n", pValue->GetInt() );  // Assuming pValue->GetDataType() == TYPE_INT...
	//     }
	KeyValues* GetFirstTrueSubKey();
	KeyValues* GetNextTrueSubKey();

	KeyValues* GetFirstValue();	// When you get a value back, you can use GetX and pass in NULL to get the value.
	KeyValues* GetNextValue();


	// Data access
	int   GetInt(const char *keyName = NULL, int defaultValue = 0);
	uint64 GetUint64(const char *keyName = NULL, uint64 defaultValue = 0);
	float GetFloat(const char *keyName = NULL, float defaultValue = 0.0f);
	const char *GetString(const char *keyName = NULL, const char *defaultValue = "");
	const wchar_t *GetWString(const char *keyName = NULL, const wchar_t *defaultValue = L"");
	void *GetPtr(const char *keyName = NULL, void *defaultValue = (void*)0);
	Color GetColor(const char *keyName = NULL, const Color &defaultColor = Color(0, 0, 0, 0));
	bool GetBool(const char *keyName = NULL, bool defaultValue = false) { return GetInt(keyName, defaultValue ? 1 : 0) ? true : false; }
	bool  IsEmpty(const char *keyName = NULL);

	// Data access
	int   GetInt(int keySymbol, int defaultValue = 0);
	uint64 GetUint64(int keySymbol, uint64 defaultValue = 0);
	float GetFloat(int keySymbol, float defaultValue = 0.0f);
	const char *GetString(int keySymbol, const char *defaultValue = "");
	const wchar_t *GetWString(int keySymbol, const wchar_t *defaultValue = L"");
	void *GetPtr(int keySymbol, void *defaultValue = (void*)0);
	Color GetColor(int keySymbol /* default value is all black */);
	bool GetBool(int keySymbol, bool defaultValue = false) { return GetInt(keySymbol, defaultValue ? 1 : 0) ? true : false; }
	bool  IsEmpty(int keySymbol);

	// Key writing
	void SetWString(const char *keyName, const wchar_t *value);
	void SetString(const char *keyName, const char *value);
	void SetInt(const char *keyName, int value);
	void SetUint64(const char *keyName, uint64 value);
	void SetFloat(const char *keyName, float value);
	void SetPtr(const char *keyName, void *value);
	void SetColor(const char *keyName, Color value);
	void SetBool(const char *keyName, bool value) { SetInt(keyName, value ? 1 : 0); }

	// Memory allocation (optimized)
	void *operator new(size_t iAllocSize);
	void *operator new(size_t iAllocSize, int nBlockUse, const char *pFileName, int nLine);
	void operator delete(void *pMem);
	void operator delete(void *pMem, int nBlockUse, const char *pFileName, int nLine);

	KeyValues& operator=(KeyValues& src);

	// Adds a chain... if we don't find stuff in this keyvalue, we'll look
	// in the one we're chained to.
	void ChainKeyValue(KeyValues* pChain);

	void RecursiveSaveToFile(CUtlBuffer& buf, int indentLevel);

	bool WriteAsBinary(CUtlBuffer &buffer);
	bool ReadAsBinary(CUtlBuffer &buffer);

	// Allocate & create a new copy of the keys
	KeyValues *MakeCopy(void) const;

	// Make a new copy of all subkeys, add them all to the passed-in keyvalues
	void CopySubkeys(KeyValues *pParent) const;

	// Clear out all subkeys, and the current value
	void Clear(void);

	// Data type
	enum types_t
	{
		TYPE_NONE = 0,
		TYPE_STRING,
		TYPE_INT,
		TYPE_FLOAT,
		TYPE_PTR,
		TYPE_WSTRING,
		TYPE_COLOR,
		TYPE_UINT64,
		TYPE_COMPILED_INT_BYTE,			// hack to collapse 1 byte ints in the compiled format
		TYPE_COMPILED_INT_0,			// hack to collapse 0 in the compiled format
		TYPE_COMPILED_INT_1,			// hack to collapse 1 in the compiled format
		TYPE_NUMTYPES,
	};
	types_t GetDataType(const char *keyName = NULL);

	// Virtual deletion function - ensures that KeyValues object is deleted from correct heap
	void deleteThis();

	void SetStringValue(char const *strValue);

	// unpack a key values list into a structure
	void UnpackIntoStructure(struct KeyValuesUnpackStructure const *pUnpackTable, void *pDest);

	// Process conditional keys for widescreen support.
	bool ProcessResolutionKeys(const char *pResString);

	// Dump keyvalues recursively into a dump context
	bool Dump(IKeyValuesDumpContext *pDump, int nIndentLevel = 0);

	// Merge operations describing how two keyvalues can be combined
	enum MergeKeyValuesOp_t
	{
		MERGE_KV_ALL,
		MERGE_KV_UPDATE,	// update values are copied into storage, adding new keys to storage or updating existing ones
		MERGE_KV_DELETE,	// update values specify keys that get deleted from storage
		MERGE_KV_BORROW,	// update values only update existing keys in storage, keys in update that do not exist in storage are discarded
	};
	void MergeFrom(KeyValues *kvMerge, MergeKeyValuesOp_t eOp = MERGE_KV_ALL);

	// Assign keyvalues from a string
	static KeyValues * FromString(char const *szName, char const *szStringVal, char const **ppEndOfParse = NULL);

private:
	KeyValues(KeyValues&);	// prevent copy constructor being used

							// prevent delete being called except through deleteThis()
	~KeyValues();

	KeyValues* CreateKey(const char *keyName);

	void RecursiveCopyKeyValues(KeyValues& src);
	void RemoveEverything();
	//	void RecursiveSaveToFile( IBaseFileSystem *filesystem, CUtlBuffer &buffer, int indentLevel );
	//	void WriteConvertedString( CUtlBuffer &buffer, const char *pszString );

	// NOTE: If both filesystem and pBuf are non-null, it'll save to both of them.
	// If filesystem is null, it'll ignore f.
	void RecursiveSaveToFile(IBaseFileSystem *filesystem, FileHandle_t f, CUtlBuffer *pBuf, int indentLevel);
	void WriteConvertedString(IBaseFileSystem *filesystem, FileHandle_t f, CUtlBuffer *pBuf, const char *pszString);

	void RecursiveLoadFromBuffer(char const *resourceName, CUtlBuffer &buf, GetSymbolProc_t pfnEvaluateSymbolProc);

	// for handling #include "filename"
	void AppendIncludedKeys(CUtlVector< KeyValues * >& includedKeys);
	void ParseIncludedKeys(char const *resourceName, const char *filetoinclude,
		IBaseFileSystem* pFileSystem, const char *pPathID, CUtlVector< KeyValues * >& includedKeys, GetSymbolProc_t pfnEvaluateSymbolProc);

	// For handling #base "filename"
	void MergeBaseKeys(CUtlVector< KeyValues * >& baseKeys);
	void RecursiveMergeKeyValues(KeyValues *baseKV);

	// NOTE: If both filesystem and pBuf are non-null, it'll save to both of them.
	// If filesystem is null, it'll ignore f.
	void InternalWrite(IBaseFileSystem *filesystem, FileHandle_t f, CUtlBuffer *pBuf, const void *pData, int len);

	void Init();
	const char * ReadToken(CUtlBuffer &buf, bool &wasQuoted, bool &wasConditional);
	void WriteIndents(IBaseFileSystem *filesystem, FileHandle_t f, CUtlBuffer *pBuf, int indentLevel);

	void FreeAllocatedValue();
	void AllocateValueBlock(int size);

	bool ReadAsBinaryPooledFormat(CUtlBuffer &buf, IBaseFileSystem *pFileSystem, unsigned int poolKey, GetSymbolProc_t pfnEvaluateSymbolProc);

	bool EvaluateConditional(const char *pExpressionString, GetSymbolProc_t pfnEvaluateSymbolProc);

	uint32 m_iKeyName : 24;	// keyname is a symbol defined in KeyValuesSystem
	uint32 m_iKeyNameCaseSensitive1 : 8;	// 1st part of case sensitive symbol defined in KeyValueSystem

											// These are needed out of the union because the API returns string pointers
	char *m_sValue;
	wchar_t *m_wsValue;

	// we don't delete these
	union
	{
		int m_iValue;
		float m_flValue;
		void *m_pValue;
		unsigned char m_Color[4];
	};

	char	   m_iDataType;
	char	   m_bHasEscapeSequences; // true, if while parsing this KeyValue, Escape Sequences are used (default false)
	uint16	   m_iKeyNameCaseSensitive2;	// 2nd part of case sensitive symbol defined in KeyValueSystem;

	IKeyValuesSystem *m_pKeyValuesSystem;
	char m_bOwnsCustomKeyValuesSystem;
	char _padding_3[3];	

	KeyValues *m_pPeer;	// pointer to next key in list
	KeyValues *m_pSub;	// pointer to Start of a new sub key list
	KeyValues *m_pChain;// Search here if it's not in our list
};

//typedef KeyValues::AutoDelete KeyValuesAD;

enum KeyValuesUnpackDestinationTypes_t
{
	UNPACK_TYPE_FLOAT,										// dest is a float
	UNPACK_TYPE_VECTOR,										// dest is a Vector
	UNPACK_TYPE_VECTOR_COLOR,								// dest is a vector, src is a color
	UNPACK_TYPE_STRING,										// dest is a char *. unpacker will allocate.
	UNPACK_TYPE_INT,										// dest is an int
	UNPACK_TYPE_FOUR_FLOATS,	 // dest is an array of 4 floats. source is a string like "1 2 3 4"
	UNPACK_TYPE_TWO_FLOATS,		 // dest is an array of 2 floats. source is a string like "1 2"
};

#define UNPACK_FIXED( kname, kdefault, dtype, ofs ) { kname, kdefault, dtype, ofs, 0 }
#define UNPACK_VARIABLE( kname, kdefault, dtype, ofs, sz ) { kname, kdefault, dtype, ofs, sz }
#define UNPACK_END_MARKER { NULL, NULL, UNPACK_TYPE_FLOAT, 0 }

struct KeyValuesUnpackStructure
{
	char const *m_pKeyName;									// null to terminate tbl
	char const *m_pKeyDefault;								// null ok
	KeyValuesUnpackDestinationTypes_t m_eDataType;			// UNPACK_TYPE_INT, ..
	size_t m_nFieldOffset;									// use offsetof to set
	size_t m_nFieldSize;									// for strings or other variable length
};

//-----------------------------------------------------------------------------
// inline methods
//-----------------------------------------------------------------------------
inline int   KeyValues::GetInt(int keySymbol, int defaultValue)
{
	KeyValues *dat = FindKey(keySymbol);
	return dat ? dat->GetInt((const char *)NULL, defaultValue) : defaultValue;
}

inline uint64 KeyValues::GetUint64(int keySymbol, uint64 defaultValue)
{
	KeyValues *dat = FindKey(keySymbol);
	return dat ? dat->GetUint64((const char *)NULL, defaultValue) : defaultValue;
}

inline float KeyValues::GetFloat(int keySymbol, float defaultValue)
{
	KeyValues *dat = FindKey(keySymbol);
	return dat ? dat->GetFloat((const char *)NULL, defaultValue) : defaultValue;
}

inline const char *KeyValues::GetString(int keySymbol, const char *defaultValue)
{
	KeyValues *dat = FindKey(keySymbol);
	return dat ? dat->GetString((const char *)NULL, defaultValue) : defaultValue;
}

inline const wchar_t *KeyValues::GetWString(int keySymbol, const wchar_t *defaultValue)
{
	KeyValues *dat = FindKey(keySymbol);
	return dat ? dat->GetWString((const char *)NULL, defaultValue) : defaultValue;
}

inline void *KeyValues::GetPtr(int keySymbol, void *defaultValue)
{
	KeyValues *dat = FindKey(keySymbol);
	return dat ? dat->GetPtr((const char *)NULL, defaultValue) : defaultValue;
}

inline Color KeyValues::GetColor(int keySymbol)
{
	Color defaultValue(0, 0, 0, 0);
	KeyValues *dat = FindKey(keySymbol);
	return dat ? dat->GetColor() : defaultValue;
}

inline bool  KeyValues::IsEmpty(int keySymbol)
{
	KeyValues *dat = FindKey(keySymbol);
	return dat ? dat->IsEmpty() : true;
}


//
// KeyValuesDumpContext and generic implementations
//

class IKeyValuesDumpContext
{
public:
	virtual bool KvBeginKey(KeyValues *pKey, int nIndentLevel) = 0;
	virtual bool KvWriteValue(KeyValues *pValue, int nIndentLevel) = 0;
	virtual bool KvEndKey(KeyValues *pKey, int nIndentLevel) = 0;
};

class IKeyValuesDumpContextAsText : public IKeyValuesDumpContext
{
public:
	virtual bool KvBeginKey(KeyValues *pKey, int nIndentLevel);
	virtual bool KvWriteValue(KeyValues *pValue, int nIndentLevel);
	virtual bool KvEndKey(KeyValues *pKey, int nIndentLevel);

public:
	virtual bool KvWriteIndent(int nIndentLevel);
	virtual bool KvWriteText(char const *szText) = 0;
};

class CKeyValuesDumpContextAsDevMsg : public IKeyValuesDumpContextAsText
{
public:
	// Overrides developer level to dump in DevMsg, zero to dump as Msg
	CKeyValuesDumpContextAsDevMsg(int nDeveloperLevel = 1) : m_nDeveloperLevel(nDeveloperLevel) {}

public:
	virtual bool KvBeginKey(KeyValues *pKey, int nIndentLevel);
	virtual bool KvWriteText(char const *szText);

protected:
	int m_nDeveloperLevel;
};

inline bool KeyValuesDumpAsDevMsg(KeyValues *pKeyValues, int nIndentLevel = 0, int nDeveloperLevel = 1)
{
	CKeyValuesDumpContextAsDevMsg ctx(nDeveloperLevel);
	return pKeyValues->Dump(&ctx, nIndentLevel);
}

struct FlashlightState_t
{
	FlashlightState_t()
	{
		throw "not implemented";
	}
};

//-----------------------------------------------------------------------------
// Indicates the type of translucency of an unmodulated renderable
//-----------------------------------------------------------------------------
enum RenderableTranslucencyType_t
{
	RENDERABLE_IS_OPAQUE = 0,
	RENDERABLE_IS_TRANSLUCENT,
	RENDERABLE_IS_TWO_PASS,	// has both translucent and opaque sub-partsa
};

enum RenderableModelType_t
{
	RENDERABLE_MODEL_UNKNOWN_TYPE = -1,
	RENDERABLE_MODEL_ENTITY = 0,
	RENDERABLE_MODEL_STUDIOMDL,
	RENDERABLE_MODEL_STATIC_PROP,
	RENDERABLE_MODEL_BRUSH,
};

//-----------------------------------------------------------------------------
// Handles to a client shadow
//-----------------------------------------------------------------------------
typedef unsigned short ClientShadowHandle_t;

enum
{
	CLIENTSHADOW_INVALID_HANDLE = (ClientShadowHandle_t)~0
};

//-----------------------------------------------------------------------------
// Safe accessor to an entity
//-----------------------------------------------------------------------------
typedef unsigned int HTOOLHANDLE;
enum
{
	HTOOLHANDLE_INVALID = 0
};

//-----------------------------------------------------------------------------
// Flags for the creation method
//-----------------------------------------------------------------------------
enum ShadowFlags_t
{
	SHADOW_FLAGS_FLASHLIGHT = (1 << 0),
	SHADOW_FLAGS_SHADOW = (1 << 1),
	SHADOW_FLAGS_SIMPLE_PROJECTION = (1 << 2),

	// Update this if you add flags
	SHADOW_FLAGS_LAST_FLAG = SHADOW_FLAGS_SIMPLE_PROJECTION
};

//-----------------------------------------------------------------------------
// If you change this, change the flags in IClientShadowMgr.h also
//-----------------------------------------------------------------------------
enum ClientShadowFlags_t
{
	SHADOW_FLAGS_USE_RENDER_TO_TEXTURE = (SHADOW_FLAGS_LAST_FLAG << 1),
	SHADOW_FLAGS_ANIMATING_SOURCE = (SHADOW_FLAGS_LAST_FLAG << 2),
	SHADOW_FLAGS_USE_DEPTH_TEXTURE = (SHADOW_FLAGS_LAST_FLAG << 3),
	SHADOW_FLAGS_CUSTOM_DRAW = (SHADOW_FLAGS_LAST_FLAG << 4),
	// Update this if you add flags
	CLIENT_SHADOW_FLAGS_LAST_FLAG = SHADOW_FLAGS_CUSTOM_DRAW
};


//-----------------------------------------------------------------------------
// Opaque pointer returned from Find* methods, don't store this, you need to 
// Attach it to a tool entity or discard after searching
//-----------------------------------------------------------------------------
typedef void *EntitySearchResult;
typedef void *ParticleSystemSearchResult;


//-----------------------------------------------------------------------------
// Purpose: Client side tool interace (right now just handles IClientRenderables).
//  In theory could support hooking into client side entities directly
//-----------------------------------------------------------------------------
class IClientTools : public IBaseInterface
{
public:
	// Allocates or returns the handle to an entity previously found using the Find* APIs below
	virtual HTOOLHANDLE		AttachToEntity(EntitySearchResult entityToAttach) = 0;
	virtual void			DetachFromEntity(EntitySearchResult entityToDetach) = 0;

	virtual EntitySearchResult	GetEntity(HTOOLHANDLE handle) = 0;

	// Checks whether a handle is still valid.
	virtual bool			IsValidHandle(HTOOLHANDLE handle) = 0;

	// Iterates the list of entities which have been associated with tools
	virtual int				GetNumRecordables() = 0;
	virtual HTOOLHANDLE		GetRecordable(int index) = 0;

	// Iterates through ALL entities (separate list for client vs. server)
	virtual EntitySearchResult	NextEntity(EntitySearchResult currentEnt) = 0;
	EntitySearchResult			FirstEntity() { return NextEntity(NULL); }

	// Use this to turn on/off the presence of an underlying game entity
	virtual void			SetEnabled(HTOOLHANDLE handle, bool enabled) = 0;

	// Use this to tell an entity to post "state" to all listening tools
	virtual void			SetRecording(HTOOLHANDLE handle, bool recording) = 0;

	// Some entities are marked with ShouldRecordInTools false, such as ui entities, etc.
	virtual bool			ShouldRecord(HTOOLHANDLE handle) = 0;

	virtual HTOOLHANDLE		GetToolHandleForEntityByIndex(int entindex) = 0;

	virtual int				GetModelIndex(HTOOLHANDLE handle) = 0;// sub_105C8F90?
	virtual const char*		GetModelName(HTOOLHANDLE handle) = 0;
	virtual const char*		GetClassname(HTOOLHANDLE handle) = 0;

	virtual void			AddClientRenderable(IClientRenderable *pRenderable, bool bDrawWithViewModels, RenderableTranslucencyType_t nType, RenderableModelType_t nModelType = RENDERABLE_MODEL_UNKNOWN_TYPE) = 0;
	virtual void			RemoveClientRenderable(IClientRenderable *pRenderable) = 0;
	virtual void			SetTranslucencyType(IClientRenderable *pRenderable, RenderableTranslucencyType_t nType) = 0;
	virtual void			MarkClientRenderableDirty(IClientRenderable *pRenderable) = 0;
	virtual void			UpdateProjectedTexture(ClientShadowHandle_t h, bool bForce) = 0;

	virtual bool			DrawSprite(IClientRenderable *pRenderable, float scale, float frame, int rendermode, int renderfx, const Color &color, float flProxyRadius, int *pVisHandle) = 0;
	virtual void			DrawSprite(const Vector &vecOrigin, float flWidth, float flHeight, color32 color) = 0;

	virtual EntitySearchResult	GetLocalPlayer() = 0;
	virtual bool			GetLocalPlayerEyePosition(Vector& org, QAngle& ang, float &fov) = 0;

	// See ClientShadowFlags_t above
	virtual ClientShadowHandle_t CreateShadow(CBaseHandle handle, int nFlags) = 0;
	virtual void			DestroyShadow(ClientShadowHandle_t h) = 0;

	virtual ClientShadowHandle_t CreateFlashlight(const FlashlightState_t &lightState) = 0;
	virtual void			DestroyFlashlight(ClientShadowHandle_t h) = 0;
	virtual void			UpdateFlashlightState(ClientShadowHandle_t h, const FlashlightState_t &lightState) = 0;

	virtual void			AddToDirtyShadowList(ClientShadowHandle_t h, bool force = false) = 0;
	virtual void			MarkRenderToTextureShadowDirty(ClientShadowHandle_t h) = 0;

	// Global toggle for recording
	virtual void			EnableRecordingMode(bool bEnable) = 0;
	virtual bool			IsInRecordingMode() const = 0;

	// Trigger a temp entity
	virtual void			TriggerTempEntity(KeyValues *pKeyValues) = 0;

	// get owning weapon (for viewmodels)
	virtual int				GetOwningWeaponEntIndex(int entindex) = 0;
	virtual int				GetEntIndex(EntitySearchResult entityToAttach) = 0;

	virtual int				FindGlobalFlexcontroller(char const *name) = 0;
	virtual char const		*GetGlobalFlexControllerName(int idx) = 0;

	// helper for traversing ownership hierarchy
	virtual EntitySearchResult	GetOwnerEntity(EntitySearchResult currentEnt) = 0;

	// common and useful types to query for hierarchically
	virtual bool			IsPlayer(EntitySearchResult currentEnt) = 0;
	virtual bool			IsCombatCharacter(EntitySearchResult currentEnt) = 0;
	virtual bool			IsNPC(EntitySearchResult currentEnt) = 0;
	virtual bool			IsRagdoll(EntitySearchResult currentEnt) = 0;
	virtual bool			IsViewModel(EntitySearchResult currentEnt) = 0;
	virtual bool			IsViewModelOrAttachment(EntitySearchResult currentEnt) = 0;
	virtual bool			IsWeapon(EntitySearchResult currentEnt) = 0;
	virtual bool			IsSprite(EntitySearchResult currentEnt) = 0;
	virtual bool			IsProp(EntitySearchResult currentEnt) = 0;
	virtual bool			IsBrush(EntitySearchResult currentEnt) = 0;

	virtual Vector			GetAbsOrigin(HTOOLHANDLE handle) = 0;
	virtual QAngle			GetAbsAngles(HTOOLHANDLE handle) = 0;

	// This reloads a portion or all of a particle definition file.
	// It's up to the client to decide if it cares about this file
	// Use a UtlBuffer to crack the data
	virtual void			ReloadParticleDefintions(const char *pFileName, const void *pBufData, int nLen) = 0;

	// ParticleSystem iteration, query, modification
	virtual ParticleSystemSearchResult	FirstParticleSystem() { return NextParticleSystem(NULL); }
	virtual ParticleSystemSearchResult	NextParticleSystem(ParticleSystemSearchResult sr) = 0;
	virtual void						SetRecording(ParticleSystemSearchResult sr, bool bRecord) = 0;

	// Sends a mesage from the tool to the client
	virtual void			PostToolMessage(KeyValues *pKeyValues) = 0; //55

	// Indicates whether the client should render particle systems
	virtual void			EnableParticleSystems(bool bEnable) = 0; //56

	// Is the game rendering in 3rd person mode?
	virtual bool			IsRenderingThirdPerson() const = 0; //57
};

#define SOURCESDK_CSGO_VCLIENTTOOLS_INTERFACE_VERSION "VCLIENTTOOLS001"


// handle to a KeyValues key name symbol
typedef int HKeySymbol;
#define SOURCESDK_INVALID_KEY_SYMBOL (-1)

//-----------------------------------------------------------------------------
// Purpose: Interface to shared data repository for KeyValues (included in vgui_controls.lib)
//			allows for central data storage point of KeyValues symbol table
//-----------------------------------------------------------------------------
class IKeyValuesSystem
{
public:
	virtual ~IKeyValuesSystem() = 0;

	// registers the size of the KeyValues in the specified instance
	// so it can build a properly sized memory pool for the KeyValues objects
	// the sizes will usually never differ but this is for versioning safety
	virtual void RegisterSizeofKeyValues(int size) = 0;

	// allocates/frees a KeyValues object from the shared mempool
	virtual void *AllocKeyValuesMemory(int size) = 0;
	virtual void FreeKeyValuesMemory(void *pMem) = 0;

	// symbol table access (used for key names)
	virtual HKeySymbol GetSymbolForString(const char *name, bool bCreate = true) = 0;
	virtual const char *GetStringForSymbol(HKeySymbol symbol) = 0;

	// for debugging, adds KeyValues record into global list so we can track memory leaks
	virtual void AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name) = 0;
	virtual void RemoveKeyValuesFromMemoryLeakList(void *pMem) = 0;
};

IKeyValuesSystem *KeyValuesSystem();


#define SOURCESDK_CSGO_MAXSTUDIOPOSEPARAM	24

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IToolSystem;
struct SpatializationInfo_t;
class KeyValues;
class CBoneList;


//-----------------------------------------------------------------------------
// Standard messages
//-----------------------------------------------------------------------------
struct EffectRecordingState_t
{
	bool m_bVisible : 1;
	bool m_bThirdPerson : 1;
	Color m_Color;
	float m_Scale;
	const char *m_pMaterialName;
	int m_nAttachment;
	Vector m_vecAttachment; // only used if m_nAttachment is -1
};

struct BaseEntityRecordingState_t
{
	BaseEntityRecordingState_t() :
		m_flTime(0.0f),
		m_pModelName(0),
		m_nOwner(-1),
		m_fEffects(0),
		m_bVisible(false),
		m_bRecordFinalVisibleSample(false),
		m_numEffects(0),
		m_pEffects(NULL),
		m_nFollowEntity(-1)
	{
		m_vecRenderOrigin.Init();
		m_vecRenderAngles.Init();
	}

	float m_flTime;
	const char *m_pModelName;
	int m_nOwner;
	int m_fEffects;
	bool m_bVisible : 1;
	bool m_bRecordFinalVisibleSample : 1;
	Vector m_vecRenderOrigin;
	QAngle m_vecRenderAngles;
	int m_nFollowEntity;

	int m_numEffects;
	EffectRecordingState_t *m_pEffects;
};

struct SpriteRecordingState_t
{
	float m_flRenderScale;
	float m_flFrame;
	int m_nRenderMode;
	bool m_nRenderFX;
	Color m_Color;
	float m_flProxyRadius;
};

struct BaseAnimatingHighLevelRecordingState_t
{
	BaseAnimatingHighLevelRecordingState_t()
		: m_bClearIkTargets(false),
		m_bIsRagdoll(false),
		m_bShouldCreateIkContext(false),
		m_nNumPoseParams(0),
		m_flCycle(0.0f),
		m_flPlaybackRate(1.0f),
		m_flCycleRate(0.0f),
		m_nFrameCount(0),
		m_bInterpEffectActive(false)
	{
	}

	bool m_bClearIkTargets;
	bool m_bIsRagdoll;
	bool m_bShouldCreateIkContext;
	int m_nNumPoseParams;

	float m_flCycle;
	float m_flPlaybackRate;
	float m_flCycleRate;
	int m_nFrameCount;

	float m_flPoseParameter[SOURCESDK_CSGO_MAXSTUDIOPOSEPARAM];

	bool m_bInterpEffectActive;
};

struct BaseAnimatingRecordingState_t
{
	BaseAnimatingHighLevelRecordingState_t m_highLevelState;

	int m_nSkin;
	int m_nBody;
	int m_nSequence;
	CBoneList *m_pBoneList;
};

struct BaseFlexRecordingState_t
{
	int m_nFlexCount;
	float *m_pDestWeight;
	Vector m_vecViewTarget;
};

struct CameraRecordingState_t
{
	bool m_bThirdPerson;
	float m_flFOV;
	Vector m_vecEyePosition;
	QAngle m_vecEyeAngles;
};

struct MonitorRecordingState_t
{
	bool	m_bActive;
	float	m_flFOV;
	bool	m_bFogEnabled;
	float	m_flFogStart;
	float	m_flFogEnd;
	Color	m_FogColor;
};

struct EntityTeleportedRecordingState_t
{
	Vector m_vecTo;
	QAngle m_qaTo;
	bool m_bTeleported;
	bool m_bViewOverride;
	matrix3x4_t m_teleportMatrix;
};

struct PortalRecordingState_t
{
	int				m_nPortalId;
	int				m_nLinkedPortalId;
	float			m_fStaticAmount;
	float			m_fSecondaryStaticAmount;
	float			m_fOpenAmount;
	bool			m_bIsPortal2; //for any set of portals, one must be portal 1, and the other portal 2. Uses different render targets
};

struct ParticleSystemCreatedState_t
{
	int				m_nParticleSystemId;
	const char *	m_pName;
	float			m_flTime;
	int				m_nOwner;
};

struct ParticleSystemDestroyedState_t
{
	int				m_nParticleSystemId;
	float			m_flTime;
};

struct ParticleSystemStopEmissionState_t
{
	int				m_nParticleSystemId;
	float			m_flTime;
	bool			m_bInfiniteOnly;
};

struct ParticleSystemSetControlPointObjectState_t
{
	int				m_nParticleSystemId;
	float			m_flTime;
	int				m_nControlPoint;
	int				m_nObject;
};

struct ParticleSystemSetControlPointPositionState_t
{
	int				m_nParticleSystemId;
	float			m_flTime;
	int				m_nControlPoint;
	Vector			m_vecPosition;
};

struct ParticleSystemSetControlPointOrientationState_t
{
	int				m_nParticleSystemId;
	float			m_flTime;
	int				m_nControlPoint;
	Quaternion		m_qOrientation;
};

#define SOURCESDK_CSGO_MAXSTUDIOBONES		256		// total bones actually used

class CBoneList
{
public:

	CBoneList();

	void Release();

	static CBoneList *Alloc();

	unsigned int GetWriteSize() const
	{
		return 2 + m_nBones * (sizeof(Vector) + sizeof(Quaternion));
	}

	// The order of these data members must be maintained in order for the server
	// demo system.  ServerDemoPacket_BaseAnimating::GetSize() depends on this.

private:
	bool		m_bShouldDelete : 1;

public:
	uint16		m_nBones : 15;
	Vector		m_vecPos[SOURCESDK_CSGO_MAXSTUDIOBONES];
	Quaternion	m_quatRot[SOURCESDK_CSGO_MAXSTUDIOBONES];
};


#undef IBaseFileSystem
#undef CBaseHandle
#undef IClientRenderable

//-----------------------------------------------------------------------------
// Purpose:	Do a an inline AddRef then return the pointer, useful when
//			returning an object from a function
//-----------------------------------------------------------------------------

#define SOURCESDK_CSGO_RetAddRef( p ) ( (p)->AddRef(), (p) )
#define SOURCESDK_CSGO_InlineAddRef( p ) ( (p)->AddRef(), (p) )

class CFunctor abstract : public IRefCounted_csgo
{
public:
	CFunctor()
	{
//#ifdef DEBUG
		m_nUserID = 0;
//#endif
	}

	virtual ~CFunctor() // new in CS:GO // :002
	{
		
	}

	virtual void operator()() = 0; // :003

	unsigned m_nUserID; // For debugging
};

class ICallQueue
{
public:
	void QueueFunctor(CFunctor *pFunctor)
	{
		QueueFunctorInternal(SOURCESDK_CSGO_RetAddRef(pFunctor));
	}

	// FUCK THESE: //FUNC_GENERATE_QUEUE_METHODS();

// private: // this won't do
public:
	virtual void QueueFunctorInternal(CFunctor *pFunctor) = 0;
};

class IGameEvent abstract
{
public:
	virtual ~IGameEvent() {};
	virtual const char *GetName() const = 0;	// get event name

	virtual bool IsReliable() const = 0; // if event handled reliable
	virtual bool IsLocal() const = 0; // if event is never networked
	virtual bool IsEmpty(const char *keyName = NULL) = 0; // check if data field exists

														  // Data access
	virtual bool GetBool(const char *keyName = NULL, bool defaultValue = false) = 0;
	virtual int GetInt(const char *keyName = NULL, int defaultValue = 0) = 0;
	virtual uint64 GetUint64(const char *keyName = NULL, uint64 defaultValue = 0) = 0;
	virtual float GetFloat(const char *keyName = NULL, float defaultValue = 0.0f) = 0;
	virtual const char *GetString(const char *keyName = NULL, const char *defaultValue = "") = 0;

	virtual void SetBool(const char *keyName, bool value) = 0;
	virtual void SetInt(const char *keyName, int value) = 0;
	virtual void SetUint64(const char *keyName, uint64 value) = 0;
	virtual void SetFloat(const char *keyName, float value) = 0;
	virtual void SetString(const char *keyName, const char *value) = 0;
};

class CGameEventDescriptor;

class CGameEvent : public IGameEvent
{
public:
	CGameEventDescriptor	*m_pDescriptor;
	KeyValues				*m_pDataKeys;
};

//-----------------------------------------------------------------------------
// Purpose: Engine interface into client side prediction system
//-----------------------------------------------------------------------------
class IPrediction abstract
{
public:
	virtual			~IPrediction(void) {};

	virtual void	Init(void) = 0;
	virtual void	Shutdown(void) = 0;

	// Run prediction
	virtual void	Update
	(
		int startframe,				// World update ( un-modded ) most recently received
		bool validframe,			// Is frame data valid
		int incoming_acknowledged,	// Last command acknowledged to have been run by server (un-modded)
		int outgoing_command		// Last command (most recent) sent to server (un-modded)
	) = 0;

	// We are about to get a network update from the server.  We know the update #, so we can pull any
	//  data purely predicted on the client side and transfer it to the new from data state.
	virtual void	PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet) = 0;
	virtual void	PostEntityPacketReceived(void) = 0;
	virtual void	PostNetworkDataReceived(int commands_acknowledged) = 0;

	virtual void	OnReceivedUncompressedPacket(void) = 0;

	// The engine needs to be able to access a few predicted values
	virtual void	GetViewOrigin(Vector& org) = 0;
	virtual void	SetViewOrigin(Vector& org) = 0;
	virtual void	GetViewAngles(QAngle& ang) = 0;
	virtual void	SetViewAngles(QAngle& ang) = 0;
	virtual void	GetLocalViewAngles(QAngle& ang) = 0;
	virtual void	SetLocalViewAngles(QAngle& ang) = 0;
};

#define SOURCESDK_CSGO_VCLIENT_PREDICTION_INTERFACE_VERSION	"VClientPrediction001"


namespace vgui {

typedef unsigned int VPANEL;
typedef unsigned long HScheme;

class KeyValues;
struct DmxElementUnpackStructure_t;
class CDmxElement;

class SurfacePlat;
class IClientPanel;

//!! must be removed
class Panel;

//-----------------------------------------------------------------------------
// Purpose: interface from Client panels -> vgui panels
//-----------------------------------------------------------------------------
class IPanel : public IBaseInterface
{
public:
	virtual void Init(VPANEL vguiPanel, IClientPanel *panel) = 0;

	// methods
	virtual void SetPos(VPANEL vguiPanel, int x, int y) = 0;
	virtual void GetPos(VPANEL vguiPanel, int &x, int &y) = 0;
	virtual void SetSize(VPANEL vguiPanel, int wide, int tall) = 0;
	virtual void GetSize(VPANEL vguiPanel, int &wide, int &tall) = 0;
	virtual void SetMinimumSize(VPANEL vguiPanel, int wide, int tall) = 0;
	virtual void GetMinimumSize(VPANEL vguiPanel, int &wide, int &tall) = 0;
	virtual void SetZPos(VPANEL vguiPanel, int z) = 0;
	virtual int  GetZPos(VPANEL vguiPanel) = 0;

	virtual void GetAbsPos(VPANEL vguiPanel, int &x, int &y) = 0;
	virtual void GetClipRect(VPANEL vguiPanel, int &x0, int &y0, int &x1, int &y1) = 0;
	virtual void SetInset(VPANEL vguiPanel, int left, int top, int right, int bottom) = 0;
	virtual void GetInset(VPANEL vguiPanel, int &left, int &top, int &right, int &bottom) = 0;

	virtual void SetVisible(VPANEL vguiPanel, bool state) = 0;
	virtual bool IsVisible(VPANEL vguiPanel) = 0;
	virtual void SetParent(VPANEL vguiPanel, VPANEL newParent) = 0;
	virtual int GetChildCount(VPANEL vguiPanel) = 0;
	virtual VPANEL GetChild(VPANEL vguiPanel, int index) = 0;
	virtual CUtlVector< VPANEL > &GetChildren(VPANEL vguiPanel) = 0;
	virtual VPANEL GetParent(VPANEL vguiPanel) = 0;
	virtual void MoveToFront(VPANEL vguiPanel) = 0;
	virtual void MoveToBack(VPANEL vguiPanel) = 0;
	virtual bool HasParent(VPANEL vguiPanel, VPANEL potentialParent) = 0;
	virtual bool IsPopup(VPANEL vguiPanel) = 0;
	virtual void SetPopup(VPANEL vguiPanel, bool state) = 0;
	virtual bool IsFullyVisible(VPANEL vguiPanel) = 0;

	// gets the scheme this panel uses
	virtual HScheme GetScheme(VPANEL vguiPanel) = 0;
	// gets whether or not this panel should scale with screen resolution
	virtual bool IsProportional(VPANEL vguiPanel) = 0;
	// returns true if auto-deletion flag is set
	virtual bool IsAutoDeleteSet(VPANEL vguiPanel) = 0;
	// deletes the Panel * associated with the vpanel
	virtual void DeletePanel(VPANEL vguiPanel) = 0;

	// input interest
	virtual void SetKeyBoardInputEnabled(VPANEL vguiPanel, bool state) = 0;
	virtual void SetMouseInputEnabled(VPANEL vguiPanel, bool state) = 0;
	virtual bool IsKeyBoardInputEnabled(VPANEL vguiPanel) = 0;
	virtual bool IsMouseInputEnabled(VPANEL vguiPanel) = 0;

	// calculates the panels current position within the hierarchy
	virtual void Solve(VPANEL vguiPanel) = 0;

	// gets names of the object (for debugging purposes)
	virtual const char *GetName(VPANEL vguiPanel) = 0;
	virtual const char *GetClassName(VPANEL vguiPanel) = 0;

	// delivers a message to the panel
	virtual void SendMessage(VPANEL vguiPanel, KeyValues *params, VPANEL ifromPanel) = 0;

	// these pass through to the IClientPanel
	virtual void Think(VPANEL vguiPanel) = 0;
	virtual void PerformApplySchemeSettings(VPANEL vguiPanel) = 0;
	virtual void PaintTraverse(VPANEL vguiPanel, bool forceRepaint, bool allowForce = true) = 0;
	virtual void Repaint(VPANEL vguiPanel) = 0;
	virtual VPANEL IsWithinTraverse(VPANEL vguiPanel, int x, int y, bool traversePopups) = 0;
	virtual void OnChildAdded(VPANEL vguiPanel, VPANEL child) = 0;
	virtual void OnSizeChanged(VPANEL vguiPanel, int newWide, int newTall) = 0;

	virtual void InternalFocusChanged(VPANEL vguiPanel, bool lost) = 0;
	virtual bool RequestInfo(VPANEL vguiPanel, KeyValues *outputData) = 0;
	virtual void RequestFocus(VPANEL vguiPanel, int direction = 0) = 0;
	virtual bool RequestFocusPrev(VPANEL vguiPanel, VPANEL existingPanel) = 0;
	virtual bool RequestFocusNext(VPANEL vguiPanel, VPANEL existingPanel) = 0;
	virtual VPANEL GetCurrentKeyFocus(VPANEL vguiPanel) = 0;
	virtual int GetTabPosition(VPANEL vguiPanel) = 0;

	// used by ISurface to store platform-specific data
	virtual SurfacePlat *Plat(VPANEL vguiPanel) = 0;
	virtual void SetPlat(VPANEL vguiPanel, SurfacePlat *Plat) = 0;

	// returns a pointer to the vgui controls baseclass Panel *
	// destinationModule needs to be passed in to verify that the returned Panel * is from the same module
	// it must be from the same module since Panel * vtbl may be different in each module
	virtual Panel *GetPanel(VPANEL vguiPanel, const char *destinationModule) = 0;

	virtual bool IsEnabled(VPANEL vguiPanel) = 0;
	virtual void SetEnabled(VPANEL vguiPanel, bool state) = 0;

	// Used by the drag/drop manager to always draw on top
	virtual bool IsTopmostPopup(VPANEL vguiPanel) = 0;
	virtual void SetTopmostPopup(VPANEL vguiPanel, bool state) = 0;

	virtual void SetMessageContextId(VPANEL vguiPanel, int nContextId) = 0;
	virtual int GetMessageContextId(VPANEL vguiPanel) = 0;

	virtual const DmxElementUnpackStructure_t *GetUnpackStructure(VPANEL vguiPanel) const = 0;
	virtual void OnUnserialized(VPANEL vguiPanel, CDmxElement *pElement) = 0;

	// sibling pins
	virtual void SetSiblingPin(VPANEL vguiPanel, VPANEL newSibling, byte iMyCornerToPin = 0, byte iSiblingCornerToPinTo = 0) = 0;
};

#define SOURCESDK_CSGO_VGUI_PANEL_INTERFACE_VERSION "VGUI_Panel009"

//-----------------------------------------------------------------------------
// Purpose: Wraps contextless windows system functions
//-----------------------------------------------------------------------------
class ISurface abstract : public SOURCESDK::CSGO::IAppSystem
{
public:
	// call to Shutdown surface; surface can no longer be used after this is called
	virtual void Shutdown() = 0; //:004 (overload)

	// frame
	virtual void RunFrame() = 0; //:009

	// hierarchy root
	virtual VPANEL GetEmbeddedPanel() = 0; //:010

	virtual void _ISurface_011(void) abstract = 0;
	virtual void _ISurface_012(void) abstract = 0;
	virtual void _ISurface_013(void) abstract = 0;
	virtual void _ISurface_014(void) abstract = 0;
	virtual void _ISurface_015(void) abstract = 0;
	virtual void _ISurface_016(void) abstract = 0;
	virtual void _ISurface_017(void) abstract = 0;
	virtual void _ISurface_018(void) abstract = 0;
	virtual void _ISurface_019(void) abstract = 0;
	virtual void _ISurface_020(void) abstract = 0;
	virtual void _ISurface_021(void) abstract = 0;
	virtual void _ISurface_022(void) abstract = 0;
	virtual void _ISurface_023(void) abstract = 0;
	virtual void _ISurface_024(void) abstract = 0;
	virtual void _ISurface_025(void) abstract = 0;
	virtual void _ISurface_026(void) abstract = 0;
	virtual void _ISurface_027(void) abstract = 0;
	virtual void _ISurface_028(void) abstract = 0;
	virtual void _ISurface_029(void) abstract = 0;
	virtual void _ISurface_030(void) abstract = 0;
	virtual void _ISurface_031(void) abstract = 0;
	virtual void _ISurface_032(void) abstract = 0;
	virtual void _ISurface_033(void) abstract = 0;
	virtual void _ISurface_034(void) abstract = 0;
	virtual void _ISurface_035(void) abstract = 0;
	virtual void _ISurface_036(void) abstract = 0;
	virtual void _ISurface_037(void) abstract = 0;
	virtual void _ISurface_038(void) abstract = 0;
	virtual void _ISurface_039(void) abstract = 0;
	virtual void _ISurface_040(void) abstract = 0;
	virtual void _ISurface_041(void) abstract = 0;
	virtual void _ISurface_042(void) abstract = 0;
	virtual void _ISurface_043(void) abstract = 0;
	virtual void _ISurface_044(void) abstract = 0;
	virtual void _ISurface_045(void) abstract = 0;
	virtual void _ISurface_046(void) abstract = 0;
	virtual void _ISurface_047(void) abstract = 0;
	virtual void _ISurface_048(void) abstract = 0;
	virtual void _ISurface_049(void) abstract = 0;
	virtual void _ISurface_050(void) abstract = 0;
	virtual void _ISurface_051(void) abstract = 0;
	virtual void _ISurface_052(void) abstract = 0;
	virtual void _ISurface_053(void) abstract = 0;
	virtual void _ISurface_054(void) abstract = 0;
	virtual void _ISurface_055(void) abstract = 0;
	virtual void _ISurface_056(void) abstract = 0;
	virtual void _ISurface_057(void) abstract = 0;
	virtual void _ISurface_058(void) abstract = 0;
	virtual void _ISurface_059(void) abstract = 0;
	virtual void _ISurface_060(void) abstract = 0;
	virtual void _ISurface_061(void) abstract = 0;
	virtual void _ISurface_062(void) abstract = 0;
	virtual void _ISurface_063(void) abstract = 0;
	virtual void _ISurface_064(void) abstract = 0;
	virtual void _ISurface_065(void) abstract = 0;
	
	virtual void UnlockCursor() = 0; //:066
	virtual void LockCursor() = 0; //:067

	virtual void _ISurface_068(void) abstract = 0;
	virtual void _ISurface_069(void) abstract = 0;
	virtual void _ISurface_070(void) abstract = 0;
	virtual void _ISurface_071(void) abstract = 0;
	virtual void _ISurface_072(void) abstract = 0;
	virtual void _ISurface_073(void) abstract = 0;
	virtual void _ISurface_074(void) abstract = 0;
	virtual void _ISurface_075(void) abstract = 0;
	virtual void _ISurface_076(void) abstract = 0;
	virtual void _ISurface_077(void) abstract = 0;
	virtual void _ISurface_078(void) abstract = 0;
	virtual void _ISurface_079(void) abstract = 0;
	virtual void _ISurface_080(void) abstract = 0;
	virtual void _ISurface_081(void) abstract = 0;
	virtual void _ISurface_082(void) abstract = 0;
	virtual void _ISurface_083(void) abstract = 0;
	virtual void _ISurface_084(void) abstract = 0;
	virtual void _ISurface_085(void) abstract = 0;
	virtual void _ISurface_086(void) abstract = 0;
	virtual void _ISurface_087(void) abstract = 0;
	virtual void _ISurface_088(void) abstract = 0;
	virtual void _ISurface_089(void) abstract = 0;
	virtual void _ISurface_090(void) abstract = 0;
	virtual void _ISurface_091(void) abstract = 0;
	virtual void _ISurface_092(void) abstract = 0;
	virtual void EnableMouseCapture(VPANEL panel, bool state) = 0; //:093
	virtual void _ISurface_094(void) abstract = 0;
	virtual void _ISurface_095(void) abstract = 0;
	virtual void _ISurface_096(void) abstract = 0;
	virtual void _ISurface_097(void) abstract = 0;
	virtual void _ISurface_098(void) abstract = 0;
	virtual void _ISurface_099(void) abstract = 0;
	virtual void _ISurface_100(void) abstract = 0;
	virtual void _ISurface_101(void) abstract = 0;
	virtual void _ISurface_102(void) abstract = 0;
	virtual void _ISurface_103(void) abstract = 0;
	virtual void _ISurface_104(void) abstract = 0;
	virtual void _ISurface_105(void) abstract = 0;
	virtual void _ISurface_106(void) abstract = 0;

	virtual const wchar_t *GetTitle(VPANEL panel) = 0; //:107
	virtual bool IsCursorLocked(void) const = 0; //:108
};

#define SOURCESDK_CSGO_VGUI_VGUI_SURFACE_INTERFACE_VERSION "VGUI_Surface031"

} // namespace vgui

//-----------------------------------------------------------------------------
// The standard trace filter... NOTE: Most normal traces inherit from CTraceFilter!!!
//-----------------------------------------------------------------------------
enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};

class __declspec(novtable) ITraceFilter abstract
{
public:
	virtual bool ShouldHitEntity(IHandleEntity *pEntity, int contentsMask) = 0;
	virtual TraceType_t	GetTraceType() const = 0;
};


//-----------------------------------------------------------------------------
// Classes are expected to inherit these + implement the ShouldHitEntity method
//-----------------------------------------------------------------------------

// This is the one most normal traces will inherit from
class CTraceFilter : public ITraceFilter
{
public:
	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
};

class CTraceFilterEntitiesOnly : public ITraceFilter
{
public:
	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_ENTITIES_ONLY;
	}
};


//-----------------------------------------------------------------------------
// Classes need not inherit from these
//-----------------------------------------------------------------------------
class CTraceFilterWorldOnly : public ITraceFilter
{
public:
	bool ShouldHitEntity(IHandleEntity *pServerEntity, int contentsMask)
	{
		return false;
	}
	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_WORLD_ONLY;
	}
};

class CTraceFilterWorldAndPropsOnly : public ITraceFilter
{
public:
	bool ShouldHitEntity(IHandleEntity *pServerEntity, int contentsMask)
	{
		return false;
	}
	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
};

class CTraceFilterHitAll : public CTraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity *pServerEntity, int contentsMask)
	{
		return true;
	}
};

class __declspec(align(16)) VectorAligned : public Vector
{
public:
	float w;	// this space is used anyway
};

SOURCESDK_FORCEINLINE void VectorSubtract(const Vector& a, const Vector& b, Vector& c)
{
	SOURCESDK_CHECK_VALID(a);
	SOURCESDK_CHECK_VALID(b);
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

inline void VectorClear(Vector& a)
{
	a.x = a.y = a.z = 0.0f;
}

SOURCESDK_FORCEINLINE void VectorCopy(const Vector& src, Vector& dst)
{
	SOURCESDK_CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

SOURCESDK_FORCEINLINE void VectorAdd(const Vector& a, const Vector& b, Vector& c)
{
	SOURCESDK_CHECK_VALID(a);
	SOURCESDK_CHECK_VALID(b);
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

enum Collision_Group_t
{
	COLLISION_GROUP_NONE  = 0,
	COLLISION_GROUP_DEBRIS,			// Collides with nothing but world and static stuff
	COLLISION_GROUP_DEBRIS_TRIGGER, // Same as debris, but hits triggers
	COLLISION_GROUP_INTERACTIVE_DEBRIS,	// Collides with everything except other interactive debris or debris
	COLLISION_GROUP_INTERACTIVE,	// Collides with everything except interactive debris or debris
	COLLISION_GROUP_PLAYER,
	COLLISION_GROUP_BREAKABLE_GLASS,
	COLLISION_GROUP_VEHICLE,
	COLLISION_GROUP_PLAYER_MOVEMENT,  // For HL2, same as Collision_Group_Player, for
										// TF2, this filters out other players and CBaseObjects
	COLLISION_GROUP_NPC,			// Generic NPC group
	COLLISION_GROUP_IN_VEHICLE,		// for any entity inside a vehicle
	COLLISION_GROUP_WEAPON,			// for any weapons that need collision detection
	COLLISION_GROUP_VEHICLE_CLIP,	// vehicle clip brush to restrict vehicle movement
	COLLISION_GROUP_PROJECTILE,		// Projectiles!
	COLLISION_GROUP_DOOR_BLOCKER,	// Blocks entities not permitted to get near moving doors
	COLLISION_GROUP_PASSABLE_DOOR,	// Doors that the player shouldn't collide with
	COLLISION_GROUP_DISSOLVING,		// Things that are dissolving are in this group
	COLLISION_GROUP_PUSHAWAY,		// Nonsolid on client and server, pushaway in player code

	COLLISION_GROUP_NPC_ACTOR,		// Used so NPCs in scripts ignore the player.
	COLLISION_GROUP_NPC_SCRIPTED,	// USed for NPCs in scripts that should not collide with each other
	COLLISION_GROUP_PZ_CLIP,



	COLLISION_GROUP_DEBRIS_BLOCK_PROJECTILE, // Only collides with bullets

	LAST_SHARED_COLLISION_GROUP
};

//-----------------------------------------------------------------------------
// A ray...
//-----------------------------------------------------------------------------
struct Ray_t
{
	VectorAligned  m_Start;	// starting point, centered within the extents
	VectorAligned  m_Delta;	// direction + length of the ray
	VectorAligned  m_StartOffset;	// Add this to m_Start to get the actual ray start
	VectorAligned  m_Extents;	// Describes an axis aligned box extruded along a ray
	const matrix3x4_t *m_pWorldAxisTransform;
	bool	m_IsRay;	// are the extents zero?
	bool	m_IsSwept;	// is delta != 0?

	Ray_t() : m_pWorldAxisTransform(SOURCESDK_NULL) {}

	void Init(Vector const& start, Vector const& end)
	{
		SOURCESDK_Assert(&end);
		VectorSubtract(end, start, m_Delta);

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		VectorClear(m_Extents);
		m_pWorldAxisTransform = SOURCESDK_NULL;
		m_IsRay = true;

		// Offset m_Start to be in the center of the box...
		VectorClear(m_StartOffset);
		VectorCopy(start, m_Start);
	}

	void Init(Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs)
	{
		SOURCESDK_Assert(&end);
		VectorSubtract(end, start, m_Delta);

		m_pWorldAxisTransform = SOURCESDK_NULL;
		m_IsSwept = (m_Delta.LengthSqr() != 0);

		VectorSubtract(maxs, mins, m_Extents);
		m_Extents *= 0.5f;
		m_IsRay = (m_Extents.LengthSqr() < 1e-6);

		// Offset m_Start to be in the center of the box...
		VectorAdd(mins, maxs, m_StartOffset);
		m_StartOffset *= 0.5f;
		VectorAdd(start, m_StartOffset, m_Start);
		m_StartOffset *= -1.0f;
	}

	// compute inverse delta
	Vector InvDelta() const
	{
		Vector vecInvDelta;
		for (int iAxis = 0; iAxis < 3; ++iAxis)
		{
			if (m_Delta[iAxis] != 0.0f)
			{
				vecInvDelta[iAxis] = 1.0f / m_Delta[iAxis];
			}
			else
			{
				vecInvDelta[iAxis] = FLT_MAX;
			}
		}
		return vecInvDelta;
	}

private:
};

// plane_t structure
// !!! if this is changed, it must be changed in asm code too !!!
// FIXME: does the asm code even exist anymore?
// FIXME: this should move to a different file
struct cplane_t
{
	Vector	normal;
	float	dist;
	byte	type;			// for fast side tests
	byte	signbits;		// signx + (signy<<1) + (signz<<1)
	byte	pad[2];

#ifdef SOURCESDK_VECTOR_NO_SLOW_OPERATIONS
	cplane_t() {}

private:
	// No copy constructors allowed if we're in optimal mode
	cplane_t(const cplane_t& vOther);
#endif
};

// Note: These flags need to match the bspfile.h DISPTRI_TAG_* flags.
#define SOURCESDK_CSGO_DISPSURF_FLAG_SURFACE		(1<<0)
#define SOURCESDK_CSGO_DISPSURF_FLAG_WALKABLE		(1<<1)
#define SOURCESDK_CSGO_DISPSURF_FLAG_BUILDABLE		(1<<2)
#define SOURCESDK_CSGO_DISPSURF_FLAG_SURFPROP1		(1<<3)
#define SOURCESDK_CSGO_DISPSURF_FLAG_SURFPROP2		(1<<4)

//=============================================================================
// Base Trace Structure
// - shared between engine/game dlls and tools (vrad)
//=============================================================================

class CBaseTrace
{
public:

	// Displacement flags tests.
	bool IsDispSurface(void) { return ((dispFlags & SOURCESDK_CSGO_DISPSURF_FLAG_SURFACE) != 0); }
	bool IsDispSurfaceWalkable(void) { return ((dispFlags & SOURCESDK_CSGO_DISPSURF_FLAG_WALKABLE) != 0); }
	bool IsDispSurfaceBuildable(void) { return ((dispFlags & SOURCESDK_CSGO_DISPSURF_FLAG_BUILDABLE) != 0); }
	bool IsDispSurfaceProp1(void) { return ((dispFlags & SOURCESDK_CSGO_DISPSURF_FLAG_SURFPROP1) != 0); }
	bool IsDispSurfaceProp2(void) { return ((dispFlags & SOURCESDK_CSGO_DISPSURF_FLAG_SURFPROP2) != 0); }

public:

	// these members are aligned!!
	Vector			startpos;				// start position
	Vector			endpos;					// final position
	cplane_t		plane;					// surface normal at impact

	float			fraction;				// time completed, 1.0 = didn't hit anything

	int				contents;				// contents on other side of surface hit
	unsigned short	dispFlags;				// displacement flags for marking surfaces with data

	bool			allsolid;				// if true, plane is not valid
	bool			startsolid;				// if true, the initial point was in a solid area

	CBaseTrace() {}

private:
	// No copy constructors allowed
	CBaseTrace(const CBaseTrace& vOther);
};

struct csurface_t
{
	const char	*name;
	short		surfaceProps;
	unsigned short	flags;		// BUGBUG: These are declared per surface, not per material, but this database is per-material now
};

//-----------------------------------------------------------------------------
// Purpose: A trace is returned when a box is swept through the world
// NOTE: eventually more of this class should be moved up into the base class!!
//-----------------------------------------------------------------------------
class CGameTrace : public CBaseTrace
{
public:

	// Returns true if hEnt points at the world entity.
	// If this returns true, then you can't use GetHitBoxIndex().
	bool DidHitWorld() const;

	// Returns true if we hit something and it wasn't the world.
	bool DidHitNonWorldEntity() const;

	// Gets the entity's network index if the trace has hit an entity.
	// If not, returns -1.
	int GetEntityIndex() {
		if (m_pEnt)
			return m_pEnt->entindex();
		else
			return -1;
	}

	// Returns true if there was any kind of impact at all
	bool DidHit() const {
		return fraction < 1 || allsolid || startsolid;
	}

	// The engine doesn't know what a CBaseEntity is, so it has a backdoor to 
	// let it get at the edict.
#if defined( ENGINE_DLL )
	void SetEdict(edict_t *pEdict);
	edict_t* GetEdict() const;
#endif	


public:

	float			fractionleftsolid;	// time we left a solid, only valid if we started in solid
	csurface_t		surface;			// surface hit (impact surface)

	int				hitgroup;			// 0 == generic, non-zero is specific body part

	short			physicsbone;		// physics bone hit by trace in studio
	unsigned short	worldSurfaceIndex;	// Index of the msurface2_t, if applicable

//#if defined( CLIENT_DLL )
	C_BaseEntity_csgo *m_pEnt;
//#else
//	CBaseEntity *m_pEnt;
//#endif

	// NOTE: this member is overloaded.
	// If hEnt points at the world entity, then this is the static prop index.
	// Otherwise, this is the hitbox index.
	int			hitbox;					// box hit by trace in studio

	CGameTrace() {}

private:
	// No copy constructors allowed
	CGameTrace(const CGameTrace& vOther);
};


typedef CGameTrace trace_t;

#define SOURCESDK_CSGO_INTERFACEVERSION_ENGINETRACE_CLIENT	"EngineTraceClient004"

class __declspec(novtable) IEngineTrace abstract
{
public:
	virtual void _UNUSED_GetPointContents(void) = 0;
	virtual void _UNUSED_GetPointContents_WorldOnly(void) = 0;
	virtual void _UNUSED_GetPointContents_Collideable(void) = 0;
	virtual void _UNUSED_ClipRayToEntit(void) = 0;
	virtual void _UNUSED_ClipRayToCollideable(void) = 0;

	// A version that simply accepts a ray (can work as a traceline or tracehull)
	virtual void	TraceRay(const Ray_t &ray, unsigned int fMask, ITraceFilter *pTraceFilter, trace_t *pTrace) = 0;

	
	// ...
	// More we don't care about for now.
};

#define SOURCESDK_CSGO_MAX_COORD_INTEGER			(16384)

#define SOURCESDK_CSGO_COORD_EXTENT				(2*SOURCESDK_CSGO_MAX_COORD_INTEGER)

// Maximum traceable distance ( assumes cubic world and trace from one corner to opposite )
// COORD_EXTENT * sqrt(3)
#define SOURCESDK_CSGO_MAX_TRACE_LENGTH			( 1.732050807569 * SOURCESDK_CSGO_COORD_EXTENT )		


#define	SOURCESDK_CSGO_CONTENTS_SOLID			0x1		// an eye is never valid in a solid
#define SOURCESDK_CSGO_CONTENTS_WINDOW			0x2		// translucent, but not watery (glass)
#define SOURCESDK_CSGO_CONTENTS_MOVEABLE		0x4000
#define	SOURCESDK_CSGO_CONTENTS_MONSTER		0x2000000	// should never be on a brush, only in game
#define	SOURCESDK_CSGO_CONTENTS_DEBRIS			0x4000000
#define	SOURCESDK_CSGO_CONTENTS_GRATE			0x8		// alpha-tested "grate" textures.  Bullets/sight pass through, but solids don't

#define	SOURCESDK_CSGO_MASK_ALL					(0xFFFFFFFF)
#define SOURCESDK_CSGO_MASK_SHOT_HULL				(SOURCESDK_CSGO_CONTENTS_SOLID|SOURCESDK_CSGO_CONTENTS_MOVEABLE|SOURCESDK_CSGO_CONTENTS_MONSTER|SOURCESDK_CSGO_CONTENTS_WINDOW|SOURCESDK_CSGO_CONTENTS_DEBRIS|SOURCESDK_CSGO_CONTENTS_GRATE)


///////////////////

#define SORUCESDK_CSGO_VENGINE_GAMEUIFUNCS_VERSION "VENGINE_GAMEUIFUNCS_VERSION005"
#define SOURCECSDK_CSGO_PANORAMAUIENGINE "PanoramaUIEngine001"

typedef void * CGameUIFuncs_SomeShit_t;

class __declspec(novtable) CGameUIFuncs abstract {
public:
	virtual void _Unknown_000(void) = 0;
	virtual void _Unknown_001(void) = 0;
	virtual void _Unknown_002(void) = 0;
	virtual void _Unknown_003(void) = 0;
	virtual void _Unknown_004(void) = 0;
	virtual void _Unknown_005(void) = 0;
	virtual void _Unknown_006(void) = 0;
	virtual void _Unknown_007(void) = 0;
	virtual void _Unknown_008(void) = 0;
	virtual void _Unknown_009(void) = 0;
	virtual void _Unknown_010(void) = 0;

	virtual CGameUIFuncs_SomeShit_t * GetSomeShit(void) = 0;
};

namespace panorama {

class __declspec(novtable) CTopLevelWindowSource2 abstract {
public:
	virtual void RunFrame(void * hWnd, int flags) = 0;
	virtual void _Unknown_001(void) = 0;
	virtual void SetVisible(bool value) = 0;
	virtual void _Unknown_003(void) = 0;
	virtual void _Unknown_004(void) = 0;
	virtual void _Unknown_005(void) = 0;
	virtual void _Unknown_006(void) = 0;
	virtual void _Unknown_007(void) = 0;
	virtual void _Unknown_008(void) = 0;
	virtual void _Unknown_009(void) = 0;
	virtual void _Unknown_010(void) = 0;
	virtual void _Unknown_011(void) = 0;
	virtual void _Unknown_012(void) = 0;
	virtual void _Unknown_013(void) = 0;
	virtual void _Unknown_014(void) = 0;
	virtual void _Unknown_015(void) = 0;
	virtual void _Unknown_016(void) = 0;
	virtual void _Unknown_017(void) = 0;
	virtual void _Unknown_018(void) = 0;
	virtual void _Unknown_019(void) = 0;
	virtual void _Unknown_020(void) = 0;
	virtual void _Unknown_021(void) = 0;
	virtual void _Unknown_022(void) = 0;
	virtual void _Unknown_023(void) = 0;
	virtual void _Unknown_024(void) = 0;
	virtual void _Unknown_025(void) = 0;
	virtual void _Unknown_026(void) = 0;
	virtual void _Unknown_027(void) = 0;
	virtual void _Unknown_028(void) = 0;
	virtual void _Unknown_029(void) = 0;
	virtual void _Unknown_030(void) = 0;
	virtual void _Unknown_031(void) = 0;
	virtual void _Unknown_032(void) = 0;
	virtual void _Unknown_033(void) = 0;
	virtual void _Unknown_034(void) = 0;
	virtual void _Unknown_035(void) = 0;
	virtual void _Unknown_036(void) = 0;
	virtual void _Unknown_037(void) = 0;
	virtual void _Unknown_038(void) = 0;
	virtual void _Unknown_039(void) = 0;
	virtual void _Unknown_040(void) = 0;
	virtual void _Unknown_041(void) = 0;
	virtual void _Unknown_042(void) = 0;
	virtual void _Unknown_043(void) = 0;
	virtual void _Unknown_044(void) = 0;
	virtual void _Unknown_045(void) = 0;
	virtual void _Unknown_046(void) = 0;
	virtual void _Unknown_047(void) = 0;
	virtual void _Unknown_048(void) = 0;
	virtual void _Unknown_049(void) = 0;
	virtual void _Unknown_050(void) = 0;
	virtual void _Unknown_051(void) = 0;
	virtual void _Unknown_052(void) = 0;
	virtual void _Unknown_053(void) = 0;
	virtual void _Unknown_054(void) = 0;
	virtual void _Unknown_055(void) = 0;
	virtual void _Unknown_056(void) = 0;
	virtual void _Unknown_057(void) = 0;
	virtual void _Unknown_058(void) = 0;
	virtual void _Unknown_059(void) = 0;
	virtual void _Unknown_060(void) = 0;
	virtual void _Unknown_061(void) = 0;
	virtual void _Unknown_062(void) = 0;
	virtual void _Unknown_063(void) = 0;
	virtual void _Unknown_064(void) = 0;
	virtual void _Unknown_065(void) = 0;
	virtual void _Unknown_066(void) = 0;
	virtual void _Unknown_067(void) = 0;
	virtual void _Unknown_068(void) = 0;
	virtual void _Unknown_069(void) = 0;
	virtual void _Unknown_070(void) = 0;
	virtual void _Unknown_071(void) = 0;
	virtual void _Unknown_072(void) = 0;
	virtual void _Unknown_073(void) = 0;
	virtual void _Unknown_074(void) = 0;
	virtual void _Unknown_075(void) = 0;
	virtual void _Unknown_076(void) = 0;
	virtual void _Unknown_077(void) = 0;
	virtual void _Unknown_078(void) = 0;
	virtual void _Unknown_079(void) = 0;
	virtual void _Unknown_080(void) = 0;
	virtual void _Unknown_081(void) = 0;
	virtual void _Unknown_082(void) = 0;
	virtual void _Unknown_083(void) = 0;

	virtual void LayoutAndPaintIfNeeded(void) = 0;
	virtual void PaintEmptyFrame(void) = 0;

};

class __declspec(novtable) CUIEngineSource2 abstract {
public:
	virtual void _Unknown_000(void) = 0;
	virtual void _Unknown_001(void) = 0;
	virtual void _Unknown_002(void) = 0;
	virtual void _Unknown_003(void) = 0;
	virtual void _Unknown_004(void) = 0;
	virtual void _Unknown_005(void) = 0;
	virtual void _Unknown_006(void) = 0;
	virtual void _Unknown_007(void) = 0;
	virtual void _Unknown_008(void) = 0;
	virtual void _Unknown_009(void) = 0;
	virtual void _Unknown_010(void) = 0;
	virtual void _Unknown_011(void) = 0;
	virtual void _Unknown_012(void) = 0;

	virtual CTopLevelWindowSource2 *  CreateTopLevelWindow(int x, int y, int width, int height, int unk1_0, int unk2_0, int unk3_0, int unk4_1, const char * jsScope /* CSGOJsRegistration */, CGameUIFuncs_SomeShit_t * someShit) = 0;

};

class __declspec(novtable) CPanoramaUIEngine abstract  {
public:
	virtual void _Unknown_000(void) = 0;
	virtual void _Unknown_001(void) = 0;
	virtual void _Unknown_002(void) = 0;
	virtual void _Unknown_003(void) = 0;
	virtual void _Unknown_004(void) = 0;
	virtual void _Unknown_005(void) = 0;
	virtual void _Unknown_006(void) = 0;
	virtual void _Unknown_007(void) = 0;
	virtual void _Unknown_008(void) = 0;
	virtual void _Unknown_009(void) = 0;
	virtual void _Unknown_010(void) = 0;

	virtual CUIEngineSource2 * GetUIEngineSoruce2(void) = 0;
};

class __declspec(novtable) CDebugger abstract {
public:

};

} // panorama

#define SOURCESDK_CSGO_PANORAMAUICLIENT_VERSION "PanoramaUIClient001"

class __declspec(novtable) CPanoramaUIClient abstract {
public:
	virtual void _Unknown_000(void) = 0;
	virtual void _Unknown_001(void) = 0;
	virtual void _Unknown_002(void) = 0;
	virtual void _Unknown_003(void) = 0;
	virtual void _Unknown_004(void) = 0;
	virtual void _Unknown_005(void) = 0;
	virtual void _Unknown_006(void) = 0;
	virtual void _Unknown_007(void) = 0;
	virtual void _Unknown_008(void) = 0;
	virtual void _Unknown_009(void) = 0;
	virtual void _Unknown_010(void) = 0;
	virtual void _Unknown_011(void) = 0;
	virtual void _Unknown_012(void) = 0;

	virtual panorama::CDebugger * CreateDebugger(panorama::CTopLevelWindowSource2 * topLevelWindow, const char * factory = "Debugger") = 0;
};

#define SOURCESDK_CSGO_VSERVERTOOLS_INTERFACE_VERSION "VSERVERTOOLS001"

class IServerEntity;
class CEntityRespawnInfo;

class __declspec(novtable) IServerTools abstract : public IBaseInterface
{
public:
	virtual IServerEntity *GetIServerEntity(IClientEntity_csgo *pClientEntity) = 0;
	virtual bool SnapPlayerToPosition(const Vector &org, const QAngle &ang, IClientEntity_csgo *pClientPlayer = NULL) = 0;
	virtual bool GetPlayerPosition(Vector &org, QAngle &ang, IClientEntity_csgo *pClientPlayer = NULL) = 0;
	virtual bool SetPlayerFOV(int fov, IClientEntity_csgo *pClientPlayer = NULL) = 0;
	virtual int GetPlayerFOV(IClientEntity_csgo *pClientPlayer = NULL) = 0;
	virtual bool IsInNoClipMode(IClientEntity_csgo *pClientPlayer = NULL) = 0;

	// entity searching
	virtual void *FirstEntity(void) = 0;
	virtual void *NextEntity(void *pEntity) = 0;
	virtual void *FindEntityByHammerID(int iHammerID) = 0;

	// entity query
	virtual bool GetKeyValue(void *pEntity, const char *szField, char *szValue, int iMaxLen) = 0;
	virtual bool SetKeyValue(void *pEntity, const char *szField, const char *szValue) = 0;
	virtual bool SetKeyValue(void *pEntity, const char *szField, float flValue) = 0;
	virtual bool SetKeyValue(void *pEntity, const char *szField, const Vector &vecValue) = 0;

	// entity spawning
	virtual void *CreateEntityByName(const char *szClassName) = 0;
	virtual void DispatchSpawn(void *pEntity) = 0;
	virtual bool DestroyEntityByHammerId(int iHammerID) = 0;

	// This function respawns the entity into the same entindex slot AND tricks the EHANDLE system into thinking it's the same
	// entity version so anyone holding an EHANDLE to the entity points at the newly-respawned entity.
	virtual bool RespawnEntitiesWithEdits(CEntityRespawnInfo *pInfos, int nInfos) = 0;

	// This reloads a portion or all of a particle definition file.
	// It's up to the server to decide if it cares about this file
	// Use a UtlBuffer to crack the data
	virtual void ReloadParticleDefintions(const char *pFileName, const void *pBufData, int nLen) = 0;

	virtual void AddOriginToPVS(const Vector &org) = 0;
	virtual void MoveEngineViewTo(const Vector &vPos, const QAngle &vAngles) = 0;

	// Call UTIL_Remove on the entity.
	virtual void RemoveEntity(int nHammerID) = 0;

	// one more we don't know.
};

class CRefCountedAfxDummy
{

};

class CBase3dView : public CRefCountedAfxDummy, protected CViewSetup_csgo
{
public:

	CViewSetup_csgo & AfxHackGetViewSetup()
	{
		return *this;
	}
};

class CRendering3dView : public CBase3dView
{
};

// GameEvents related:

#define SOURCESDK_CSGO_MAX_EVENT_NAME_LENGTH 32

class CGameEventDescriptor
{
public:
	CGameEventDescriptor()
	{
		name_index = -1;
		eventid = -1;
		keys = NULL;
		local = false;
		reliable = true;
	}

public:
	int	name_index;
	int	eventid;
	KeyValues *keys;
	bool local;
	bool reliable;
	// ... more stuf we don't care about.
};

//-----------------------------------------------------------------------------
// DrawModel flags
//-----------------------------------------------------------------------------
enum
{
	STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
	STUDIORENDER_DRAW_GROUP_MASK = 0x03,

	STUDIORENDER_DRAW_NO_FLEXES = 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,

	STUDIORENDER_DRAW_ACCURATETIME = 0x10,		// Use accurate timing when drawing the model.
	STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,

	STUDIORENDER_DRAW_WIREFRAME = 0x80,

	STUDIORENDER_DRAW_ITEM_BLINK = 0x100,

	STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,

	STUDIORENDER_NO_SKIN = 0x400,

	STUDIORENDER_SKIP_DECALS = 0x800,
};

struct StudioRenderConfig_t
{
	//TODO ...
};

typedef void studiohdr_t;

struct studiohwdata_t {
	// TODO ...
};

typedef void LightDesc_t;
class Vector4D;
struct DrawModelResults_t;
typedef void * StudioDecalHandle_t;
struct ColorMeshInfo_t;

enum
{
	MATERIAL_MAX_LIGHT_COUNT = 4,
};

struct DrawModelInfo_t
{
	studiohdr_t		*m_pStudioHdr;
	studiohwdata_t	*m_pHardwareData;
	StudioDecalHandle_t m_Decals;
	int				m_Skin;
	int				m_Body;
	int				m_HitboxSet;
	void			*m_pClientEntity;
	int				m_Lod;
	ColorMeshInfo_t	*m_pColorMeshes;
	bool			m_bStaticLighting;

	// TODO ... more we currently don't care about:

	//MaterialLightingState_t	m_LightingState;

	//IMPLEMENT_OPERATOR_EQUAL(DrawModelInfo_t);
};

//-----------------------------------------------------------------------------
// Studio render interface
//-----------------------------------------------------------------------------
class __declspec(novtable) IStudioRender abstract : public IAppSystem
{
public:
	virtual void BeginFrame(void) = 0;
	virtual void EndFrame(void) = 0;

	// Used for the mat_stub console command.
	virtual void Mat_Stub(IMaterialSystem_csgo *pMatSys) = 0;

	// Updates the rendering configuration 
	virtual void UpdateConfig(const StudioRenderConfig_t& config) = 0;
	virtual void GetCurrentConfig(StudioRenderConfig_t& config) = 0;

	// Load, unload model data
	virtual bool LoadModel(studiohdr_t *pStudioHdr, void *pVtxData, studiohwdata_t	*pHardwareData) = 0;
	virtual void UnloadModel(studiohwdata_t *pHardwareData) = 0;

	// Refresh the studiohdr since it was lost...
	virtual void RefreshStudioHdr(studiohdr_t* pStudioHdr, studiohwdata_t* pHardwareData) = 0;

	// This is needed to do eyeglint and calculate the correct texcoords for the eyes.
	virtual void SetEyeViewTarget(const studiohdr_t *pStudioHdr, int nBodyIndex, const Vector& worldPosition) = 0;

	// Methods related to lighting state
	// NOTE: SetAmbientLightColors assumes that the arraysize is the same as 
	// returned from GetNumAmbientLightSamples
	virtual int GetNumAmbientLightSamples() = 0;
	virtual const Vector *GetAmbientLightDirections() = 0;
	virtual void SetAmbientLightColors(const Vector4D *pAmbientOnlyColors) = 0;
	virtual void SetAmbientLightColors(const Vector *pAmbientOnlyColors) = 0;
	virtual void SetLocalLights(int numLights, const LightDesc_t *pLights) = 0;

	// Sets information about the camera location + orientation
	virtual void SetViewState(const Vector& viewOrigin, const Vector& viewRight,
		const Vector& viewUp, const Vector& viewPlaneNormal) = 0;

	// LOD stuff
	virtual int GetNumLODs(const studiohwdata_t &hardwareData) const = 0;
	virtual float GetLODSwitchValue(const studiohwdata_t &hardwareData, int lod) const = 0;
	virtual void SetLODSwitchValue(studiohwdata_t &hardwareData, int lod, float switchValue) = 0;

	// Sets the color/alpha modulation
	virtual void SetColorModulation(float const* pColor) = 0;
	virtual void SetAlphaModulation(float flAlpha) = 0;

	// Draws the model
	virtual void DrawModel(DrawModelResults_t *pResults, const DrawModelInfo_t& info,
		matrix3x4_t *pBoneToWorld, float *pFlexWeights, float *pFlexDelayedWeights, const Vector &modelOrigin, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;

	// Methods related to static prop rendering
	virtual void DrawModelStaticProp(const DrawModelInfo_t& drawInfo, const matrix3x4_t &modelToWorld, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;
	virtual void DrawStaticPropDecals(const DrawModelInfo_t &drawInfo, const matrix3x4_t &modelToWorld) = 0;
	virtual void DrawStaticPropShadows(const DrawModelInfo_t &drawInfo, const matrix3x4_t &modelToWorld, int flags) = 0;

	// TODO ... More we don't carea bout:
/*
	// Causes a material to be used instead of the materials the model was compiled with
	virtual void ForcedMaterialOverride(IMaterial_csgo *newMaterial, OverrideType_t nOverrideType = OVERRIDE_NORMAL) = 0;

	// Create, destroy list of decals for a particular model
	virtual StudioDecalHandle_t CreateDecalList(studiohwdata_t *pHardwareData) = 0;
	virtual void DestroyDecalList(StudioDecalHandle_t handle) = 0;

	// Add decals to a decal list by doing a planar projection along the ray
	// The BoneToWorld matrices must be set before this is called
	virtual void AddDecal(StudioDecalHandle_t handle, studiohdr_t *pStudioHdr, matrix3x4_t *pBoneToWorld,
		const Ray_t & ray, const Vector& decalUp, IMaterial_csgo* pDecalMaterial, float radius, int body, bool noPokethru = false, int maxLODToDecal = ADDDECAL_TO_ALL_LODS) = 0;

	// Compute the lighting at a point and normal
	virtual void ComputeLighting(const Vector* pAmbient, int lightCount,
		LightDesc_t* pLights, const Vector& pt, const Vector& normal, Vector& lighting) = 0;

	// Compute the lighting at a point, constant directional component is passed
	// as flDirectionalAmount
	virtual void ComputeLightingConstDirectional(const Vector* pAmbient, int lightCount,
		LightDesc_t* pLights, const Vector& pt, const Vector& normal, Vector& lighting, float flDirectionalAmount) = 0;

	// Shadow state (affects the models as they are rendered)
	virtual void AddShadow(IMaterial_csgo* pMaterial, void* pProxyData, FlashlightState_t *m_pFlashlightState = NULL, VMatrix *pWorldToTexture = NULL, ITexture_csgo *pFlashlightDepthTexture = NULL) = 0;
	virtual void ClearAllShadows() = 0;

	// Gets the model LOD; pass in the screen size in pixels of a sphere 
	// of radius 1 that has the same origin as the model to get the LOD out...
	virtual int ComputeModelLod(studiohwdata_t* pHardwareData, float unitSphereSize, float *pMetric = NULL) = 0;

	// Return a number that is usable for budgets, etc.
	// Things that we care about:
	// 1) effective triangle count (factors in batch sizes, state changes, etc)
	// 2) texture memory usage
	// Get Triangles returns the LOD used
	virtual void GetPerfStats(DrawModelResults_t *pResults, const DrawModelInfo_t &info, CUtlBuffer *pSpewBuf = NULL) const = 0;
	virtual void GetTriangles(const DrawModelInfo_t& info, matrix3x4_t *pBoneToWorld, GetTriangles_Output_t &out) = 0;

	// Returns materials used by a particular model
	virtual int GetMaterialList(studiohdr_t *pStudioHdr, int count, IMaterial** ppMaterials) = 0;
	virtual int GetMaterialListFromBodyAndSkin(MDLHandle_t studio, int nSkin, int nBody, int nCountOutputMaterials, IMaterial** ppOutputMaterials) = 0;

	// no debug modes, just fastest drawing path
	virtual void DrawModelArrayStaticProp(const DrawModelInfo_t& drawInfo, int nInstanceCount, const MeshInstanceData_t *pInstanceData, ColorMeshInfo_t **pColorMeshes) = 0;

	// draw an array of models with the same state
	virtual void DrawModelArray(const StudioModelArrayInfo_t &drawInfo, int nCount,
		StudioArrayInstanceData_t *pInstanceData, int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;

	// draw an array of models with the same state
	virtual void DrawModelShadowArray(int nCount, StudioArrayData_t *pShadowData,
		int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;

	// draw an array of models with the same state
	virtual void DrawModelArray(const StudioModelArrayInfo2_t &info, int nCount, StudioArrayData_t *pArrayData,
		int nInstanceStride, int flags = STUDIORENDER_DRAW_ENTIRE_MODEL) = 0;
*/
};

#define SOURCESDK_CSGO_STUDIO_RENDER_INTERFACE_VERSION			"VStudioRender026"

} // namespace CSGO {

struct mstudiobone_t
{
	int	sznameindex;
	inline char * const pszName( void ) const { return ((char *)this) + sznameindex; }
	int	parent;
	int	bonecontroller[6];

	Vector pos;
	Quaternion quat;
	Vector rot;

	Vector posscale;
	Vector rotscale;

	matrix3x4_t			poseToBone;
	Quaternion			qAlignment;
	int					flags;
	int					proctype;
	int					procindex;		// procedural rule
	mutable int			physicsbone;	// index into physically simulated bone
	inline void *pProcedure( ) const { if (procindex == 0) return NULL; else return  (void *)(((byte *)this) + procindex); };
	int					surfacepropidx;	// index into string tablefor property name
	inline char * const pszSurfaceProp( void ) const { return ((char *)this) + surfacepropidx; }
	int					contents;		// See BSPFlags.h for the contents flags

	int					unused[8];		// remove as appropriate

	mstudiobone_t(){}
private:
	// No copy constructors allowed
	mstudiobone_t(const mstudiobone_t& vOther);
};


struct studiohdr_t
{
	unsigned char __unknown[0x9c];
	int numbones;
	int boneindex;
	inline const mstudiobone_t *pBone( int i ) const { Assert( i >= 0 && i < numbones); return (mstudiobone_t *)(((byte *)this) + boneindex) + i; };
};

class CStudioHdr
{
private:
	mutable const studiohdr_t		*m_pStudioHdr;
public:
	inline int numbones( void ) const { return m_pStudioHdr->numbones; };
	inline const mstudiobone_t *pBone( int i ) const { return m_pStudioHdr->pBone( i ); };
};

#define SOURCESDK_BONE_USED_BY_ANYTHING 0x000FFF00

} // namespace SOURCESDK {
