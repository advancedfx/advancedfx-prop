#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-01-21 dominik.matrixstorm.com
//
// First changes:
// 2015-01-21 dominik.matrixstorm.com

extern bool csgo_SndMixTimeScalePatch_enable;
extern float csgo_SndMixTimeScalePatch_value;

bool Hook_csgo_SndMixTimeScalePatch(void);
