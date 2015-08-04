#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-08-03 dominik.matrixstorm.com
//
// First changes:
// 2010-04-22 dominik.matrixstorm.com

#include "CampathDrawer.h"

#include "SourceInterfaces.h"
#include "hlaeFolder.h"
#include "RenderView.h"
#include "WrpVEngineClient.h"

#include <stdio.h>
#include <string>


CCampathDrawer g_CampathDrawer;

extern WrpVEngineClient * g_VEngineClient;


bool GetShaderDirectory(std::string & outShaderDirectory)
{
	outShaderDirectory.assign(GetHlaeFolder());

	outShaderDirectory.append("resources\\AfxHookSource\\shaders\\");

	return true;
}

// CCampathDrawer //////////////////////////////////////////////////////////////

CCampathDrawer::CCampathDrawer()
: m_Draw(false)
, m_RebuildDrawing(true)
{
	m_Device = 0;
	m_PsoFileName = 0;
	m_VsoFileName = 0;
	m_ReloadPixelShader = false;
	m_ReloadVertexShader = false;
	m_PixelShader = 0;
	m_VertexShader = 0;

	LoadVso("afx_line_vs20.fxo");
	LoadPso("afx_line_ps20.fxo");
}

CCampathDrawer::~CCampathDrawer()
{
	free(m_PsoFileName);
	free(m_VsoFileName);
}


void CCampathDrawer::BeginDevice(IDirect3DDevice9 * device)
{
	EndDevice();

	if(0 == device)
		return;

	device->AddRef();

	m_Device = device;

	LoadShader();

	g_Hook_VClient_RenderView.m_CamPath.OnChanged_set(this);

	m_RebuildDrawing = true;
}

void CCampathDrawer::CamPathChanged(CamPath * obj)
{
	m_RebuildDrawing = true;
}

void CCampathDrawer::Draw_set(bool value)
{
	m_Draw = value;
}

bool CCampathDrawer::Draw_get(void)
{
	return m_Draw;
}

void CCampathDrawer::EndDevice()
{
	if(0 == m_Device)
		return;

	g_Hook_VClient_RenderView.m_CamPath.OnChanged_set(0);

	UnloadDrawing();

	UnloadShader();

	m_Device->Release();
	m_Device = 0;
}

void CCampathDrawer::LoadPso(char const * fileName)
{
	if(m_PsoFileName) free(m_PsoFileName);
	m_PsoFileName = 0;

	if(fileName)
	{
		size_t size = 1 +strlen(fileName);

		m_PsoFileName = (char *)malloc(size);

		if(m_PsoFileName)
			strcpy_s(m_PsoFileName, size, fileName);

		m_ReloadPixelShader = true;
	}
}

void CCampathDrawer::LoadVso(char const * fileName)
{
	if(m_VsoFileName) free(m_VsoFileName);
	m_VsoFileName = 0;

	if(fileName)
	{
		size_t size = 1 +strlen(fileName);

		m_VsoFileName = (char *)malloc(size);

		if(m_VsoFileName)
			strcpy_s(m_VsoFileName, size, fileName);

		m_ReloadVertexShader = true;
	}
}

DWORD * CCampathDrawer::LoadShaderFileInMemory(char const * fileName)
{
	if(!fileName)
		return 0;

	std::string shaderDir;

	if(!GetShaderDirectory(shaderDir))
		return 0;

	shaderDir.append(fileName);

	FILE * file = 0;
	bool bOk = 0 == fopen_s(&file, shaderDir.c_str(), "rb");
	DWORD * so = 0;
	size_t size = 0;

	bOk = bOk 
		&& 0 != file
		&& 0 == fseek(file, 0, SEEK_END)
	;

	if(bOk)
	{
		size = ftell(file);

		so = (DWORD *)malloc(
			(size & 0x3) == 0 ? size : size +(4-(size & 0x3))
		);
		fseek(file, 0, SEEK_SET);
		bOk = 0 != so;
	}

	if(bOk)
	{
		bOk = size == fread(so, 1, size, file);
	}

	if(file) fclose(file);

	if(bOk)
		return so;
	
	if(so) free(so);
	
	return 0;
}

void CCampathDrawer::LoadShader()
{
	LoadVertexShader();
	LoadPixelShader();
}

void CCampathDrawer::LoadPixelShader()
{
	UnloadPixelShader();

	if(!m_Device || !m_PsoFileName)
		return;

	DWORD * so = LoadShaderFileInMemory(m_PsoFileName);

	if(so && SUCCEEDED(m_Device->CreatePixelShader(so, &m_PixelShader)))
		m_PixelShader->AddRef();
	else
		m_PixelShader = 0;

	if(so) free(so);

	m_ReloadPixelShader = false;
}

