#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-10-01 dominik.matrixstorm.com
//
// First changes:
// 2015-10-01 dominik.matrixstorm.com

extern bool g_csgo_S_StartSound_Debug;

bool csgo_S_StartSound_Install(void);

void csgo_S_StartSound_Block_Add(char const * szMask);
void csgo_S_StartSound_Block_Print(void);
void csgo_S_StartSound_Block_Remove(int index);
void csgo_S_StartSound_Block_Clear(void);
