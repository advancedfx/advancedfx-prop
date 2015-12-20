#include "stdafx.h"

#include "d3d9Hooks.h"

#include "SourceInterfaces.h"
#include "CampathDrawer.h"
#include "AfxShaders.h"

#include <shared/detours.h>

typedef struct __declspec(novtable) Interface_s abstract {} * Interface_t;
typedef void * (__stdcall Interface_s::*InterfaceFn_t) (void *);
#define IFACE_PASSTHROUGH(iface,method,ifacePtr) \
	__pragma(warning( suppress : 4731 )) \
	virtual void __stdcall iface ##method(void) \
	{ \
		static InterfaceFn_t fn = (InterfaceFn_t)&iface::method; \
		{ \
		__asm POP EBP \
		__asm MOV EAX, ifacePtr \
		__asm MOV [ESP+4], EAX \
		__asm MOV EAX, fn \
		__asm JMP EAX \
		} \
	}

extern bool g_bD3D9DebugPrint;

ULONG g_NewDirect3DDevice9_RefCount = 1;
IDirect3DDevice9 * g_OldDirect3DDevice9 = 0;
struct NewDirect3DDevice9
{
private:
	bool m_Override_D3DRS_SRGBWRITEENABLE;
	DWORD m_D3DRS_SRGBWRITEENABLE;

	bool m_Override_D3DRS_ZWRITEENABLE;
	DWORD m_D3DRS_ZWRITEENABLE;

	float m_OriginalValue_ps_c0[4];
	bool m_Override_ps_c0;
	float m_OverrideValue_ps_c0[4];

	float m_OriginalValue_ps_c12[4];
	bool m_Override_ps_c12_y;
	float m_OverrideValue_ps_c12_y;

	float m_OriginalValue_ps_c29[4];
	bool m_Override_ps_c29_w;
	float m_OverrideValue_ps_c29_w;

	IDirect3DVertexShader9 * m_Original_VertexShader;
	bool m_Override_VertexShader;
	IDirect3DPixelShader9 * m_Original_PixelShader;
	bool m_Override_PixelShader;

public:
	NewDirect3DDevice9()
	: m_Override_D3DRS_ZWRITEENABLE(false)
	, m_D3DRS_ZWRITEENABLE(TRUE)
	, m_Override_ps_c0(false)
	, m_Override_ps_c12_y(false)
	, m_Override_ps_c29_w(false)
	, m_Original_VertexShader(0)
	, m_Override_VertexShader(false)
	, m_Original_PixelShader(0)
	, m_Override_PixelShader(false)
	{
	}

	void OverrideBegin_D3DRS_SRGBWRITEENABLE(DWORD value)
	{
		m_Override_D3DRS_SRGBWRITEENABLE = true;
		g_OldDirect3DDevice9->SetRenderState(D3DRS_SRGBWRITEENABLE, value);
	}

	void OverrideEnd_D3DRS_SRGBWRITEENABLE(void)
	{
		g_OldDirect3DDevice9->SetRenderState(D3DRS_SRGBWRITEENABLE, m_D3DRS_SRGBWRITEENABLE);
		m_Override_D3DRS_SRGBWRITEENABLE = false;
	}

	void Override_SetVertexShader(IDirect3DVertexShader9 * override_VertexShader)
	{
		m_Override_VertexShader = 0 != override_VertexShader;
		
		if(override_VertexShader)
			g_OldDirect3DDevice9->SetVertexShader(override_VertexShader);
		else
			g_OldDirect3DDevice9->SetVertexShader(m_Original_VertexShader);
	}

	void Override_SetPixelShader(IDirect3DPixelShader9 * override_PixelShader)
	{
		m_Override_PixelShader = 0 != override_PixelShader;

		if(override_PixelShader)
			g_OldDirect3DDevice9->SetPixelShader(override_PixelShader);
		else
			g_OldDirect3DDevice9->SetPixelShader(m_Original_PixelShader);
	}

	void OverrideBegin_ps_c0(float values[4])
	{
		m_Override_ps_c0 = true;
		m_OverrideValue_ps_c0[0] = values[0];
		m_OverrideValue_ps_c0[1] = values[1];
		m_OverrideValue_ps_c0[2] = values[2];
		m_OverrideValue_ps_c0[3] = values[3];

		g_OldDirect3DDevice9->SetPixelShaderConstantF(0, m_OverrideValue_ps_c0, 1);
	}

