#pragma once

#include <d3d9.h>

typedef IDirect3D9 * (WINAPI * Direct3DCreate9_t)(UINT SDKVersion);

extern Direct3DCreate9_t old_Direct3DCreate9;

IDirect3D9 * WINAPI new_Direct3DCreate9(UINT SDKVersion);