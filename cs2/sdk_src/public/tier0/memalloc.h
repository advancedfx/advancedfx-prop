//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: This header should never be used directly from leaf code!!!
// Instead, just add the file memoverride.cpp into your project and all this
// will automagically be used
//
// $NoKeywords: $
//=============================================================================//

#ifndef SOURCESDK_CS2_TIER0_MEMALLOC_H
#define SOURCESDK_CS2_TIER0_MEMALLOC_H

#ifdef _WIN32
#pragma once
#endif

#include "../../../../AfxHookSource/SourceSdkShared.h"

namespace SOURCESDK {
namespace CS2 {

class CBufferString;

enum MemoryState
{
	MemoryState_UnexpectedlyAllocated = 0,
	MemoryState_UnexpectedlyFreed = 1,
	MemoryState_UnexpectedlyUnrecognized = 2,
	MemoryState_Corrupt = 3,
	MemoryState_Invalid = 4,
	MemoryState_Operational = 5,
	MemoryState_Unknown = 6
};

enum MemoryDebugType : int16
{
	MemoryDebugType_None = 0,
	MemoryDebugType_Light = 1,
	MemoryDebugType_Full = 2
};

struct MemoryInfoState
{
	MemoryInfoState( MemoryDebugType type, bool debug = false ) : m_MemType( type ), m_bDebug( debug ) { }