void CCampathDrawer::LoadVertexShader()
{
	UnloadVertexShader();

	if(!m_Device || !m_VsoFileName)
		return;

	DWORD * so = LoadShaderFileInMemory(m_VsoFileName);

	if(so && SUCCEEDED(m_Device->CreateVertexShader(so, &m_VertexShader)))
		m_VertexShader->AddRef();
	else
		m_VertexShader = 0;

	if(so) free(so);

	m_ReloadVertexShader = false;
}

void CCampathDrawer::OnPostRenderAllTools()
{
	if(!m_Draw)
		return;

	if(!(m_Device && m_VertexShader && m_PixelShader && g_VEngineClient))
	{
		static bool firstError = true;

		if(firstError)
		{
			firstError = false;
			Tier0_Msg("AFXERROR: CCampathDrawer::OnEndScene: Missing required dependencies.\n");
		}

		return;
	}

	RebuildDrawing();

	if(g_Hook_VClient_RenderView.m_CamPath.GetSize()<1)
		return;

	// Save device state:

	IDirect3DPixelShader9 * oldPixelShader = 0;
	m_Device->GetPixelShader(&oldPixelShader);
	if(oldPixelShader) oldPixelShader->AddRef();

	IDirect3DVertexShader9 * oldVertexShader = 0;
	m_Device->GetVertexShader(&oldVertexShader);
	if(oldVertexShader) oldVertexShader->AddRef();

	IDirect3DVertexBuffer9 * oldVertexBuffer = 0;
	UINT oldVertexBufferOffset;
	UINT oldVertexBufferStride;
	m_Device->GetStreamSource(0, &oldVertexBuffer, &oldVertexBufferOffset, &oldVertexBufferStride);
	// this is done already according to doc: // if(oldVertexBuffer) oldVertexBuffer->AddRef();

	IDirect3DIndexBuffer9 * oldIndexBuffer = 0;
	m_Device->GetIndices(&oldIndexBuffer);
	// this is done already according to doc: // if(oldIndexBuffer) oldIndexBuffer->AddRef();

	IDirect3DVertexDeclaration9 * oldDeclaration;
	m_Device->GetVertexDeclaration(&oldDeclaration);
	if(oldDeclaration) oldDeclaration->AddRef();

	DWORD oldFVF;
	m_Device->GetFVF(&oldFVF);

	FLOAT oldCViewProj[4][4];
	m_Device->GetVertexShaderConstantF(8, oldCViewProj[0], 4);

	FLOAT oldCScreenInfo[4];
	m_Device->GetVertexShaderConstantF(48, oldCScreenInfo, 1);

	DWORD oldSrgbWriteEnable;
	m_Device->GetRenderState(D3DRS_SRGBWRITEENABLE, &oldSrgbWriteEnable);

	DWORD oldColorWriteEnable;
	m_Device->GetRenderState(D3DRS_COLORWRITEENABLE, &oldColorWriteEnable);

	DWORD oldZEnable;
	m_Device->GetRenderState(D3DRS_ZENABLE, &oldZEnable);

	DWORD oldZWriteEnable;
	m_Device->GetRenderState(D3DRS_ZWRITEENABLE, &oldZWriteEnable);
	
	DWORD oldZFunc;
	m_Device->GetRenderState(D3DRS_ZFUNC, &oldZFunc);

	// Draw:

	m_Device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	m_Device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);
	m_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	m_Device->SetVertexShader(m_VertexShader);
	
	m_Device->SetVertexShaderConstantF(8, m_WorldToScreenMatrix.m[0], 4);

	int screenWidth, screenHeight;
	g_VEngineClient->GetScreenSize(screenWidth, screenHeight);
	FLOAT newCScreenInfo[4] = { 0 != screenWidth ? 1.0f / screenWidth : 0.0f, 0 != screenHeight ? 1.0f / screenHeight : 0.0f, 4.0f, 0.0f};
	m_Device->SetVertexShaderConstantF(48, newCScreenInfo, 1);

	m_Device->SetPixelShader(m_PixelShader);

	m_Device->SetFVF(CCampathDrawer_VertexFVF);

	CamPathValue cpv = g_Hook_VClient_RenderView.m_CamPath.GetBegin().GetValue();

	struct {
		Vertex v1;
		Vertex v2;
		Vertex v3;
		Vertex v4;
	} vertexData = {
		{
			cpv.X, cpv.Y, cpv.Z-10.0,
			D3DCOLOR_RGBA(255,0,0,255),
			1.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, 1.0f
		},
		{
			cpv.X, cpv.Y, cpv.Z-10.0,
			D3DCOLOR_RGBA(255,0,0,255),
			-1.0f, -1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, 1.0f
		},
		{
			cpv.X, cpv.Y, cpv.Z+10.0,
			D3DCOLOR_RGBA(255,255,0,255),
			1.0f, 1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, 1.0f
		},
		{
			cpv.X, cpv.Y, cpv.Z+10.0,
			D3DCOLOR_RGBA(255,255,0,255),
			-1.0f, 1.0f,
			0.0f, 0.0f, -1.0f,
			0.0f, 0.0f, 1.0f
		}
	};

