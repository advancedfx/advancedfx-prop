//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Header: $
// $NoKeywords: $
//=============================================================================//

#ifndef UTLMAP_H
#define UTLMAP_H

#ifdef _WIN32
#pragma once
#endif

namespace SOURCESDK {
namespace CS2 {

// https://github.com/danielkrupinski/Osiris/blob/master/Source/CS2/Classes/CUtlMap.h
template <typename Key, typename Value, typename Index = int>
struct Node {
    Index left;
    Index right;
    Index parent;
    Index type;
    Key key;
    Value value;
};

template <typename Key, typename Value, typename Index = int>
struct CUtlMap {
    Index size;
    Index allocationCount;
    Node<Key, Value, Index>* memory;
    Index root;
    Index numElements;
};

} // namespace CS2 {
} // namespace SOURCESDK {

#endif // UTLMAP_H
