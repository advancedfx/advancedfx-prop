#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-09-23 dominik.matrixstorm.com
//
// First changes:
// 2016-09-19 dominik.matrixstorm.com

#include "SourceInterfaces.h"
#include "AfxInterfaces.h"

/// <summary>Hooks the current material system render context, if it's not hooked yet. Returns hooked context.</summary>
IAfxMatRenderContext * MatRenderContextHook(SOURCESDK::IMaterialSystem_csgo * materialSystem);

void MatRenderContextHook_Shutdown(void);