/*
	float x,y,z,w;

	x = vertexData.v1.x * m_WorldToScreenMatrix.m[0][0] + vertexData.v1.y * m_WorldToScreenMatrix.m[0][1] + vertexData.v1.z * m_WorldToScreenMatrix.m[0][2] +m_WorldToScreenMatrix.m[0][3];
	y = vertexData.v1.x * m_WorldToScreenMatrix.m[1][0] + vertexData.v1.y * m_WorldToScreenMatrix.m[1][1] + vertexData.v1.z * m_WorldToScreenMatrix.m[1][2] +m_WorldToScreenMatrix.m[1][3];
	z = vertexData.v1.x * m_WorldToScreenMatrix.m[2][0] + vertexData.v1.y * m_WorldToScreenMatrix.m[2][1] + vertexData.v1.z * m_WorldToScreenMatrix.m[2][2] +m_WorldToScreenMatrix.m[2][3];
	w = vertexData.v1.x * m_WorldToScreenMatrix.m[3][0] + vertexData.v1.y * m_WorldToScreenMatrix.m[3][1] + vertexData.v1.z * m_WorldToScreenMatrix.m[3][2] +m_WorldToScreenMatrix.m[3][3];
	if(w)
	{
		x = x/w;
		y = y/w;
		z = z/w;
		w = 1.0f;
	}
	vertexData.v1.x = x;
	vertexData.v1.y = y;
	vertexData.v1.z = z;
	Tier0_Msg("CPV1: %f %f %f %f\n", x,y,z,w);

	x = vertexData.v2.x * m_WorldToScreenMatrix.m[0][0] + vertexData.v2.y * m_WorldToScreenMatrix.m[0][1] + vertexData.v2.z * m_WorldToScreenMatrix.m[0][2] +m_WorldToScreenMatrix.m[0][3];
	y = vertexData.v2.x * m_WorldToScreenMatrix.m[1][0] + vertexData.v2.y * m_WorldToScreenMatrix.m[1][1] + vertexData.v2.z * m_WorldToScreenMatrix.m[1][2] +m_WorldToScreenMatrix.m[1][3];
	z = vertexData.v2.x * m_WorldToScreenMatrix.m[2][0] + vertexData.v2.y * m_WorldToScreenMatrix.m[2][1] + vertexData.v2.z * m_WorldToScreenMatrix.m[2][2] +m_WorldToScreenMatrix.m[2][3];
	w = vertexData.v2.x * m_WorldToScreenMatrix.m[3][0] + vertexData.v2.y * m_WorldToScreenMatrix.m[3][1] + vertexData.v2.z * m_WorldToScreenMatrix.m[3][2] +m_WorldToScreenMatrix.m[3][3];
	if(w)
	{
		x = x/w;
		y = y/w;
		z = z/w;
		w = 1.0f;
	}
	vertexData.v2.x = x;
	vertexData.v2.y = y;
	vertexData.v2.z = z;
	Tier0_Msg("CPV2: %f %f %f %f\n", x,y,z,w);

	x = vertexData.v3.x * m_WorldToScreenMatrix.m[0][0] + vertexData.v3.y * m_WorldToScreenMatrix.m[0][1] + vertexData.v3.z * m_WorldToScreenMatrix.m[0][2] +m_WorldToScreenMatrix.m[0][3];
	y = vertexData.v3.x * m_WorldToScreenMatrix.m[1][0] + vertexData.v3.y * m_WorldToScreenMatrix.m[1][1] + vertexData.v3.z * m_WorldToScreenMatrix.m[1][2] +m_WorldToScreenMatrix.m[1][3];
	z = vertexData.v3.x * m_WorldToScreenMatrix.m[2][0] + vertexData.v3.y * m_WorldToScreenMatrix.m[2][1] + vertexData.v3.z * m_WorldToScreenMatrix.m[2][2] +m_WorldToScreenMatrix.m[2][3];
	w = vertexData.v3.x * m_WorldToScreenMatrix.m[3][0] + vertexData.v3.y * m_WorldToScreenMatrix.m[3][1] + vertexData.v3.z * m_WorldToScreenMatrix.m[3][2] +m_WorldToScreenMatrix.m[3][3];
	if(w)
	{
		x = x/w;
		y = y/w;
		z = z/w;
		w = 1.0f;
	}
	vertexData.v3.x = x;
	vertexData.v3.y = y;
	vertexData.v3.z = z;
	Tier0_Msg("CPV3: %f %f %f %f\n", x,y,z,w);

	x = vertexData.v4.x * m_WorldToScreenMatrix.m[0][0] + vertexData.v4.y * m_WorldToScreenMatrix.m[0][1] + vertexData.v4.z * m_WorldToScreenMatrix.m[0][2] +m_WorldToScreenMatrix.m[0][3];
	y = vertexData.v4.x * m_WorldToScreenMatrix.m[1][0] + vertexData.v4.y * m_WorldToScreenMatrix.m[1][1] + vertexData.v4.z * m_WorldToScreenMatrix.m[1][2] +m_WorldToScreenMatrix.m[1][3];
	z = vertexData.v4.x * m_WorldToScreenMatrix.m[2][0] + vertexData.v4.y * m_WorldToScreenMatrix.m[2][1] + vertexData.v4.z * m_WorldToScreenMatrix.m[2][2] +m_WorldToScreenMatrix.m[2][3];
	w = vertexData.v4.x * m_WorldToScreenMatrix.m[3][0] + vertexData.v4.y * m_WorldToScreenMatrix.m[3][1] + vertexData.v4.z * m_WorldToScreenMatrix.m[3][2] +m_WorldToScreenMatrix.m[3][3];
	if(w)
	{
		x = x/w;
		y = y/w;
		z = z/w;
		w = 1.0f;
	}
	vertexData.v4.x = x;
	vertexData.v4.y = y;
	vertexData.v4.z = z;
	Tier0_Msg("CPV4: %f %f %f %f\n", x,y,z,w);
*/

	m_Device->DrawPrimitiveUP(
		D3DPT_TRIANGLESTRIP,
		2,
		&vertexData,
		sizeof(Vertex)
	);

	// Restore device state:

	m_Device->SetPixelShader(oldPixelShader);
	if(oldPixelShader) oldPixelShader->Release();

	m_Device->SetVertexShader(oldVertexShader);
	if(oldVertexShader) oldVertexShader->Release();

	m_Device->SetStreamSource(0, oldVertexBuffer, oldVertexBufferOffset, oldVertexBufferStride);
	if(oldVertexBuffer) oldVertexBuffer->Release();

	m_Device->SetIndices(oldIndexBuffer);
	if(oldIndexBuffer) oldIndexBuffer->Release();

	m_Device->SetFVF(oldFVF);

	m_Device->SetVertexDeclaration(oldDeclaration);
	if(oldDeclaration) oldDeclaration->Release();

	m_Device->SetVertexShaderConstantF(8, oldCViewProj[0], 4);
	m_Device->SetVertexShaderConstantF(48, oldCScreenInfo, 1);

	m_Device->SetRenderState(D3DRS_ZFUNC, oldZFunc);
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE, oldZWriteEnable);
	m_Device->SetRenderState(D3DRS_ZENABLE, oldZEnable);
	m_Device->SetRenderState(D3DRS_COLORWRITEENABLE, oldColorWriteEnable);
	m_Device->SetRenderState(D3DRS_SRGBWRITEENABLE, oldSrgbWriteEnable);
}

