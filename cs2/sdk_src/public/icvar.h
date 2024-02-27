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

// size: 10*8 Bytes
struct Cvar_s {

	const char* m_pszName;

	void* m_defaultValue;
	void* m_minValue;
	void* m_maxValue;
	const char* m_pszHelpString;
	EConVarType m_eVarType;

	// This gets copied from the ConVarDesc_t on creation
	short unk1;

	unsigned int m_iTimesChanged;
	int64 m_nFlags;
	unsigned int m_iCallbackIndex;

	// Used when setting default, max, min values from the ConVarDesc_t
	// although that's not the only place of usage
	// flags seems to be:
	// (1 << 0) Skip setting value to split screen slots and also something keyvalues related
	// (1 << 1) Skip setting default value
	// (1 << 2) Skip setting min/max values
	int m_nUnknownAllocFlags;

	CVValue_t m_Value= {}; 
};

// size: 8*8 Bytes
class CCmd {
public:
	CCmd(const char * pszName, const char * pszHelpString, int nFlags, ICommandCallback * pCommandCallback) {
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

	int GetFlags() const {
		return m_nFlags;
	}

	void SetFlags(int value) {
		m_nFlags = value;
	}

	CvarIterator GetNextCommand() const {
		return m_NextCommand;
	}

private:
	const char 					*m_pszName;
	const char 					*m_pszHelpString;
	int							m_nFlags;
	int _unknown_20 = 0;
	void * m_pCommandCallback;
	size_t _unknown_32 = 0x0101; // this indicates that we are using the interface version and not a flat callback I guess.
	size_t _unknown_40 = 0;
	size_t _unknown_48 = 0x01;
	CvarIterator m_NextCommand;
};


typedef int CVarDLLIdentifier_t;

//-----------------------------------------------------------------------------
// Purpose: DLL interface to ConVars/ConCommands
//-----------------------------------------------------------------------------
SOURCESDK_abstract_class ICvar : public IAppSystem
{
public:
	virtual ConVarHandle	FindConVar( const char *name, bool bDiallowDeveloper = true ) = 0; //:011
	virtual ConVarHandle	FindFirstConVar() = 0; //:012
	virtual ConVarHandle	FindNextConVar( ConVarHandle prev ) = 0; //:013
	virtual void			CallChangeCallback( ConVarHandle cvarid, CSplitScreenSlot nSlot, CVValue_t *pNewValue, CVValue_t *pOldValue ) = 0; //:014

	virtual ConCommandHandle	FindCommand( const char *name, bool bDiallowDeveloper = true ) = 0; //:015
	virtual ConCommandHandle	FindFirstCommand() = 0; //:016
	virtual ConCommandHandle	FindNextCommand( ConCommandHandle prev ) = 0; //:016
	virtual void				DispatchConCommand( ConCommandHandle cmd, const CCommandContext &ctx, const CCommand &args ) = 0; //:018

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

	virtual Cvar_s * GetCvar( size_t i ); //:037

	virtual CvarIterator RegisterConCommand( CCmd * pCmd  ) = 0; //:038
	virtual void _Unknown_039(void) = 0;

	virtual CCmd * GetCmd( size_t i ); //:040;
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