	void OverrideEnd_ps_c0(void)
	{
		m_Override_ps_c0 = false;
		g_OldDirect3DDevice9->SetPixelShaderConstantF(0, m_OriginalValue_ps_c0, 1);
	}

	void OverrideBegin_ps_c12_y(float value)
	{
		m_Override_ps_c12_y = true;
		m_OverrideValue_ps_c12_y = value;

		float tmp[4] = { m_OriginalValue_ps_c12[0], value, m_OriginalValue_ps_c12[2], m_OriginalValue_ps_c12[3] };
		g_OldDirect3DDevice9->SetPixelShaderConstantF(12, tmp, 1);
	}

	void OverrideEnd_ps_c12_y(void)
	{
		m_Override_ps_c12_y = false;
		g_OldDirect3DDevice9->SetPixelShaderConstantF(12, m_OriginalValue_ps_c12, 1);
	}

	void OverrideBegin_ps_c29_w(float value)
	{
		m_Override_ps_c29_w = true;
		m_OverrideValue_ps_c29_w = value;

		float tmp[4] = { m_OriginalValue_ps_c29[0], m_OriginalValue_ps_c29[1], m_OriginalValue_ps_c29[2], value };
		g_OldDirect3DDevice9->SetPixelShaderConstantF(29, tmp, 1);
	}

	void OverrideEnd_ps_c29_w(void)
	{
		m_Override_ps_c29_w = false;
		g_OldDirect3DDevice9->SetPixelShaderConstantF(29, m_OriginalValue_ps_c29, 1);
	}

	void OverrideBegin_D3DRS_ZWRITEENABLE(DWORD value)
	{
		m_Override_D3DRS_ZWRITEENABLE = true;
		g_OldDirect3DDevice9->SetRenderState(D3DRS_ZWRITEENABLE, value);
	}

	void OverrideEnd_D3DRS_ZWRITEENABLE(void)
	{
		g_OldDirect3DDevice9->SetRenderState(D3DRS_ZWRITEENABLE, m_D3DRS_ZWRITEENABLE);
		m_Override_D3DRS_ZWRITEENABLE = false;
	}

    /*** IUnknown methods ***/
    IFACE_PASSTHROUGH(IDirect3DDevice9, QueryInterface, g_OldDirect3DDevice9);

	STDMETHOD_(ULONG,AddRef)(THIS)
	{
		ULONG result = g_OldDirect3DDevice9->AddRef();

		++g_NewDirect3DDevice9_RefCount;

		return result;
	}

    STDMETHOD_(ULONG,Release)(THIS)
	{
		--g_NewDirect3DDevice9_RefCount;

		if(0 == g_NewDirect3DDevice9_RefCount)
		{
			g_CampathDrawer.EndDevice();
			g_AfxShaders.EndDevice();

			if(m_Original_VertexShader)
			{
				m_Original_VertexShader->Release();
				m_Original_VertexShader = 0;
			}

			if(m_Original_PixelShader)
			{
				m_Original_PixelShader->Release();
				m_Original_PixelShader = 0;
			}
		}

		return g_OldDirect3DDevice9->Release();
	}

    /*** IDirect3DDevice9 methods ***/
    IFACE_PASSTHROUGH(IDirect3DDevice9, TestCooperativeLevel, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetAvailableTextureMem, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, EvictManagedResources, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetDirect3D, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetDeviceCaps, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetDisplayMode, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetCreationParameters, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetCursorProperties, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetCursorPosition, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, ShowCursor, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateAdditionalSwapChain, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetSwapChain, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetNumberOfSwapChains, g_OldDirect3DDevice9);
    
	STDMETHOD(Reset)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		g_CampathDrawer.Reset();