void CCampathDrawer::OnSetMaterial(CONST D3DMATERIAL9* pMaterial)
{
	memcpy(&m_OldMaterial, pMaterial, sizeof(m_OldMaterial));
}

void CCampathDrawer::OnSetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	switch(State)
	{
	case D3DRS_COLORWRITEENABLE:
		m_OldColorWriteEnable = Value;
		break;
	}
}

void CCampathDrawer::OnSetupEngineView()
{
	m_WorldToScreenMatrix = g_VEngineClient->WorldToScreenMatrix();
}

void CCampathDrawer::RebuildDrawing()
{
	if(!m_RebuildDrawing || !m_Device)
		return;

	UnloadDrawing();

	m_RebuildDrawing = false;
}

void CCampathDrawer::UnloadDrawing()
{
	if(!m_Device)
		return;


}

void CCampathDrawer::UnloadShader()
{
	UnloadPixelShader();
	UnloadVertexShader();
}

void CCampathDrawer::UnloadPixelShader()
{
	if(!m_PixelShader)
		return;

	m_PixelShader->Release();
	m_PixelShader = 0;
}

void CCampathDrawer::UnloadVertexShader()
{
	if(!m_VertexShader)
		return;

	m_VertexShader->Release();
	m_VertexShader = 0;
}
