#pragma once

#define _UNUSEDFN2(x,y) x##y
#define _UNUSEDFN1(x,y) _UNUSEDFN2(x,y)
#define _UNUSEDFN _UNUSEDFN1(_UNUSED_, __LINE__)

//
// shortened
//

#define FORCEINLINE __forceinline
#define FORCEINLINE_CVAR FORCEINLINE

#define FCVAR_NONE				0 

#define CREATEINTERFACE_PROCNAME	"CreateInterface"


class CSysModule;
class ConVar_003;

typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

CreateInterfaceFn Sys_GetFactory( CSysModule *pModule );


typedef float vec_t;


typedef void ( *FnChangeCallback_003 )( ConVar_003 *var, char const *pOldString );

typedef void ( *FnCommandCallback_003 )( void );

#define COMMAND_COMPLETION_MAXITEMS_003		64
#define COMMAND_COMPLETION_ITEM_LENGTH_003	64

typedef int  ( *FnCommandCompletionCallback_003 )( char const *partial, char commands[ COMMAND_COMPLETION_MAXITEMS_003 ][ COMMAND_COMPLETION_ITEM_LENGTH_003 ] );




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


// ConCommandBase_003 /////////////////////////////////////////////////////////////

class MdtConCommands;

class ConCommandBase_003
{
public:
	friend MdtConCommands; // ugly hack, just like Valve did, cuz the interface / class design is fucked up

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

	static ConCommandBase_003 const	*GetCommands( void );
	static void					AddToList( ConCommandBase_003 *var );
	static void					RemoveFlaggedCommands( int flag );

	/// <remarks> not implemented </remarks>
	static void					RevertFlaggedCvars( int flag );

	static ConCommandBase_003 const	*FindCommand( char const *name );

protected:
	static ConCommandBase_003 * GetMdtCommands( void );
	ConCommandBase_003 * GetMdtNext();
	bool MdtRegisterCommand();

private:
	static ConCommandBase_003 * m_CommandRoot;
	bool m_IsRegistered;
	ConCommandBase_003 * m_Next;
	int m_Flags;
	char * m_HelpText;
	char * m_Name;

};


// MdtConCommands //////////////////////////////////////////////////////////////

class ICvar_003;

class MdtConCommands {
public:
	static bool ConCommandBase_003_RegisterCommand(ConCommandBase_003 * command);

	static void RegisterCommands(ICvar_003 * cvarIface);

private:
	static ICvar_003 * m_CvarIface;

};


// ConCommand_003 //////////////////////////////////////////////////////////////

class ConCommand_003 : public ConCommandBase_003
{
public:
	ConCommand_003( void );
	ConCommand_003( char const *pName, FnCommandCallback_003 callback, char const *pHelpString = 0, int flags = 0, FnCommandCompletionCallback_003 completionFunc = 0 );

	virtual						~ConCommand_003( void );

	virtual	bool				IsCommand( void ) const;

	virtual int					AutoCompleteSuggest( char const *partial, char commands[ COMMAND_COMPLETION_MAXITEMS_003 ][ COMMAND_COMPLETION_ITEM_LENGTH_003 ] );

	virtual bool				CanAutoComplete( void );

	// Invoke the function
	virtual void				Dispatch( void );

private:
	FnCommandCallback_003			m_Callback;
	FnCommandCompletionCallback_003	m_CompletionFunc;
};


// ConVar_003 //////////////////////////////////////////////////////////////////

class ConVar_003 : public ConCommandBase_003
{
public:
	ConVar_003( char const *pName, char const *pDefaultValue, int flags = 0);
	ConVar_003( char const *pName, char const *pDefaultValue, int flags, char const *pHelpString );
	ConVar_003( char const *pName, char const *pDefaultValue, int flags, char const *pHelpString, bool bMin, float fMin, bool bMax, float fMax );
	ConVar_003( char const *pName, char const *pDefaultValue, int flags, char const *pHelpString, FnChangeCallback_003 callback );
	ConVar_003( char const *pName, char const *pDefaultValue, int flags, char const *pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_003 callback );

	virtual						~ConVar_003( void );

