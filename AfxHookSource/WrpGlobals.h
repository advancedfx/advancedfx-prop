#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-04-30 by dominik.matrixstorm.com
//
// First changes:
// 2014-04-30 by dominik.matrixstorm.com

class WrpGlobals abstract {
public:
	virtual float curtime_get(void) abstract = 0;
};

class WrpGlobalsCsGo : public WrpGlobals
{
public:
	WrpGlobalsCsGo(void * pGlobals);

	virtual float curtime_get(void);

private:
	void * m_pGlobals;
};

class WrpGlobalsOther : public WrpGlobals
{
public:
	WrpGlobalsOther(void * pGlobals);

	virtual float curtime_get(void);

private:
	void * m_pGlobals;
};
