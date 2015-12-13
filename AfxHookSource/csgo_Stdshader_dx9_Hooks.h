#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-12-13 dominik.matrixstorm.com
//
// First changes:
// 2015-12-06 dominik.matrixstorm.com

// WARNING:
//
// This code SHOULD NOT BE USED and is only left here for future redesign:
//
// This code was designed for a single threaded apartment (STA), however
// actually runs in a multi threaded apartment (MTA) and thus is
// not working properly.

#include "SourceInterfaces.h"

bool csgo_Stdshader_dx9_Hooks_Init(void);

class IOnDrawElements_Hook abstract
{
public:
	virtual	void SetVertexShader(const char* pFileName, int nStaticVshIndex, int vshIndex = -1) = 0;
	virtual	void SetPixelShader(const char* pFileName, int nStaticPshIndex = 0, int pshIndex = 0) = 0;
};

/// <summary>
/// Supported Shaders: VertexlitGeneric.
/// </summary>
/// <param name="hook">
/// Set 0 to unhook.
/// </param>
/// <remarks>
/// Currently this doesn't work properly, because it wasn't implemented to be called from concurring
/// threads, which it is though.
/// </remarks>
void csgo_StdShader_OnDrawElements_Hook(IOnDrawElements_Hook * hook);
