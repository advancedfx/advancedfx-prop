#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2013-12-27 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include <Windows.h>

extern bool g_Script_CanConsolePrint;

bool JsStartUp();
void JsTest();
void JsShutDown();
bool JsIsRunning();

bool JsExecute(char const * script);

void ScriptEvent_OnGlBegin(unsigned int mode);
void ScriptEvent_OnGlEnd();
bool ScriptEnvent_OnSwapBuffers(HDC hDC, BOOL & bSwapRes);
