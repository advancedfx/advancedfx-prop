#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-08-14 dominik.matrixstorm.com
//
// First changes:
// 2014-04-30 dominik.matrixstorm.com

#include "WrpGlobals.h"

#include "addresses.h"

// WrpGlobalsCsGo //////////////////////////////////////////////////////////////

WrpGlobalsCsGo::WrpGlobalsCsGo(void * pGlobals)
{
	m_pGlobals = pGlobals;
}

float WrpGlobalsCsGo::absoluteframetime_get(void)
{
	return *(float *)((unsigned char *)m_pGlobals +AFXADDR_GET(cstrike_gpGlobals_OFS_absoluteframetime));
}

float WrpGlobalsCsGo::curtime_get(void)
{
	return *(float *)((unsigned char *)m_pGlobals +AFXADDR_GET(csgo_gpGlobals_OFS_curtime));
}

float WrpGlobalsCsGo::interval_per_tick_get(void)
{
	return *(float *)((unsigned char *)m_pGlobals +AFXADDR_GET(csgo_gpGlobals_OFS_interval_per_tick));
}


// WrpGlobalsOther /////////////////////////////////////////////////////////////

WrpGlobalsOther::WrpGlobalsOther(void * pGlobals)
{
	m_pGlobals = pGlobals;
}

float WrpGlobalsOther::absoluteframetime_get(void)
{
	return *(float *)((unsigned char *)m_pGlobals +AFXADDR_GET(cstrike_gpGlobals_OFS_absoluteframetime));
}

float WrpGlobalsOther::curtime_get(void)
{
	return *(float *)((unsigned char *)m_pGlobals +AFXADDR_GET(cstrike_gpGlobals_OFS_curtime));
}

float WrpGlobalsOther::interval_per_tick_get(void)
{
	return *(float *)((unsigned char *)m_pGlobals +AFXADDR_GET(cstrike_gpGlobals_OFS_interval_per_tick));
}