	virtual bool				IsBitSet( int flag ) const;
	virtual char const*			GetHelpText( void ) const;
	virtual bool				IsRegistered( void ) const;
	virtual char const			*GetName( void ) const;
	virtual void				AddFlags( int flags );
	virtual	bool				IsCommand( void ) const;

	// Install a change callback (there shouldn't already be one....)
	void InstallChangeCallback( FnChangeCallback_003 callback );

	// Retrieve value
	FORCEINLINE_CVAR float			GetFloat( void ) const;
	FORCEINLINE_CVAR int				GetInt( void ) const;
	FORCEINLINE_CVAR bool			GetBool() const {  return !!GetInt(); }
	FORCEINLINE_CVAR char const	   *GetString( void ) const;

	// Any function that allocates/frees memory needs to be virtual or else you'll have crashes
	//  from alloc/free across dll/exe boundaries.
	
	// These just call into the IConCommandBaseAccessor to check flags and set the var (which ends up calling InternalSetValue).
	virtual void				SetValue( char const *value );
	virtual void				SetValue( float value );
	virtual void				SetValue( int value );
	
	// Reset to default value
	void						Revert( void );

	// True if it has a min/max setting
	bool						GetMin( float& minVal ) const;
	bool						GetMax( float& maxVal ) const;
	char const					*GetDefault( void ) const;

	static void					RevertAll( void );
};


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


// IVEngineClient_012 //////////////////////////////////////////////////////////

#define VENGINE_CLIENT_INTERFACE_VERSION_012		"VEngineClient012"

class IVEngineClient_012 abstract
{
public:
	virtual void _UNUSEDFN() = 0; // GetIntersectingSurfaces
	virtual void _UNUSEDFN() = 0; // GetLightForPoint
	virtual void _UNUSEDFN() = 0; // TraceLineMaterialAndLighting
	virtual void _UNUSEDFN() = 0; // ParseFile
	virtual void _UNUSEDFN() = 0; // CopyFile

	// Gets the dimensions of the game window
	virtual void				GetScreenSize( int& width, int& height ) = 0;

	// Forwards szCmdString to the server, sent reliably if bReliable is set
	virtual void				ServerCmd( const char *szCmdString, bool bReliable = true ) = 0;
	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	virtual void				ClientCmd( const char *szCmdString ) = 0;

	virtual void _UNUSEDFN() = 0; // GetPlayerInfo

	virtual void _UNUSEDFN() = 0; // GetPlayerForUserID

	virtual void _UNUSEDFN() = 0; // TextMessageGet

	// Returns true if the console is visible
	virtual bool				Con_IsVisible( void ) = 0;

	virtual void _UNUSEDFN() = 0; // GetLocalPlayer

	virtual void _UNUSEDFN() = 0; // LoadModel

	// Get accurate, sub-frame clock ( profiling use )
	virtual float				Time( void ) = 0; 

	// Get the exact server timesstamp ( server time ) from the last message received from the server
	virtual float				GetLastTimeStamp( void ) = 0; 

	virtual void _UNUSEDFN() = 0; // GetSentence
	virtual void _UNUSEDFN() = 0; // GetSentenceLength
	virtual void _UNUSEDFN() = 0; // IsStreaming

	// Copy current view orientation into va
	virtual void				GetViewAngles( QAngle& va ) = 0;
	// Set current view orientation from va
	virtual void				SetViewAngles( QAngle& va ) = 0;
	
	// Retrieve the current game's maxclients setting
	virtual int					GetMaxClients( void ) = 0;

	virtual void _UNUSEDFN() = 0; // Key_Event

	virtual void _UNUSEDFN() = 0; // Key_LookupBinding

	virtual void _UNUSEDFN() = 0; // StartKeyTrapMode
	virtual void _UNUSEDFN() = 0; // CheckDoneKeyTrapping

	// Returns true if the player is fully connected and active in game (i.e, not still loading)
	virtual bool				IsInGame( void ) = 0;
	// Returns true if the player is connected, but not necessarily active in game (could still be loading)
	virtual bool				IsConnected( void ) = 0;
	// Returns true if the loading plaque should be drawn
	virtual bool				IsDrawingLoadingImage( void ) = 0;

	// Prints the formatted string to the notification area of the screen ( down the right hand edge
	//  numbered lines starting at position 0
	virtual void				Con_NPrintf( int pos, const char *fmt, ... ) = 0;
	
