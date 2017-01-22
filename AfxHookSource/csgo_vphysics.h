#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2017-01-22 dominik.matrixstorm.com
//
// First changes:
// 2017-01-22 dominik.matrixstorm.com

bool Hook_csgo_vphsyics_frametime_lowerlimit(void);

void csgo_vphysics_SetMaxFps(double value);
double csgo_vphysics_GetMaxFps(void);