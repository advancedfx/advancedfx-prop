#include "utlstring.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace SOURCESDK {
namespace CS2 {

extern HMODULE GetTier0DllHandle(); // utlmemory.cpp

void CUtlString::Set(const char *pValue) {
    static void ( __fastcall * pFn)(void*,const char *) = (void (*)(void*,const char *)) GetProcAddress(GetTier0DllHandle(),"?Set@CUtlString@@QEAAXPEBD@Z");
    if(pFn) return pFn(this,pValue);
    return;    
}

void CUtlString::Purge() {
    static void ( __fastcall * pFn)(void*) = (void (*)(void*)) GetProcAddress(GetTier0DllHandle(),"?Purge@CUtlString@@QEAAXXZ");
    if(pFn) return pFn(this);
    return;
}

} // namespace CS2 {
} // namespace SOURCESDK {
