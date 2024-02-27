
//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "basetypes.h"
#include "../tier1/convar.h"
//#include "tier1/strtools.h"
//#include "tier1/characterset.h"
#include "../tier1/utlvector.h"
//#include "tier1/utlbuffer.h"
//#include "tier1/tier1.h"
//#include "tier1/convar_serverbounded.h"
#include "../icvar.h"
//#include "tier0/dbg.h"
//#include "Color.h"
//#if defined( _X360 )
//#include "xbox/xbox_console.h"
//#endif
//#include "tier0/memdbgon.h"

#define V_strlen strlen
#define V_stricmp_fast stricmp
#define V_atoi atoi

namespace SOURCESDK {
namespace CS2 {

// Comment this out when we release.
//#define ALLOW_DEVELOPMENT_CVARS

//-----------------------------------------------------------------------------
//
// Con Commands start here
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Global methods
//-----------------------------------------------------------------------------
//static characterset_t s_BreakSet;
//static bool s_bBuiltBreakSet = false;


//-----------------------------------------------------------------------------
// Tokenizer class
//-----------------------------------------------------------------------------
CCommand::CCommand()
{
    /*TODO
	if ( !s_bBuiltBreakSet )
	{
		s_bBuiltBreakSet = true;
		CharacterSetBuild( &s_BreakSet, "{}()':" );
	}
    */

	Reset();
}

CCommand::CCommand( int nArgC, const char **ppArgV )
{
	SOURCESDK_Assert( nArgC > 0 );

    /*TODO
	if ( !s_bBuiltBreakSet )
	{
		s_bBuiltBreakSet = true;
		CharacterSetBuild( &s_BreakSet, "{}()':" );
	}
    */

	Reset();

	char *pBuf = m_ArgvBuffer.Base();
	char *pSBuf = m_ArgSBuffer.Base();
	for ( int i = 0; i < nArgC; ++i )
	{
		m_Args.AddToTail( pBuf );
		int nLen = V_strlen( ppArgV[i] );
		memcpy( pBuf, ppArgV[i], nLen+1 );
		if ( i == 0 )
		{
			m_nArgv0Size = nLen;
		}
		pBuf += nLen+1;

		bool bContainsSpace = strchr( ppArgV[i], ' ' ) != NULL;
		if ( bContainsSpace )
		{
			*pSBuf++ = '\"';
		}
		memcpy( pSBuf, ppArgV[i], nLen );
		pSBuf += nLen;
		if ( bContainsSpace )
		{
			*pSBuf++ = '\"';
		}

		if ( i != nArgC - 1 )
		{
			*pSBuf++ = ' ';
		}
	}
}

void CCommand::Reset()
{
	m_nArgv0Size = 0;
	m_ArgSBuffer.RemoveAll();
	m_ArgvBuffer.RemoveAll();
	m_Args.RemoveAll();
}
/*
characterset_t* CCommand::DefaultBreakSet()
{
	return &s_BreakSet;
}*/
/*
bool CCommand::Tokenize( const char *pCommand, characterset_t *pBreakSet )
{
	Reset();
	if ( !pCommand )
		return false;

	// Use default break set
	if ( !pBreakSet )
	{
		pBreakSet = &s_BreakSet;
	}

	// Copy the current command into a temp buffer
	// NOTE: This is here to avoid the pointers returned by DequeueNextCommand
	// to become invalid by calling AddText. Is there a way we can avoid the memcpy?
	int nLen = V_strlen( pCommand );
	if ( nLen >= COMMAND_MAX_LENGTH - 1 )
	{
		Warning( "CCommand::Tokenize: Encountered command which overflows the tokenizer buffer.. Skipping!\n" );
		return false;
	}

	memcpy( m_ArgSBuffer.Base(), pCommand, nLen + 1 );

	// Parse the current command into the current command buffer
	CUtlBuffer bufParse( m_ArgSBuffer.Base(), nLen, CUtlBuffer::TEXT_BUFFER | CUtlBuffer::READ_ONLY);
	int nArgvBufferSize = 0;
	while ( bufParse.IsValid() && ( m_Args.Count() < COMMAND_MAX_ARGC ) )
	{
		char *pArgvBuf = &m_ArgvBuffer[nArgvBufferSize];
		int nMaxLen = COMMAND_MAX_LENGTH - nArgvBufferSize;
		int nStartGet = bufParse.TellGet();
		int	nSize = bufParse.ParseToken( pBreakSet, pArgvBuf, nMaxLen );
		if ( nSize < 0 )
			break;

		// Check for overflow condition
		if ( nMaxLen == nSize )
		{
			Reset();
			return false;
		}

		if ( m_Args.Count() == 1 )
		{
			// Deal with the case where the arguments were quoted
			m_nArgv0Size = bufParse.TellGet();
			bool bFoundEndQuote = m_ArgSBuffer[m_nArgv0Size-1] == '\"';
			if ( bFoundEndQuote )
			{
				--m_nArgv0Size;
			}
			m_nArgv0Size -= nSize;
			Assert( m_nArgv0Size != 0 );

			// The StartGet check is to handle this case: "foo"bar
			// which will parse into 2 different args. ArgS should point to bar.
			bool bFoundStartQuote = ( m_nArgv0Size > nStartGet ) && ( m_ArgSBuffer[m_nArgv0Size-1] == '\"' );
			Assert( bFoundEndQuote == bFoundStartQuote );
			if ( bFoundStartQuote )
			{
				--m_nArgv0Size;
			}
		}

		m_Args.AddToTail( pArgvBuf );
		if( m_Args.Count() >= COMMAND_MAX_ARGC )
		{
			Warning( "CCommand::Tokenize: Encountered command which overflows the argument buffer.. Clamped!\n" );
		}

		nArgvBufferSize += nSize + 1;
		Assert( nArgvBufferSize <= COMMAND_MAX_LENGTH );
	}

	return true;
}

*/
//-----------------------------------------------------------------------------
// Helper function to parse arguments to commands.
//-----------------------------------------------------------------------------
int CCommand::FindArg( const char *pName ) const
{
	int nArgC = ArgC();
	for ( int i = 1; i < nArgC; i++ )
	{
		if ( !V_stricmp_fast( Arg(i), pName ) )
			return (i+1) < nArgC ? i+1 : -1;
	}
	return -1;
}

int CCommand::FindArgInt( const char *pName, int nDefaultVal ) const
{
	int idx = FindArg( pName );
	if ( idx != -1 )
		return V_atoi( m_Args[idx] );
	else
		return nDefaultVal;
}

} // namespace CS2 {
} // namespace SOURCESDK {

