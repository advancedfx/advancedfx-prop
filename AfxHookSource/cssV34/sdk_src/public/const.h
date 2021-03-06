//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef SOURCESDK_CSSV34_CONST_H
#define SOURCESDK_CSSV34_CONST_H

#ifdef _WIN32
#pragma once
#endif


namespace SOURCESDK {
namespace CSSV34 {

// the command line param that tells the engine to use steam
#define SOURCESDK_CSSV34_STEAM_PARM					"-steam"
// the command line param to tell dedicated server to restart 
// if they are out of date
#define SOURCESDK_CSSV34_AUTO_RESTART "-autoupdate"

// the message a server sends when a clients steam login is expired
#define SOURCESDK_CSSV34_INVALID_STEAM_TICKET "Invalid STEAM UserID Ticket\n"
#define SOURCESDK_CSSV34_INVALID_STEAM_LOGON "No Steam logon\n"
#define SOURCESDK_CSSV34_INVALID_STEAM_VACBANSTATE "VAC banned from secure server\n"
#define SOURCESDK_CSSV34_INVALID_STEAM_LOGGED_IN_ELSEWHERE "This Steam account is being used in another location\n"

// This is the default, see shareddefs.h for mod-specific value, which can override this
#define SOURCESDK_CSSV34_DEFAULT_TICK_INTERVAL	(0.015)				// 15 msec is the default
#define SOURCESDK_CSSV34_MINIMUM_TICK_INTERVAL   (0.001)
#define SOURCESDK_CSSV34_MAXIMUM_TICK_INTERVAL	(0.1)

// This is the max # of players the engine can handle
#define SOURCESDK_CSSV34_ABSOLUTE_PLAYER_LIMIT 255  // not 256, so we can send the limit as a byte 
#define SOURCESDK_CSSV34_ABSOLUTE_PLAYER_LIMIT_DW	( (SOURCESDK_CSSV34_ABSOLUTE_PLAYER_LIMIT/32) + 1 )

#define SOURCESDK_CSSV34_MAX_PLAYER_NAME_LENGTH		32	// a player name may have 31 chars + 0
#define SOURCESDK_CSSV34_MAX_MAP_NAME				32	
#define	SOURCESDK_CSSV34_MAX_NETWORKID_LENGTH		64  // num chars for a network (i.e steam) ID

// BUGBUG: Reconcile with or derive this from the engine's internal definition!
// FIXME: I added an extra bit because I needed to make it signed
#define SOURCESDK_CSSV34_SP_MODEL_INDEX_BITS			11

// How many bits to use to encode an edict.
#define	SOURCESDK_CSSV34_MAX_EDICT_BITS				11			// # of bits needed to represent max edicts
// Max # of edicts in a level
#define	SOURCESDK_CSSV34_MAX_EDICTS					(1<<SOURCESDK_CSSV34_MAX_EDICT_BITS)

// How many bits to use to encode an server class index
#define SOURCESDK_CSSV34_MAX_SERVER_CLASS_BITS		9
// Max # of networkable server classes
#define SOURCESDK_CSSV34_MAX_SERVER_CLASSES			(1<<SOURCESDK_CSSV34_MAX_SERVER_CLASS_BITS)

#define SOURCESDK_CSSV34_SIGNED_GUID_LEN 32 // Hashed CD Key (32 hex alphabetic chars + 0 terminator )

// Used for networking ehandles.
#define SOURCESDK_CSSV34_NUM_ENT_ENTRY_BITS		(SOURCESDK_CSSV34_MAX_EDICT_BITS + 1)
#define SOURCESDK_CSSV34_NUM_ENT_ENTRIES			(1 << SOURCESDK_CSSV34_NUM_ENT_ENTRY_BITS)
#define SOURCESDK_CSSV34_ENT_ENTRY_MASK			(SOURCESDK_CSSV34_NUM_ENT_ENTRIES - 1)
#define SOURCESDK_CSSV34_INVALID_EHANDLE_INDEX	0xFFFFFFFF

#define SOURCESDK_CSSV34_NUM_SERIAL_NUM_BITS		(32 - SOURCESDK_CSSV34_NUM_ENT_ENTRY_BITS)


// Networked ehandles use less bits to encode the serial number.
#define SOURCESDK_CSSV34_NUM_NETWORKED_EHANDLE_SERIAL_NUMBER_BITS	10
#define SOURCESDK_CSSV34_NUM_NETWORKED_EHANDLE_BITS					(SOURCESDK_CSSV34_MAX_EDICT_BITS + SOURCESDK_CSSV34_NUM_NETWORKED_EHANDLE_SERIAL_NUMBER_BITS)
#define SOURCESDK_CSSV34_INVALID_NETWORKED_EHANDLE_VALUE				((1 << SOURCESDK_CSSV34_NUM_NETWORKED_EHANDLE_BITS) - 1)

// This is the maximum amount of data a PackedEntity can have. Having a limit allows us
// to use static arrays sometimes instead of allocating memory all over the place.
#define SOURCESDK_CSSV34_MAX_PACKEDENTITY_DATA	2048

// This is the maximum number of properties that can be delta'd. Must be evenly divisible by 8.
#define SOURCESDK_CSSV34_MAX_PACKEDENTITY_PROPS	1024

// a client can have up to 4 customization files (logo, sounds, models, txt).
#define SOURCESDK_CSSV34_MAX_CUSTOM_FILES		4		// max 4 files
#define SOURCESDK_CSSV34_MAX_CUSTOM_FILE_SIZE	131072	

//
// Constants shared by the engine and dlls
// This header file included by engine files and DLL files.
// Most came from server.h

// CBaseEntity::m_fFlags
// PLAYER SPECIFIC FLAGS FIRST BECAUSE WE USE ONLY A FEW BITS OF NETWORK PRECISION
#define	SOURCESDK_CSSV34_FL_ONGROUND				(1<<0)	// At rest / on the ground
#define SOURCESDK_CSSV34_FL_DUCKING				(1<<1)	// Player flag -- Player is fully crouched
#define	SOURCESDK_CSSV34_FL_WATERJUMP			(1<<2)	// player jumping out of water
#define SOURCESDK_CSSV34_FL_ONTRAIN				(1<<3) // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define SOURCESDK_CSSV34_FL_INRAIN				(1<<4)	// Indicates the entity is standing in rain
#define SOURCESDK_CSSV34_FL_FROZEN				(1<<5) // Player is frozen for 3rd person camera
#define SOURCESDK_CSSV34_FL_ATCONTROLS			(1<<6) // Player can't move, but keeps key inputs for controlling another entity
#define	SOURCESDK_CSSV34_FL_CLIENT				(1<<7)	// Is a player
#define SOURCESDK_CSSV34_FL_FAKECLIENT			(1<<8)	// Fake client, simulated server side; don't send network messages to them

// NOTE if you move things up, make sure to change this value
#define SOURCESDK_CSSV34_PLAYER_FLAG_BITS		9

// NON-PLAYER SPECIFIC (i.e., not used by GameMovement or the client .dll ) -- Can still be applied to players, though
#define	SOURCESDK_CSSV34_FL_INWATER				(1<<9)	// In water
#define	SOURCESDK_CSSV34_FL_FLY					(1<<10)	// Changes the SV_Movestep() behavior to not need to be on ground
#define	SOURCESDK_CSSV34_FL_SWIM					(1<<11)	// Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)
#define	SOURCESDK_CSSV34_FL_CONVEYOR				(1<<12)
#define	SOURCESDK_CSSV34_FL_NPC					(1<<13)
#define	SOURCESDK_CSSV34_FL_GODMODE				(1<<14)
#define	SOURCESDK_CSSV34_FL_NOTARGET				(1<<15)
#define	SOURCESDK_CSSV34_FL_AIMTARGET			(1<<16)	// set if the crosshair needs to aim onto the entity
#define	SOURCESDK_CSSV34_FL_PARTIALGROUND		(1<<17)	// not all corners are valid
#define SOURCESDK_CSSV34_FL_STATICPROP			(1<<18)	// Eetsa static prop!		
#define SOURCESDK_CSSV34_FL_GRAPHED				(1<<19) // worldgraph has this ent listed as something that blocks a connection
#define SOURCESDK_CSSV34_FL_GRENADE				(1<<20)
#define SOURCESDK_CSSV34_FL_STEPMOVEMENT			(1<<21)	// Changes the SV_Movestep() behavior to not do any processing
#define SOURCESDK_CSSV34_FL_DONTTOUCH			(1<<22)	// Doesn't generate touch functions, generates Untouch() for anything it was touching when this flag was set
#define SOURCESDK_CSSV34_FL_BASEVELOCITY			(1<<23)	// Base velocity has been applied this frame (used to convert base velocity into momentum)
#define SOURCESDK_CSSV34_FL_WORLDBRUSH			(1<<24)	// Not moveable/removeable brush entity (really part of the world, but represented as an entity for transparency or something)
#define SOURCESDK_CSSV34_FL_OBJECT				(1<<25) // Terrible name. This is an object that NPCs should see. Missiles, for example.
#define SOURCESDK_CSSV34_FL_KILLME				(1<<26)	// This entity is marked for death -- will be freed by game DLL
#define SOURCESDK_CSSV34_FL_ONFIRE				(1<<27)	// You know...
#define SOURCESDK_CSSV34_FL_DISSOLVING			(1<<28) // We're dissolving!
#define SOURCESDK_CSSV34_FL_TRANSRAGDOLL			(1<<29) // In the process of turning into a client side ragdoll.
#define SOURCESDK_CSSV34_FL_UNBLOCKABLE_BY_PLAYER (1<<30) // pusher that can't be blocked by the player

// edict->movetype values
enum MoveType_t
{
	MOVETYPE_NONE		= 0,	// never moves
	MOVETYPE_ISOMETRIC,			// For players
	MOVETYPE_WALK,				// Player only - moving on the ground
	MOVETYPE_STEP,				// gravity, special edge handling -- monsters use this
	MOVETYPE_FLY,				// No gravity, but still collides with stuff
	MOVETYPE_FLYGRAVITY,		// flies through the air + is affected by gravity
	MOVETYPE_VPHYSICS,			// uses VPHYSICS for simulation
	MOVETYPE_PUSH,				// no clip to world, push and crush
	MOVETYPE_NOCLIP,			// No gravity, no collisions, still do velocity/avelocity
	MOVETYPE_LADDER,			// Used by players only when going onto a ladder
	MOVETYPE_OBSERVER,			// Observer movement, depends on player's observer mode
	MOVETYPE_CUSTOM,			// Allows the entity to describe its own physics

