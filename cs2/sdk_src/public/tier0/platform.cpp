#include "platform.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace SOURCESDK {
namespace CS2 {

extern HMODULE GetTier0DllHandle(); // utlmemory.cpp

const char * Plat_GetGameDirectory( int unknown ) {
	typedef const char * (__fastcall * Plat_GetGameDirectory_t)(int unknown);
    static auto pFn = (Plat_GetGameDirectory_t)GetProcAddress(GetTier0DllHandle(),"Plat_GetGameDirectory");
    if(pFn) return pFn(unknown);
	return "";
}

}
}