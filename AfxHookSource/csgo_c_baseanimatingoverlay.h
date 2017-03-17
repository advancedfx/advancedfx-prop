#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2017-03-14 dominik.matrixstorm.com
//
// First changes:
// 2017-03-12 dominik.matrixstorm.com

extern float g_csgo_mystique_annimation_factor;

void Enable_csgo_PlayerAnimStateFix_set(int value);
int Enable_csgo_PlayerAnimStateFix_get(void);

bool Hook_csgo_PlayerAnimStateFix(void);