	MemoryDebugType m_MemType;
	bool m_bDebug;
};


//-----------------------------------------------------------------------------
// NOTE! This should never be called directly from leaf code
// Just use new,delete,malloc,free etc. They will call into this eventually
//-----------------------------------------------------------------------------
SOURCESDK_abstract_class IMemAlloc
{
	// AMNOTE: A lot of functions in here might be stubbed out and not do what their description tells
	// this heavily depends on the allocator implementation and should be taken into account when using this directly!

private:
	virtual ~IMemAlloc() = 0;

	// Release versions
	virtual void *Alloc( size_t nSize ) = 0;
public:
	virtual void *Realloc( void *pMem, size_t nSize ) = 0;
	virtual void Free( void *pMem ) = 0;

private:
	virtual void *AllocAligned( size_t nSize, size_t align ) = 0;
public:
	virtual void *ReallocAligned( void *pMem, size_t nSize, size_t align ) = 0;
	virtual void FreeAligned( void *pMem ) = 0;

	inline void *IndirectAlloc( size_t nSize ) { return Alloc( nSize ); }
	inline void *IndirectAllocAligned( size_t nSize, size_t align ) { return AllocAligned( nSize, align ); }

	// AMNOTE: It's unclear if the functions below are actually a debug variants, as in binaries they are
	// absolutely the same to the above functions, but with a different name as they aren't merged together
	// in the vtable in the win binaries. So it's mostly a guess, same goes to their params.
	// ===============================================================
private:
	virtual void *AllocDbg( size_t nSize ) = 0;
public:
	virtual void *ReallocDbg( void *pMem, size_t nSize ) = 0;
	virtual void FreeDbg( void *pMem ) = 0;

private:
	virtual void *AllocAlignedDbg( size_t nSize, size_t align ) = 0;
public:
	virtual void *ReallocAlignedDbg( void *pMem, size_t nSize, size_t align ) = 0;
	virtual void FreeAlignedDbg( void *pMem ) = 0;
	// ===============================================================

	// Region-based allocations
	// Use MEMALLOC_REGION_* defines for the region arg
	// AMNOTE: Region name is mostly a guess!
	virtual void *RegionAlloc( uint8 region, size_t nSize ) = 0;
	virtual void RegionFree( uint8 region, void *pMem ) = 0;

	virtual void *RegionAllocAligned( uint8 region, size_t nSize, size_t align ) = 0;
	virtual void RegionFreeAligned( uint8 region, void *pMem ) = 0;

	// Returns size of a particular allocation
	virtual size_t GetSize( void *pMem ) = 0;
	virtual size_t GetSizeAligned( void *pMem ) = 0;

	// If out arg is NULL or "<stdout>" it would output all the info to console,
	// otherwise a file with that name would be created at the game root folder
	virtual void DumpStats( const char *out_path ) = 0;

	// AMNOTE: Stub, returns -1
	virtual int unk001() = 0;

	// AMNOTE: Stub
	virtual void unk002() = 0;

	// AMNOTE: Stub, returns false and writes -1 to the ret_out
	virtual bool unk003( int *ret_out ) = 0;

	// AMNOTE: Stub, returns false
	virtual bool unk004() = 0;

	// AMNOTE: Stub
	virtual void unk005() = 0;

	virtual void CompactOnFail() = 0;

	// Logs the out of memory message, breaks in the debugger and exits the process
	virtual void ReportFailedAllocation( int nSize ) = 0;

	// memset's at the pMem location with nSize bytes of specified type, where:
	// MemoryDebugType_None - would do nothing;
	// MemoryDebugType_Light - would memset with 0xDD bytes;
	// MemoryDebugType_Full - would memset with 0xD8 bytes;
	// the input pMem is returned
	// 
	// NOTE: This would do nothing if the allocator is not in the debug memory mode or if
	// state has the m_bDebug set to false
	virtual void *MemSetDbg( void *pMem, size_t nSize, MemoryInfoState state ) = 0;

	// Returns true if the underlying allocator is using DebugMemoryType different to DebugMemoryType::DebugMemoryType_None
	// would also return true if the state argument has m_bDebug set as true
	virtual bool IsInDebugMode( MemoryInfoState state ) = 0;

	// If memory is not in the debug mode, returns either MemoryState_Invalid or MemoryState_Operational
	// Otherwise a more deep check would be performed
	virtual MemoryState GetMemoryState( void *pMem ) = 0;

	// Logs a warning and breaks in a debugger if the pMem state doesn't match provided state
	virtual void ReportBadMemory( void *pMem, MemoryState state = MemoryState_Operational ) = 0;

	// Returns memory debug type of this allocator
	virtual MemoryDebugType GetMemoryDebugType() = 0;

	// Returns previous total allocation size that was used
	// Directly limits how much bytes could be allocated with (Re)Alloc* functions
	virtual size_t SetTotalAllocationSize( size_t new_total_alloc_size ) = 0;

	// Returns previous allocation limit size that was used
	// Directly limits how much bytes could be allocated with (Re)Alloc* functions
	virtual size_t SetAllocationLimitSize( size_t new_alloc_limit_size ) = 0;

	// Writes detailed info about this allocator and settings used
	// Example output of a non debug allocator:
	// Heap: standard allocator pass-through to low-level
	// Low - level allocator : jemalloc
	//
	// Example output of a debug allocator with custom settings:
	// Heap: standard allocator + mem init + stackstats + light verifier + full mem debug
	// Low - level allocator : jemalloc
	virtual void GetAllocatorDescription( CBufferString &buf ) = 0;

	// Returns true if stackstats is enabled and -memstackstats_disable_pools launch option was used
	virtual bool IsStackStatsPoolsDisabled() = 0;

	// Returns true if stackstats is enabled for this allocator
	virtual bool IsStackStatsEnabled() = 0;

	// AMNOTE: Stub, returns 0
	virtual int unk101() = 0;

	// AMNOTE: Stub
	virtual void unk102() = 0;

	// AMNOTE: Copies data to an unknown struct of byte size 56
	// Returns true if data was written, false otherwise
	virtual bool unk103( void *out_val ) = 0;

	// Calls the lower-level allocator functions directly
	virtual void *AllocRaw( size_t nSize ) = 0;
	virtual void *ReallocRaw( void *pMem, size_t nSize ) = 0;
	virtual void FreeRaw( void *pMem ) = 0;
	virtual size_t GetSizeRaw( void *pMem ) = 0;
};


//-----------------------------------------------------------------------------
// Singleton interface
//-----------------------------------------------------------------------------
//MEM_INTERFACE IMemAlloc *g_pMemAlloc;
extern IMemAlloc *g_pMemAlloc;

inline void *MemAlloc_ReallocAligned( void *ptr, size_t size, size_t align )
{
	return g_pMemAlloc->ReallocAligned( ptr, size, align );
}

inline void MemAlloc_FreeAligned( void *pMemBlock )
{
	g_pMemAlloc->FreeAligned( pMemBlock );
}

inline void MemAlloc_FreeAligned( void *pMemBlock, const char *pszFile, int nLine )
{
	g_pMemAlloc->FreeAligned( pMemBlock /*, pszFile, nLine*/ );
}

inline size_t MemAlloc_GetSizeAligned( void *pMemBlock )
{
	return g_pMemAlloc->GetSizeAligned( pMemBlock );
}



} // namespace SOURCESDK {
} // namespace CS2 {

#endif /* SOURCESDK_CS2_TIER0_MEMALLOC_H */