	// should always be defined as the last item in the list
	MOVETYPE_LAST		= MOVETYPE_CUSTOM,

	MOVETYPE_MAX_BITS	= 4
};

// edict->movecollide values
enum MoveCollide_t
{
	MOVECOLLIDE_DEFAULT = 0,

	// These ones only work for MOVETYPE_FLY + MOVETYPE_FLYGRAVITY
	MOVECOLLIDE_FLY_BOUNCE,	// bounces, reflects, based on elasticity of surface and object - applies friction (adjust velocity)
	MOVECOLLIDE_FLY_CUSTOM,	// Touch() will modify the velocity however it likes
	MOVECOLLIDE_FLY_SLIDE,  // slides along surfaces (no bounce) - applies friciton (adjusts velocity)

	MOVECOLLIDE_COUNT,		// Number of different movecollides

	// When adding new movecollide types, make sure this is correct
	MOVECOLLIDE_MAX_BITS = 3
};

// edict->solid values
// NOTE: Some movetypes will cause collisions independent of SOLID_NOT/SOLID_TRIGGER when the entity moves
// SOLID only effects OTHER entities colliding with this one when they move - UGH!

// Solid type basically describes how the bounding volume of the object is represented
// NOTE: SOLID_BBOX MUST BE 2, and SOLID_VPHYSICS MUST BE 6
// NOTE: These numerical values are used in the FGD by the prop code (see prop_dynamic)
enum SolidType_t
{
	SOLID_NONE			= 0,	// no solid model
	SOLID_BSP			= 1,	// a BSP tree
	SOLID_BBOX			= 2,	// an AABB
	SOLID_OBB			= 3,	// an OBB (not implemented yet)
	SOLID_OBB_YAW		= 4,	// an OBB, constrained so that it can only yaw
	SOLID_CUSTOM		= 5,	// Always call into the entity for tests
	SOLID_VPHYSICS		= 6,	// solid vphysics object, get vcollide from the model and collide with that
	SOLID_LAST,
};

enum SolidFlags_t
{
	FSOLID_CUSTOMRAYTEST		= 0x0001,	// Ignore solid type + always call into the entity for ray tests
	FSOLID_CUSTOMBOXTEST		= 0x0002,	// Ignore solid type + always call into the entity for swept box tests
	FSOLID_NOT_SOLID			= 0x0004,	// Are we currently not solid?
	FSOLID_TRIGGER				= 0x0008,	// This is something may be collideable but fires touch functions
											// even when it's not collideable (when the FSOLID_NOT_SOLID flag is set)
	FSOLID_NOT_STANDABLE		= 0x0010,	// You can't stand on this
	FSOLID_VOLUME_CONTENTS		= 0x0020,	// Contains volumetric contents (like water)
	FSOLID_FORCE_WORLD_ALIGNED	= 0x0040,	// Forces the collision rep to be world-aligned even if it's SOLID_BSP or SOLID_VPHYSICS
	FSOLID_USE_TRIGGER_BOUNDS	= 0x0080,	// Uses a special trigger bounds separate from the normal OBB
	FSOLID_ROOT_PARENT_ALIGNED	= 0x0100,	// Collisions are defined in root parent's local coordinate space

