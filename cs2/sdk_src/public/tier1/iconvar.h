//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $NoKeywords: $
//===========================================================================//

#ifndef SOURCESDK_CS2_ICONVAR_H
#define SOURCESDK_CS2_ICONVAR_H

#if _WIN32
#pragma once
#endif

//#include "tier0/dbg.h"
//#include "tier0/platform.h"
//#include "tier1/strtools.h"
//#include "color.h"

#include "../../../../AfxHookSource/SourceSdkShared.h"

namespace SOURCESDK {
namespace CS2 {

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class IConVar;
class CCommand;


//-----------------------------------------------------------------------------
// ConVar flags
//-----------------------------------------------------------------------------
// The default, no flags at all
#define SOURCESDK_CS2_FCVAR_NONE				0 

// Command to ConVars and ConCommands
// ConVar Systems
#define SOURCESDK_CS2_FCVAR_UNREGISTERED		(1<<0)	// If this is set, don't add to linked list, etc.
#define SOURCESDK_CS2_FCVAR_DEVELOPMENTONLY	(1<<1)	// Hidden in released products. Flag is removed automatically if ALLOW_DEVELOPMENT_CVARS is defined.
#define SOURCESDK_CS2_FCVAR_GAMEDLL			(1<<2)	// defined by the game DLL
#define SOURCESDK_CS2_FCVAR_CLIENTDLL			(1<<3)  // defined by the client DLL
#define SOURCESDK_CS2_FCVAR_HIDDEN			(1<<4)	// Hidden. Doesn't appear in find or auto complete. Like DEVELOPMENTONLY, but can't be compiled out.

// ConVar only
#define SOURCESDK_CS2_FCVAR_PROTECTED			(1<<5)  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
#define SOURCESDK_CS2_FCVAR_SPONLY			(1<<6)  // This cvar cannot be changed by clients connected to a multiplayer server.
#define	SOURCESDK_CS2_FCVAR_ARCHIVE			(1<<7)	// set to cause it to be saved to vars.rc
#define	SOURCESDK_CS2_FCVAR_NOTIFY			(1<<8)	// notifies players when changed
#define	SOURCESDK_CS2_FCVAR_USERINFO			(1<<9)	// changes the client's info string

#define SOURCESDK_CS2_FCVAR_PRINTABLEONLY		(1<<10)  // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
#define SOURCESDK_CS2_FCVAR_UNLOGGED			(1<<11)  // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
#define SOURCESDK_CS2_FCVAR_NEVER_AS_STRING	(1<<12)  // never try to print that cvar

// It's a ConVar that's shared between the client and the server.
// At signon, the values of all such ConVars are sent from the server to the client (skipped for local
//  client, of course )
// If a change is requested it must come from the console (i.e., no remote client changes)
// If a value is changed while a server is active, it's replicated to all connected clients
#define SOURCESDK_CS2_FCVAR_REPLICATED		(1<<13)	// server setting enforced on clients, TODO rename to FCAR_SERVER at some time
#define SOURCESDK_CS2_FCVAR_CHEAT				(1<<14) // Only useable in singleplayer / debug / multiplayer & sv_cheats
#define SOURCESDK_CS2_FCVAR_SS				(1<<15) // causes varnameN where N == 2 through max splitscreen slots for mod to be autogenerated
#define SOURCESDK_CS2_FCVAR_DEMO				(1<<16) // record this cvar when starting a demo file
#define SOURCESDK_CS2_FCVAR_DONTRECORD		(1<<17) // don't record these command in demofiles
#define SOURCESDK_CS2_FCVAR_SS_ADDED			(1<<18) // This is one of the "added" FCVAR_SS variables for the splitscreen players
#define SOURCESDK_CS2_FCVAR_RELEASE			(1<<19) // Cvars tagged with this are the only cvars avaliable to customers
#define SOURCESDK_CS2_FCVAR_RELOAD_MATERIALS	(1<<20)	// If this cvar changes, it forces a material reload
#define SOURCESDK_CS2_FCVAR_RELOAD_TEXTURES	(1<<21)	// If this cvar changes, if forces a texture reload

#define SOURCESDK_CS2_FCVAR_NOT_CONNECTED		(1<<22)	// cvar cannot be changed by a client that is connected to a server
#define SOURCESDK_CS2_FCVAR_MATERIAL_SYSTEM_THREAD (1<<23)	// Indicates this cvar is read from the material system thread
#define SOURCESDK_CS2_FCVAR_ARCHIVE_XBOX		(1<<24) // cvar written to config.cfg on the Xbox

#define SOURCESDK_CS2_FCVAR_SERVER_CAN_EXECUTE	(1<<28)// the server is allowed to execute this command on clients via ClientCommand/NET_StringCmd/CBaseClientState::ProcessStringCmd.
#define SOURCESDK_CS2_FCVAR_SERVER_CANNOT_QUERY	(1<<29)// If this is set, then the server is not allowed to query this cvar's value (via IServerPluginHelpers::StartQueryCvarValue).
#define SOURCESDK_CS2_FCVAR_CLIENTCMD_CAN_EXECUTE	(1<<30)	// IVEngineClient::ClientCmd is allowed to execute this command. 
// Note: IVEngineClient::ClientCmd_Unrestricted can run any client command.

#define SOURCESDK_CS2_FCVAR_ACCESSIBLE_FROM_THREADS	(1<<25)	// used as a debugging tool necessary to check material system thread convars
// #define FCVAR_AVAILABLE			(1<<26)
// #define FCVAR_AVAILABLE			(1<<27)
// #define FCVAR_AVAILABLE			(1<<31)

#define SOURCESDK_CS2_FCVAR_MATERIAL_THREAD_MASK ( SOURCESDK_CS2_FCVAR_RELOAD_MATERIALS | SOURCESDK_CS2_FCVAR_RELOAD_TEXTURES | SOURCESDK_CS2_FCVAR_MATERIAL_SYSTEM_THREAD )	

//-----------------------------------------------------------------------------
// Called when a ConVar changes value
// NOTE: For FCVAR_NEVER_AS_STRING ConVars, pOldValue == NULL
//-----------------------------------------------------------------------------
typedef void(*FnChangeCallback_t)(IConVar *var, const char *pOldValue, float flOldValue);


//-----------------------------------------------------------------------------
// Abstract interface for ConVars
//-----------------------------------------------------------------------------
SOURCESDK_abstract_class IConVar
{
public:
	// Value set
	virtual void SetValue(const char *pValue) = 0;
	virtual void SetValue(float flValue) = 0;
	virtual void SetValue(int nValue) = 0;
	virtual void SetValue(Color value) = 0;

	// Return name of command
	virtual const char *GetName(void) const = 0;

	// Return name of command (usually == GetName(), except in case of FCVAR_SS_ADDED vars
	virtual const char *GetBaseName(void) const = 0;

	// Accessors.. not as efficient as using GetState()/GetInfo()
	// if you call these methods multiple times on the same IConVar
	virtual bool IsFlagSet(int nFlag) const = 0;

	virtual int GetSplitScreenPlayerSlot() const = 0;
};

} // namespace SOURCESDK {
} // namespace CS2 {

#endif // SOURCESDK_CS2_ICONVAR_H
