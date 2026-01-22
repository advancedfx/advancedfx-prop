//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
//===========================================================================//

#ifndef SOURCESDK_CS2_ICVAR_H
#define SOURCESDK_CS2_ICVAR_H
#ifdef _WIN32
#pragma once
#endif

#include "appframework/IAppSystem.h"
#include "tier1/convar.h"
#include "tier1/utlvector.h"
#include "tier0/memalloc.h"
#include "../../../AfxHookSource/SourceSdkShared.h"

#include <string>
#include <windows.h>

namespace SOURCESDK {
namespace CS2 {

#define SOURCESDK_CS2_MAX_VALID_CVARS 8192
#define SOURCESDK_CS2_MAX_VALID_CVAR_INDEX (SOURCESDK_CS2_MAX_VALID_CVARS-1)

struct CvarIterator{
	size_t index = (unsigned short)-1;

	bool IsValid() const {
		return index != (unsigned short)-1;
	}

	size_t GetIndex() const {
		return index;
	}
};

class ICommandCallback {
public:
	virtual void CommandCallback(void * _unknown1_rdx_ptr, CCommand * pArgs) = 0;
};


//-----------------------------------------------------------------------------
// Purpose: Internal structure of ConVar objects
//-----------------------------------------------------------------------------
enum EConVarType : int16_t
{
	EConVarType_Invalid = -1,
	EConVarType_Bool,
	EConVarType_Int16,
	EConVarType_UInt16,
	EConVarType_Int32,
	EConVarType_UInt32,
	EConVarType_Int64,
	EConVarType_UInt64,
	EConVarType_Float32,
	EConVarType_Float64,
	EConVarType_String,
	EConVarType_Color,
	EConVarType_Vector2,
	EConVarType_Vector3,
	EConVarType_Vector4,
	EConVarType_Qangle,
	EConVarType_MAX
};

struct Cvar_s {

	const char* m_pszName;

	// Default value is expected to always be present,
	// even if convar wasn't created with default value
	// it would use global per type default value in that case
	CVValue_t* m_defaultValue;

	// Min/Max Could be nullptr if not set
	CVValue_t* m_minValue;
	CVValue_t* m_maxValue;

	const char* m_pszHelpString;
	EConVarType m_eVarType;

	// Might be set by a gameinfo config via "version" key
	short m_Version;

	unsigned int m_iTimesChanged;
	int64 m_nFlags;

	// Index into a linked list of cvar callbacks
	unsigned int m_iCallbackIndex;
	// Index into a linked list of cvar filter callbacks
	unsigned int m_iFilterCBIndex;

	int m_GameInfoFlags;
	int m_UserInfoByteIndex;

	void * unk0x48;
	void * unk0x50;

	// This one leaks memory on destroy like the original thing in AlliedModders hl2sdk/cs2 (but cvars are only allocated one-time during a run of the game).
	CVValue_t m_Value= {}; 
};

// size: 8*8 Bytes
class CCmd {
public:
	CCmd(const char * pszName, const char * pszHelpString, int64 nFlags, ICommandCallback * pCommandCallback) {
		m_pszName = pszName;
		m_pszHelpString = pszHelpString;
		m_nFlags = nFlags;
		m_pCommandCallback = pCommandCallback;
	}

	const char * GetName() const {
		return m_pszName;
	}

	const char * GetHelpString() const {
		return m_pszHelpString;
	}

	int64 GetFlags() const {
		return m_nFlags;
	}

	void SetFlags(int64 value) {
		m_nFlags = value;
	}

