#ifndef SOURCESDK_CS2_KEYVALUES3_H
#define SOURCESDK_CS2_KEYVALUES3_H

#ifdef _WIN32
#pragma once
#endif

#include "generichash.h"

#include <string.h>


namespace SOURCESDK {
namespace CS2 {

class CKV3MemberName
{
public:
    CKV3MemberName(int iHashCode, int iUnknown, const char* pszString): m_nHashCode(iHashCode), m_iUnknown(iUnknown), m_pszString(pszString) {

    }

	inline CKV3MemberName(const char* pszString): m_nHashCode(0), m_iUnknown(-1), m_pszString("")
	{	
		if (!pszString || !pszString[0])
			return;

		m_nHashCode = MurmurHash2LowerCase(pszString, strlen(pszString), 0x31415926);
		m_pszString = pszString;

#if 0
		if (g_bUpdateStringTokenDatabase)
		{
			RegisterStringToken(m_nHashCode, pszString, 0, true);
		}
#endif
	}

	inline CKV3MemberName(): m_nHashCode(0), m_pszString("") {}
	inline CKV3MemberName(unsigned int nHashCode, const char* pszString = ""): m_nHashCode(nHashCode), m_pszString(pszString) {}

	inline unsigned int GetHashCode() const { return m_nHashCode; }
	inline const char* GetString() const { return m_pszString; }

private:
	unsigned int m_nHashCode;
    unsigned int m_iUnknown;
	const char* m_pszString;
};

} // namespace CS2 {
} // namespace SOURCESDK {

#endif