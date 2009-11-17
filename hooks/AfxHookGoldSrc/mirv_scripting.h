#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-16 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

extern bool g_Script_CanConsolePrint;

bool JsStartUp();
void JsShutDown();
bool JsIsRunning();

bool JsExecute(char const * script);