	CvarIterator GetNextCommand() const {
		return m_NextCommand;
	}

private:
	const char 					*m_pszName;
	const char 					*m_pszHelpString;
	int64							m_nFlags;
	void * m_pCommandCallback;
	size_t _unknown_32 = 0x0101; // this indicates that we are using the interface version and not a flat callback I guess.
	size_t _unknown_40 = 0;
	size_t _unknown_48 = 0x01;
	CvarIterator m_NextCommand;
};


typedef int CVarDLLIdentifier_t;
typedef void * FnCvarCallbacksReader_t;

//-----------------------------------------------------------------------------
// Purpose: DLL interface to ConVars/ConCommands
//-----------------------------------------------------------------------------
SOURCESDK_abstract_class ICvar : public IAppSystem
{
public:
	virtual ConVarHandle	FindConVar( const char *name, bool bDiallowDeveloper = true ) = 0; //:011
	virtual ConVarHandle	FindFirstConVar() = 0; //:012
	virtual ConVarHandle	FindNextConVar( ConVarHandle prev ) = 0; //:013

	virtual void			CallChangeCallback( ConVarHandle cvar, const CSplitScreenSlot nSlot, const CVValue_t* pNewValue, const CVValue_t* pOldValue, void *__unk01 = nullptr ) = 0; //:014
	// Would call cb for every change callback defined for this cvar
	virtual void			IterateConVarCallbacks( ConVarHandle cvar, FnCvarCallbacksReader_t cb ) = 0; //:015
	// If returns false value shouldn't be modified
	virtual bool			CallFilterCallback( ConVarHandle cvar, const CSplitScreenSlot nSlot, const CVValue_t *pNewValue, const CVValue_t *pOldValue, void *__unk01 = nullptr ) = 0; //:016

	virtual ConCommandHandle	FindCommand( const char *name, bool bDiallowDeveloper = true ) = 0; //:017
	virtual ConCommandHandle	FindFirstCommand() = 0; //:018
	virtual ConCommandHandle	FindNextCommand( ConCommandHandle prev ) = 0; //:019
	virtual void				DispatchConCommand( ConCommandHandle cmd, const CCommandContext &ctx, const CCommand &args ) = 0; //:020

	virtual void _Unknown_021(void) = 0; // InstallGlobalChangeCallback
	virtual void _Unknown_022(void) = 0; // RemoveGlobalChangeCallback
	virtual void _Unknown_023(void) = 0; // CallGlobalChangeCallbacks
	virtual void _Unknown_024(void) = 0; // ResetConVarsToDefaultValuesByFlag
	virtual void _Unknown_025(void) = 0; // SetMaxSplitScreenSlots
	virtual void _Unknown_026(void) = 0; // GetMaxSplitScreenSlots
	virtual void _Unknown_027(void) = 0; // RegisterCreationListeners
	virtual void _Unknown_028(void) = 0; // RemoveCreationListeners
	virtual void _Unknown_029(void) = 0; // unknown
	virtual void _Unknown_030(void) = 0; // ResetConVarsToDefaultValuesByName
	virtual void _Unknown_031(void) = 0; // TakeConVarSnapshot
	virtual void _Unknown_032(void) = 0; // ResetConVarsToSnapshot
	virtual void _Unknown_033(void) = 0; // DestroyConVarSnapshot
	virtual void _Unknown_034(void) = 0; // GetCharacterSet
	virtual void _Unknown_035(void) = 0; // SetConVarsFromGameInfo
	virtual void _Unknown_036(void) = 0; // StripDevelopmentFlags
	virtual void _Unknown_037(void) = 0; // GetTotalUserInfoCvarsByteSize
	virtual void _Unknown_038(void) = 0; // CopyUserInfoCvarDefaults
	virtual void _Unknown_039(void) = 0; // RegisterConVar
	virtual void _Unknown_040(void) = 0; // UnregisterConVarCallbacks
	virtual void _Unknown_041(void) = 0; // LockConVarValueInitialisation
	virtual void _Unknown_042(void) = 0; 

	virtual Cvar_s * GetCvar( size_t i ); //:043

	virtual CvarIterator RegisterConCommand( CCmd * pCmd, int64 nAdditionalFlags = 0 ) = 0; //:044
	virtual void UnregisterConCommand( size_t i ) = 0; //:045

	virtual CCmd * GetCmd( size_t i ); //:046;
};

//-----------------------------------------------------------------------------
// These global names are defined by tier1.h, duplicated here so you
// don't have to include tier1.h
//-----------------------------------------------------------------------------

// These are marked DLL_EXPORT for Linux.
//DECLARE_TIER1_INTERFACE( ICvar, cvar );
//DECLARE_TIER1_INTERFACE( ICvar, g_pCVar );
extern ICvar * cvar;
extern ICvar * g_pCVar;


} // namespace SOURCESDK {
} // namespace CS2 {

#endif // SOURCESDK_CS2_ICVAR_H
