#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-10-02 by dominik.matrixstorm.com
//
// First changes:
// 2009-09-30 by dominik.matrixstorm.com

// Based on Source engine SDK:
// Copyright (c) 1996-2005, Valve Corporation, All rights reserved


#include "SourceInterfaces.h"

#include "WrpConsole.h"

#include <windows.h>

Tier0MsgFn Tier0_Msg=0;
Tier0DMsgFn Tier0_DMsg=0;
Tier0MsgFn Tier0_Warning=0;
Tier0DMsgFn Tier0_DWarning=0;
Tier0MsgFn Tier0_Log=0;
Tier0DMsgFn Tier0_DLog=0;
Tier0MsgFn Tier0_Error=0;

Tier0MsgFn Tier0_ConMsg=0;
Tier0MsgFn Tier0_ConWarning=0;
Tier0MsgFn Tier0_ConLog=0;
Tier0MsgFn Tier0_ConDMsg=0;
Tier0MsgFn Tier0_ConDWarning=0;
Tier0MsgFn Tier0_ConDLog=0;

namespace SOURCESDK {

static int s_nDLLIdentifier = -1;

CreateInterfaceFn Sys_GetFactory( CSysModule *pModule )
{
	if ( !pModule )
		return NULL;

	HMODULE	hDLL = reinterpret_cast<HMODULE>(pModule);

	return reinterpret_cast<CreateInterfaceFn>(GetProcAddress( hDLL, CREATEINTERFACE_PROCNAME ));
}

IMemAlloc_csgo * Get_g_pMemAlloc(void)
{
	static IMemAlloc_csgo * result = 0;
	static bool firstRun = true;

	if(!firstRun)
	{
		return result;
	}
	firstRun = false;

	HMODULE hModule = GetModuleHandle("tier0");
	result = *(IMemAlloc_csgo **)GetProcAddress(hModule, "g_pMemAlloc");

	return result;
}

// Vector //////////////////////////////////////////////////////////////////////

void Vector::Init(vec_t ix, vec_t iy, vec_t iz)
{
	x = ix;
	y = iy;
	z = iz;
}

//
// Helpers for wrapping command args:

// ArgsFromConCommand_003 //////////////////////////////////////////////////////

class ArgsFromConCommand_003 :
	public ::IWrpCommandArgs
{
public:
	/// <comments> implements IWrpCommandArgs </comments>
	virtual int ArgC();

	/// <comments> implements IWrpCommandArgs </comments>
	virtual char const * ArgV(int i);

	void DoNothing() {}
};

int ArgsFromConCommand_003::ArgC() {
	return WrpConCommands::GetVEngineClient_012()->Cmd_Argc();
}

char const * ArgsFromConCommand_003::ArgV(int i) {
	return WrpConCommands::GetVEngineClient_012()->Cmd_Argv(i);
}

ArgsFromConCommand_003 g_ArgsFromConCommand_003;

// ArgsFromCCommand_004 ////////////////////////////////////////////////////////

class ArgsFromCCommand_004 :
	public ::IWrpCommandArgs
{
public:
	/// <remarks> SetCommand must have been supplied with the command arg first </remarks>
	/// <comments> implements IWrpCommandArgs </comments>
	virtual int ArgC();

	/// <remarks> SetCommand must have been supplied with the command arg first </remarks>
	/// <comments> implements IWrpCommandArgs </comments>
	virtual char const * ArgV(int i);

	void SetCommand(CCommand_004 const * cmd);

private:
	CCommand_004 const * m_Cmd;

};

int ArgsFromCCommand_004::ArgC() {
	return m_Cmd->ArgC();
}

char const * ArgsFromCCommand_004::ArgV(int i) {
	return (m_Cmd->ArgV())[i];
}

void ArgsFromCCommand_004::SetCommand(CCommand_004 const * cmd) {
	m_Cmd = cmd;
}

ArgsFromCCommand_004 g_ArgsFromCCommand_004; // I dunno why () wouldn't work, crzy shit.

// ArgsFromCCommand_007 ////////////////////////////////////////////////////////

class ArgsFromCCommand_007 :
	public ::IWrpCommandArgs
{
public:
	/// <remarks> SetCommand must have been supplied with the command arg first </remarks>
	/// <comments> implements IWrpCommandArgs </comments>
	virtual int ArgC();

	/// <remarks> SetCommand must have been supplied with the command arg first </remarks>
	/// <comments> implements IWrpCommandArgs </comments>
	virtual char const * ArgV(int i);

	void SetCommand(CCommand_007 const * cmd);

private:
	CCommand_007 const * m_Cmd;

};

int ArgsFromCCommand_007::ArgC() {
	return m_Cmd->ArgC();
}

char const * ArgsFromCCommand_007::ArgV(int i) {
	return (m_Cmd->ArgV())[i];
}

void ArgsFromCCommand_007::SetCommand(CCommand_007 const * cmd) {
	m_Cmd = cmd;
}

ArgsFromCCommand_007 g_ArgsFromCCommand_007;

// ConCommand_003 //////////////////////////////////////////////////////////////

ConCommand_003::ConCommand_003( char const *pName, WrpCommandCallback callback, char const *pHelpString, int flags)
{
	Create(pName, callback, pHelpString, flags);
}

ConCommand_003::~ConCommand_003( void ) {
}

void ConCommand_003::Create( char const *pName, WrpCommandCallback callback, char const *pHelpString, int flags) {
	m_Callback = callback;

	BaseClass::Create(pName, pHelpString, flags);
}


bool ConCommand_003::IsCommand( void ) const {
	return true;
}

int ConCommand_003::AutoCompleteSuggest(void * dummy1, void * dummy2) {
	return 0;
}

bool ConCommand_003::CanAutoComplete( void ) {
	return false;
}

void ConCommand_003::Dispatch( void ) {
	if(m_Callback)
		m_Callback((::IWrpCommandArgs *)&g_ArgsFromConCommand_003);
}


// ConCommand_004 //////////////////////////////////////////////////////////////

ConCommand_004::ConCommand_004(const char *pName, FnCommandCallbackV1_t_004 callback,  const char *pHelpString, int flags)
{
	// Set the callback
	m_fnCommandCallbackV1 = callback;
	m_bUsingNewCommandCallback = false;
	m_bUsingCommandCallbackInterface = false;

	// Setup the rest
	BaseClass::Create( pName, pHelpString, flags );
}

ConCommand_004::ConCommand_004(const char *pName, WrpCommandCallback callback, const char *pHelpString, int flags)
{
	// Set the callback
	m_fnCommandCallback = callback;
	m_bUsingNewCommandCallback = true;
	m_bUsingCommandCallbackInterface = false;

	// Setup the rest
	BaseClass::Create( pName, pHelpString, flags );
}

ConCommand_004::ConCommand_004(const char *pName, ICommandCallback_004 *pCallback, const char *pHelpString, int flags)
{
	// Set the callback
	m_pCommandCallback = pCallback;
	m_bUsingNewCommandCallback = false;
	m_bUsingCommandCallbackInterface = true;

	// Setup the rest
	BaseClass::Create( pName, pHelpString, flags );
}


ConCommand_004::~ConCommand_004( void )
{
}


bool ConCommand_004::IsCommand( void ) const
{ 
	return true;
}


void ConCommand_004::Dispatch( const CCommand_004 &command )
{
	if ( m_bUsingNewCommandCallback )
	{
		if ( m_fnCommandCallback )
		{
			g_ArgsFromCCommand_004.SetCommand(&command);
			m_fnCommandCallback((::IWrpCommandArgs *)&g_ArgsFromCCommand_004);
			return;
		}
	}
	else if ( m_bUsingCommandCallbackInterface )
	{
		if ( m_pCommandCallback )
		{
			m_pCommandCallback->CommandCallback( command );
			return;
		}
	}
	else
	{
		if ( m_fnCommandCallbackV1 )
		{
			( *m_fnCommandCallbackV1 )();
			return;
		}
	}

	// throw "Command without callback!!!";
}


int	ConCommand_004::AutoCompleteSuggest(void * dummy1, void * dummy2)
{
	return 0;
}


bool ConCommand_004::CanAutoComplete( void )
{
	return false;
}


// ConCommand_007 //////////////////////////////////////////////////////////////

ConCommand_007::ConCommand_007( const char *pName, WrpCommandCallback callback, const char *pHelpString, int flags, FnCommandCompletionCallback_007 completionFunc )
{
	// Set the callback
	m_fnCommandCallback = callback;
	m_bUsingNewCommandCallback = true;
	m_bUsingCommandCallbackInterface = false;

	// Set completition callback:
	m_fnCompletionCallback = completionFunc;
	m_bHasCompletionCallback = false; // not supported

	// Setup the rest
	BaseClass::Create( pName, pHelpString, flags );
}

ConCommand_007::~ConCommand_007( void )
{
}


bool ConCommand_007::IsCommand( void ) const
{ 
	return true;
}


void ConCommand_007::Dispatch( const CCommand_007 &command )
{
	if ( m_bUsingNewCommandCallback )
	{
		if ( m_fnCommandCallback )
		{
			g_ArgsFromCCommand_007.SetCommand(&command);
			m_fnCommandCallback((::IWrpCommandArgs *)&g_ArgsFromCCommand_007);
			return;
		}
	}
	else if ( m_bUsingCommandCallbackInterface )
	{
		if ( m_pCommandCallback )
		{
			m_pCommandCallback->CommandCallback( command );
			return;
		}
	}
	else
	{
		if ( m_fnCommandCallbackV1 )
		{
			( *m_fnCommandCallbackV1 )();
			return;
		}
	}

	// throw "Command without callback!!!";
}


int	ConCommand_007::AutoCompleteSuggest(void * dummy1, void * dummy2)
{
	return 0;
}


bool ConCommand_007::CanAutoComplete( void )
{
	return false;
}


// ConCommandBase_003 //////////////////////////////////////////////////////////

ConCommandBase_003 * ConCommandBase_003::s_pConCommandBases = 0;
IConCommandBaseAccessor_003	* ConCommandBase_003::s_pAccessor = 0;

ConCommandBase_003::ConCommandBase_003(void) {
	m_Flags = 0;
	m_HelpText = 0;
	m_IsRegistered = false;
	m_Name = 0;
	m_Next = 0;
}

ConCommandBase_003::ConCommandBase_003(char const *pName, char const *pHelpString, int flags) {
	Create(pName, pHelpString, flags);
}


void ConCommandBase_003::Create(char const *pName, char const *pHelpString, int flags) {
	m_Flags = flags;

	if(!pHelpString) pHelpString = "";
	m_HelpText = (char *)malloc((1+strlen(pHelpString))*sizeof(char));
	strcpy(m_HelpText, pHelpString);

	m_IsRegistered = false;

	m_Name = (char *)malloc((1+strlen(pName))*sizeof(char));
	strcpy(m_Name, pName);

	m_Next = 0;

	// AddToList:
	// This will not work for some reason if part of the list
	// is already live, may be it's a mistake
	// in the Source SDK.
	//	m_Next = s_pConCommandBases;
	//	s_pConCommandBases = this;

	// If List is already live, register Instantly:
	if ( s_pAccessor )
	{
		Init();
	}
}

ConCommandBase_003::~ConCommandBase_003(void) {
	m_Flags = 0;

	delete m_HelpText;
	m_HelpText = 0;
	
	delete m_Name;
	m_Name = 0;

	m_Next = 0;
}

bool ConCommandBase_003::IsCommand(void) const {
	return true;
}

bool ConCommandBase_003::IsBitSet( int flag ) const {
	return 0 != (m_Flags & flag);
}

void ConCommandBase_003::AddFlags( int flags ) {
	m_Flags |= flags;
}

char const * ConCommandBase_003::GetName( void ) const {
	return m_Name;
}

char const * ConCommandBase_003::GetHelpText( void ) const {
	return m_HelpText;
}

const ConCommandBase_003 * ConCommandBase_003::GetNext( void ) const {
	return m_Next;
}

void ConCommandBase_003::SetNext( ConCommandBase_003 *next ) {
	m_Next = next;
}
	
bool ConCommandBase_003::IsRegistered( void ) const {
	return m_IsRegistered;
}

void ConCommandBase_003::_NOT_IMPLEMENTED_GetCommands( void ) {
	throw "not implemented";
}

void ConCommandBase_003::_NOT_IMPLEMENTED_AddToList( void ) {
	throw "not implemented";
}

void ConCommandBase_003::_NOT_IMPLEMENTED_RemoveFlaggedCommands( void ) {
	throw "not implemented";
}

void ConCommandBase_003::_NOT_IMPLEMENTED_RevertFlaggedCvars( void ) {
	throw "not implemented";
}

void ConCommandBase_003::_NOT_IMPLEMENTED_FindCommand( void ) {
	throw "not implemented";
}

void ConCommandBase_003::Init()
{
	if ( !s_pAccessor )
		return;
	
	if ( m_IsRegistered )
		return;

	s_pAccessor->RegisterConCommandBase( this );

	m_IsRegistered = true;
}

void ConCommandBase_003::_NOT_IMPLEMENTED_CopyString( void ) {
	throw "not implemented";
}


// ConCommandBase_004 /////////////////////////////////////////////////////////////

ConCommandBase_004 * ConCommandBase_004::s_pConCommandBases = 0;
IConCommandBaseAccessor_004	* ConCommandBase_004::s_pAccessor = 0;

ConCommandBase_004::ConCommandBase_004(void) {
	m_Flags = 0;
	m_HelpText = 0;
	m_IsRegistered = false;
	m_Name = 0;
	m_Next = 0;
}

ConCommandBase_004::ConCommandBase_004(char const *pName, char const *pHelpString, int flags) {
	Create(pName, pHelpString, flags);
}

void ConCommandBase_004::Create(const char *pName, const char *pHelpString, int flags) {

	m_Flags = flags;

	if(!pHelpString) pHelpString = "";
	m_HelpText = (char *)malloc((1+strlen(pHelpString))*sizeof(char));
	strcpy(m_HelpText, pHelpString);

	m_IsRegistered = false;

	m_Name = (char *)malloc((1+strlen(pName))*sizeof(char));
	strcpy(m_Name, pName);

	m_Next = 0;

	// AddToList:
	// This will not work for some reason if part of the list
	// is already live, may be it's a mistake
	// in the Source SDK.
	//m_Next = s_pConCommandBases;
	//s_pConCommandBases = this;

	// If List is already live, register Instantly:
	if ( s_pAccessor )
	{
		Init();
	}
}

ConCommandBase_004::~ConCommandBase_004(void) {
	m_Flags = 0;

	delete m_HelpText;
	m_HelpText = 0;
	
	delete m_Name;
	m_Name = 0;

	m_Next = 0;
}

bool ConCommandBase_004::IsCommand(void) const {
	return true;
}

bool ConCommandBase_004::IsFlagSet( int flag ) const {
	return 0 != (m_Flags & flag);
}

void ConCommandBase_004::AddFlags( int flags ) {
	m_Flags |= flags;
}

CVarDLLIdentifier_t_004 ConCommandBase_004::GetDLLIdentifier() const
{
	return s_nDLLIdentifier;
}

char const * ConCommandBase_004::GetName( void ) const {
	return m_Name;
}

char const * ConCommandBase_004::GetHelpText( void ) const {
	return m_HelpText;
}

const ConCommandBase_004 * ConCommandBase_004::GetNext( void ) const {
	return m_Next;
}


ConCommandBase_004 * ConCommandBase_004::GetNext( void ) {
	return m_Next;
}

bool ConCommandBase_004::IsRegistered( void ) const {
	return m_IsRegistered;
}

void ConCommandBase_004::Init()
{
	if ( !s_pAccessor )
		return;
	
	if ( m_IsRegistered )
		return;

	s_pAccessor->RegisterConCommandBase( this );

	m_IsRegistered = true;
}

void ConCommandBase_004::_NOT_IMPLEMENTED_Shutdown() {
	throw "not implemented";
}

void ConCommandBase_004::_NOT_IMPLEMENTED_CopyString( void ) {
	throw "not implemented";
}



// ConCommandBase_007 /////////////////////////////////////////////////////////////

ConCommandBase_007 * ConCommandBase_007::s_pConCommandBases = 0;
IConCommandBaseAccessor_007	* ConCommandBase_007::s_pAccessor = 0;

ConCommandBase_007::ConCommandBase_007(void) {
	m_nFlags = 0;
	m_pszHelpString = 0;
	m_bRegistered = false;
	m_pszName = 0;
	m_pNext = 0;
}

ConCommandBase_007::ConCommandBase_007(char const *pName, char const *pHelpString, int flags) {
	Create(pName, pHelpString, flags);
}

void ConCommandBase_007::Create(const char *pName, const char *pHelpString, int flags) {

	m_nFlags = flags;

	if(!pHelpString) pHelpString = "";
	m_pszHelpString = (char *)malloc((1+strlen(pHelpString))*sizeof(char));
	strcpy(m_pszHelpString, pHelpString);

	m_bRegistered = false;

	m_pszName = (char *)malloc((1+strlen(pName))*sizeof(char));
	strcpy(m_pszName, pName);

	m_pNext = 0;

	// AddToList:
	// This will not work for some reason if part of the list
	// is already live, may be it's a mistake
	// in the Source SDK.
	//m_Next = s_pConCommandBases;
	//s_pConCommandBases = this;

	// If List is already live, register Instantly:
	if ( s_pAccessor )
	{
		Init();
	}
}

ConCommandBase_007::~ConCommandBase_007(void) {
	m_nFlags = 0;

	delete m_pszHelpString;
	m_pszHelpString = 0;
	
	delete m_pszName;
	m_pszName = 0;

	m_pNext = 0;
}

bool ConCommandBase_007::IsCommand(void) const {
	return true;
}

bool ConCommandBase_007::IsFlagSet( int flag ) const {
	return 0 != (m_nFlags & flag);
}

void ConCommandBase_007::AddFlags( int flags ) {
	m_nFlags |= flags;
}

void ConCommandBase_007::RemoveFlags( int flags )
{
	m_nFlags &= ~flags;
}

int ConCommandBase_007::GetFlags() const
{
	return m_nFlags;
}

CVarDLLIdentifier_t_007 ConCommandBase_007::GetDLLIdentifier() const
{
	return s_nDLLIdentifier;
}

char const * ConCommandBase_007::GetName( void ) const {
	return m_pszName;
}

char const * ConCommandBase_007::GetHelpText( void ) const {
	return m_pszHelpString;
}

const ConCommandBase_007 * ConCommandBase_007::GetNext( void ) const {
	return m_pNext;
}


ConCommandBase_007 * ConCommandBase_007::GetNext( void ) {
	return m_pNext;
}

bool ConCommandBase_007::IsRegistered( void ) const {
	return m_bRegistered;
}

void ConCommandBase_007::Init()
{
	if ( !s_pAccessor )
		return;
	
	if ( m_bRegistered )
		return;

	s_pAccessor->RegisterConCommandBase( this );

	m_bRegistered = true;
}

void ConCommandBase_007::_NOT_IMPLEMENTED_Shutdown() {
	throw "not implemented";
}

void ConCommandBase_007::_NOT_IMPLEMENTED_CopyString( void ) {
	throw "not implemented";
}

// MdtMatrix ///////////////////////////////////////////////////////////////////

MdtMatrix::MdtMatrix()
{
	m[0][0] = 1; m[0][1] = 0; m[0][2] = 0; m[0][3] = 0;
	m[1][0] = 0; m[1][1] = 1; m[1][2] = 0; m[1][3] = 0;
	m[2][0] = 0; m[2][1] = 0; m[2][2] = 1; m[2][3] = 0;
	m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}

MdtMatrix::MdtMatrix(const MdtMatrix & mdtMatrix)
{
	memcpy(m[0],mdtMatrix.m[0], sizeof(m));
}


namespace CSGO {

//-----------------------------------------------------------------------------
// Purpose: Get the name of the current key section
//-----------------------------------------------------------------------------
const char *KeyValues::GetName( void ) const
{
	return KeyValuesSystem()->GetStringForSymbol(m_iKeyName);
}

//-----------------------------------------------------------------------------
// Purpose: Get the symbol name of the current key section
//-----------------------------------------------------------------------------
int KeyValues::GetNameSymbol() const
{
	return m_iKeyName;
}

//-----------------------------------------------------------------------------
// Purpose: looks up a key by symbol name
//-----------------------------------------------------------------------------
KeyValues *KeyValues::FindKey(int keySymbol) const
{
	for (KeyValues *dat = m_pSub; dat != NULL; dat = dat->m_pPeer)
	{
		if (dat->m_iKeyName == keySymbol)
			return dat;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Find a keyValue, create it if it is not found.
//			Set bCreate to true to create the key if it doesn't already exist 
//			(which ensures a valid pointer will be returned)
//-----------------------------------------------------------------------------
KeyValues *KeyValues::FindKey(const char *keyName, bool bCreate)
{
	// return the current key if a NULL subkey is asked for
	if (!keyName || !keyName[0])
		return this;

	// look for '/' characters deliminating sub fields
	char szBuf[256];
	const char *subStr = strchr(keyName, '/');
	const char *searchStr = keyName;

	// pull out the substring if it exists
	if (subStr)
	{
		int size = subStr - keyName;
		Q_memcpy( szBuf, keyName, size );
		szBuf[size] = 0;
		searchStr = szBuf;
	}

	// lookup the symbol for the search string
	HKeySymbol iSearchStr = KeyValuesSystem()->GetSymbolForString( searchStr, bCreate );

	if ( iSearchStr == INVALID_KEY_SYMBOL )
	{
		// not found, couldn't possibly be in key value list
		return NULL;
	}

	KeyValues *lastItem = NULL;
	KeyValues *dat;
	// find the searchStr in the current peer list
	for (dat = m_pSub; dat != NULL; dat = dat->m_pPeer)
	{
		lastItem = dat;	// record the last item looked at (for if we need to append to the end of the list)

		// symbol compare
		if (dat->m_iKeyName == iSearchStr)
		{
			break;
		}
	}

	if ( !dat && m_pChain )
	{
		dat = m_pChain->FindKey(keyName, false);
	}

	// make sure a key was found
	if (!dat)
	{
		if (bCreate)
		{
			/*
			// we need to create a new key
			dat = new KeyValues( searchStr );
//			Assert(dat != NULL);

			dat->UsesEscapeSequences( m_bHasEscapeSequences != 0 );	// use same format as parent
			//dat->UsesConditionals( m_bEvaluateConditionals != 0 );

			// insert new key at end of list
			if (lastItem)
			{
				lastItem->m_pPeer = dat;
			}
			else
			{
				m_pSub = dat;
			}
			dat->m_pPeer = NULL;

			// a key graduates to be a submsg as soon as it's m_pSub is set
			// this should be the only place m_pSub is set
			m_iDataType = TYPE_NONE;
			*/
			throw "KeyValues::FindKey not implemented for bCreate == true!";
		}
		else
		{
			return NULL;
		}
	}
	
	// if we've still got a subStr we need to keep looking deeper in the tree
	if ( subStr )
	{
		// recursively chain down through the paths in the string
		return dat->FindKey(subStr + 1, bCreate);
	}

	return dat;
}


//-----------------------------------------------------------------------------
// Purpose: Get the integer value of a keyName. Default value is returned
//			if the keyName can't be found.
//-----------------------------------------------------------------------------
int KeyValues::GetInt(const char *keyName, int defaultValue)
{
	KeyValues *dat = FindKey(keyName, false);
	if (dat)
	{
		switch (dat->m_iDataType)
		{
		case TYPE_STRING:
			return atoi(dat->m_sValue);
		case TYPE_WSTRING:
			return _wtoi(dat->m_wsValue);
		case TYPE_FLOAT:
			return (int)dat->m_flValue;
		case TYPE_UINT64:
			// can't convert, since it would lose data
			Assert(0);
			return 0;
		case TYPE_INT:
		case TYPE_PTR:
		default:
			return dat->m_iValue;
		};
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------
// Purpose: Get the pointer value of a keyName. Default value is returned
//			if the keyName can't be found.
//-----------------------------------------------------------------------------
void *KeyValues::GetPtr(const char *keyName, void *defaultValue)
{
	KeyValues *dat = FindKey(keyName, false);
	if (dat)
	{
		switch (dat->m_iDataType)
		{
		case TYPE_PTR:
			return dat->m_pValue;

		case TYPE_WSTRING:
		case TYPE_STRING:
		case TYPE_FLOAT:
		case TYPE_INT:
		case TYPE_UINT64:
		default:
			return NULL;
		};
	}
	return defaultValue;
}

typedef IKeyValuesSystem * (*vstdblib_KeyValuesSystem_t)(void);

IKeyValuesSystem *KeyValuesSystem()
{
	static vstdblib_KeyValuesSystem_t vstdblib_KeyValuesSystem = 0;
	static bool firstRun = true;

	if (firstRun)
	{
		firstRun = false;

		HMODULE hModule = GetModuleHandleA("vstdlib");
		if (hModule)
		{
			vstdblib_KeyValuesSystem = (vstdblib_KeyValuesSystem_t)GetProcAddress(hModule, "KeyValuesSystem");
		}
	}

	return vstdblib_KeyValuesSystem();
}

} // namespace CSGO {
} // namespace SOURCESDK {
