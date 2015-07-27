#pragma once

#include <d3d9.h>

typedef IDirect3D9 * (WINAPI * Direct3DCreate9_t)(UINT SDKVersion);

extern Direct3DCreate9_t old_Direct3DCreate9;

IDirect3D9 * WINAPI new_Direct3DCreate9(UINT SDKVersion);

/// <summary>Sets the D3DRS_SRGBWRITEENABLE state on the device.</summary>
/// <param name="enable">Can be TRUE or FALSE usually.</param>
/// <returns>Previous value</returns>
DWORD AfxD3D9SRGBWriteEnableFix(DWORD enable);

HRESULT AfxD3D9SetVertexShaderConstantF(UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount);