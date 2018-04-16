//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $NoKeywords: $
//===========================================================================//

#ifndef SOURCESDK_CSGO_CONVAR_H
#define SOURCESDK_CSGO_CONVAR_H

#if _WIN32
#pragma once
#endif

//#include "tier0/dbg.h"
#include <csgo/sdk_src/public/tier1/iconvar.h>
//#include "tier1/utlvector.h"
//#include "tier1/utlstring.h"
//#include "color.h"
#include <csgo/sdk_src/public/icvar.h>

#include <SourceSdkShared.h>
#include <csgo/sdk_src/public/tier0/memalloc.h>

class WrpConVarRef;

namespace SOURCESDK {
namespace CSGO {

#define SOURCESDK_CSGO_CVAR_INTERFACE_VERSION "VEngineCvar007"

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------
class ConVar;
class CCommand;
class ConCommand;
class ConCommandBase;
struct characterset_t;



//-----------------------------------------------------------------------------
// Any executable that wants to use ConVars need to implement one of
// these to hook up access to console variables.
//-----------------------------------------------------------------------------
class IConCommandBaseAccessor
{
public:
	// Flags is a combination of FCVAR flags in cvar.h.
	// hOut is filled in with a handle to the variable.
	virtual bool RegisterConCommandBase(ConCommandBase *pVar) = 0;
};


//-----------------------------------------------------------------------------
// Called when a ConCommand needs to execute
//-----------------------------------------------------------------------------
typedef void(*FnCommandCallbackV1_t)(void);
typedef void(*FnCommandCallback_t)(const CCommand &command);

#define SOURCESDK_CSGO_COMMAND_COMPLETION_MAXITEMS		64
#define SOURCESDK_CSGO_COMMAND_COMPLETION_ITEM_LENGTH	64

//-----------------------------------------------------------------------------
// Returns 0 to COMMAND_COMPLETION_MAXITEMS worth of completion strings
//-----------------------------------------------------------------------------
typedef int(*FnCommandCompletionCallback)(const char *partial, char commands[SOURCESDK_CSGO_COMMAND_COMPLETION_MAXITEMS][SOURCESDK_CSGO_COMMAND_COMPLETION_ITEM_LENGTH]);


//-----------------------------------------------------------------------------
// Interface version
//-----------------------------------------------------------------------------
class ICommandCallback
{
public:
	virtual void CommandCallback(const CCommand &command) = 0;
};

class ICommandCompletionCallback;


//-----------------------------------------------------------------------------
// Purpose: The base console invoked command/cvar interface
//-----------------------------------------------------------------------------
class ConCommandBase
{
	friend class CCvar;
	friend class ConVar;
	friend class ConCommand;
	friend void ConVar_Register( int nCVarFlag, IConCommandBaseAccessor *pAccessor );
	friend void ConVar_PublishToVXConsole();

	// FIXME: Remove when ConVar changes are done
	friend class CDefaultCvar;

public:
								ConCommandBase( void );
								ConCommandBase( const char *pName, const char *pHelpString = 0, 
									int flags = 0 );

	virtual						~ConCommandBase( void );

	virtual	bool				IsCommand( void ) const;

	// Check flag
	virtual bool				IsFlagSet( int flag ) const;
	// Set flag
	virtual void				AddFlags( int flags );
	// Clear flag
	virtual void				RemoveFlags( int flags );

	virtual int					GetFlags() const;

	// Return name of cvar
	virtual const char			*GetName( void ) const;

	// Return help text for cvar
	virtual const char			*GetHelpText( void ) const;

	// Deal with next pointer
	const ConCommandBase		*GetNext( void ) const;
	ConCommandBase				*GetNext( void );
	
	virtual bool				IsRegistered( void ) const;

	// Returns the DLL identifier
	virtual CVarDLLIdentifier_t	GetDLLIdentifier() const;

protected:
	virtual void				Create( const char *pName, const char *pHelpString = 0, 
									int flags = 0 );

	// Used internally by OneTimeInit to initialize/shutdown
	virtual void				Init();
	void						Shutdown();

	// Internal copy routine ( uses new operator from correct module )
	char						*CopyString( const char *from );

private:
	// Next ConVar in chain
	// Prior to register, it points to the next convar in the DLL.
	// Once registered, though, m_pNext is reset to point to the next
	// convar in the global list
	ConCommandBase				*m_pNext;

	// Has the cvar been added to the global list?
	bool						m_bRegistered;

	// Static data
	const char 					*m_pszName;
	const char 					*m_pszHelpString;
	
	// ConVar flags
	int							m_nFlags;

protected:
	// ConVars add themselves to this list for the executable. 
	// Then ConVar_Register runs through  all the console variables 
	// and registers them into a global list stored in vstdlib.dll
	static ConCommandBase		*s_pConCommandBases;

	// ConVars in this executable use this 'global' to access values.
	static IConCommandBaseAccessor	*s_pAccessor;
};


//-----------------------------------------------------------------------------
// Command tokenizer
//-----------------------------------------------------------------------------
class CCommand
{
public:
	CCommand();
	CCommand( int nArgC, const char **ppArgV );
	bool _NOT_IMPLEMENTED_Tokenize(void);
	void _NOT_IMPLEMENTED_Reset(void);

	int ArgC() const;
	const char **ArgV() const;
	const char *ArgS() const;					// All args that occur after the 0th arg, in string form
	const char *GetCommandString() const;		// The entire command in string form, including the 0th arg
	const char *operator[]( int nIndex ) const;	// Gets at arguments
	const char *Arg( int nIndex ) const;		// Gets at arguments
	
	// Helper functions to parse arguments to commands.
	void _NOT_IMPLEMENTED_FindArg(void) const;
	void _NOT_IMPLEMENTED_FindArgInt(void) const;

	static int MaxCommandLength();
	static void _NOT_IMPLEMENTED_DefaultBreakSet();

private:
	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

	int		m_nArgc;
	int		m_nArgv0Size;
	char	m_pArgSBuffer[ COMMAND_MAX_LENGTH ];
	char	m_pArgvBuffer[ COMMAND_MAX_LENGTH ];
	const char*	m_ppArgv[ COMMAND_MAX_ARGC ];
};

inline int CCommand::MaxCommandLength()
{
	return COMMAND_MAX_LENGTH - 1;
}

inline int CCommand::ArgC() const
{
	return m_nArgc;
}

inline const char **CCommand::ArgV() const
{
	return m_nArgc ? (const char**)m_ppArgv : 0;
}

inline const char *CCommand::ArgS() const
{
	return m_nArgv0Size ? &m_pArgSBuffer[m_nArgv0Size] : "";
}

inline const char *CCommand::GetCommandString() const
{
	return m_nArgc ? m_pArgSBuffer : "";
}

inline const char *CCommand::Arg( int nIndex ) const
{
	// FIXME: Many command handlers appear to not be particularly careful
	// about checking for valid argc range. For now, we're going to
	// do the extra check and return an empty string if it's out of range
	if ( nIndex < 0 || nIndex >= m_nArgc )
		return "";
	return m_ppArgv[nIndex];
}

inline const char *CCommand::operator[]( int nIndex ) const
{
	return Arg( nIndex );
}


//-----------------------------------------------------------------------------
// Purpose: The console invoked command
//-----------------------------------------------------------------------------
class ConCommand : public ConCommandBase
{
friend class CCvar;

public:
	typedef ConCommandBase BaseClass;

	/// <remarks>not implemented</remarks>
	ConCommand(const char *pName, FnCommandCallbackV1_t callback,
		const char *pHelpString = 0, int flags = 0, FnCommandCompletionCallback completionFunc = 0);

	/// <remarks>not implemented</remarks>
	ConCommand(const char *pName, FnCommandCallback_t callback,
		const char *pHelpString = 0, int flags = 0, FnCommandCompletionCallback completionFunc = 0);

	ConCommand(const char *pName, ICommandCallback *pCallback,
		const char *pHelpString = 0, int flags = 0, ICommandCompletionCallback *pCommandCompletionCallback = 0);

	virtual ~ConCommand( void );

	virtual	bool IsCommand( void ) const;

	/// <remarks> we don't support autocompletition, thus we always return 0 </remarks>
	virtual int	AutoCompleteSuggest(void * dummy1, void * dummy2);

	/// <remarks> we don't support autocompletition, thus we always return false </remarks>
	virtual bool CanAutoComplete( void );

	// Invoke the function
	virtual void Dispatch( const CCommand &command );

private:
	// NOTE: To maintain backward compat, we have to be very careful:
	// All public virtual methods must appear in the same order always
	// since engine code will be calling into this code, which *does not match*
	// in the mod code; it's using slightly different, but compatible versions
	// of this class. Also: Be very careful about adding new fields to this class.
	// Those fields will not exist in the version of this class that is instanced
	// in mod code.

	// Call this function when executing the command
	union
	{
		FnCommandCallbackV1_t m_fnCommandCallbackV1;
		FnCommandCallback_t m_fnCommandCallback;
		ICommandCallback *m_pCommandCallback; 
	};

	union
	{
		FnCommandCompletionCallback	m_fnCompletionCallback;
		ICommandCompletionCallback *m_pCommandCompletionCallback;
	};

