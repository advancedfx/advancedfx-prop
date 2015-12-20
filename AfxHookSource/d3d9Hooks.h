#pragma once

#include <d3d9.h>

typedef IDirect3D9 * (WINAPI * Direct3DCreate9_t)(UINT SDKVersion);
typedef HRESULT (WINAPI * Direct3DCreate9Ex_t)(UINT SDKVersion, IDirect3D9Ex**);

extern Direct3DCreate9_t old_Direct3DCreate9;
extern Direct3DCreate9Ex_t old_Direct3DCreate9Ex;

IDirect3D9 * WINAPI new_Direct3DCreate9(UINT SDKVersion);
HRESULT WINAPI new_Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3DDevice);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9SetModulationColorFix(float const color[4]);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9OverrideBegin_D3DRS_SRGBWRITEENABLE(DWORD value);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9OverrideEnd_D3DRS_SRGBWRITEENABLE(void);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9OverrideBegin_D3DRS_ZWRITEENABLE(DWORD value);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9OverrideEnd_D3DRS_ZWRITEENABLE(void);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_OverrideBegin_ps_c0(float values[4]);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_OverrideEnd_ps_c0(void);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_OverrideBegin_ps_c12_y(float value);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_OverrideEnd_ps_c12_y(void);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_OverrideBegin_ps_c29_w(float value);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_OverrideEnd_ps_c29_w(void);


/// <param name="replacer">set 0 to end override</param>
/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_Override_SetVertexShader(IDirect3DVertexShader9 * replacer);

/// <param name="replacer">set 0 to end override</param>
/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_Override_SetPixelShader(IDirect3DPixelShader9 * replacer);
