//===== Copyright � 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef SOURCESDK_CS2_INTERFACES_H
#define SOURCESDK_CS2_INTERFACES_H

#if defined( COMPILER_MSVC )
#pragma once
#endif

namespace SOURCESDK {
namespace CS2 {

//-----------------------------------------------------------------------------
// Interface creation function
//-----------------------------------------------------------------------------
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

// [....]

} // namespace SOURCESDK {
} // namespace CS2 {

#endif // SOURCESDK_CS2_INTERFACES_H

