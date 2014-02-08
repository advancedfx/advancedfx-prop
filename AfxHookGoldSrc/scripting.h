#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2014-01-08 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include <Windows.h>

extern bool g_Script_CanConsolePrint;

bool ScriptEngine_StartUp();
void ScriptEngine_ShutDown();
bool ScriptEngine_IsRunning();

bool ScriptEngine_Execute(char const * script);

void ScriptEvent_OnGlBegin(unsigned int mode);
void ScriptEvent_OnGlEnd();
void ScriptEvent_OnHudBegin();
bool ScriptEvent_OnHudEnd();
bool ScriptEnvent_OnSwapBuffers(HDC hDC, BOOL & bSwapRes);