		return g_OldDirect3DDevice9->Reset(pPresentationParameters);
	}

	IFACE_PASSTHROUGH(IDirect3DDevice9, Present, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetBackBuffer, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetRasterStatus, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetDialogBoxMode, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetGammaRamp, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetGammaRamp, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateTexture, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateVolumeTexture, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateCubeTexture, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateVertexBuffer, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateIndexBuffer, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateRenderTarget, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateDepthStencilSurface, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, UpdateSurface, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, UpdateTexture, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetRenderTargetData, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetFrontBufferData, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, StretchRect, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, ColorFill, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateOffscreenPlainSurface, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetRenderTarget, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetRenderTarget, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetDepthStencilSurface, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetDepthStencilSurface, g_OldDirect3DDevice9);

	STDMETHOD(BeginScene)(THIS)
	{
		return g_OldDirect3DDevice9->BeginScene();
	}

    STDMETHOD(EndScene)(THIS)
	{
		//g_CampathDrawer.OnEndScene();

		g_bD3D9DebugPrint = false;

		return g_OldDirect3DDevice9->EndScene();
	}

    STDMETHOD(Clear)(THIS_ DWORD Count,CONST D3DRECT* pRects,DWORD Flags,D3DCOLOR Color,float Z,DWORD Stencil)
	{
		if(g_bD3D9DebugPrint)
		{
			Tier0_Msg("Clear:");
			Tier0_Msg(" Flags=");
			if(Flags & D3DCLEAR_STENCIL) Tier0_Msg("|D3DCLEAR_STENCIL");
			if(Flags & D3DCLEAR_TARGET) Tier0_Msg("|D3DCLEAR_TARGET");
			if(Flags & D3DCLEAR_ZBUFFER) Tier0_Msg("|D3DCLEAR_ZBUFFER");
			Tier0_Msg(" Color=0x%08x", Color);
			Tier0_Msg(" Z=%f",Z);
			Tier0_Msg("\n");
		}

		return g_OldDirect3DDevice9->Clear(Count, pRects, Flags, Color, Z, Stencil);
	}

    IFACE_PASSTHROUGH(IDirect3DDevice9, SetTransform, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetTransform, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, MultiplyTransform, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetViewport, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetViewport, g_OldDirect3DDevice9);
    
	STDMETHOD(SetMaterial)(THIS_ CONST D3DMATERIAL9* pMaterial)
	{
		return g_OldDirect3DDevice9->SetMaterial(pMaterial);
	}

    IFACE_PASSTHROUGH(IDirect3DDevice9, GetMaterial, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetLight, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetLight, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, LightEnable, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetLightEnable, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetClipPlane, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetClipPlane, g_OldDirect3DDevice9);
    
    STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE State,DWORD Value)
	{
		if(g_bD3D9DebugPrint)
		{
			Tier0_Msg("SetRenderState:");

			switch(State)
			{
			case D3DRS_ZWRITEENABLE:
				Tier0_Msg("D3DRS_ZWRITEENABLE: %s",
					Value & TRUE  ? "TRUE" : "false"
					);
				break;
			case D3DRS_ZFUNC:
				Tier0_Msg("D3DRS_ZFUNC: ");
				switch(Value)
				{
				case D3DCMP_NEVER:
					Tier0_Msg("D3DCMP_NEVER");
					break;
				case D3DCMP_LESS:
					Tier0_Msg("D3DCMP_LESS");
					break;
				case D3DCMP_EQUAL:
					Tier0_Msg("D3DCMP_EQUAL");
					break;
				case D3DCMP_LESSEQUAL:
					Tier0_Msg("D3DCMP_LESSEQUAL");
					break;
				case D3DCMP_GREATER:
					Tier0_Msg("D3DCMP_GREATER");
					break;
				case D3DCMP_NOTEQUAL:
					Tier0_Msg("D3DCMP_NOTEQUAL");
					break;
				case D3DCMP_GREATEREQUAL:
					Tier0_Msg("D3DCMP_GREATEREQUAL");
					break;
				case D3DCMP_ALWAYS:
					Tier0_Msg("D3DCMP_ALWAYS");
					break;
				default:
					Tier0_Msg("other");
				}
				break;
			case D3DRS_COLORWRITEENABLE:
				Tier0_Msg("D3DRS_COLORWRITEENABLE: R:%s G:%s B:%s A:%s",
					Value & D3DCOLORWRITEENABLE_RED ? "ON" : "off",
					Value & D3DCOLORWRITEENABLE_GREEN ? "ON" : "off",
					Value & D3DCOLORWRITEENABLE_BLUE ? "ON" : "off",
					Value & D3DCOLORWRITEENABLE_ALPHA ? "ON" : "off"
					);
				break;
			default:
				Tier0_Msg("other");
			}

			Tier0_Msg("\n");

		}

		switch(State)
		{
		case D3DRS_SRGBWRITEENABLE:
			m_D3DRS_SRGBWRITEENABLE = Value;
			if(m_Override_D3DRS_SRGBWRITEENABLE)
				return D3D_OK;
			break;
		case D3DRS_ZWRITEENABLE:
			m_D3DRS_ZWRITEENABLE = Value;
			if(m_Override_D3DRS_ZWRITEENABLE)
				return D3D_OK;
			break;
		}

		return g_OldDirect3DDevice9->SetRenderState(State, Value);
	}

    IFACE_PASSTHROUGH(IDirect3DDevice9, GetRenderState, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateStateBlock, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, BeginStateBlock, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, EndStateBlock, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetClipStatus, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetClipStatus, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetTexture, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetTexture, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetTextureStageState, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetTextureStageState, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetSamplerState, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetSamplerState, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, ValidateDevice, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetPaletteEntries, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetPaletteEntries, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetCurrentTexturePalette, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetCurrentTexturePalette, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetScissorRect, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetScissorRect, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetSoftwareVertexProcessing, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetSoftwareVertexProcessing, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetNPatchMode, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetNPatchMode, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DrawPrimitive, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DrawIndexedPrimitive, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DrawPrimitiveUP, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DrawIndexedPrimitiveUP, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, ProcessVertices, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateVertexDeclaration, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetVertexDeclaration, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetVertexDeclaration, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetFVF, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetFVF, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateVertexShader, g_OldDirect3DDevice9);  
    
    STDMETHOD(SetVertexShader)(THIS_ IDirect3DVertexShader9* pShader)
	{
		if(m_Original_VertexShader) m_Original_VertexShader->Release();
		m_Original_VertexShader = pShader;
		if(pShader) pShader->AddRef();

		return !m_Override_VertexShader ? g_OldDirect3DDevice9->SetVertexShader(pShader) : D3D_OK;
	}

	IFACE_PASSTHROUGH(IDirect3DDevice9, GetVertexShader, g_OldDirect3DDevice9);
	
    STDMETHOD(SetVertexShaderConstantF)(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
	{
		if(g_bD3D9DebugPrint)
		{
			int lo = StartRegister;
			int hi = StartRegister+Vector4fCount;
			bool inRange = lo <= 8 && 8 < hi || lo <= 9 && 9 < hi || lo <= 10 && 10 < hi || lo <= 11 && 11 < hi;

			if(inRange)
			{
				int min = lo;
				if(8>min) min = 8;

				int maxC = hi;
				if(12<maxC) maxC = 12;

				Tier0_Msg("SetVertexShaderConstantF:\n");
				for(int i=min; i<maxC; i++)
				{
					int idx = i - StartRegister;
					Tier0_Msg("\t%i: %f %f %f %f\n", i, pConstantData[4*idx+0], pConstantData[4*idx+1], pConstantData[4*idx+2], pConstantData[4*idx+3]);
				}
			}
		}

		return g_OldDirect3DDevice9->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount);
	}
	
	IFACE_PASSTHROUGH(IDirect3DDevice9, GetVertexShaderConstantF, g_OldDirect3DDevice9);
	
    STDMETHOD(SetVertexShaderConstantI)(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount)
	{
		return g_OldDirect3DDevice9->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount);
	}
	
	IFACE_PASSTHROUGH(IDirect3DDevice9, GetVertexShaderConstantI, g_OldDirect3DDevice9);
	
    STDMETHOD(SetVertexShaderConstantB)(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount)
	{
		return g_OldDirect3DDevice9->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
	}
	
	IFACE_PASSTHROUGH(IDirect3DDevice9, GetVertexShaderConstantB, g_OldDirect3DDevice9);
    
	IFACE_PASSTHROUGH(IDirect3DDevice9, SetStreamSource, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetStreamSource, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetStreamSourceFreq, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetStreamSourceFreq, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, SetIndices, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, GetIndices, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreatePixelShader, g_OldDirect3DDevice9);
    
    STDMETHOD(SetPixelShader)(THIS_ IDirect3DPixelShader9* pShader)
	{
		if(m_Original_PixelShader) m_Original_PixelShader->Release();
		m_Original_PixelShader = pShader;
		if(pShader) pShader->AddRef();

		return !m_Override_PixelShader ? g_OldDirect3DDevice9->SetPixelShader(pShader) : D3D_OK;
	}
    
	IFACE_PASSTHROUGH(IDirect3DDevice9, GetPixelShader, g_OldDirect3DDevice9);
	
    STDMETHOD(SetPixelShaderConstantF)(THIS_ UINT StartRegister,CONST float* pConstantData,UINT Vector4fCount)
	{
		HRESULT result = g_OldDirect3DDevice9->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount);

		if(StartRegister <= 0 && 0 < StartRegister+Vector4fCount)
		{
			m_OriginalValue_ps_c0[0] = pConstantData[4*(0 -StartRegister)+0];
			m_OriginalValue_ps_c0[1] = pConstantData[4*(0 -StartRegister)+1];
			m_OriginalValue_ps_c0[2] = pConstantData[4*(0 -StartRegister)+2];
			m_OriginalValue_ps_c0[3] = pConstantData[4*(0 -StartRegister)+3];

			g_OldDirect3DDevice9->SetPixelShaderConstantF(0, m_Override_ps_c0 ? m_OverrideValue_ps_c0 : m_OriginalValue_ps_c0, 1);
		}

		if(StartRegister <= 12 && 12 < StartRegister+Vector4fCount)
		{
			m_OriginalValue_ps_c12[0] = pConstantData[4*(12 -StartRegister)+0];
			m_OriginalValue_ps_c12[1] = pConstantData[4*(12 -StartRegister)+1];
			m_OriginalValue_ps_c12[2] = pConstantData[4*(12 -StartRegister)+2];
			m_OriginalValue_ps_c12[3] = pConstantData[4*(12 -StartRegister)+3];

			float tmp[4] = { m_OriginalValue_ps_c12[0], m_Override_ps_c12_y ? m_OverrideValue_ps_c12_y : m_OriginalValue_ps_c12[1], m_OriginalValue_ps_c12[2], m_OriginalValue_ps_c12[3] };
			g_OldDirect3DDevice9->SetPixelShaderConstantF(12, tmp, 1);
		}

		if(StartRegister <= 29 && 29 < StartRegister+Vector4fCount)
		{
			m_OriginalValue_ps_c29[0] = pConstantData[4*(29 -StartRegister)+0];
			m_OriginalValue_ps_c29[1] = pConstantData[4*(29 -StartRegister)+1];
			m_OriginalValue_ps_c29[2] = pConstantData[4*(29 -StartRegister)+2];
			m_OriginalValue_ps_c29[3] = pConstantData[4*(29 -StartRegister)+3];

			float tmp[4] = { m_OriginalValue_ps_c29[0], m_OriginalValue_ps_c29[1], m_OriginalValue_ps_c29[2], m_Override_ps_c29_w ? m_OverrideValue_ps_c29_w :  m_OriginalValue_ps_c29[3] };
			g_OldDirect3DDevice9->SetPixelShaderConstantF(29, tmp, 1);
		}

		return result;
	}

	IFACE_PASSTHROUGH(IDirect3DDevice9, GetPixelShaderConstantF, g_OldDirect3DDevice9);

    STDMETHOD(SetPixelShaderConstantI)(THIS_ UINT StartRegister,CONST int* pConstantData,UINT Vector4iCount)
	{
		return g_OldDirect3DDevice9->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
	}
	
	IFACE_PASSTHROUGH(IDirect3DDevice9, GetPixelShaderConstantI, g_OldDirect3DDevice9);
	
    STDMETHOD(SetPixelShaderConstantB)(THIS_ UINT StartRegister,CONST BOOL* pConstantData,UINT  BoolCount)
	{
		return g_OldDirect3DDevice9->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
	}
	
	IFACE_PASSTHROUGH(IDirect3DDevice9, GetPixelShaderConstantB, g_OldDirect3DDevice9);
    
	IFACE_PASSTHROUGH(IDirect3DDevice9, DrawRectPatch, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DrawTriPatch, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, DeletePatch, g_OldDirect3DDevice9);
    IFACE_PASSTHROUGH(IDirect3DDevice9, CreateQuery, g_OldDirect3DDevice9);
} g_NewDirect3DDevice9;

