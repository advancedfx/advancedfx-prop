#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-04-22 by dominik.matrixstorm.com
//
// First changes:
// 2010-04-22 by dominik.matrixstorm.com

#include <d3d9.h>

class MirvShader
{
public:
	MirvShader();
	~MirvShader();

	bool Enabled_get();
	void Enabled_set(bool value);

	void LoadPso(char const * fileName);

	IDirect3DPixelShader9 * OnSetPixelShader(IDirect3DPixelShader9* pShader);


private:
	bool m_Enabled;
	IDirect3DDevice9 * m_Device;
	char * m_PsoFileName;
	bool m_ReloadShader;
	IDirect3DPixelShader9 * m_Shader;

	void BeginDevice(IDirect3DDevice9 * device);

	void Device_set(IDirect3DDevice9 * device);

	void EndDevice();

	void LoadShader();
	void UnloadShader();
};

extern MirvShader g_MirvShader;
