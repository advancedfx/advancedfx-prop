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
#include "tier1/iconvar.h"
//#include "tier1/utlvector.h"
#include "tier0/memalloc.h"
#include "../../../AfxHookSource/SourceSdkShared.h"

#include <string>
#include <windows.h>

namespace SOURCESDK {
namespace CS2 {


class ConCommandBase;
class ConCommand;
class ConVar;
class Color;


//-----------------------------------------------------------------------------
// ConVars/ComCommands are marked as having a particular DLL identifier
//-----------------------------------------------------------------------------
typedef int CVarDLLIdentifier_t;


//-----------------------------------------------------------------------------
// Used to display console messages
//-----------------------------------------------------------------------------
SOURCESDK_abstract_class IConsoleDisplayFunc
{
public:
	virtual void ColorPrint( const Color& clr, const char *pMessage ) = 0;
	virtual void Print( const char *pMessage ) = 0;
	virtual void DPrint( const char *pMessage ) = 0;

	virtual void GetConsoleText( char *pchText, size_t bufSize ) const = 0;
};


//-----------------------------------------------------------------------------
// Purpose: Applications can implement this to modify behavior in ICvar
//-----------------------------------------------------------------------------
//#define CVAR_QUERY_INTERFACE_VERSION "VCvarQuery001"
//abstract_class ICvarQuery : public IAppSystem
//{
//public:
//	// Can these two convars be aliased?
//	virtual bool AreConVarsLinkable( const ConVar *child, const ConVar *parent ) = 0;
//};
class ICvarQuery;


// size: 7*8 Bytes
struct Cvar_s {
	const char 					*m_pszName;
	void * _unknown_8;
	void * _unknown_16;
	void * _unknown_24;
	const char 					*m_pszHelpString;
	void * _unknown_40;
	int							m_nFlags;	
};

// size: 10*8 Bytes
struct Cmd_s {
	const char 					*m_pszName;
	const char 					*m_pszHelpString;
	int							m_nFlags;	
};


//-----------------------------------------------------------------------------
// Purpose: DLL interface to ConVars/ConCommands
//-----------------------------------------------------------------------------
SOURCESDK_abstract_class ICvar : public IAppSystem
{
public:
	struct Iterator{
		size_t index;

		bool IsValid() const {
			return index != (unsigned short)-1;
		}

		size_t GetIndex() const {
			return index;
		}
	};

	// Allocate a unique DLL identifier
	virtual CVarDLLIdentifier_t AllocateDLLIdentifier() = 0; //:010

	virtual void _Unknown_011(void) = 0;

	// Usless, doesn't iterate over hidden cvars:
	virtual Iterator GetCvarBegin() = 0; //:012
	virtual Iterator GetCvarNext( Iterator iterator ) = 0; //:013

	virtual void _Unknown_014(void) = 0;
	virtual void _Unknown_015(void) = 0;

	// Usless, doesn't iterate over hidden cmds:
	virtual Iterator GetCmdBegin() = 0; //:016
	virtual Iterator GetCmdNext( Iterator iterator ) = 0; //:017

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

	virtual void _Unknown_038(void) = 0;
	virtual void _Unknown_039(void) = 0;

	virtual Cmd_s * GetCmd( size_t i ); //:040;

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