	virtual void _UNUSEDFN() = 0; // Con_NXPrintf

	// During ConCommand processing functions, use this function to get the total # of tokens passed to the command parser
	virtual int					Cmd_Argc( void ) = 0;	
	// During ConCommand processing, this API is used to access each argument passed to the parser
	virtual const char			*Cmd_Argv( int arg ) = 0;

	virtual void _UNUSEDFN() = 0; // IsBoxVisible

	virtual void _UNUSEDFN() = 0; // IsBoxInViewCluster
	
	virtual void _UNUSEDFN() = 0; // CullBox

	// Allow the sound system to paint additional data (during lengthy rendering operations) to prevent stuttering sound.
	virtual void				Sound_ExtraUpdate( void ) = 0;

	// Get the current game directory ( e.g., hl2, tf2, cstrike, hl1 )
	virtual const char			*GetGameDirectory( void ) = 0;

	virtual void _UNUSEDFN() = 0; // WorldToScreenMatrix
	virtual void _UNUSEDFN() = 0; // WorldToViewMatrix
	virtual void _UNUSEDFN() = 0; // GameLumpVersion
	virtual void _UNUSEDFN() = 0; // GameLumpSize
	virtual void _UNUSEDFN() = 0; // LoadGameLump
	virtual void _UNUSEDFN() = 0; // LevelLeafCount
	virtual void _UNUSEDFN() = 0; // GetBSPTreeQuery
	virtual void _UNUSEDFN() = 0; // LinearToGamma
	virtual void _UNUSEDFN() = 0; // LightStyleValue
	virtual void _UNUSEDFN() = 0; // ComputeDynamicLighting
	virtual void _UNUSEDFN() = 0; // GetAmbientLightColor

	// Returns the dx support level
	virtual int			GetDXSupportLevel() = 0;

	// GR - returns the HDR support status
	virtual bool        SupportsHDR() = 0;

	virtual void _UNUSEDFN() = 0; // Mat_Stub

	// Get the name of the current map
	virtual char const	*GetLevelName( void ) = 0;
#ifndef _XBOX
	virtual void _UNUSEDFN() = 0; // GetVoiceTweakAPI
#endif
	// Tell engine stats gathering system that the rendering frame is beginning/ending
	virtual void		EngineStats_BeginFrame( void ) = 0;
	virtual void		EngineStats_EndFrame( void ) = 0;
	
	virtual void _UNUSEDFN() = 0; // FireEvents
	virtual void _UNUSEDFN() = 0; // GetLeavesArea
	virtual void _UNUSEDFN() = 0; // DoesBoxTouchAreaFrustum
	virtual void _UNUSEDFN() = 0; // SetHearingOrigin
	virtual void _UNUSEDFN() = 0; // SentenceGroupPick
	virtual void _UNUSEDFN() = 0; // SentenceGroupPickSequential
	virtual void _UNUSEDFN() = 0; // SentenceIndexFromName
	virtual void _UNUSEDFN() = 0; // SentenceNameFromIndex
	virtual void _UNUSEDFN() = 0; // SentenceGroupIndexFromName
	virtual void _UNUSEDFN() = 0; // SentenceGroupNameFromIndex
	virtual void _UNUSEDFN() = 0; // SentenceLength
	virtual void _UNUSEDFN() = 0; // ComputeLighting
	virtual void _UNUSEDFN() = 0; // ActivateOccluder
	virtual void _UNUSEDFN() = 0; // IsOccluded

	// The save restore system allocates memory from a shared memory pool, use this allocator to allocate/free saverestore 
	//  memory.
	virtual void		*SaveAllocMemory( size_t num, size_t size ) = 0;
	virtual void		SaveFreeMemory( void *pSaveMem ) = 0;

	virtual void _UNUSEDFN() = 0; // GetNetChannelInfo
	virtual void _UNUSEDFN() = 0; // DebugDrawPhysCollide
	virtual void _UNUSEDFN() = 0; // CheckPoint
	virtual void _UNUSEDFN() = 0; // DrawPortals

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

	virtual void _UNUSEDFN() = 0; // SetOcclusionParameters

	virtual void _UNUSEDFN() = 0; // GetUILanguage