ULONG g_NewDirect3DDevice9Ex_RefCount = 1;
IDirect3DDevice9Ex * g_OldDirect3DDevice9Ex = 0;
struct NewDirect3DDevice9Ex
{
public:
    /*** IUnknown methods ***/
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, QueryInterface, g_OldDirect3DDevice9Ex);

	STDMETHOD_(ULONG,AddRef)(THIS)
	{
		ULONG result = g_OldDirect3DDevice9Ex->AddRef();

		++g_NewDirect3DDevice9Ex_RefCount;

		return result;
	}

    STDMETHOD_(ULONG,Release)(THIS)
	{
		--g_NewDirect3DDevice9Ex_RefCount;

		if(0 == g_NewDirect3DDevice9Ex_RefCount)
		{
			g_CampathDrawer.EndDevice();
			g_AfxShaders.EndDevice();
		}

		return g_OldDirect3DDevice9Ex->Release();
	}

    /*** IDirect3DDevice9Ex methods ***/
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, TestCooperativeLevel, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetAvailableTextureMem, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, EvictManagedResources, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetDirect3D, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetDeviceCaps, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetDisplayMode, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetCreationParameters, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetCursorProperties, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetCursorPosition, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, ShowCursor, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateAdditionalSwapChain, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetSwapChain, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetNumberOfSwapChains, g_OldDirect3DDevice9Ex);
    
	STDMETHOD(Reset)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		g_CampathDrawer.Reset();

		return g_OldDirect3DDevice9Ex->Reset(pPresentationParameters);
	}

	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, Present, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetBackBuffer, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetRasterStatus, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetDialogBoxMode, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetGammaRamp, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetGammaRamp, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateTexture, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateVolumeTexture, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateCubeTexture, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateVertexBuffer, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateIndexBuffer, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateRenderTarget, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateDepthStencilSurface, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, UpdateSurface, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, UpdateTexture, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetRenderTargetData, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetFrontBufferData, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, StretchRect, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, ColorFill, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateOffscreenPlainSurface, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetRenderTarget, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetRenderTarget, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetDepthStencilSurface, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetDepthStencilSurface, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, BeginScene, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, EndScene, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, Clear, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetTransform, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetTransform, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, MultiplyTransform, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetViewport, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetViewport, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetMaterial, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetMaterial, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetLight, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetLight, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, LightEnable, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetLightEnable, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetClipPlane, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetClipPlane, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetRenderState, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetRenderState, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateStateBlock, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, BeginStateBlock, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, EndStateBlock, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetClipStatus, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetClipStatus, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetTexture, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetTexture, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetTextureStageState, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetTextureStageState, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetSamplerState, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetSamplerState, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, ValidateDevice, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetPaletteEntries, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetPaletteEntries, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetCurrentTexturePalette, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetCurrentTexturePalette, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetScissorRect, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetScissorRect, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetSoftwareVertexProcessing, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetSoftwareVertexProcessing, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetNPatchMode, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetNPatchMode, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, DrawPrimitive, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, DrawIndexedPrimitive, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, DrawPrimitiveUP, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, DrawIndexedPrimitiveUP, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, ProcessVertices, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateVertexDeclaration, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetVertexDeclaration, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetVertexDeclaration, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetFVF, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetFVF, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateVertexShader, g_OldDirect3DDevice9Ex);  
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetVertexShader, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetVertexShader, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetVertexShaderConstantF, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetVertexShaderConstantF, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetVertexShaderConstantI, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetVertexShaderConstantI, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetVertexShaderConstantB, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetVertexShaderConstantB, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetStreamSource, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetStreamSource, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetStreamSourceFreq, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetStreamSourceFreq, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetIndices, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetIndices, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreatePixelShader, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetPixelShader, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetPixelShader, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetPixelShaderConstantF, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetPixelShaderConstantF, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetPixelShaderConstantI, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetPixelShaderConstantI, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetPixelShaderConstantB, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetPixelShaderConstantB, g_OldDirect3DDevice9Ex);
	IFACE_PASSTHROUGH(IDirect3DDevice9Ex, DrawRectPatch, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, DrawTriPatch, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, DeletePatch, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateQuery, g_OldDirect3DDevice9Ex);

    /*** IDirect3DDevice9Ex methods ***/
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetConvolutionMonoKernel, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, ComposeRects, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, PresentEx, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetGPUThreadPriority, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetGPUThreadPriority, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, WaitForVBlank, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CheckResourceResidency, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, SetMaximumFrameLatency, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetMaximumFrameLatency, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CheckDeviceState, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateRenderTargetEx, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateOffscreenPlainSurfaceEx, g_OldDirect3DDevice9Ex);
    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, CreateDepthStencilSurfaceEx, g_OldDirect3DDevice9Ex);
    
	STDMETHOD(ResetEx)(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX *pFullscreenDisplayMode)
	{
		g_CampathDrawer.Reset();

		return g_OldDirect3DDevice9Ex->ResetEx(pPresentationParameters, pFullscreenDisplayMode);
	}

    IFACE_PASSTHROUGH(IDirect3DDevice9Ex, GetDisplayModeEx, g_OldDirect3DDevice9Ex);
} g_NewDirect3DDevice9Ex;

