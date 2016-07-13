#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-09-18 dominik.matrixstorm.com
//
// First changes:
// 2015-09-18 dominik.matrixstorm.com

#include "csgo_view.h"

#include "addresses.h"

SOURCESDK::IViewRender_csgo * GetView_csgo(void)
{
	SOURCESDK::IViewRender_csgo ** pView = (SOURCESDK::IViewRender_csgo **)AFXADDR_GET(csgo_view);

	if(pView)
		return *pView;

	return 0;
}

