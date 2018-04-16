//===== Copyright © 2005-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: A higher level link library for general use in the game and tools.
//
//===========================================================================//


#ifndef SOURCESDK_CSGO_INTERFACES_H
#define SOURCESDK_CSGO_INTERFACES_H

#if defined( COMPILER_MSVC )
#pragma once
#endif

namespace SOURCESDK {
namespace CSGO {

//-----------------------------------------------------------------------------
// Interface creation function
//-----------------------------------------------------------------------------
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

// [....]

} // namespace SOURCESDK {
} // namespace CSGO {

#endif // SOURCESDK_CSGO_INTERFACES_H

