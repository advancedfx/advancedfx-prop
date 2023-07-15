//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $NoKeywords: $
//===========================================================================//

#ifndef SOURCESDK_CS2_CONVAR_H
#define SOURCESDK_CS2_CONVAR_H

#if _WIN32
#pragma once
#endif

//#include "tier0/dbg.h"
#include "iconvar.h"
//#include "tier1/utlvector.h"
//#include "tier1/utlstring.h"
//#include "color.h"
#include "../icvar.h"

#include "../../../../AfxHookSource/SourceSdkShared.h"
#include "../tier0/memalloc.h"

class WrpConVarRef;

namespace SOURCESDK {
namespace CS2 {

#define SOURCESDK_CS2_CVAR_INTERFACE_VERSION "VEngineCvar007"

} // namespace SOURCESDK {
} // namespace CS2 {

#endif // SOURCESDK_CS2_CONVAR_H
