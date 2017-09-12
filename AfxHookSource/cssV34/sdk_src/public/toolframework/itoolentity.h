//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef SOURCESDK_CSSV34_ITOOLENTITY_H
#define SOURCESDK_CSSV34_ITOOLENTITY_H
#ifdef _WIN32
#pragma once
#endif

#include <SourceSdkShared.h>
//#include <cssV34/sdk_src/public/tier1/interface.h>
//#include <cssV34/sdk_src/public/tier1/utlvector.h>
//#include <cssV34/sdk_src/public/Color.h>
#include <cssV34/sdk_src/public/basehandle.h>
#include <cssV34/sdk_src/public/iclientrenderable.h>
#include <cssV34/sdk_src/public/engine/ishadowmgr.h>

namespace SOURCESDK {
namespace CSSV34 {

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IServerEntity;
class IClientEntity;
class IToolSystem;
class IClientRenderable;
//class Vector;
//class QAngle;
class CBaseEntity;
class CBaseAnimating;
class CTakeDamageInfo;
class ITempEntsSystem;
class IEntityFactoryDictionary;
class CBaseTempEntity;
class CGlobalEntityList;
class IEntityFindFilter;

class KeyValues;

//-----------------------------------------------------------------------------
// Safe accessor to an entity
//-----------------------------------------------------------------------------
typedef unsigned int HTOOLHANDLE;
enum
{
	HTOOLHANDLE_INVALID = 0
};


//-----------------------------------------------------------------------------
// If you change this, change the flags in IClientShadowMgr.h also
//-----------------------------------------------------------------------------
enum ClientShadowFlags_t
{
	SHADOW_FLAGS_USE_RENDER_TO_TEXTURE	= (SHADOW_FLAGS_LAST_FLAG<<1),
	SHADOW_FLAGS_ANIMATING_SOURCE		= (SHADOW_FLAGS_LAST_FLAG<<2),
	SHADOW_FLAGS_USE_DEPTH_TEXTURE		= (SHADOW_FLAGS_LAST_FLAG<<3),
	// Update this if you add flags
	CLIENT_SHADOW_FLAGS_LAST_FLAG		= SHADOW_FLAGS_USE_DEPTH_TEXTURE
};


//-----------------------------------------------------------------------------
// Opaque pointer returned from Find* methods, don't store this, you need to 
// Attach it to a tool entity or discard after searching
//-----------------------------------------------------------------------------
typedef void *EntitySearchResult;


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
#ifndef SOURCESDK_CSSV34_NO_TOOLFRAMEWORK
#define ToolsEnabled() 1
#else
#define ToolsEnabled() 0
#endif

//-----------------------------------------------------------------------------
// Purpose: Client side tool interace (right now just handles IClientRenderables).
//  In theory could support hooking into client side entities directly
//-----------------------------------------------------------------------------
class IClientTools : public IBaseInterface
{
public:
	// Allocates or returns the handle to an entity previously found using the Find* APIs below
	virtual HTOOLHANDLE		AttachToEntity( EntitySearchResult entityToAttach ) = 0;

	// Checks whether a handle is still valid.
	virtual bool			IsValidHandle( HTOOLHANDLE handle ) = 0;

	// Iterates the list of entities which have been associated with tools
	virtual int				GetNumRecordables() = 0;
	virtual HTOOLHANDLE		GetRecordable( int index ) = 0;

	// Iterates through ALL entities (separate list for client vs. server)
	virtual EntitySearchResult	NextEntity( EntitySearchResult currentEnt ) = 0;
	EntitySearchResult			FirstEntity() { return NextEntity(SOURCESDK_NULL); }

	// Use this to turn on/off the presence of an underlying game entity
	virtual void			SetEnabled( HTOOLHANDLE handle, bool enabled ) = 0;
	// Use this to tell an entity to post "state" to all listening tools
	virtual void			SetRecording( HTOOLHANDLE handle, bool recording ) = 0;

	virtual HTOOLHANDLE		GetToolHandleForEntityByIndex( int entindex ) = 0;

	virtual int				GetModelIndex( HTOOLHANDLE handle ) = 0;
	virtual const char*		GetModelName ( HTOOLHANDLE handle ) = 0;
	virtual const char*		GetClassname ( HTOOLHANDLE handle ) = 0;

	virtual void			AddClientRenderable( IClientRenderable *pRenderable, int renderGroup ) = 0;
	virtual void			RemoveClientRenderable( IClientRenderable *pRenderable ) = 0;
	virtual void			SetRenderGroup( IClientRenderable *pRenderable, int renderGroup ) = 0;
	virtual void			MarkClientRenderableDirty( IClientRenderable *pRenderable ) = 0;

