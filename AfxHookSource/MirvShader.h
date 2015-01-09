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

	bool IsBlockedVertexShaderConstant(UINT StartRegister);
	bool IsBlockedPixelShaderConstant(UINT StartRegister);

	void OnBeginScene();
	void OnClear(DWORD Count);
	void OnSetRenderState(D3DRENDERSTATETYPE State,DWORD Value);
	IDirect3DVertexShader9 * OnSetVertexShader(IDirect3DVertexShader9* pShader);
	IDirect3DPixelShader9 * OnSetPixelShader(IDirect3DPixelShader9* pShader);

	void SetInRenderWorld(bool value);

private:
	unsigned int m_ClearCount;
	bool m_Enabled;
	bool m_DepthEnabled;
	IDirect3DDevice9 * m_Device;
	bool m_InRenderWorld;
	IDirect3DPixelShader9 * m_PixelShader;
	char * m_PsoFileName;
	bool m_ReloadPixelShader;
	bool m_ReloadVertexShader;
	IDirect3DVertexShader9 * m_VertexShader;
	char * m_VsoFileName;

	bool IsActive();

	void BeginDevice(IDirect3DDevice9 * device);

	void Device_set(IDirect3DDevice9 * device);

	void EndDevice();

	void LoadPso(char const * fileName);
	void LoadVso(char const * fileName);

	DWORD * LoadShaderFileInMemory(char const * fileName);

	void LoadShader();
	void LoadPixelShader();
	void LoadVertexShader();
	
	void UnloadShader();
	void UnloadPixelShader();
	void UnloadVertexShader();

};

extern MirvShader g_MirvShader;
