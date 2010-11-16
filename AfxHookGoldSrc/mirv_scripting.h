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

void JsSetScriptFolder(char const * scriptfolder);

void ScriptEvent_OnGlBegin(unsigned int mode);
void ScriptEvent_OnGlEnd();

void ScriptEvent_OnRecordStarting();
void ScriptEvent_OnRecordEnded();

// true - call (default), false - block
bool ScriptEvent_OnRenderViewBegin();

// true - loop, false end (default)
bool ScriptEvent_OnRenderViewEnd();