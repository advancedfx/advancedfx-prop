//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: This header should never be used directly from leaf code!!!
// Instead, just add the file memoverride.cpp into your project and all this
// will automagically be used
//
// $NoKeywords: $
//=============================================================================//

#ifndef SOURCESDK_BM_TIER0_MEMALLOC_H
#define SOURCESDK_BM_TIER0_MEMALLOC_H

#ifdef _WIN32
#pragma once
#endif

#include <SourceSdkShared.h>

namespace SOURCESDK {
namespace BM {

struct _CrtMemState;


typedef size_t (*MemAllocFailHandler_t)( size_t );

//-----------------------------------------------------------------------------
// NOTE! This should never be called directly from leaf code
// Just use new,delete,malloc,free etc. They will call into this eventually
//-----------------------------------------------------------------------------
SOURCESDK_abstract_class IMemAlloc
{
private:
	// Release versions
	virtual void *Alloc( size_t nSize ) = 0;
public:
	virtual void *Realloc( void *pMem, size_t nSize ) = 0;

	virtual void Free( void *pMem ) = 0;
    virtual void *Expand_NoLongerSupported( void *pMem, size_t nSize ) = 0;

private:
	// Debug versions
    virtual void *Alloc( size_t nSize, const char *pFileName, int nLine ) = 0;
public:
    virtual void *Realloc( void *pMem, size_t nSize, const char *pFileName, int nLine ) = 0;
    virtual void  Free( void *pMem, const char *pFileName, int nLine ) = 0;
    virtual void *Expand_NoLongerSupported( void *pMem, size_t nSize, const char *pFileName, int nLine ) = 0;

	inline void *IndirectAlloc( size_t nSize )										{ return Alloc( nSize ); }
	inline void *IndirectAlloc( size_t nSize, const char *pFileName, int nLine )	{ return Alloc( nSize, pFileName, nLine ); }

	// Returns size of a particular allocation
	virtual size_t GetSize( void *pMem ) = 0;

    // Force file + line information for an allocation
    virtual void PushAllocDbgInfo( const char *pFileName, int nLine ) = 0;
    virtual void PopAllocDbgInfo() = 0;

	// FIXME: Remove when we have our own allocator
	// these methods of the Crt debug code is used in our codebase currently
	virtual int32 CrtSetBreakAlloc( int32 lNewBreakAlloc ) = 0;
	virtual	int CrtSetReportMode( int nReportType, int nReportMode ) = 0;
	virtual int CrtIsValidHeapPointer( const void *pMem ) = 0;
	virtual int CrtIsValidPointer( const void *pMem, unsigned int size, int access ) = 0;
	virtual int CrtCheckMemory( void ) = 0;
	virtual int CrtSetDbgFlag( int nNewFlag ) = 0;
	virtual void CrtMemCheckpoint( _CrtMemState *pState ) = 0;

	// FIXME: Make a better stats interface
	virtual void DumpStats() = 0;
	virtual void DumpStatsFileBase( char const *pchFileBase ) = 0;
	virtual size_t ComputeMemoryUsedBy( char const *pchSubStr ) = 0;

	// FIXME: Remove when we have our own allocator
	virtual void* CrtSetReportFile( int nRptType, void* hFile ) = 0;
	virtual void* CrtSetReportHook( void* pfnNewHook ) = 0;
	virtual int CrtDbgReport( int nRptType, const char * szFile,
			int nLine, const char * szModule, const char * pMsg ) = 0;

	virtual int heapchk() = 0;

	virtual bool IsDebugHeap() = 0;

	virtual void GetActualDbgInfo( const char *&pFileName, int &nLine ) = 0;
	virtual void RegisterAllocation( const char *pFileName, int nLine, size_t nLogicalSize, size_t nActualSize, unsigned nTime ) = 0;
	virtual void RegisterDeallocation( const char *pFileName, int nLine, size_t nLogicalSize, size_t nActualSize, unsigned nTime ) = 0;

	virtual int GetVersion() = 0;

	virtual void CompactHeap() = 0;

	// Function called when malloc fails or memory limits hit to attempt to free up memory (can come in any thread)
	virtual MemAllocFailHandler_t SetAllocFailHandler( MemAllocFailHandler_t pfnMemAllocFailHandler ) = 0;

	virtual void DumpBlockStats( void * ) = 0;

#if defined( _MEMTEST )	
	virtual void SetStatsExtraInfo( const char *pMapName, const char *pComment ) = 0;
#endif

	// Returns 0 if no failure, otherwise the size_t of the last requested chunk
	virtual size_t MemoryAllocFailed() = 0;

	virtual void CompactIncremental() = 0; 

	virtual void OutOfMemory( size_t nBytesAttempted = 0 ) = 0;

	// Region-based allocations
	virtual void *RegionAlloc( int region, size_t nSize ) = 0;
	virtual void *RegionAlloc( int region, size_t nSize, const char *pFileName, int nLine ) = 0;

	// Replacement for ::GlobalMemoryStatus which accounts for unused memory in our system
	virtual void GlobalMemoryStatus( size_t *pUsedMemory, size_t *pFreeMemory ) = 0;
};

//-----------------------------------------------------------------------------
// Singleton interface
//-----------------------------------------------------------------------------
//MEM_INTERFACE IMemAlloc *g_pMemAlloc;
extern IMemAlloc *g_pMemAlloc;

} // namespace BM {
} // namespace SOURCESDK {

#endif /* SOURCESDK_BM_TIER0_MEMALLOC_H */
