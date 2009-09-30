#include "stdafx.h"

#include "SourceInterfaces.h"

#include <windows.h>



IVEngineClient_012 * g_VEngineClient = 0;
ICvar_003 * g_Cvar = 0;

CreateInterfaceFn Sys_GetFactory( CSysModule *pModule )
{
	if ( !pModule )
		return NULL;

	HMODULE	hDLL = reinterpret_cast<HMODULE>(pModule);

	return reinterpret_cast<CreateInterfaceFn>(GetProcAddress( hDLL, CREATEINTERFACE_PROCNAME ));
}

// ConCommand_003 //////////////////////////////////////////////////////////////

ConCommand_003::ConCommand_003()
: ConCommandBase_003()
{
	m_Callback = 0;
	m_CompletionFunc = 0;
}

ConCommand_003::ConCommand_003( char const *pName, FnCommandCallback_003 callback, char const *pHelpString, int flags, FnCommandCompletionCallback_003 completionFunc)
: ConCommandBase_003(pName, pHelpString, flags)
{
	m_Callback = callback;
	m_CompletionFunc = completionFunc;
}

ConCommand_003::~ConCommand_003( void ) {
}

bool ConCommand_003::IsCommand( void ) const {
	return true;
}

int ConCommand_003::AutoCompleteSuggest( char const *partial, char commands[ COMMAND_COMPLETION_MAXITEMS_003 ][ COMMAND_COMPLETION_ITEM_LENGTH_003 ] ) {
	if ( !m_CompletionFunc )
		return 0;

	return ( m_CompletionFunc )( partial, commands );
}

bool ConCommand_003::CanAutoComplete( void ) {
	return 0 != m_CompletionFunc;
}

void ConCommand_003::Dispatch( void ) {
	if(m_Callback)
		m_Callback();
}

// ConCommandBase_003 /////////////////////////////////////////////////////////////

ConCommandBase_003 * ConCommandBase_003::m_CommandRoot = 0;

ConCommandBase_003::ConCommandBase_003(void) {
	m_Flags = 0;
	m_HelpText = 0;
	m_IsRegistered = false;
	m_Name = 0;
	m_Next = 0;
}

ConCommandBase_003::ConCommandBase_003(char const *pName, char const *pHelpString, int flags) {
	m_Flags = flags;

	if(!pHelpString) pHelpString = "";
	m_HelpText = (char *)malloc((1+strlen(pHelpString))*sizeof(char));
	strcpy(m_HelpText, pHelpString);

	m_IsRegistered = false;

	m_Name = (char *)malloc((1+strlen(pName))*sizeof(char));
	strcpy(m_Name, pName);

	m_Next = 0;

	//
	MdtRegisterCommand();
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

ConCommandBase_003 const *ConCommandBase_003::GetCommands( void ) {
	return m_CommandRoot;
}

void ConCommandBase_003::AddToList( ConCommandBase_003 *var ) {
	var->SetNext(m_CommandRoot);
	m_CommandRoot = var;
}

void ConCommandBase_003::RemoveFlaggedCommands( int flag ) {
	ConCommandBase_003 * last = 0;
	for(ConCommandBase_003 * cur = m_CommandRoot; cur; cur = cur->m_Next ) {
		if(cur->IsBitSet(flag)) {
			if(last)
				last->m_Next = cur->m_Next;
			cur->SetNext(0);
		}
		last = cur;
	}
}

void ConCommandBase_003::RevertFlaggedCvars( int flag ) {
	// not implemented
}

ConCommandBase_003 const *ConCommandBase_003::FindCommand( char const *name ) {
	for(ConCommandBase_003 * cur = m_CommandRoot; cur; cur = cur->m_Next )
		if ( !_stricmp( name, cur->GetName() ) )
			return cur;		
}


ConCommandBase_003 * ConCommandBase_003::GetMdtCommands( void ) {
	return m_CommandRoot;
}

ConCommandBase_003 * ConCommandBase_003::GetMdtNext() {
	return m_Next;
}

bool ConCommandBase_003::MdtRegisterCommand() {
	if(!m_IsRegistered)
		m_IsRegistered = MdtConCommands::ConCommandBase_003_RegisterCommand(this);

	return m_IsRegistered;
}



// MdtConCommands //////////////////////////////////////////////////////////////

ICvar_003 * MdtConCommands::m_CvarIface = 0;


bool MdtConCommands::ConCommandBase_003_RegisterCommand(ConCommandBase_003 * command) {
	if(!m_CvarIface)
		return false;

	m_CvarIface->RegisterConCommandBase(command);
	
	return true;
}

void MdtConCommands::RegisterCommands(ICvar_003 * cvarIface) {
	if(m_CvarIface)
		// already registered, new commands will reigster themselfs
		return;
	
	m_CvarIface = cvarIface;

	for(ConCommandBase_003 * cur = ConCommandBase_003::GetMdtCommands(); cur; cur = cur->GetMdtNext()) {
		cur->MdtRegisterCommand();
	}
}


