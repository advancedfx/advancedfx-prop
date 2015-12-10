#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-12-07 dominik.matrixstorm.com
//
// First changes:
// 2015-12-06 dominik.matrixstorm.com

#include "SourceInterfaces.h"

bool csgo_Stdshader_dx9_Hooks_Init(void);

class IOnDrawElements_Hook abstract
{
public:
	virtual	void SetVertexShader(const char* pFileName, int nStaticVshIndex ) = 0;
	virtual	void SetPixelShader(const char* pFileName, int nStaticPshIndex = 0 ) = 0;
	virtual void SetVertexShaderIndex(int vshIndex = -1 ) = 0;
	virtual void SetPixelShaderIndex(int pshIndex = 0 ) = 0;

	/// <returns> The value to use actually (override). </returns>
	virtual bool EnableColorWrites(bool bEnable) = 0;

	/// <returns> The value to use actually (override). </returns>
	virtual bool EnableAlphaWrites(bool bEnable) = 0;
};

void csgo_DepthWrite_Hook(IOnDrawElements_Hook * hook);
void csgo_DepthWrite_Unhook();
