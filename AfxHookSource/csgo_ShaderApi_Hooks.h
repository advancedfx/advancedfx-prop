#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-12-06 dominik.matrixstorm.com
//
// First changes:
// 2015-12-06 dominik.matrixstorm.com

#include "SourceInterfaces.h"

class IAfxShaderShadow abstract
{
};

class IAfxShaderShadowSetVertexShader abstract
{
public:
	virtual	void SetVertexShader(IAfxShaderShadow * caller, const char* pFileName, int nStaticVshIndex ) = 0;
};

class IAfxShaderShadowSetPixelShader abstract
{
public:
	virtual	void SetPixelShader(IAfxShaderShadow * caller, const char* pFileName, int nStaticPshIndex = 0 ) = 0;
};

class IAfxShaderShadowEnableColorWrites abstract
{
public:
	/// <returns> The value to use actually (override). </returns>
	virtual bool EnableColorWrites(IAfxShaderShadow * caller,  bool bEnable ) = 0;
};

class IAfxShaderShadowEnableAlphaWrites abstract
{
public:
	/// <returns> The value to use actually (override). </returns>
	virtual bool EnableAlphaWrites(IAfxShaderShadow * caller,  bool bEnable ) = 0;
};

class IAfxShaderDynamicAPI abstract
{
};

class IAfxShaderDynamicAPISetVertexShaderIndex abstract
{
public:
	virtual void SetVertexShaderIndex(IAfxShaderDynamicAPI * caller, int vshIndex = -1 ) = 0;
};

class IAfxShaderDynamicAPISetPixelShaderIndex abstract
{
public:
	virtual void SetPixelShaderIndex(IAfxShaderDynamicAPI * caller, int pshIndex = 0 ) = 0;
};

/// <remarks>
/// Can only wrap one parent at a time. Currently not implemented for multiple parents!<br />
/// Also asumes that the wrapper needs to be valid in the current scope only!
/// </remarks>
IShaderShadow_csgo * Wrap_IShaderShadow_csgo(IShaderShadow_csgo * parent,
	IAfxShaderShadowSetVertexShader * onSetVertexShader,
	IAfxShaderShadowSetPixelShader * onSetPixelShader,
	IAfxShaderShadowEnableColorWrites * onEnableColorWrites,
	IAfxShaderShadowEnableAlphaWrites * onEnableAlphaWrites
);

/// <remarks>
/// Can only wrap one parent at a time. Currently not implemented for multiple parents!<br />
/// Also asumes that the wrapper needs to be valid in the current scope only!
/// </remarks>
IShaderDynamicAPI_csgo * Wrap_IShaderDynamicAPI_csgo(IShaderDynamicAPI_csgo * parent, IAfxShaderDynamicAPISetVertexShaderIndex * onSetVertexShaderIndex, IAfxShaderDynamicAPISetPixelShaderIndex * onSetPixelShaderIndex);