IDirect3D9 * g_OldDirect3D9;
struct NewDirect3D9
{
    /*** IUnknown methods ***/
	IFACE_PASSTHROUGH(IDirect3D9, QueryInterface, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, AddRef, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, Release, g_OldDirect3D9);

    /*** IDirect3D9 methods ***/
	IFACE_PASSTHROUGH(IDirect3D9, RegisterSoftwareDevice, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, GetAdapterCount, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, GetAdapterIdentifier, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, GetAdapterModeCount, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, EnumAdapterModes, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, GetAdapterDisplayMode, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, CheckDeviceType, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, CheckDeviceFormat, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, CheckDeviceMultiSampleType, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, CheckDepthStencilMatch, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, CheckDeviceFormatConversion, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, GetDeviceCaps, g_OldDirect3D9);
	IFACE_PASSTHROUGH(IDirect3D9, GetAdapterMonitor, g_OldDirect3D9);

    STDMETHOD(CreateDevice)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface)
	{
		HRESULT hRet = g_OldDirect3D9->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
		if(ppReturnedDeviceInterface)
		{
			g_OldDirect3DDevice9 = *ppReturnedDeviceInterface;
			
			g_AfxShaders.BeginDevice(g_OldDirect3DDevice9);
			g_CampathDrawer.BeginDevice(g_OldDirect3DDevice9);

			*ppReturnedDeviceInterface = reinterpret_cast<IDirect3DDevice9 *>(&g_NewDirect3DDevice9);
		}
		return hRet;
	}

} g_NewDirect3D9;

