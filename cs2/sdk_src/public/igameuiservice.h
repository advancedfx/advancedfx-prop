//===== Copyright 1996-2023, Valve Corporation, All rights reserved. ======//
//
// Purpose: Interfaces between the client.dll and engine
//
//===========================================================================//

#ifndef SOURCESDK_CS2_GAMEUISERVICE_INT_H
#define SOURCESDK_CS2_GAMEUISERVICE_INT_H
#ifdef _WIN32
#pragma once
#endif

namespace SOURCESDK {
namespace CS2 {

// change this when the new version is incompatable with the old
#define SOURCESDK_CS2_GAMEUISERVICE_INTERFACE_VERSION "GameUIService_001"

SOURCESDK_abstract_class IGameUIService
{
public:
    virtual void _Unknown_000(void) = 0;
    virtual void _Unknown_001(void) = 0;
    virtual void _Unknown_002(void) = 0;
    virtual void _Unknown_003(void) = 0;
    virtual void _Unknown_004(void) = 0;
    virtual void _Unknown_005(void) = 0;
    virtual void _Unknown_006(void) = 0;
    virtual void _Unknown_007(void) = 0;
    virtual void _Unknown_008(void) = 0;
    virtual void _Unknown_009(void) = 0;
    virtual void _Unknown_010(void) = 0;
    virtual void _Unknown_011(void) = 0;
    virtual void _Unknown_012(void) = 0;
    virtual void _Unknown_013(void) = 0;
    virtual void _Unknown_014(void) = 0;
    virtual void _Unknown_015(void) = 0;
    virtual void _Unknown_016(void) = 0;
    virtual void _Unknown_017(void) = 0;
    virtual void _Unknown_018(void) = 0;
    virtual void _Unknown_019(void) = 0;
    virtual void _Unknown_020(void) = 0;
    virtual void _Unknown_021(void) = 0;
    virtual void _Unknown_022(void) = 0;
    virtual void _Unknown_023(void) = 0;
    virtual void _Unknown_024(void) = 0;
    virtual bool Con_IsVisible(void) = 0; //:025
};

} // namespace CS2 {
} // namespace SOURCESDK {

#endif // SOURCESDK_CS2_GAMEUISERVICE_INT_H
