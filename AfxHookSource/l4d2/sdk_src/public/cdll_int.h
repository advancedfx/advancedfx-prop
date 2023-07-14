//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: Interfaces between the client.dll and engine
//
//===========================================================================//

#ifndef SOURCESDK_L4D2_CDLL_INT_H
#define SOURCESDK_L4D2_CDLL_INT_H
#ifdef _WIN32
#pragma once
#endif


#include <SourceSdkShared.h>

namespace SOURCESDK {
namespace L4D2 {

SOURCESDK_FORWARD_DECLARE_HANDLE(InputContextHandle_t);
class CSteamAPIContext;
struct adsp_auto_params_t;
class ClientClass;
struct model_t;
class CSentence;
struct vrect_t;
struct cmodel_t;
class IMaterial;
class CAudioSource;
class CMeasureSection;
class SurfInfo;
class ISpatialQuery;
struct cache_user_t;
class IMaterialSystem;
//class VMatrix;
struct ScreenFade_t;
struct ScreenShake_t;
class CViewSetup;
class CEngineSprite;
class CGlobalVarsBase;
class CPhysCollide;
class CSaveRestoreData;
class INetChannelInfo;
struct datamap_t;
struct typedescription_t;
class CStandardRecvProxies;
struct client_textmessage_t;
class IAchievementMgr;
class ISPSharedMemory;
class IDemoRecorder;
struct AudioState_t;
class CGamestatsData;
class IMaterialProxy;
struct InputEvent_t;
struct ButtonCode_t {
    // Invalid
};
typedef void player_info_t;

#define SOURCESDK_L4D2_VENGINE_CLIENT_INTERFACE_VERSION "VEngineClient013"

//-----------------------------------------------------------------------------
// Purpose: Interface exposed from the engine to the client .dll
//-----------------------------------------------------------------------------
/// <comments>
///		Supported by: Portal First Slice
///		</comments>
class IVEngineClient abstract
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

	virtual void				HideLoadingPlaque(void) = 0;	

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
	virtual char const	*GetLevelNameShort(void) = 0;

#if !defined( NO_VOICE )
	virtual void _UNUSED_GetVoiceTweakAPI(void)=0;
#endif
	// Tell engine stats gathering system that the rendering frame is beginning/ending
	virtual void		EngineStats_BeginFrame( void ) = 0;
	virtual void		EngineStats_EndFrame( void ) = 0;
	
	virtual void _UNUSED_FireEvents(void)=0;
	virtual void _UNUSED_GetLeavesArea(void)=0;
	virtual void _UNUSED_DoesBoxTouchAreaFrustum(void)=0;
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
	virtual void _UNKNOWN_076(void)=0;
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
	virtual bool		IsPaused( void ) = 0; //:087

	virtual float GetTimescale( void ) const = 0;
	
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
	virtual float		GetScreenAspectRatio( int viewportWidth, int viewportHeight ) = 0;

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

	// ... more
};

} // namespace SOURCESDK {
} // namespace L4D2 {

#endif // SOURCESDK_L4D2_CDLL_INT_H
