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

#ifndef SOURCESDK_CS2_CONVAR_H
#define SOURCESDK_CS2_CONVAR_H

#if _WIN32
#pragma once
#endif

//#include "tier0/dbg.h"
#include "iconvar.h"
#include "../tier1/utlvector.h"
//#include "tier1/utlstring.h"
//#include "color.h"

#include "../../../../AfxHookSource/SourceSdkShared.h"
#include "../tier0/platform.h"
#include "../tier0/memalloc.h"
#include "../playerslot.h"

class WrpConVarRef;

namespace SOURCESDK {
namespace CS2 {

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class ConVar;
class CCommand;
class ConCommand;
class ConCommandBase;
class ConVarRefAbstract;
class ConCommandRefAbstract;
struct characterset_t;

class SOURCESDK_CS2_ALIGN8 ConVarHandle
{
public:
    bool IsValid() { return value != kInvalidConVarHandle; }
	uint32 Get() { return value; }
	void Set( uint32 _value ) { value = _value; }

private:
    uint32 value = kInvalidConVarHandle;

private:
	static const uint32 kInvalidConVarHandle = 0xFFFFFFFF;
} SOURCESDK_CS2_ALIGN8_POST;
enum CommandTarget_t
{
	CT_NO_TARGET = -1,
	CT_FIRST_SPLITSCREEN_CLIENT = 0,
	CT_LAST_SPLITSCREEN_CLIENT = 3,
};

class CCommandContext
{
public:
	CCommandContext(CommandTarget_t nTarget, CPlayerSlot nSlot) :
		m_nTarget(nTarget), m_nPlayerSlot(nSlot)
	{
	}

	CommandTarget_t GetTarget() const
	{
		return m_nTarget;
	}

	CPlayerSlot GetPlayerSlot() const
	{
		return m_nPlayerSlot;
	}

private:
	CommandTarget_t m_nTarget;
	CPlayerSlot m_nPlayerSlot;
};

class SOURCESDK_CS2_ALIGN8 ConCommandHandle
{
public:
    bool IsValid() { return value != kInvalidConCommandHandle; }
	uint16 Get() { return value; }
	void Set( uint16 _value ) { value = _value; }
	void Reset() { value = kInvalidConCommandHandle; }

	bool HasCallback() const;
	void Dispatch( const CCommandContext& context, const CCommand& command );

	void Unregister();

private:
    uint16 value = kInvalidConCommandHandle;

private:
	static const uint16 kInvalidConCommandHandle = 0xFFFF;
} SOURCESDK_CS2_ALIGN8_POST;

struct CSplitScreenSlot
{
	CSplitScreenSlot( int index )
	{
		m_Data = index;
	}
	
	int Get() const
	{
		return m_Data;
	}
	
	int m_Data;
};

class Vector2D {
	vec_t x, y;
};

class Vector4D {
	vec_t x, y, z, w;
};

union CVValue_t
{
	bool		m_bValue;
	short		m_i16Value;
	uint16		m_u16Value;
	int			m_i32Value;
	uint		m_u32Value;
	int64		m_i64Value;
	uint64		m_u64Value;
	float		m_flValue;
	double		m_dbValue;
	const char *m_szValue;
	Color		m_clrValue;
	Vector2D	m_vec2Value;
	Vector		m_vec3Value;
	Vector4D	m_vec4Value;
	QAngle		m_angValue;
};

#define SOURCESDK_CS2_CVAR_INTERFACE_VERSION "VEngineCvar007"

//-----------------------------------------------------------------------------
// Command tokenizer
//-----------------------------------------------------------------------------
class CCommand
{
public:
	CCommand();
	CCommand( int nArgC, const char **ppArgV );
	//TODO//bool Tokenize( const char *pCommand, characterset_t *pBreakSet = NULL );
	void Reset();

	int ArgC() const;
	const char **ArgV() const;
	const char *ArgS() const;					// All args that occur after the 0th arg, in string form
	const char *GetCommandString() const;		// The entire command in string form, including the 0th arg
	const char *operator[]( int nIndex ) const;	// Gets at arguments
	const char *Arg( int nIndex ) const;		// Gets at arguments
	
	// Helper functions to parse arguments to commands.
	// 
	// Returns index of argument, or -1 if no such argument.
	int FindArg( const char *pName ) const;

	int FindArgInt( const char *pName, int nDefaultVal ) const;

	static int MaxCommandLength();
	//TODO//static characterset_t* DefaultBreakSet();

private:
	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

    int m_nArgv0Size;
    CUtlVectorFixedGrowable<char, COMMAND_MAX_LENGTH> m_ArgSBuffer;
    CUtlVectorFixedGrowable<char, COMMAND_MAX_LENGTH> m_ArgvBuffer;
    CUtlVectorFixedGrowable<char*, COMMAND_MAX_ARGC> m_Args;
};

inline int CCommand::MaxCommandLength()
{
	return COMMAND_MAX_LENGTH - 1;
}

inline int CCommand::ArgC() const
{
	return m_Args.Count();
}

inline const char **CCommand::ArgV() const
{
	return ArgC() ? (const char**)m_Args.Base() : NULL;
}

inline const char *CCommand::ArgS() const
{
	return m_nArgv0Size ? (m_ArgSBuffer.Base() + m_nArgv0Size) : "";
}

inline const char *CCommand::GetCommandString() const
{
	return ArgC() ? m_ArgSBuffer.Base() : "";
}

inline const char *CCommand::Arg( int nIndex ) const
{
	// FIXME: Many command handlers appear to not be particularly careful
	// about checking for valid argc range. For now, we're going to
	// do the extra check and return an empty string if it's out of range
	if ( nIndex < 0 || nIndex >= ArgC() )
		return "";
	return m_Args[nIndex];
}

inline const char *CCommand::operator[]( int nIndex ) const
{
	return Arg( nIndex );
}


} // namespace CS2 {
} // namespace SOURCESDK {

#endif // SOURCESDK_CS2_CONVAR_H
