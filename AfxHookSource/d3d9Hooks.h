#pragma once

#include <d3d9.h>

typedef IDirect3D9 * (WINAPI * Direct3DCreate9_t)(UINT SDKVersion);
typedef HRESULT (WINAPI * Direct3DCreate9Ex_t)(UINT SDKVersion, IDirect3D9Ex**);

extern Direct3DCreate9_t old_Direct3DCreate9;
extern Direct3DCreate9Ex_t old_Direct3DCreate9Ex;

IDirect3D9 * WINAPI new_Direct3DCreate9(UINT SDKVersion);
HRESULT WINAPI new_Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3DDevice);

/// <summary>Sets the D3DRS_SRGBWRITEENABLE state on the device.</summary>
/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
/// <param name="enable">Can be TRUE or FALSE usually.</param>
/// <returns>Previous value</returns>
DWORD AfxD3D9SRGBWriteEnableFix(DWORD enable);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9SetModulationColorFix(float const color[4]);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9OverrideBegin_D3DRS_ZWRITEENABLE(DWORD value);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9OverrideEnd_D3DRS_ZWRITEENABLE(void);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_OverrideBegin_ps_c12_y(float value);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_OverrideEnd_ps_c12_y(void);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_OverrideBegin_ps_c29_w(float value);

/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_OverrideEnd_ps_c29_w(void);


class IAfxGetDirect3DVertexShader9 abstract
{
public:
	virtual IDirect3DVertexShader9 * GetDirect3DVertexShader9(void) = 0;
};

/// <param name="replacer">set 0 to end override</param>
/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_Override_SetVertexShader(IAfxGetDirect3DVertexShader9 * replacer);

class IAfxGetDirect3DPixelShader9 abstract
{
public:
	virtual IDirect3DPixelShader9 * GetDirect3DPixelShader9(void) = 0;
};

/// <param name="replacer">set 0 to end override</param>
/// <remarks>IDirect3D9Device only (i.e. CS:GO but not CSS).</remarks>
void AfxD3D9_Override_SetPixelShader(IAfxGetDirect3DPixelShader9 * replacer);
