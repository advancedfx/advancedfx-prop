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

#include <windows.h>


static int s_nDLLIdentifier = -1;


CreateInterfaceFn Sys_GetFactory( CSysModule *pModule )
{
	if ( !pModule )
		return NULL;

	HMODULE	hDLL = reinterpret_cast<HMODULE>(pModule);

	return reinterpret_cast<CreateInterfaceFn>(GetProcAddress( hDLL, CREATEINTERFACE_PROCNAME ));
}


// ConCommand_003 //////////////////////////////////////////////////////////////

ConCommand_003::ConCommand_003( char const *pName, FnCommandCallback_003 callback, char const *pHelpString, int flags)
{
	m_Callback = callback;

	BaseClass::Create(pName, pHelpString, flags);
}

ConCommand_003::~ConCommand_003( void ) {
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
	MessageBox(0, "ConCommand_003::Dispatch", "AFX_DEBUG", MB_OK);
	if(m_Callback)
		m_Callback();
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

ConCommand_004::ConCommand_004(const char *pName, FnCommandCallback_t_004 callback, const char *pHelpString, int flags)
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
			( *m_fnCommandCallback )( command );
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
	this->SetNext(s_pConCommandBases);
	s_pConCommandBases = this;

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
	return 0 != (m_Flags | flag);
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
	m_Next = s_pConCommandBases;
	s_pConCommandBases = this;

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
	return 0 != (m_Flags | flag);
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


