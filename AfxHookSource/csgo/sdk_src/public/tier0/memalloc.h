//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: This header should never be used directly from leaf code!!!
// Instead, just add the file memoverride.cpp into your project and all this
// will automagically be used
//
// $NoKeywords: $
//=============================================================================//

#ifndef SOURCESDK_CSGO_TIER0_MEMALLOC_H
#define SOURCESDK_CSGO_TIER0_MEMALLOC_H

#ifdef _WIN32
#pragma once
#endif

#include <SourceSdkShared.h>

namespace SOURCESDK {
namespace CSGO {

//-----------------------------------------------------------------------------
// NOTE! This should never be called directly from leaf code
// Just use new,delete,malloc,free etc. They will call into this eventually
//-----------------------------------------------------------------------------
SOURCESDK_abstract_class IMemAlloc
{
public:
	virtual void _UNUSED_000(void) = 0;
	virtual void* Alloc(size_t nSize) = 0;
	virtual void _UNUSED_002(void) = 0;
	virtual void _UNUSED_003(void) = 0;
	virtual void _UNUSED_004(void) = 0;

	virtual void Free(void *pMem) = 0; //:005

	virtual void _UNUSED_006(void) = 0;
	virtual void _UNUSED_007(void) = 0;

	virtual size_t GetSize(void *pMem) = 0; //:008

											// There is more, but we don't need it at the moment
											// [....]
};

//-----------------------------------------------------------------------------
// Singleton interface
//-----------------------------------------------------------------------------
//MEM_INTERFACE IMemAlloc *g_pMemAlloc;
extern IMemAlloc *g_pMemAlloc;


} // namespace SOURCESDK {
} // namespace CSGO {

#endif /* SOURCESDK_CSGO_TIER0_MEMALLOC_H */