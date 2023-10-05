//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#if defined( _WIN32 ) && !defined( _X360 )
#include <windows.h>		// for WideCharToMultiByte and MultiByteToWideChar
#elif defined(POSIX)
#include <wchar.h> // wcslen()
#define _alloca alloca
#define _wtoi(arg) wcstol(arg, NULL, 10)
#define _wtoi64(arg) wcstoll(arg, NULL, 10)
#endif

#include "../../../AfxHookSource/SourceSdkShared.h"
#include "../public/tier1/KeyValues.h"
//#include "filesystem.h"
#include "../public/vstdlib/IKeyValuesSystem.h"

#define SOURCESDK_INVALID_KEY_SYMBOL (-1)

namespace SOURCESDK {
namespace CS2 {

//-----------------------------------------------------------------------------
// Purpose: Get the name of the current key section
//-----------------------------------------------------------------------------
const char *KeyValues::GetName( void ) const
{
	return KeyValuesSystem()->GetStringForSymbol(HKeySymbol(a.m_iKeyNameCaseSensitive));
}

//-----------------------------------------------------------------------------
// Purpose: Get the symbol name of the current key section
//-----------------------------------------------------------------------------
HKeySymbol KeyValues::GetNameSymbol() const
{
	return HKeySymbol(a.m_iKeyNameCaseSensitive);
}

//-----------------------------------------------------------------------------
// Purpose: looks up a key by symbol name
//-----------------------------------------------------------------------------
KeyValues *KeyValues::FindKey(HKeySymbol keySymbol) const
{
	for (KeyValues *dat = a.m_pSub; dat != NULL; dat = dat->a.m_pPeer)
	{
		if (dat->a.m_iKeyNameCaseSensitive == keySymbol.Get())
			return dat;
	}

	return NULL;
}

const KeyValues *KeyValues::FindKey( const char *keyName ) const {

    return const_cast<KeyValues*>(this)->FindKey(keyName, false);
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
	    memcpy( szBuf, keyName, size );
		szBuf[size] = 0;
		searchStr = szBuf;
	}

	// lookup the symbol for the search string
	HKeySymbol iSearchStr = KeyValuesSystem()->GetSymbolForString( searchStr, bCreate );

	if ( iSearchStr.Get() == SOURCESDK_INVALID_KEY_SYMBOL )
	{
		// not found, couldn't possibly be in key value list
		return NULL;
	}

	KeyValues *lastItem = NULL;
	KeyValues *dat = nullptr;
    if(a.m_bStoredSubKey) {
        // find the searchStr in the current peer list
        for (dat = a.m_pSub; dat != NULL; dat = dat->a.m_pPeer)
        {
            lastItem = dat;	// record the last item looked at (for if we need to append to the end of the list)

            // symbol compare
            if (dat->a.m_iKeyNameCaseSensitive == iSearchStr.Get())
            {
                break;
            }
        }
    }

	//if ( !dat && m_pChain )
	//{
	//	dat = m_pChain->FindKey(keyName, false);
	//}

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

// ---------------------------------------------------------------------------- -
// Purpose: Return the first subkey in the list
//-----------------------------------------------------------------------------
KeyValues *KeyValues::GetFirstSubKey() const
{
	return a.m_pSub;
}

//-----------------------------------------------------------------------------
// Purpose: Return the next subkey
//-----------------------------------------------------------------------------
KeyValues *KeyValues::GetNextKey() const
{
	return a.m_pPeer;
}

//-----------------------------------------------------------------------------
// Purpose: Sets this key's peer to the KeyValues passed in
//-----------------------------------------------------------------------------
void KeyValues::SetNextKey(KeyValues *pDat)
{
	a.m_pPeer = pDat;
}


KeyValues* KeyValues::GetFirstTrueSubKey() const
{
	KeyValues *pRet = a.m_pSub;
	while (pRet && pRet->a.m_iDataType != TYPE_NONE)
		pRet = pRet->a.m_pPeer;

	return pRet;
}

KeyValues* KeyValues::GetNextTrueSubKey() const
{
	KeyValues *pRet = a.m_pPeer;
	while (pRet && pRet->a.m_iDataType != TYPE_NONE)
		pRet = pRet->a.m_pPeer;

	return pRet;
}

KeyValues* KeyValues::GetFirstValue() const
{
	KeyValues *pRet =a.m_pSub;
	while (pRet && pRet->a.m_iDataType == TYPE_NONE)
		pRet = pRet->a.m_pPeer;

	return pRet;
}

KeyValues* KeyValues::GetNextValue() const
{
	KeyValues *pRet = a.m_pPeer;
	while (pRet && pRet->a.m_iDataType == TYPE_NONE)
		pRet = pRet->a.m_pPeer;

	return pRet;
}


//-----------------------------------------------------------------------------
// Purpose: Get the integer value of a keyName. Default value is returned
//			if the keyName can't be found.
//-----------------------------------------------------------------------------
int KeyValues::GetInt(const char *keyName, int defaultValue) const
{
	const KeyValues *dat = FindKey(keyName);
	if (dat)
	{
		switch (dat->a.m_iDataType)
		{
		case TYPE_STRING:
			return atoi(dat->a.m_sValue);
		case TYPE_WSTRING:
			return _wtoi(dat->a.m_wsValue);
		case TYPE_FLOAT:
			return (int)dat->a.m_flValue;
		case TYPE_UINT64:
			// can't convert, since it would lose data
			SOURCESDK_Assert(0);
			return 0;
		case TYPE_INT:
		case TYPE_PTR:
		default:
			return dat->a.m_iValue;
		};
	}
	return defaultValue;
}

//-----------------------------------------------------------------------------
// Purpose: Get the pointer value of a keyName. Default value is returned
//			if the keyName can't be found.
//-----------------------------------------------------------------------------
void *KeyValues::GetPtr(const char *keyName, void *defaultValue) const
{
	const KeyValues *dat = FindKey(keyName);
	if (dat)
	{
		switch (dat->a.m_iDataType)
		{
		case TYPE_PTR:
			return dat->a.m_pValue;

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

		HMODULE hModule = GetModuleHandleA("tier0.dll");
		if (hModule)
		{
			vstdblib_KeyValuesSystem = (vstdblib_KeyValuesSystem_t)GetProcAddress(hModule, "KeyValuesSystem");
		}
	}

	return vstdblib_KeyValuesSystem();
}

} // namespace CSS {
} // namespace SOURCESDK {

const char * GetStringForSymbol(int value) {
	return SOURCESDK::CS2::KeyValuesSystem()->GetStringForSymbol(SOURCESDK::CS2::HKeySymbol(value));
}