	FSOLID_MAX_BITS	= 9
};

//-----------------------------------------------------------------------------
// A couple of inline helper methods
//-----------------------------------------------------------------------------
inline bool IsSolid( SolidType_t solidType, int nSolidFlags )
{
	return (solidType != SOLID_NONE) && ((nSolidFlags & FSOLID_NOT_SOLID) == 0);
}


// m_lifeState values
#define	SOURCESDK_CSSV34_LIFE_ALIVE				0 // alive
#define	SOURCESDK_CSSV34_LIFE_DYING				1 // playing death animation or still falling off of a ledge waiting to hit ground
#define	SOURCESDK_CSSV34_LIFE_DEAD				2 // dead. lying still.
#define SOURCESDK_CSSV34_LIFE_RESPAWNABLE		3
#define SOURCESDK_CSSV34_LIFE_DISCARDBODY		4

// entity effects
enum
{
	EF_BONEMERGE			= 0x001,	// Performs bone merge on client side
	EF_BRIGHTLIGHT 			= 0x002,	// DLIGHT centered at entity origin
	EF_DIMLIGHT 			= 0x004,	// player flashlight
	EF_NOINTERP				= 0x008,	// don't interpolate the next frame
	EF_NOSHADOW				= 0x010,	// Don't cast no shadow
	EF_NODRAW				= 0x020,	// don't draw entity
	EF_NORECEIVESHADOW		= 0x040,	// Don't receive no shadow
	EF_BONEMERGE_FASTCULL	= 0x080,	// For use with EF_BONEMERGE. If this is set, then it places this ent's origin at its
										// parent and uses the parent's bbox + the max extents of the aiment.
										// Otherwise, it sets up the parent's bones every frame to figure out where to place
										// the aiment, which is inefficient because it'll setup the parent's bones even if
										// the parent is not in the PVS.
	EF_ITEM_BLINK			= 0x100,	// blink an item so that the user notices it.
	EF_PARENT_ANIMATES		= 0x200,	// always assume that the parent entity is animating
	EF_MAX_BITS = 10
};

#define SOURCESDK_CSSV34_EF_PARITY_BITS	3
#define SOURCESDK_CSSV34_EF_PARITY_MASK  ((1<<SOURCESDK_CSSV34_EF_PARITY_BITS)-1)

// How many bits does the muzzle flash parity thing get?
#define SOURCESDK_CSSV34_EF_MUZZLEFLASH_BITS 2

// plats
#define	SOURCESDK_CSSV34_PLAT_LOW_TRIGGER	1

// Trains
#define	SOURCESDK_CSSV34_SF_TRAIN_WAIT_RETRIGGER	1
#define SOURCESDK_CSSV34_SF_TRAIN_PASSABLE		8		// Train is not solid -- used to make water trains

// view angle update types for CPlayerState::fixangle
#define SOURCESDK_CSSV34_FIXANGLE_NONE			0
#define SOURCESDK_CSSV34_FIXANGLE_ABSOLUTE		1
#define SOURCESDK_CSSV34_FIXANGLE_RELATIVE		2

// Break Model Defines

#define SOURCESDK_CSSV34_BREAK_GLASS		0x01
#define SOURCESDK_CSSV34_BREAK_METAL		0x02
#define SOURCESDK_CSSV34_BREAK_FLESH		0x04
#define SOURCESDK_CSSV34_BREAK_WOOD		0x08

#define SOURCESDK_CSSV34_BREAK_SMOKE		0x10
#define SOURCESDK_CSSV34_BREAK_TRANS		0x20
#define SOURCESDK_CSSV34_BREAK_CONCRETE	0x40

// If this is set, then we share a lighting origin with the last non-slave breakable sent down to the client
#define SOURCESDK_CSSV34_BREAK_SLAVE		0x80

// Colliding temp entity sounds

#define SOURCESDK_CSSV34_BOUNCE_GLASS	SOURCESDK_CSSV34_BREAK_GLASS
#define	SOURCESDK_CSSV34_BOUNCE_METAL	SOURCESDK_CSSV34_BREAK_METAL
#define SOURCESDK_CSSV34_BOUNCE_FLESH	SOURCESDK_CSSV34_BREAK_FLESH
#define SOURCESDK_CSSV34_BOUNCE_WOOD		SOURCESDK_CSSV34_BREAK_WOOD
#define SOURCESDK_CSSV34_BOUNCE_SHRAP	0x10
#define SOURCESDK_CSSV34_BOUNCE_SHELL	0x20
#define	SOURCESDK_CSSV34_BOUNCE_CONCRETE SOURCESDK_CSSV34_BREAK_CONCRETE
#define SOURCESDK_CSSV34_BOUNCE_SHOTSHELL 0x80

// Temp entity bounce sound types
#define SOURCESDK_CSSV34_TE_BOUNCE_NULL		0
#define SOURCESDK_CSSV34_TE_BOUNCE_SHELL		1
#define SOURCESDK_CSSV34_TE_BOUNCE_SHOTSHELL	2

// Rendering constants
// if this is changed, update common/MaterialSystem/Sprite.cpp
enum RenderMode_t
{	
	kRenderNormal,			// src
	kRenderTransColor,		// c*a+dest*(1-a)
	kRenderTransTexture,	// src*a+dest*(1-a)
	kRenderGlow,			// src*a+dest -- No Z buffer checks -- Fixed size in screen space
	kRenderTransAlpha,		// src*srca+dest*(1-srca)
	kRenderTransAdd,		// src*a+dest
	kRenderEnvironmental,	// not drawn, used for environmental effects
	kRenderTransAddFrameBlend, // use a fractional frame value to blend between animation frames
	kRenderTransAlphaAdd,	// src + dest*(1-a)
	kRenderWorldGlow,		// Same as kRenderGlow but not fixed size in screen space
	kRenderNone,			// Don't render.
};

enum RenderFx_t
{	
	kRenderFxNone = 0, 
	kRenderFxPulseSlow, 
	kRenderFxPulseFast, 
	kRenderFxPulseSlowWide, 
	kRenderFxPulseFastWide, 
	kRenderFxFadeSlow, 
	kRenderFxFadeFast, 
	kRenderFxSolidSlow, 
	kRenderFxSolidFast, 	   
	kRenderFxStrobeSlow, 
	kRenderFxStrobeFast, 
	kRenderFxStrobeFaster, 
	kRenderFxFlickerSlow, 
	kRenderFxFlickerFast,
	kRenderFxNoDissipation,
	kRenderFxDistort,			// Distort/scale/translate flicker
	kRenderFxHologram,			// kRenderFxDistort + distance fade
	kRenderFxExplode,			// Scale up really big!
	kRenderFxGlowShell,			// Glowing Shell
	kRenderFxClampMinScale,		// Keep this sprite from getting very small (SPRITES only!)
	kRenderFxEnvRain,			// for environmental rendermode, make rain
	kRenderFxEnvSnow,			//  "        "            "    , make snow
	kRenderFxSpotlight,			// TEST CODE for experimental spotlight
	kRenderFxRagdoll,			// HACKHACK: TEST CODE for signalling death of a ragdoll character
	kRenderFxPulseFastWider,
	kRenderFxMax
};

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
	COLLISION_GROUP_PLAYER_MOVEMENT,  // For HL2, same as Collision_Group_Player
										
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

	LAST_SHARED_COLLISION_GROUP
};

} // namespace CSSV34 {
} // namespace SOURCESDK {

//#include <csv34/sdk_src/public/tier0/bbasetypes.h>

namespace SOURCESDK {
namespace CSSV34 {

#define SOURCESDK_CSSV34_SOUND_NORMAL_CLIP_DIST	1000.0f

// How many networked area portals do we allow?
#define SOURCESDK_CSSV34_MAX_AREA_STATE_BYTES		32
#define SOURCESDK_CSSV34_MAX_AREA_PORTAL_STATE_BYTES 24

} // namespace CSSV34 {
} // namespace SOURCESDK {

#endif