IDirect3D9Ex * g_OldDirect3D9Ex;
struct NewDirect3D9Ex
{
    /*** IUnknown methods ***/
	IFACE_PASSTHROUGH(IDirect3D9Ex, QueryInterface, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, AddRef, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, Release, g_OldDirect3D9Ex);

    /*** IDirect3D9 methods ***/
	IFACE_PASSTHROUGH(IDirect3D9Ex, RegisterSoftwareDevice, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, GetAdapterCount, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, GetAdapterIdentifier, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, GetAdapterModeCount, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, EnumAdapterModes, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, GetAdapterDisplayMode, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, CheckDeviceType, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, CheckDeviceFormat, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, CheckDeviceMultiSampleType, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, CheckDepthStencilMatch, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, CheckDeviceFormatConversion, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, GetDeviceCaps, g_OldDirect3D9Ex);
	IFACE_PASSTHROUGH(IDirect3D9Ex, GetAdapterMonitor, g_OldDirect3D9Ex);

    STDMETHOD(CreateDevice)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface)
	{
		HRESULT hRet = g_OldDirect3D9Ex->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
		if(ppReturnedDeviceInterface)
		{
			g_OldDirect3DDevice9 = *ppReturnedDeviceInterface;

			g_AfxShaders.BeginDevice(g_OldDirect3DDevice9);
			g_CampathDrawer.BeginDevice(g_OldDirect3DDevice9);

			*ppReturnedDeviceInterface = reinterpret_cast<IDirect3DDevice9 *>(&g_NewDirect3DDevice9);
		}
		return hRet;
	}

    /*** IDirect3D9Ex methods ***/
    IFACE_PASSTHROUGH(IDirect3D9Ex, GetAdapterModeCountEx, g_OldDirect3D9Ex);
    IFACE_PASSTHROUGH(IDirect3D9Ex, EnumAdapterModesEx, g_OldDirect3D9Ex);
    IFACE_PASSTHROUGH(IDirect3D9Ex, GetAdapterDisplayModeEx, g_OldDirect3D9Ex);
    
	STDMETHOD(CreateDeviceEx)(THIS_ UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX* pFullscreenDisplayMode,IDirect3DDevice9Ex** ppReturnedDeviceInterface)
	{
		HRESULT hRet = g_OldDirect3D9Ex->CreateDeviceEx(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, pFullscreenDisplayMode, ppReturnedDeviceInterface);
		if(ppReturnedDeviceInterface)
		{
			g_OldDirect3DDevice9Ex = *ppReturnedDeviceInterface;

			g_AfxShaders.BeginDevice(g_OldDirect3DDevice9Ex);
			g_CampathDrawer.BeginDevice(g_OldDirect3DDevice9Ex);

			*ppReturnedDeviceInterface = reinterpret_cast<IDirect3DDevice9Ex *>(&g_NewDirect3DDevice9Ex);
		}
		return hRet;
	}
    
	IFACE_PASSTHROUGH(IDirect3D9Ex, GetAdapterLUID, g_OldDirect3D9Ex);
} g_NewDirect3D9Ex;


