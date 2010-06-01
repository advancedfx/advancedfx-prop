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
	m_Device = 0;
	m_Enabled = false;
	m_PsoFileName = 0;
	m_ReloadShader = false;
	m_Shader = 0;
}

MirvShader::~MirvShader()
{
	//EndDevice();
	free(m_PsoFileName);
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


void MirvShader::LoadPso(char const * fileName)
{
	if(m_PsoFileName) free(m_PsoFileName);
	m_PsoFileName = 0;

	if(fileName)
	{
		m_PsoFileName = (char *)malloc(1 +strlen(fileName));

		if(m_PsoFileName)
			strcpy(m_PsoFileName, fileName);

		m_ReloadShader = true;
	}
}

void MirvShader::LoadShader()
{
	UnloadShader();

	if(!m_Device || !m_PsoFileName)
		return;

	std::string shaderDir;

	if(!GetShaderDirectory(shaderDir))
		return;

	shaderDir.append("\\");
	shaderDir.append(m_PsoFileName);

	Tier0_Msg("%s\n", shaderDir.c_str());

	FILE * file = fopen(shaderDir.c_str(), "rb");
	DWORD * pso = 0;
	size_t size = 0;

	bool bOk =
		0 != file
		&& 0 == fseek(file, 0, SEEK_END)
	;

	if(bOk)
	{
		size = ftell(file);

		pso = (DWORD *)malloc(ftell(file));
		fseek(file, 0, SEEK_SET);
		bOk = 0 != pso;
	}

	if(bOk)
	{
		bOk = size == fread(pso, 1, size, file);
	}

	if(bOk && SUCCEEDED(m_Device->CreatePixelShader(pso, &m_Shader)))
		m_Shader->AddRef();
	else
		m_Shader = 0;

	if(file) fclose(file);
	if(pso) free(pso);
}


IDirect3DPixelShader9 * MirvShader::OnSetPixelShader(IDirect3DPixelShader9* pShader)
{
	if(pShader && !m_Device)
	{
		IDirect3DDevice9 * device;

		if(SUCCEEDED(pShader->GetDevice(&device)))
			Device_set(device);
	}

	if(m_Enabled)
	{
		if(m_ReloadShader)
		{
			m_ReloadShader = false;
			LoadShader();
		}

		if(0 != m_Shader)
			return m_Shader;

		m_Enabled = false;
	}

	return pShader;
}

void MirvShader::UnloadShader()
{
	if(!m_Shader)
		return;

	m_Shader->Release();
	m_Shader = 0;
}

