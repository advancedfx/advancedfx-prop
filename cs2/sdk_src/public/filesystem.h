//===== Copyright (c) 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#ifdef _WIN32
#pragma once
#endif

namespace SOURCESDK {
namespace CS2 {

enum SearchPathAdd_t
{
	PATH_ADD_TO_HEAD,			// First path searched
	PATH_ADD_TO_TAIL,			// Last path searched
	PATH_ADD_TO_TAIL_ATINDEX,	// First path searched
};

enum SearchPathPriority_t
{
	SEARCH_PATH_PRIORITY_DEFAULT     = 0,
	SEARCH_PATH_PRIORITY_LOOSE_FILES = 1,
	SEARCH_PATH_PRIORITY_VPK         = 2,
};

// These are actually known, but the order in SDK is wrong.
// https://github.com/alliedmodders/hl2sdk/blob/cs2/public/filesystem.h

class IFileSystem 
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
    virtual void _Unknown_025(void) = 0;
    virtual void _Unknown_026(void) = 0;
    virtual void _Unknown_027(void) = 0;
    virtual void _Unknown_028(void) = 0;
    virtual void _Unknown_029(void) = 0;

    virtual void _Unknown_030(void) = 0;
	virtual void AddSearchPath( const char *pPath, const char *pathID, SearchPathAdd_t addType = PATH_ADD_TO_TAIL, SearchPathPriority_t priority = SEARCH_PATH_PRIORITY_DEFAULT, int unknown = 0 ) = 0;
    virtual void _Unknown_032(void) = 0;
    virtual void _Unknown_033(void) = 0;
    virtual void _Unknown_034(void) = 0;
    virtual void _Unknown_035(void) = 0;
    virtual void _Unknown_036(void) = 0;
    virtual void _Unknown_037(void) = 0;
    virtual void _Unknown_038(void) = 0;
    virtual void _Unknown_039(void) = 0;

    virtual void _Unknown_040(void) = 0;
    virtual void _Unknown_041(void) = 0;
    virtual void _Unknown_042(void) = 0;
    virtual void _Unknown_043(void) = 0;
    virtual void _Unknown_044(void) = 0;
    virtual void _Unknown_045(void) = 0;
    virtual void _Unknown_046(void) = 0;
    virtual void _Unknown_047(void) = 0;
    virtual void _Unknown_048(void) = 0;
    virtual void _Unknown_049(void) = 0;

    virtual void _Unknown_050(void) = 0;
    virtual void _Unknown_051(void) = 0;
    virtual void _Unknown_052(void) = 0;
    virtual void _Unknown_053(void) = 0;
    virtual void _Unknown_054(void) = 0;
    virtual void _Unknown_055(void) = 0;
    virtual void _Unknown_056(void) = 0;
    virtual void _Unknown_057(void) = 0;
    virtual void _Unknown_058(void) = 0;
    virtual void _Unknown_059(void) = 0;

    virtual void _Unknown_060(void) = 0;
    virtual void _Unknown_061(void) = 0;
    virtual void _Unknown_062(void) = 0;
    virtual void _Unknown_063(void) = 0;
    virtual void _Unknown_064(void) = 0;
    virtual void _Unknown_065(void) = 0;
    virtual void _Unknown_066(void) = 0;
    virtual void _Unknown_067(void) = 0;
    virtual void _Unknown_068(void) = 0;
    virtual void _Unknown_069(void) = 0;

    virtual void _Unknown_070(void) = 0;
    virtual void _Unknown_071(void) = 0;
    virtual void _Unknown_072(void) = 0;
    virtual void _Unknown_073(void) = 0;
	virtual void PrintSearchPaths( void ) = 0;
};

} // namespace CS2 {
} // namespace SOURCESDK {

#endif // FILESYSTEM_H