	virtual bool			DrawSprite( IClientRenderable *pRenderable, float scale, float frame, int rendermode, int renderfx, const Color &color, int *pVisHandle ) = 0;

	virtual EntitySearchResult	GetLocalPlayer() = 0;
	virtual bool			GetLocalPlayerEyePosition( Vector& org, QAngle& ang, float &fov ) = 0;

	// See ClientShadowFlags_t above
	virtual ClientShadowHandle_t CreateShadow( CBaseHandle handle, int nFlags ) = 0;
	virtual void			DestroyShadow( ClientShadowHandle_t h ) = 0;

	virtual void			AddToDirtyShadowList( ClientShadowHandle_t h, bool force = false ) = 0;
	virtual void			MarkRenderToTextureShadowDirty( ClientShadowHandle_t h ) = 0;

	// Global toggle for recording
	virtual void			EnableRecordingMode( bool bEnable ) = 0;
	virtual bool			IsInRecordingMode() const = 0;

	// Trigger a temp entity
	virtual void			TriggerTempEntity( KeyValues *pKeyValues ) = 0;

	// get owning weapon (for viewmodels)
	virtual int				GetOwningWeaponEntIndex( int entindex ) = 0;
	virtual int				GetEntIndex( EntitySearchResult entityToAttach ) = 0;

	virtual int				FindGlobalFlexcontroller( char const *name ) = 0;
	virtual char const		*GetGlobalFlexControllerName( int idx ) = 0;

	// helper for traversing ownership hierarchy
	virtual EntitySearchResult	GetOwnerEntity( EntitySearchResult currentEnt ) = 0;

	// common and useful types to query for hierarchically
	virtual bool			IsPlayer			 ( EntitySearchResult currentEnt ) = 0;
	virtual bool			IsBaseCombatCharacter( EntitySearchResult currentEnt ) = 0;
	virtual bool			IsNPC				 ( EntitySearchResult currentEnt ) = 0;

	virtual Vector			GetAbsOrigin( HTOOLHANDLE handle ) = 0;
	virtual QAngle			GetAbsAngles( HTOOLHANDLE handle ) = 0;
};

#define SOURCESDK_CSSV34_VCLIENTTOOLS_INTERFACE_VERSION "VCLIENTTOOLS001"


/*
//-----------------------------------------------------------------------------
// Purpose: Interface from engine to tools for manipulating entities
//-----------------------------------------------------------------------------
class IServerTools : public IBaseInterface
{
public:
	virtual IServerEntity *GetIServerEntity( IClientEntity *pClientEntity ) = 0;
	virtual bool SnapPlayerToPosition( const Vector &org, const QAngle &ang, IClientEntity *pClientPlayer = SOURCESDK_NULL ) = 0;
	virtual bool GetPlayerPosition( Vector &org, QAngle &ang, IClientEntity *pClientPlayer = SOURCESDK_NULL ) = 0;
	virtual bool SetPlayerFOV( int fov, IClientEntity *pClientPlayer = SOURCESDK_NULL ) = 0;
	virtual int GetPlayerFOV( IClientEntity *pClientPlayer = SOURCESDK_NULL ) = 0;
};

#define VSERVERTOOLS_INTERFACE_VERSION "VSERVERTOOLS001"

//-----------------------------------------------------------------------------
// Purpose: Client side tool interace (right now just handles IClientRenderables).
//  In theory could support hooking into client side entities directly
//-----------------------------------------------------------------------------
class IServerChoreoTools : public IBaseInterface
{
public:

	// Iterates through ALL entities (separate list for client vs. server)
	virtual EntitySearchResult	NextChoreoEntity( EntitySearchResult currentEnt ) = 0;
	EntitySearchResult			FirstChoreoEntity() { return NextChoreoEntity( SOURCESDK_NULL ); } 
	virtual const char			*GetSceneFile( EntitySearchResult sr ) = 0;

	// For interactive editing
	virtual int					GetEntIndex( EntitySearchResult sr ) = 0;
	virtual void				ReloadSceneFromDisk( int entindex ) = 0;
};

#define VSERVERCHOREOTOOLS_INTERFACE_VERSION "VSERVERCHOREOTOOLS001"
*/

} // namespace CSSV34 {
} // namespace SOURCESDK {

#endif // SOURCESDK_CSSV34_ITOOLENTITY_H
