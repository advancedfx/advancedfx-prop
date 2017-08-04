//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef SOURCESDK_TF2_BONELIST_H
#define SOURCESDK_TF2_BONELIST_H
#ifdef _WIN32
#pragma once
#endif

//#include "studio.h"
#include <SourceSdkShared.h>

namespace SOURCESDK {
namespace TF2 {


#define SOURCESDK_TF2_MAXSTUDIOBONES	128	// total bones actually used
#define SOURCESDK_TF2_MAXSTUDIOFLEXCTRL	96	// maximum number of flexcontrollers (input sliders)


class CBoneList
{
public:

	CBoneList();

	void Release();

	static CBoneList *Alloc();

public:

	int			m_nBones;
	Vector		m_vecPos[SOURCESDK_TF2_MAXSTUDIOBONES];
	Quaternion	m_quatRot[SOURCESDK_TF2_MAXSTUDIOBONES];

private:
	bool		m_bShouldDelete;
};

class CFlexList
{
public:

	CFlexList();

	void Release();

	static CFlexList *Alloc();

public:

	int			m_nNumFlexes;
	float		m_flexWeights[SOURCESDK_TF2_MAXSTUDIOFLEXCTRL];

private:
	bool		m_bShouldDelete;
};

} // namespace TF2 {
} // namespace SOURCESDK {

#endif // SOURCESDK_TF2_BONELIST_H
