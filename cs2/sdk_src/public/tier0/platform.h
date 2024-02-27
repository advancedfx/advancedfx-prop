//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// $NoKeywords: $
//
//===========================================================================//

#ifndef SOURCESDK_CS2_PLATFORM_H
#define SOURCESDK_CS2_PLATFORM_H

#if _WIN32
#pragma once
#endif

namespace SOURCESDK {
namespace CS2 {

//TODO: ...
#define SOURCESDK_CS2_COMPILER_MSVC

//-----------------------------------------------------------------------------
// Various compiler-specific keywords
//-----------------------------------------------------------------------------
#ifdef SOURCESDK_CS2_COMPILER_MSVC
	// decls for aligning data
	#define SOURCESDK_CS2_DECL_ALIGN(x)			__declspec( align( x ) )

#elif defined ( SOURCESDK_CS2_COMPILER_GCC )
	#define SOURCESDK_CS2_DECL_ALIGN(x)			__attribute__( ( aligned( x ) ) )
#endif

#if defined( SOURCESDK_CS2_GNUC )
// gnuc has the align decoration at the end
#define SOURCESDK_CS2_ALIGN4
#define SOURCESDK_CS2_ALIGN8
#define SOURCESDK_CS2_ALIGN16
#define SOURCESDK_CS2_ALIGN32
#define SOURCESDK_CS2_ALIGN128

#define SOURCESDK_CS2_ALIGN4_POST SOURCESDK_CS2_DECL_ALIGN(4)
#define SOURCESDK_CS2_ALIGN8_POST SOURCESDK_CS2_DECL_ALIGN(8)
#define SOURCESDK_CS2_ALIGN16_POST SOURCESDK_CS2_DECL_ALIGN(16)
#define SOURCESDK_CS2_ALIGN32_POST SOURCESDK_CS2_DECL_ALIGN(32)
#define SOURCESDK_CS2_ALIGN128_POST SOURCESDK_CS2_DECL_ALIGN(128)
#else
// MSVC has the align at the start of the struct
#define SOURCESDK_CS2_ALIGN4 SOURCESDK_CS2_DECL_ALIGN(4)
#define SOURCESDK_CS2_ALIGN8 SOURCESDK_CS2_DECL_ALIGN(8)
#define SOURCESDK_CS2_ALIGN16 SOURCESDK_CS2_DECL_ALIGN(16)
#define SOURCESDK_CS2_ALIGN32 SOURCESDK_CS2_DECL_ALIGN(32)
#define SOURCESDK_CS2_ALIGN128 SOURCESDK_CS2_DECL_ALIGN(128)

#define SOURCESDK_CS2_ALIGN4_POST
#define SOURCESDK_CS2_ALIGN8_POST
#define SOURCESDK_CS2_ALIGN16_POST
#define SOURCESDK_CS2_ALIGN32_POST
#define SOURCESDK_CS2_ALIGN128_POST
#endif


template <class T>
inline T* CopyConstruct( T* pMemory, T const& src )
{
	return ::new( pMemory ) T(src);
}

template <class T>
inline void Destruct( T* pMemory )
{
	pMemory->~T();

#ifdef _DEBUG
	memset( (void*)pMemory, 0xDD, sizeof(T) );
#endif
}

} // namespace SOURCESDK {
} // namespace CS2 {

#endif

