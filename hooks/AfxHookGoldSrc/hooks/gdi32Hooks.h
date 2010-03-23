#pragma once

#include <windows.h>

extern bool g_ForceAlpha8;

int WINAPI NewChoosePixelFormat( HDC hdc, CONST PIXELFORMATDESCRIPTOR *ppfd);
