#pragma once

#include "convar.h"

#include <malloc.h>
#include <string.h>

namespace SOURCESDK {
namespace CSGO {

// CCommand ///////////////////////////////////////////////////////////////////


CCommand::CCommand(int nArgC, const char **ppArgV)
	: m_nArgc(nArgC)
{
	m_nArgv0Size = 0 < nArgC ? strlen(ppArgV[0]) : 0;

	size_t argVLen = 0;
	size_t argSLen = 0;
	for (int i = 0; i < nArgC; ++i)
	{
		size_t curLen = strlen(ppArgV[i]);
		m_ppArgv[i] = ppArgV[i];
		if (argVLen + curLen + 1 <= COMMAND_MAX_LENGTH)
		{
			memcpy_s(m_pArgvBuffer + argVLen, curLen + 1, ppArgV[i], curLen + 1);
			argVLen += curLen;
		}
		if (0 < i && argVLen + curLen + 1 <= COMMAND_MAX_LENGTH)
		{
			memcpy_s(m_pArgSBuffer + argSLen, curLen + 1, ppArgV[i], curLen + 1);
			argSLen += curLen;
		}
	}

	m_pArgvBuffer[argVLen] = 0;
	m_pArgSBuffer[argSLen] = 0;
}


// ConCommandBase /////////////////////////////////////////////////////////////

ConCommandBase * ConCommandBase::s_pConCommandBases = 0;
IConCommandBaseAccessor	* ConCommandBase::s_pAccessor = 0;

ConCommandBase::ConCommandBase(void) {
	m_nFlags = 0;
	m_pszHelpString = 0;
	m_bRegistered = false;
	m_pszName = 0;
	m_pNext = 0;
}

ConCommandBase::ConCommandBase(char const *pName, char const *pHelpString, int flags) {
	Create(pName, pHelpString, flags);
}

void ConCommandBase::Create(const char *pName, const char *pHelpString, int flags) {

	m_nFlags = flags;

	if (!pHelpString) pHelpString = "";
	m_pszHelpString = (char *)malloc((1 + strlen(pHelpString)) * sizeof(char));
	strcpy(const_cast<char *>(m_pszHelpString), pHelpString);

	m_bRegistered = false;

	m_pszName = (char *)malloc((1 + strlen(pName)) * sizeof(char));
	strcpy(const_cast<char *>(m_pszName), pName);

	m_pNext = 0;

	// AddToList:
	// This will not work for some reason if part of the list
	// is already live, may be it's a mistake
	// in the Source SDK.
	//m_Next = s_pConCommandBases;
	//s_pConCommandBases = this;

	// If List is already live, register Instantly:
	if (s_pAccessor)
	{
		Init();
	}
}

ConCommandBase::~ConCommandBase(void) {
	m_nFlags = 0;

	delete m_pszHelpString;
	m_pszHelpString = 0;

	delete m_pszName;
	m_pszName = 0;

	m_pNext = 0;
}

bool ConCommandBase::IsCommand(void) const {
	return true;
}

bool ConCommandBase::IsFlagSet(int flag) const {
	return 0 != (m_nFlags & flag);
}

void ConCommandBase::AddFlags(int flags) {
	m_nFlags |= flags;
}

void ConCommandBase::RemoveFlags(int flags)
{
	m_nFlags &= ~flags;
}

int ConCommandBase::GetFlags() const
{
	return m_nFlags;
}

CVarDLLIdentifier_t ConCommandBase::GetDLLIdentifier() const
{
	return -1;
}

char const * ConCommandBase::GetName(void) const {
	return m_pszName;
}

char const * ConCommandBase::GetHelpText(void) const {
	return m_pszHelpString;
}

const ConCommandBase * ConCommandBase::GetNext(void) const {
	return m_pNext;
}


ConCommandBase * ConCommandBase::GetNext(void) {
	return m_pNext;
}

bool ConCommandBase::IsRegistered(void) const {
	return m_bRegistered;
}

void ConCommandBase::Init()
{
	if (!s_pAccessor)
		return;

	if (m_bRegistered)
		return;

	s_pAccessor->RegisterConCommandBase(this);

	m_bRegistered = true;
}

void ConCommandBase::Shutdown() {
	throw "not implemented";
}

char * ConCommandBase::CopyString(const char *from) {
	throw "not implemented";
}


// ConCommand //////////////////////////////////////////////////////////////

ConCommand::ConCommand(const char *pName, ICommandCallback *pCallback,
	const char *pHelpString, int flags, ICommandCompletionCallback *pCommandCompletionCallback)
{
	// Set the callback
	m_pCommandCallback = pCallback;
	m_bUsingNewCommandCallback = false;
	m_bUsingCommandCallbackInterface = true;

	// Set completition callback:
	m_pCommandCompletionCallback = pCommandCompletionCallback;
	m_bHasCompletionCallback = false; // not supported

									  // Setup the rest
	BaseClass::Create(pName, pHelpString, flags);
}

ConCommand::~ConCommand(void)
{
}


bool ConCommand::IsCommand(void) const
{
	return true;
}


void ConCommand::Dispatch(const CCommand &command)
{
	if (m_bUsingNewCommandCallback)
	{
		if (m_fnCommandCallback)
		{
			m_fnCommandCallback(command);
			return;
		}
	}
	else if (m_bUsingCommandCallbackInterface)
	{
		if (m_pCommandCallback)
		{
			m_pCommandCallback->CommandCallback(command);
			return;
		}
	}
	else
	{
		if (m_fnCommandCallbackV1)
		{
			(*m_fnCommandCallbackV1)();
			return;
		}
	}

	// throw "Command without callback!!!";
}


int	ConCommand::AutoCompleteSuggest(void * dummy1, void * dummy2)
{
	return 0;
}


bool ConCommand::CanAutoComplete(void)
{
	return false;
}

void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor *pAccessor)
{
	ConCommandBase::s_pAccessor = pAccessor;
}

void ConVar_Unregister()
{
	ConVar_Register();
}


} // namespace SOURCESDK {
} // namespace CSGO {
