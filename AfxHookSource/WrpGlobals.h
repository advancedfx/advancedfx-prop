#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-08-15 dominik.matrixstorm.com
//
// First changes:
// 2014-04-30 dominik.matrixstorm.com

class WrpGlobals abstract
{
public:
	virtual float absoluteframetime_get(void) abstract = 0;
	virtual float curtime_get(void) abstract = 0;
	virtual float interval_per_tick_get(void) abstract = 0;
};

class WrpGlobalsCsGo : public WrpGlobals
{
public:
	WrpGlobalsCsGo(void * pGlobals);

	virtual float absoluteframetime_get(void);
	virtual float curtime_get(void);
	virtual float interval_per_tick_get(void);

private:
	void * m_pGlobals;
};

class WrpGlobalsOther : public WrpGlobals
{
public:
	WrpGlobalsOther(void * pGlobals);

	virtual float absoluteframetime_get(void);
	virtual float curtime_get(void);
	virtual float interval_per_tick_get(void);

private:
	void * m_pGlobals;
};