	virtual void _UNUSEDFN() = 0; // IsSkyboxVisibleFromPoint

	// Get the pristine map entity lump string.  (e.g., used by CS to reload the map entities when restarting a round.)
	virtual const char*	GetMapEntitiesString() = 0;

	// Is the engine in map edit mode ?
	virtual bool		IsInEditMode( void ) = 0;

	// current screen aspect ratio (eg. 4.0f/3.0f, 16.0f/9.0f)
	virtual float		GetScreenAspectRatio() = 0;

	virtual void _UNUSEDFN() = 0; //SteamRefreshLogin
	virtual void _UNUSEDFN() = 0; //SteamProcessCall

	// allow other modules to know about engine versioning (one use is a proxy for network compatability)
	virtual unsigned int	GetEngineBuildNumber() = 0; // engines build
	virtual const char *	GetProductVersionString() = 0; // mods version number (steam.inf)

	virtual int		GetLastPressedEngineKey( void ) = 0;

	virtual void _UNUSEDFN() = 0; //GrabPreColorCorrectedFrame

	virtual bool			IsHammerRunning( ) const = 0;

	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// And then executes the command string immediately (vs ClientCmd() which executes in the next frame)
	virtual void			ExecuteClientCmd( const char *szCmdString ) = 0;

	virtual void _UNUSEDFN() = 0; // MapHasHDRLighting

	virtual int	GetAppID() = 0;

	virtual void _UNUSEDFN() = 0; // GetLightForPointFast
};


// IVEngineClient_013 //////////////////////////////////////////////////////////

#define VENGINE_CLIENT_INTERFACE_VERSION_013 "VEngineClient013"

// Engine -> Client.dll
class IVEngineClient_013 abstract
{
public:
	virtual void _UNUSEDFN() = 0; // GetIntersectingSurfaces
	virtual void _UNUSEDFN() = 0; // GetLightForPoint
	virtual void _UNUSEDFN() = 0; // TraceLineMaterialAndLighting
	virtual void _UNUSEDFN() = 0; // ParseFile
	virtual void _UNUSEDFN() = 0; // CopyFile

	// Gets the dimensions of the game window
	virtual void				GetScreenSize( int& width, int& height ) = 0;

	// Forwards szCmdString to the server, sent reliably if bReliable is set
	virtual void				ServerCmd( const char *szCmdString, bool bReliable = true ) = 0;
	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// Note: Calls to this are checked against FCVAR_CLIENTCMD_CAN_EXECUTE (if that bit is not set, then this function can't change it).
	//       Call ClientCmd_Unrestricted to have access to FCVAR_CLIENTCMD_CAN_EXECUTE vars.
	virtual void				ClientCmd( const char *szCmdString ) = 0;

	virtual void _UNUSEDFN() = 0; // GetPlayerInfo
	virtual void _UNUSEDFN() = 0; // GetPlayerForUserID
	virtual void _UNUSEDFN() = 0; // TextMessageGet

	// Returns true if the console is visible
	virtual bool				Con_IsVisible( void ) = 0;

	// Get the entity index of the local player
	virtual int					GetLocalPlayer( void ) = 0;

	virtual void _UNUSEDFN() = 0; // LoadModel

	// Get accurate, sub-frame clock ( profiling use )
	virtual float				Time( void ) = 0; 

	// Get the exact server timesstamp ( server time ) from the last message received from the server
	virtual float				GetLastTimeStamp( void ) = 0; 

	virtual void _UNUSEDFN() = 0; // GetSentence
	virtual void _UNUSEDFN() = 0; // GetSentenceLength
	virtual void _UNUSEDFN() = 0; // IsStreaming

	// Copy current view orientation into va
	virtual void				GetViewAngles( QAngle& va ) = 0;
	// Set current view orientation from va
	virtual void				SetViewAngles( QAngle& va ) = 0;
	
	// Retrieve the current game's maxclients setting
	virtual int					GetMaxClients( void ) = 0;

	virtual void _UNUSEDFN() = 0; // Key_LookupBinding
	virtual void _UNUSEDFN() = 0; // Key_BindingForKey
	virtual void _UNUSEDFN() = 0; // StartKeyTrapMode
	virtual void _UNUSEDFN() = 0; // CheckDoneKeyTrapping

