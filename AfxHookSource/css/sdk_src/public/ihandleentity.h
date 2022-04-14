//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef SOURCESDK_CSS_IHANDLEENTITY_H
#define SOURCESDK_CSS_IHANDLEENTITY_H
#ifdef _WIN32
#pragma once
#endif

namespace SOURCESDK {
namespace CSS {

class CBaseHandle;


// An IHandleEntity-derived class can go into an entity list and use ehandles.
class IHandleEntity
{
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle( const CBaseHandle &handle ) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
};

} // namespace SOURCESDK {
} // namespace CSS {

#endif // SOURCESDK_CSS_IHANDLEENTITY_H
