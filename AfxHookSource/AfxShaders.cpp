#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-12-02 dominik.matrixstorm.com
//
// First changes:
// 2015-12-02 dominik.matrixstorm.com

#include "AfxShaders.h"

#include "hlaeFolder.h"

#include <string>

CAfxShaders g_AfxShaders;

bool GetShaderDirectory(std::string & outShaderDirectory)
{
	outShaderDirectory.assign(GetHlaeFolder());

	outShaderDirectory.append("resources\\AfxHookSource\\shaders\\");

	return true;
}

DWORD * LoadShaderFileInMemory(char const * fileName)
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

// CAfxVertexShader ////////////////////////////////////////////////////////////

CAfxVertexShader::CAfxVertexShader()
: m_VertexShader(0)
{
}

CAfxVertexShader::~CAfxVertexShader()
{
	if(m_VertexShader) m_VertexShader->Release();
}

void CAfxVertexShader::AddRef()
{
	CAfxShader::AddRef();
}

void CAfxVertexShader::Release()
{
	CAfxShader::Release();
}

IDirect3DVertexShader9 * CAfxVertexShader::GetVertexShader()
{
	return m_VertexShader;
}

void CAfxVertexShader::BeginDevice(IDirect3DDevice9 * device, char const * name)
{
	DWORD * so = LoadShaderFileInMemory(name);

	if(so && SUCCEEDED(device->CreateVertexShader(so, &m_VertexShader)))
		m_VertexShader->AddRef();
	else
		m_VertexShader = 0;

	if(so) free(so);
}

void CAfxVertexShader::EndDevice()
{
	if(!m_VertexShader)
		return;

	m_VertexShader->Release();
	m_VertexShader = 0;
}

// CAfxPixelShader ////////////////////////////////////////////////////////////

CAfxPixelShader::CAfxPixelShader()
: m_PixelShader(0)
{
}

CAfxPixelShader::~CAfxPixelShader()
{
	if(m_PixelShader) m_PixelShader->Release();
}

void CAfxPixelShader::AddRef()
{
	CAfxShader::AddRef();
}

void CAfxPixelShader::Release()
{
	CAfxShader::Release();
}


IDirect3DPixelShader9 * CAfxPixelShader::GetPixelShader()
{
	return m_PixelShader;
}

void CAfxPixelShader::BeginDevice(IDirect3DDevice9 * device, char const * name)
{
	DWORD * so = LoadShaderFileInMemory(name);

	if(so && SUCCEEDED(device->CreatePixelShader(so, &m_PixelShader)))
		m_PixelShader->AddRef();
	else
		m_PixelShader = 0;

	if(so) free(so);
}

void CAfxPixelShader::EndDevice()
{
	if(!m_PixelShader)
		return;

	m_PixelShader->Release();
	m_PixelShader = 0;
}


// CAfxShader //////////////////////////////////////////////////////////////////

CAfxShader::CAfxShader()
: m_RefCount(0)
{
}

CAfxShader::~CAfxShader()
{
}

void CAfxShader::AddRef()
{
	++m_RefCount;
}

void CAfxShader::Release()
{
	--m_RefCount;

	if(0 == m_RefCount)
		delete this;
}

unsigned long CAfxShader::GetRefCount()
{
	return m_RefCount;
}

// CAfxShaders /////////////////////////////////////////////////////////////////

CAfxShaders::CAfxShaders()
: m_Device(0)
{
}

CAfxShaders::~CAfxShaders()
{
	for(std::map<std::string,CAfxPixelShader *>::iterator it = m_PixelShaders.begin(); it != m_PixelShaders.end(); ++it)
	{
		it->second->Release();
	}

	for(std::map<std::string,CAfxVertexShader *>::iterator it = m_VertexShaders.begin(); it != m_VertexShaders.end(); ++it)
	{
		it->second->Release();
	}
}

IAfxVertexShader * CAfxShaders::GetVertexShader(char const * name)
{
	std::string sName(name);

	std::map<std::string,CAfxVertexShader *>::iterator it = m_VertexShaders.find(sName);
	if(it!=m_VertexShaders.end())
	{
		it->second->AddRef(); // for call
		return it->second;
	}

	CAfxVertexShader * shader = new CAfxVertexShader();
	shader->AddRef(); // for list
	shader->AddRef(); // for call

	if(m_Device) shader->BeginDevice(m_Device, sName.c_str());

	return m_VertexShaders[sName] = shader;
}

IAfxPixelShader * CAfxShaders::GetPixelShader(char const * name)
{
	std::string sName(name);

	std::map<std::string,CAfxPixelShader *>::iterator it = m_PixelShaders.find(sName);
	if(it!=m_PixelShaders.end())
	{
		it->second->AddRef(); // for call
		return it->second;
	}

	CAfxPixelShader * shader = new CAfxPixelShader();
	shader->AddRef(); // for list
	shader->AddRef(); // for call

	if(m_Device) shader->BeginDevice(m_Device, sName.c_str());

	return m_PixelShaders[sName] = shader;
}
	
void CAfxShaders::BeginDevice(IDirect3DDevice9 * device)
{
	EndDevice();

	if(0 == device)
		return;

	device->AddRef();

	m_Device = device;

	for(std::map<std::string,CAfxPixelShader *>::iterator it = m_PixelShaders.begin(); it != m_PixelShaders.end(); ++it)
	{
		it->second->BeginDevice(device, it->first.c_str());
	}

	for(std::map<std::string,CAfxVertexShader *>::iterator it = m_VertexShaders.begin(); it != m_VertexShaders.end(); ++it)
	{
		it->second->BeginDevice(device, it->first.c_str());
	}
}

void CAfxShaders::EndDevice()
{
	if(0 == m_Device)
		return;

	for(std::map<std::string,CAfxPixelShader *>::iterator it = m_PixelShaders.begin(); it != m_PixelShaders.end(); ++it)
	{
		it->second->EndDevice();
	}

	for(std::map<std::string,CAfxVertexShader *>::iterator it = m_VertexShaders.begin(); it != m_VertexShaders.end(); ++it)
	{
		it->second->EndDevice();
	}

	m_Device->Release();
	m_Device = 0;
}

void CAfxShaders::ReleaseUnusedShaders()
{
	for(std::map<std::string,CAfxPixelShader *>::iterator it = m_PixelShaders.begin(); it != m_PixelShaders.end();)
	{
		if(1 == it->second->GetRefCount())
		{
			std::map<std::string,CAfxPixelShader *>::iterator er = it;
			++it;
			er->second->Release();
			m_PixelShaders.erase(er);
		}
		else
		{
			++it;
		}
	}

	for(std::map<std::string,CAfxVertexShader *>::iterator it = m_VertexShaders.begin(); it != m_VertexShaders.end();)
	{
		if(1 == it->second->GetRefCount())
		{
			std::map<std::string,CAfxVertexShader *>::iterator er = it;
			++it;
			er->second->Release();
			m_VertexShaders.erase(er);
		}
		else
		{
			++it;
		}
	}
}
