#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-04-30 by dominik.matrixstorm.com
//
// First changes:
// 2014-04-30 by dominik.matrixstorm.com

#include "WrpGlobals.h"

#include "addresses.h"

// WrpGlobalsCsGo //////////////////////////////////////////////////////////////

WrpGlobalsCsGo::WrpGlobalsCsGo(void * pGlobals)
{
	m_pGlobals = pGlobals;
}

float WrpGlobalsCsGo::curtime_get(void)
{
	return *(float *)((unsigned char *)m_pGlobals +AFXADDR_GET(csgo_gpGlobals_OFS_curtime));
}

// WrpGlobalsOther /////////////////////////////////////////////////////////////

WrpGlobalsOther::WrpGlobalsOther(void * pGlobals)
{
	m_pGlobals = pGlobals;
}

float WrpGlobalsOther::curtime_get(void)
{
	return *(float *)((unsigned char *)m_pGlobals +AFXADDR_GET(cstrike_gpGlobals_OFS_curtime));
}
