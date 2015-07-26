#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-07-20 by dominik.matrixstorm.com
//
// First changes:
// 2010-04-22 by dominik.matrixstorm.com

#include "MirvShader.h"

#include "SourceInterfaces.h"
#include "hlaeFolder.h"

#include <stdio.h>
#include <string>


extern bool g_bActionDepthBound;
extern bool g_bD3D9SRGBWriteEnableFix;

MirvShader g_MirvShader;


bool GetShaderDirectory(std::string & outShaderDirectory)
{
	outShaderDirectory.assign(GetHlaeFolder());

	outShaderDirectory.append("resources\\AfxHookSource\\shaders\\");

	return true;
}


MirvShader::MirvShader()
{
	m_Device = 0;
	m_PsoFileName = 0;
	m_VsoFileName = 0;
	m_ReloadPixelShader = false;
	m_ReloadVertexShader = false;
	m_PixelShader = 0;
	m_VertexShader = 0;

	LoadVso("afx_depthfix_vs20.fxo");
	LoadPso("afx_depthfix_ps20.fxo");
}

MirvShader::~MirvShader()
{
	free(m_PsoFileName);
	free(m_VsoFileName);
}


void MirvShader::BeginDevice(IDirect3DDevice9 * device)
{
	EndDevice();

	if(0 == device)
		return;

	device->AddRef();

	m_Device = device;

	LoadShader();
}

void MirvShader::DebugDepthFixDraw()
{
	if(g_bD3D9SRGBWriteEnableFix)
		m_Device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);

/*	if(!(m_Device && m_PixelShader && m_VertexShader))
	{
		static bool firstError = true;

		if(firstError)
		{
			firstError = false;
			Tier0_Msg("AFXERROR: MirvShader::DebugDepthFixDraw: No Device or VertexShader or PixelShader not loaded\n");
		}

		return;
	}

	IDirect3DPixelShader9 * oldPixelShader;
	m_Device->GetPixelShader(&oldPixelShader);
	if(oldPixelShader) oldPixelShader->AddRef();

	IDirect3DVertexShader9 * oldVertexShader;
	m_Device->GetVertexShader(&oldVertexShader);
	if(oldVertexShader) oldVertexShader->AddRef();

	IDirect3DVertexBuffer9 * oldVertexBuffer;
	UINT oldVertexBufferOffset;
	UINT oldVertexBufferStride;

	m_Device->GetStreamSource(0, &oldVertexBuffer, &oldVertexBufferOffset, &oldVertexBufferStride);
	if(oldVertexBuffer) oldVertexBuffer->AddRef();

	IDirect3DVertexDeclaration9 * oldDeclaration;
	m_Device->GetVertexDeclaration(&oldDeclaration);
	if(oldDeclaration) oldDeclaration->AddRef();

	//

	m_Device->SetPixelShader(m_PixelShader);
	m_Device->SetVertexShader(m_VertexShader);

	m_Device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE3(0));

	struct {
		FLOAT x, y, z;
		FLOAT u, v, w;
	} vertexData = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f};

	m_Device->DrawPrimitiveUP(
		D3DPT_POINTLIST,
		1,
		&vertexData,
		sizeof(vertexData)
	);

	//

	m_Device->SetVertexDeclaration(oldDeclaration);
	if(oldDeclaration) oldDeclaration->Release();

	m_Device->SetStreamSource(0, oldVertexBuffer, oldVertexBufferOffset, oldVertexBufferStride);
	if(oldVertexBuffer) oldVertexBuffer->Release();

	m_Device->SetVertexShader(oldVertexShader);
	if(oldVertexShader) oldVertexShader->Release();

	m_Device->SetPixelShader(oldPixelShader);
	if(oldPixelShader) oldPixelShader->Release();
*/
/*
	float vecZFilter[4] = {1.0f, 0.0f, 0.0f, 1.0f};
	m_Device->SetPixelShaderConstantF(1, vecZFilter, 1);

	float vecModulationColor[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	m_Device->SetPixelShaderConstantF(2, vecModulationColor, 1);

	float vecZFactor[4] = {8000.0f -1.0f, 1.0f, 1.0f, 1.0f};
	m_Device->SetVertexShaderConstantF(48, vecZFactor, 1);
*/
	//m_Device->Clear(0, 0, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1, 0);
}

void MirvShader::EndDevice()
{
	if(0 == m_Device)
		return;

	UnloadShader();

	m_Device->Release();
	m_Device = 0;
}

void MirvShader::LoadPso(char const * fileName)
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

void MirvShader::LoadVso(char const * fileName)
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

DWORD * MirvShader::LoadShaderFileInMemory(char const * fileName)
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

void MirvShader::LoadShader()
{
	LoadVertexShader();
	LoadPixelShader();
}

void MirvShader::LoadPixelShader()
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

void MirvShader::LoadVertexShader()
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

bool MirvShader::OnSetVertexShader(IDirect3DVertexShader9 * pShader, HRESULT &outResult)
{
	return false;

	if(!g_bActionDepthBound)
		return false;

	outResult = m_Device->SetVertexShader(m_VertexShader);

	return true;
}

bool MirvShader::OnSetPixelShader(IDirect3DPixelShader9 * pShader, HRESULT &outResult)
{
	return false;

	if(!g_bActionDepthBound)
		return false;

	outResult = m_Device->SetPixelShader(m_PixelShader);

	return true;
}

void MirvShader::UnloadShader()
{
	UnloadPixelShader();
	UnloadVertexShader();
}

void MirvShader::UnloadPixelShader()
{
	if(!m_PixelShader)
		return;

	m_PixelShader->Release();
	m_PixelShader = 0;
}

void MirvShader::UnloadVertexShader()
{
	if(!m_VertexShader)
		return;

	m_VertexShader->Release();
	m_VertexShader = 0;
}