Direct3DCreate9_t old_Direct3DCreate9 = 0;
IDirect3D9 * WINAPI new_Direct3DCreate9(UINT SDKVersion)
{
	if(D3D_SDK_VERSION == SDKVersion)
	{
		g_OldDirect3D9 = old_Direct3DCreate9(SDKVersion);
		return reinterpret_cast<IDirect3D9 *>(&g_NewDirect3D9);
	}

	return old_Direct3DCreate9(SDKVersion);
}

Direct3DCreate9Ex_t old_Direct3DCreate9Ex = 0;
HRESULT WINAPI new_Direct3DCreate9Ex(UINT SDKVersion, IDirect3D9Ex** ppD3DDevice)
{
	if(D3D_SDK_VERSION == SDKVersion)
	{
		HRESULT hResult = old_Direct3DCreate9Ex(SDKVersion, &g_OldDirect3D9Ex);

		if(ppD3DDevice) *ppD3DDevice = 0 != g_OldDirect3D9Ex ? reinterpret_cast<IDirect3D9Ex *>(&g_NewDirect3D9Ex) : 0;

		return hResult;
	}

	return old_Direct3DCreate9Ex(SDKVersion, ppD3DDevice);
}

void AfxD3D9SetModulationColorFix(float const color[4])
{
	if(!g_OldDirect3DDevice9) return;

	g_OldDirect3DDevice9->SetVertexShaderConstantF(47, color, 1);
}

