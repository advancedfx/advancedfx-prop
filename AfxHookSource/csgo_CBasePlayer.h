#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-11-02 dominik.matrixstorm.com
//
// First changes:
// 2016-10-29 dominik.matrixstorm.com

extern bool g_csgo_Block_C_BasePlayer_RecvProxy_ObserverTarget;

bool Hook_csgo_C_BasePlayer_RecvProxy_ObserverTarget(void);

extern int g_csgo_NetOnly_CPredictionCopy_TransferData_EntIndex;

bool Hook_csgo_CPredictionCopy_TransferData(void);
