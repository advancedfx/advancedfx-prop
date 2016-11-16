#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-11-15 dominik.matrixstorm.com
//
// First changes:
// 2015-09-18 dominik.matrixstorm.com

#include "SourceInterfaces.h"

SOURCESDK::IViewRender_csgo * GetView_csgo(void);

bool Hook_csgo_CViewRender_ShouldForceNoVis(void);