void AfxD3D9OverrideBegin_D3DRS_SRGBWRITEENABLE(DWORD value)
{
	if(!g_OldDirect3DDevice9) return;

	g_NewDirect3DDevice9.OverrideBegin_D3DRS_SRGBWRITEENABLE(value);
}

void AfxD3D9OverrideEnd_D3DRS_SRGBWRITEENABLE(void)
{
	if(!g_OldDirect3DDevice9) return;

	g_NewDirect3DDevice9.OverrideEnd_D3DRS_SRGBWRITEENABLE();
}


void AfxD3D9OverrideBegin_D3DRS_ZWRITEENABLE(DWORD value)
{
	if(!g_OldDirect3DDevice9) return;

	g_NewDirect3DDevice9.OverrideBegin_D3DRS_ZWRITEENABLE(value);
}

void AfxD3D9OverrideEnd_D3DRS_ZWRITEENABLE(void)
{
	if(!g_OldDirect3DDevice9) return;

	g_NewDirect3DDevice9.OverrideEnd_D3DRS_ZWRITEENABLE();
}

void AfxD3D9_OverrideBegin_ps_c0(float values[4])
{
	if(!g_OldDirect3DDevice9) return;

	g_NewDirect3DDevice9.OverrideBegin_ps_c0(values);
}

void AfxD3D9_OverrideEnd_ps_c0(void)
{
	if(!g_OldDirect3DDevice9) return;

	g_NewDirect3DDevice9.OverrideEnd_ps_c0();
}

void AfxD3D9_OverrideBegin_ps_c12_y(float value)
{
	if(!g_OldDirect3DDevice9) return;

	g_NewDirect3DDevice9.OverrideBegin_ps_c12_y(value);
}

void AfxD3D9_OverrideEnd_ps_c12_y(void)
{
	if(!g_OldDirect3DDevice9) return;

	g_NewDirect3DDevice9.OverrideEnd_ps_c12_y();
}

void AfxD3D9_OverrideBegin_ps_c29_w(float value)
{
	if(!g_OldDirect3DDevice9) return;

	g_NewDirect3DDevice9.OverrideBegin_ps_c29_w(value);
}

void AfxD3D9_OverrideEnd_ps_c29_w(void)
{
	if(!g_OldDirect3DDevice9) return;

	g_NewDirect3DDevice9.OverrideEnd_ps_c29_w();
}

void AfxD3D9_Override_SetVertexShader(IDirect3DVertexShader9 * replacer)
{
	if(!g_OldDirect3DDevice9) return;

	g_NewDirect3DDevice9.Override_SetVertexShader(replacer);
}

void AfxD3D9_Override_SetPixelShader(IDirect3DPixelShader9 * replacer)
{
	if(!g_OldDirect3DDevice9) return;

	g_NewDirect3DDevice9.Override_SetPixelShader(replacer);
}
