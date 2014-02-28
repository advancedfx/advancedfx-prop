#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2014-02-21 dominik.matrixstorm.com
//
// First changes
// 2014-02-13 dominik.matrixstorm.com

#include <halflife/external/SDL2/SDL.h>


/// <returns>True if event has been handled, otherwise false if control should be given to the base application</returns>
bool AfxGui_HandleSdlEvent(SDL_Event * event);

void AfxGui_Init();
void AfxGui_Render();
void AfxGui_SetViewport(int width, int height);
void AfxGui_SetActive(bool value);
void AfxGui_ShutDown();