	// Returns true if the player is fully connected and active in game (i.e, not still loading)
	virtual bool				IsInGame( void ) = 0;
	// Returns true if the player is connected, but not necessarily active in game (could still be loading)
	virtual bool				IsConnected( void ) = 0;
	// Returns true if the loading plaque should be drawn
	virtual bool				IsDrawingLoadingImage( void ) = 0;

	// Prints the formatted string to the notification area of the screen ( down the right hand edge
	//  numbered lines starting at position 0
	virtual void				Con_NPrintf( int pos, const char *fmt, ... ) = 0;
	
	virtual void _UNUSEDFN() = 0; // Con_NXPrintf
	virtual void _UNUSEDFN() = 0; // IsBoxVisible
	virtual void _UNUSEDFN() = 0; // IsBoxInViewCluster
	virtual void _UNUSEDFN() = 0; // CullBox
	virtual void _UNUSEDFN() = 0; // Sound_ExtraUpdate

	// Get the current game directory ( e.g., hl2, tf2, cstrike, hl1 )
	virtual const char			*GetGameDirectory( void ) = 0;

	virtual void _UNUSEDFN() = 0; // WorldToScreenMatrix
	virtual void _UNUSEDFN() = 0; // WorldToViewMatrix
	virtual void _UNUSEDFN() = 0; // GameLumpVersion
	virtual void _UNUSEDFN() = 0; // GameLumpSize
	virtual void _UNUSEDFN() = 0; // LoadGameLump
	virtual void _UNUSEDFN() = 0; // LevelLeafCount
	virtual void _UNUSEDFN() = 0; // GetBSPTreeQuery
	virtual void _UNUSEDFN() = 0; // LinearToGamma
	virtual void _UNUSEDFN() = 0; // LightStyleValue
	virtual void _UNUSEDFN() = 0; // ComputeDynamicLighting
	virtual void _UNUSEDFN() = 0; // GetAmbientLightColor
	virtual void _UNUSEDFN() = 0; // GetDXSupportLevel
	virtual void _UNUSEDFN() = 0; // SupportsHDR
	virtual void _UNUSEDFN() = 0; // Mat_Stub

	// Get the name of the current map
	virtual void GetChapterName( char *pchBuff, int iMaxLength ) = 0;
	virtual char const	*GetLevelName( void ) = 0;

#if !defined( NO_VOICE )
	virtual void _UNUSEDFN() = 0; // GetVoiceTweakAPI
#endif
	// Tell engine stats gathering system that the rendering frame is beginning/ending
	virtual void		EngineStats_BeginFrame( void ) = 0;
	virtual void		EngineStats_EndFrame( void ) = 0;
	
	virtual void _UNUSEDFN() = 0; // FireEvents
	virtual void _UNUSEDFN() = 0; // GetLeavesArea
	virtual void _UNUSEDFN() = 0; // DoesBoxTouchAreaFrustum
	virtual void _UNUSEDFN() = 0; // SetAudioState
	virtual void _UNUSEDFN() = 0; // SentenceGroupPick
	virtual void _UNUSEDFN() = 0; // SentenceGroupPickSequential
	virtual void _UNUSEDFN() = 0; // SentenceIndexFromName
	virtual void _UNUSEDFN() = 0; // SentenceNameFromIndex
	virtual void _UNUSEDFN() = 0; // SentenceGroupIndexFromName
	virtual void _UNUSEDFN() = 0; // SentenceGroupNameFromIndex
	virtual void _UNUSEDFN() = 0; // SentenceLength
	virtual void _UNUSEDFN() = 0; // ComputeLighting
	virtual void _UNUSEDFN() = 0; // ActivateOccluder
	virtual void _UNUSEDFN() = 0; // IsOccluded

	// The save restore system allocates memory from a shared memory pool, use this allocator to allocate/free saverestore 
	//  memory.
	virtual void		*SaveAllocMemory( size_t num, size_t size ) = 0;
	virtual void		SaveFreeMemory( void *pSaveMem ) = 0;

	virtual void _UNUSEDFN() = 0; // GetNetChannelInfo
	virtual void _UNUSEDFN() = 0; // DebugDrawPhysCollide
	virtual void _UNUSEDFN() = 0; // CheckPoint
	virtual void _UNUSEDFN() = 0; // DrawPortals

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

