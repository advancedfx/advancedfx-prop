//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef SOURCESDK_CSSV34_BONELIST_H
#define SOURCESDK_CSSV34_BONELIST_H
#ifdef _WIN32
#pragma once
#endif

//#include "studio.h"
#include <SourceSdkShared.h>

namespace SOURCESDK {
namespace CSSV34 {


#define SOURCESDK_CSSV34_MAXSTUDIOBONES		128		// total bones actually used
#define SOURCESDK_CSSV34_MAXSTUDIOFLEXCTRL	64		// maximum number of flexcontrollers (input sliders)



class CBoneList
{
public:

	CBoneList();

	void Release();

	static CBoneList *Alloc();

public:

	int			m_nBones;
	Vector		m_vecPos[SOURCESDK_CSSV34_MAXSTUDIOBONES ];
	Quaternion	m_quatRot[SOURCESDK_CSSV34_MAXSTUDIOBONES ];

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
	float		m_flexWeights[SOURCESDK_CSSV34_MAXSTUDIOFLEXCTRL ];

private:
	bool		m_bShouldDelete;
};

} // namespace CSSV34 {
} // namespace SOURCESDK {

#endif // SOURCESDK_CSSV34_BONELIST_H
