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
//#include "tier1/iconvar.h"
//#include "tier1/utlvector.h"
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

class CCommand {
public:
	size_t ArgC() const {
		return m_Args.m_Size;
	}

	const char * ArgS() const {
		return m_ArgSBuffer.m_pMemory;
	}

	const char * ArgV(size_t index) const {
		return m_Args.m_pMemory[index];
	}

private:
	enum
	{
		COMMAND_MAX_ARGC = 64, // might be inaccurate
		COMMAND_MAX_LENGTH = 512,
	};

	int m_nArgv0Size;

	// CUtlVectorFixedGrowable<char, COMMAND_MAX_LENGTH> m_ArgSBuffer;
	struct {
		int m_Size;
		char * m_pMemory;
		int m_nAllocationCount;
		int m_nGrowSize;
		char m_pFixedMemory[ COMMAND_MAX_LENGTH ];
	} m_ArgSBuffer;

	// CUtlVectorFixedGrowable<char, COMMAND_MAX_LENGTH> m_ArgvBuffer;
	struct {
		int m_Size;
		char * m_pMemory;
		int m_nAllocationCount;
		int m_nGrowSize;
		char m_pFixedMemory[ COMMAND_MAX_LENGTH ];
	} m_ArgvBuffer;

	// CUtlVectorFixedGrowable<char*, COMMAND_MAX_ARGC> m_Args;
	struct {
		int m_Size;
		char ** m_pMemory;
		int m_nAllocationCount;
		int m_nGrowSize;
		char * m_pFixedMemory[ COMMAND_MAX_ARGC ];
	} m_Args;
};

class ICommandCallback {
public:
	virtual void CommandCallback(void * _unknown1_rdx_ptr, CCommand * pArgs) = 0;
};

// size: 10*8 Bytes
struct Cvar_s {
	const char 					*m_pszName;
	void * _unknown_8;
	void * _unknown_16;
	void * _unknown_24;
	const char 					*m_pszHelpString;
	void * _unknown_40;
	int							m_nFlags;	
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
	// Allocate a unique DLL identifier
	virtual CVarDLLIdentifier_t AllocateDLLIdentifier() = 0; //:010

	virtual void _Unknown_011(void) = 0;

	// Usless, doesn't iterate over hidden cvars:
	virtual CvarIterator GetCvarBegin() = 0; //:012
	virtual CvarIterator GetCvarNext( CvarIterator iterator ) = 0; //:013

	virtual void _Unknown_014(void) = 0;
	virtual void _Unknown_015(void) = 0;

	// Usless, doesn't iterate over hidden cmds:
	virtual CvarIterator GetCmdBegin() = 0; //:016
	virtual CvarIterator GetCmdNext( CvarIterator iterator ) = 0; //:017

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