	virtual void _UNUSEDFN() = 0; // SetOcclusionParameters
	virtual void _UNUSEDFN() = 0; // GetUILanguage
	virtual void _UNUSEDFN() = 0; // IsSkyboxVisibleFromPoint
	virtual void _UNUSEDFN() = 0; // GetMapEntitiesString

	// Is the engine in map edit mode ?
	virtual bool		IsInEditMode( void ) = 0;

	// current screen aspect ratio (eg. 4.0f/3.0f, 16.0f/9.0f)
	virtual float		GetScreenAspectRatio() = 0;

	virtual void _UNUSEDFN() = 0; // REMOVED_SteamRefreshLogin
	virtual void _UNUSEDFN() = 0; // REMOVED_SteamProcessCall

	// allow other modules to know about engine versioning (one use is a proxy for network compatability)
	virtual unsigned int	GetEngineBuildNumber() = 0; // engines build
	virtual const char *	GetProductVersionString() = 0; // mods version number (steam.inf)

	virtual void _UNUSEDFN() = 0; // GrabPreColorCorrectedFrame

	virtual bool			IsHammerRunning( ) const = 0;

	// Inserts szCmdString into the command buffer as if it was typed by the client to his/her console.
	// And then executes the command string immediately (vs ClientCmd() which executes in the next frame)
	//
	// Note: this is NOT checked against the FCVAR_CLIENTCMD_CAN_EXECUTE vars.
	virtual void			ExecuteClientCmd( const char *szCmdString ) = 0;

	virtual void _UNUSEDFN() = 0; // MapHasHDRLighting

	virtual int	GetAppID() = 0;

	virtual void _UNUSEDFN() = 0; // GetLightForPointFast

	// This version does NOT check against FCVAR_CLIENTCMD_CAN_EXECUTE.
	virtual void			ClientCmd_Unrestricted( const char *szCmdString ) = 0;
	
	// This used to be accessible through the cl_restrict_server_commands cvar.
	// By default, Valve games restrict the server to only being able to execute commands marked with FCVAR_SERVER_CAN_EXECUTE.
	// By default, mods are allowed to execute any server commands, and they can restrict the server's ability to execute client
	// commands with this function.
	virtual void			SetRestrictServerCommands( bool bRestrict ) = 0;
	
	// If set to true (defaults to true for Valve games and false for others), then IVEngineClient::ClientCmd
	// can only execute things marked with FCVAR_CLIENTCMD_CAN_EXECUTE.
	virtual void			SetRestrictClientCommands( bool bRestrict ) = 0;

	virtual void _UNUSEDFN() = 0; // SetOverlayBindProxy
	virtual void _UNUSEDFN() = 0; // CopyFrameBufferToMaterial
	virtual void _UNUSEDFN() = 0; // ChangeTeam
	virtual void _UNUSEDFN() = 0; // ReadConfiguration
	virtual void _UNUSEDFN() = 0; // SetAchievementMgr
	virtual void _UNUSEDFN() = 0; // GetAchievementMgr
	virtual void _UNUSEDFN() = 0; // MapLoadFailed
	virtual void _UNUSEDFN() = 0; // SetMapLoadFailed
	
	virtual bool			IsLowViolence() = 0;

	virtual void _UNUSEDFN() = 0; // GetMostRecentSaveGame
	virtual void _UNUSEDFN() = 0; // SetMostRecentSaveGame
	virtual void _UNUSEDFN() = 0; // StartXboxExitingProcess
	virtual void _UNUSEDFN() = 0; // IsSaveInProgress
	virtual void _UNUSEDFN() = 0; // OnStorageDeviceAttached
	virtual void _UNUSEDFN() = 0; // OnStorageDeviceDetached
	virtual void _UNUSEDFN() = 0; // ResetDemoInterpolation
	virtual void _UNUSEDFN() = 0; // SetGamestatsData
	virtual void _UNUSEDFN() = 0; // GetGamestatsData
};


// Interfaces as globals for now:

extern IVEngineClient_012 * g_VEngineClient;
extern ICvar_003 * g_Cvar;