	bool m_bHasCompletionCallback : 1;
	bool m_bUsingNewCommandCallback : 1;
	bool m_bUsingCommandCallbackInterface : 1;
};

//-----------------------------------------------------------------------------
// Abstract interface for ConVars
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Called when a ConVar changes value
// NOTE: For FCVAR_NEVER_AS_STRING ConVars, pOldValue == NULL
//-----------------------------------------------------------------------------
typedef void ( *FnChangeCallback_t )( IConVar *var, const char *pOldValue, float flOldValue );

//-----------------------------------------------------------------------------
// Purpose: A console variable
//-----------------------------------------------------------------------------
/// <remarks>Warning, only required elements declared and defined!</remarks>
class ConVar abstract : public ConCommandBase, public IConVar
{
friend class CCvar;
friend class ConVarRef;
friend class SplitScreenConVarRef;

friend class ::WrpConVarRef;

public:
	typedef ConCommandBase BaseClass;

	/// <remarks>not implemented</remarks>
	virtual						~ConVar( void ) = 0;

	/// <remarks>not implemented</remarks>
	virtual bool				IsFlagSet( int flag ) const = 0;

	/// <remarks>not implemented</remarks>
	virtual const char*			GetHelpText( void ) const = 0;

	/// <remarks>not implemented</remarks>
	virtual bool				IsRegistered( void ) const = 0;

	/// <remarks>not implemented</remarks>
	virtual const char			*GetName( void ) const = 0;

	// Return name of command (usually == GetName(), except in case of FCVAR_SS_ADDED vars
	/// <remarks>not implemented</remarks>
	virtual const char			*GetBaseName( void ) const = 0;

	/// <remarks>not implemented</remarks>
	//virtual int					GetSplitScreenPlayerSlot() const = 0;

	/// <remarks>not implemented</remarks>
	virtual void				AddFlags( int flags ) = 0;

	/// <remarks>not implemented</remarks>
	virtual int					GetFlags() const = 0;

	/// <remarks>not implemented</remarks>
	virtual	bool				IsCommand( void ) const = 0;

	virtual float			GetFloat(void) const = 0; // new
	virtual int			GetInt(void) const = 0; // new

	// These just call into the IConCommandBaseAccessor to check flags and set the var (which ends up calling InternalSetValue).
	virtual void				SetValue( const char *value ) = 0;
	virtual void				SetValue( float value ) = 0;
	virtual void				SetValue( int value ) = 0;
	virtual void				SetValue( Color value ) = 0;
	
	// Value
	struct CVValue_t
	{
		char						*m_pszString;
		int							m_StringLength;

		// Values
		float						m_fValue;
		int							m_nValue;
	};

	SOURCESDK_FORCEINLINE_CVAR CVValue_t &GetRawValue()
	{
		return m_Value;
	}
	SOURCESDK_FORCEINLINE_CVAR const CVValue_t &GetRawValue() const
	{
		return m_Value;
	}

private:
	// Called by CCvar when the value of a var is changing.
	//virtual void				InternalSetValue(const char *value) = 0;

	// For CVARs marked FCVAR_NEVER_AS_STRING
	//virtual void				InternalSetFloatValue( float fNewValue ) = 0;
	//virtual void				InternalSetIntValue( int nValue ) = 0;
	//virtual void				InternalSetColorValue( Color value ) = 0;

	virtual bool				ClampValue( float& value ) = 0;
	virtual void				ChangeStringValue( const char *tempVal, float flOldValue ) = 0;

	virtual void				Create( const char *pName, const char *pDefaultValue, int flags = 0,
									const char *pHelpString = 0, bool bMin = false, float fMin = 0.0,
									bool bMax = false, float fMax = false, FnChangeCallback_t callback = 0 ) = 0;

	// Used internally by OneTimeInit to initialize.
	virtual void				Init() = 0;



protected:

	// This either points to "this" or it points to the original declaration of a ConVar.
	// This allows ConVars to exist in separate modules, and they all use the first one to be declared.
	// m_pParent->m_pParent must equal m_pParent (ie: m_pParent must be the root, or original, ConVar).
	ConVar						*m_pParent;

	// Static data
	const char					*m_pszDefaultValue;
	
	CVValue_t					m_Value;

	// Min/Max values
	bool						m_bHasMin;
	float						m_fMinVal;
	bool						m_bHasMax;
	float						m_fMaxVal;
	
	// Call this function when ConVar changes
	CUtlVector<FnChangeCallback_t> m_fnChangeCallbacks;
};

//-----------------------------------------------------------------------------
// Called by the framework to register ConCommands with the ICVar
//-----------------------------------------------------------------------------
void ConVar_Register(int nCVarFlag = 0, IConCommandBaseAccessor *pAccessor = SOURCESDK_NULL);
void ConVar_Unregister();


} // namespace SOURCESDK {
} // namespace CSGO {

#endif // SOURCESDK_CSGO_CONVAR_H
