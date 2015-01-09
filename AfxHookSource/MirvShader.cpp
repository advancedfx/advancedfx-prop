#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-04-22 by dominik.matrixstorm.com
//
// First changes:
// 2010-04-22 by dominik.matrixstorm.com

#include "MirvShader.h"

#include <stdio.h>
#include <string>

#include <shared/RawOutput.h>

#define DLL_NAME "AfxHookSource.dll"

MirvShader g_MirvShader;

#include "SourceInterfaces.h"

bool GetShaderDirectory(std::string & outShaderDirectory)
{
	LPSTR fileName = 0;
	HMODULE hm;
	DWORD length;

	bool bOk =
		0 != (hm = GetModuleHandle(DLL_NAME))
	;

	if(hm)
	{
		length = 100;
		fileName = (LPSTR)malloc(length);

		while(fileName && length == GetModuleFileNameA(hm, fileName, length))
			fileName = (LPSTR)realloc(fileName, (length += 100));

		if(!fileName)
			return false;

		bOk = 0 < length;
	}

	if(bOk)
	{
		outShaderDirectory.assign(fileName);

		outShaderDirectory.erase(
			strlen(fileName) -strlen(DLL_NAME),
			strlen(DLL_NAME)
		);

		outShaderDirectory.append("shader");
	}

	free(fileName);

	return bOk;
}


MirvShader::MirvShader()
{
	m_ClearCount = 0;
	m_DepthEnabled = false;
	m_Device = 0;
	m_Enabled = false;
	m_InRenderWorld = false;
	m_PsoFileName = 0;
	m_VsoFileName = 0;
	m_ReloadPixelShader = false;
	m_ReloadVertexShader = false;
	m_PixelShader = 0;
	m_VertexShader = 0;

	LoadVso("mirv_depth_vs20.fxo");
	LoadPso("mirv_depth_ps20.fxo");
}

MirvShader::~MirvShader()
{
	//EndDevice();
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

void MirvShader::Device_set(IDirect3DDevice9 * device)
{
	BeginDevice(device);
}

bool MirvShader::Enabled_get()
{
	return m_Enabled;
}


void MirvShader::Enabled_set(bool value)
{
	m_Enabled = value;
}


void MirvShader::EndDevice()
{
	if(0 == m_Device)
		return;

	UnloadShader();

	m_Device->Release();
	m_Device = 0;
}

bool MirvShader::IsActive()
{
	return m_Enabled && m_DepthEnabled;
}

bool MirvShader::IsBlockedPixelShaderConstant(UINT StartRegister)
{
	if(!IsActive()) return false;

	return true;
}

bool MirvShader::IsBlockedVertexShaderConstant(UINT StartRegister)
{
	if(!IsActive()) return false;

	switch(StartRegister)
	{
	case 8: // cViewProj0
	//case 9: // cViewProj1
	//case 10: // cViewProj2
	//case 11: // cViewProj02
	case 58:
		return false;
	}

	return true;
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

	shaderDir.append("\\");
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

void MirvShader::OnBeginScene()
{
	m_ClearCount = 0;
}

void MirvShader::OnClear(DWORD Count)
{
	if(Count == 0) m_ClearCount++;
}

IDirect3DPixelShader9 * MirvShader::OnSetPixelShader(IDirect3DPixelShader9* pShader)
{
	if(pShader && !m_Device)
	{
		IDirect3DDevice9 * device;

		if(SUCCEEDED(pShader->GetDevice(&device)))
			Device_set(device);
	}

	IDirect3DDevice9 * device;
	if(pShader && SUCCEEDED(pShader->GetDevice(&device)))
	{
		if(m_Device != device) Tier0_Warning("Pixelshader Device mismatch.");
	}

	if(IsActive())
	{
		if(m_ReloadPixelShader)
		{
			LoadPixelShader();
		}

		if(0 != m_PixelShader)
			return m_PixelShader;

		m_Enabled = false;
	}

	return pShader;
}

void MirvShader::OnSetRenderState(D3DRENDERSTATETYPE State,DWORD Value)
{
	if(State == D3DRS_ZENABLE)
	{
		m_DepthEnabled = D3DZB_FALSE != Value;
	}
}

IDirect3DVertexShader9 * MirvShader::OnSetVertexShader(IDirect3DVertexShader9* pShader)
{
	if(pShader && !m_Device)
	{
		IDirect3DDevice9 * device;

		if(SUCCEEDED(pShader->GetDevice(&device)))
			Device_set(device);
	}

	IDirect3DDevice9 * device;
	if(pShader && SUCCEEDED(pShader->GetDevice(&device)))
	{
		if(m_Device != device) Tier0_Warning("Vertexshader Device mismatch.");
	}

	if(IsActive())
	{
		if(m_ReloadVertexShader)
		{
			LoadVertexShader();
		}

		if(0 != m_VertexShader)
			return m_VertexShader;

		m_Enabled = false;
	}

	return pShader;
}

void MirvShader::SetInRenderWorld(bool value)
{
	m_InRenderWorld = value;
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
