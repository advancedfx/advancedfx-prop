#include "utlmemory.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace SOURCESDK {
namespace CS2 {


HMODULE GetTier0DllHandle() {
    static HMODULE hModule = GetModuleHandleA("tier0.dll");
    return hModule;
}

int UtlMemory_CalcNewAllocationCount( int nAllocationCount, int nGrowSize, int nNewSize, int nBytesItem ) {
    static int (* pFn)(int,int,int,int) = (int(*)(int,int,int,int)) GetProcAddress(GetTier0DllHandle(),"UtlMemory_CalcNewAllocationCount");
    if(pFn) return pFn(nAllocationCount,nGrowSize,nNewSize,nBytesItem);
    return 0;
}
void*	UtlMemory_Alloc( void* pMem, bool bRealloc, int nNewSize, int nOldSize ) {
    static void* (* pFn)(void* ,bool,int,int) = (void*(*)(void*,bool,int,int)) GetProcAddress(GetTier0DllHandle(),"UtlMemory_Alloc");
    if(pFn) return pFn(pMem,bRealloc,nNewSize,nOldSize);
    return nullptr;

}
void UtlMemory_FailedAllocation( int nTotalElements, int nNewElements ) {
    static void (* pFn)(int,int) = (void (*)(int,int)) GetProcAddress(GetTier0DllHandle(),"UtlMemory_FailedAllocation");
    if(pFn) return pFn(nTotalElements,nNewElements);
    return;
}

}
}