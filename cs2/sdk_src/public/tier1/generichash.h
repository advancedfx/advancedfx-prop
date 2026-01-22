//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Variant Pearson Hash general purpose hashing algorithm described
//			by Cargill in C++ Report 1994. Generates a 16-bit result.
//
//=============================================================================

#ifndef SOURCESDK_CS2_GENERICHASH_H
#define SOURCESDK_CS2_GENERICHASH_H

#if defined(_WIN32)
#pragma once
#endif

namespace SOURCESDK {
namespace CS2 {

typedef unsigned __int32 uint32;

// return murmurhash2 of a downcased string
uint32 MurmurHash2LowerCase( char const *pString, uint32 nSeed );
uint32 MurmurHash2LowerCase( char const *pString, int nLength, uint32 nSeed );

} // namespace CS2 {
} // namespace SOURCESDK {


#endif