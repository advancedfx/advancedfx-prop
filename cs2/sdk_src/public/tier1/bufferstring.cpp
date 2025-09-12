#include "bufferstring.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace SOURCESDK {
namespace CS2 {

extern HMODULE GetTier0DllHandle(); // utlmemory.cpp

const char * CBufferString::Insert(int nIndex, const char *pBuf, int nCount, bool bIgnoreAlignment) {
	typedef const char * (__fastcall * Insert_t)(void* This, int nIndex, const char *pBuf, int nCount, bool bIgnoreAlignment);
    static auto pFn = (Insert_t)GetProcAddress(GetTier0DllHandle(),"?Insert@CBufferString@@QEAAPEBDHPEBDH_N@Z");
    if(pFn) return pFn(this, nIndex, pBuf, nCount, bIgnoreAlignment);
	return "";
}

void CBufferString::Purge(int nAllocatedBytesToPreserve) {
	typedef const void (__fastcall * Insert_t)(void* This, int nAllocatedBytesToPreserve);
    static auto pFn = (Insert_t)GetProcAddress(GetTier0DllHandle(),"?Purge@CBufferString@@QEAAXH@Z");
    if(pFn) return pFn(this, nAllocatedBytesToPreserve);
}

const char * CBufferString::FixupPathName(char cSeparator) {
	typedef const char* (__fastcall * FixupPathName_t)(void*, char);
    static auto pFn = (FixupPathName_t)GetProcAddress(GetTier0DllHandle(),"?FixupPathName@CBufferString@@QEAAPEBDD@Z");
    if(pFn) return pFn(this, cSeparator);
	return "";
}

void CBufferString::ToLowerFast(int nStart) {
	typedef const char* (__fastcall * ToLowerFast_t)(void*, int);
    static auto pFn = (ToLowerFast_t)GetProcAddress(GetTier0DllHandle(),"?ToLowerFast@CBufferString@@QEAAXH@Z");
    if(pFn) pFn(this, nStart);
}

const char * CBufferString::FixSlashes(char cSeparator) {
	typedef const char* (__fastcall * FixSlashes_t)(void*, char);
    static auto pFn = (FixSlashes_t)GetProcAddress(GetTier0DllHandle(),"?FixSlashes@CBufferString@@QEAAPEBDD@Z");
    if(pFn) return pFn(this, cSeparator);
	return "";
}

const char * CBufferString::ExtractFileExtension(const char *pPath) {
	typedef const char* (__fastcall * ExtractFileExtension_t)(void*, const char*);
    static auto pFn = (ExtractFileExtension_t)GetProcAddress(GetTier0DllHandle(),"?ExtractFileExtension@CBufferString@@QEAAPEBDPEBD@Z");
    if(pFn) return pFn(this, pPath);
	return "";
}

} // namespace CS2 {
} // namespace SOURCESDK {
