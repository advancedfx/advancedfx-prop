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


const float c_CampathCrossRadius = 36.0f;
const float c_CampathCrossPixelWidth = 8.0f;
const float c_CameraRadius = c_CampathCrossRadius / 2.0f;
const float c_CameraPixelWidth = 2.0f;


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
, m_LineTriangleListBuffer(0)
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

	DWORD oldAlphaTestEnable;
	m_Device->GetRenderState(D3DRS_ALPHATESTENABLE, &oldAlphaTestEnable);

	DWORD oldSeparateAlphaBlendEnable;
	m_Device->GetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, &oldSeparateAlphaBlendEnable);

	DWORD oldAlphaBlendEnable;
	m_Device->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlphaBlendEnable);

	DWORD oldBlendOp;
	m_Device->GetRenderState(D3DRS_BLENDOP, &oldBlendOp);

	DWORD oldSrcBlend;
	m_Device->GetRenderState(D3DRS_SRCBLEND, &oldSrcBlend);

	DWORD oldDestBlend;
	m_Device->GetRenderState(D3DRS_DESTBLEND, &oldDestBlend);

	// Draw:

	double curTime = g_Hook_VClient_RenderView.GetCurTime();
	bool inCampath = 1 <= g_Hook_VClient_RenderView.m_CamPath.GetSize()
		&&	g_Hook_VClient_RenderView.m_CamPath.GetLowerBound() <= curTime
		&& curTime <= g_Hook_VClient_RenderView.m_CamPath.GetUpperBound();
	bool campathCanEval = 4 <= g_Hook_VClient_RenderView.m_CamPath.GetSize();
	bool campathEnabled = g_Hook_VClient_RenderView.m_CamPath.IsEnabled();

	m_Device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
	m_Device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA|D3DCOLORWRITEENABLE_BLUE|D3DCOLORWRITEENABLE_GREEN|D3DCOLORWRITEENABLE_RED);
	m_Device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	m_Device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	m_Device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	m_Device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	m_Device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, FALSE);
	m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_Device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	m_Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


	m_Device->SetVertexShader(m_VertexShader);
	
	m_Device->SetVertexShaderConstantF(8, m_WorldToScreenMatrix.m[0], 4);

	int screenWidth, screenHeight;
	g_VEngineClient->GetScreenSize(screenWidth, screenHeight);
	FLOAT newCScreenInfo[4] = { 0 != screenWidth ? 1.0f / screenWidth : 0.0f, 0 != screenHeight ? 1.0f / screenHeight : 0.0f, 0.0f, 0.0f};

	m_Device->SetPixelShader(m_PixelShader);

	m_Device->SetFVF(CCampathDrawer_VertexFVF);

	if(m_LineTriangleListBuffer)
	{
		size_t crossesCount = g_Hook_VClient_RenderView.m_CamPath.GetSize();
		size_t lineTriangleListBufferVertexCount = (crossesCount * 3 +(inCampath ? 9 : 0))* 4;

		m_Device->SetStreamSource(0, m_LineTriangleListBuffer, 0, sizeof(Vertex));

		int startVertex = 0;
		
		// Draw crosses:

		newCScreenInfo[2] = c_CampathCrossPixelWidth;
		m_Device->SetVertexShaderConstantF(48, newCScreenInfo, 1);

		for(size_t i=0; i<crossesCount; ++i)
		{
			// Draw x / forward line:
			m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, startVertex, 2);

			// Draw y / left line:
			m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, startVertex+4, 2);

			// Draw x / forward line:
			m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, startVertex+8, 2);

			startVertex += 12;
		}

		if(inCampath && campathCanEval)
		{
			newCScreenInfo[2] = c_CameraPixelWidth;
			m_Device->SetVertexShaderConstantF(48, newCScreenInfo, 1);

			// Draw camera lines:

			for(size_t i=0; i<9; ++i)
			{
				// Draw x / forward line:
				m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, startVertex, 2);

				startVertex += 4;
			}
		}
	}

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

	m_Device->SetRenderState(D3DRS_DESTBLEND, oldDestBlend);
	m_Device->SetRenderState(D3DRS_SRCBLEND, oldSrcBlend);
	m_Device->SetRenderState(D3DRS_BLENDOP, oldBlendOp);
	m_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlphaBlendEnable);
	m_Device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, oldSeparateAlphaBlendEnable);
	m_Device->SetRenderState(D3DRS_ALPHATESTENABLE, oldAlphaTestEnable);
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

void CCampathDrawer::BuildSingleLine(Vector3 from, Vector3 to, Vertex * pOutVertexData)
{
	Vector3 normal = (to-from).Normalize();
	
	pOutVertexData[1].x = pOutVertexData[0].x = (float)from.X;
	pOutVertexData[3].x = pOutVertexData[2].x = (float)to.X;
	pOutVertexData[1].y = pOutVertexData[0].y = (float)from.Y;
	pOutVertexData[3].y = pOutVertexData[2].y = (float)to.Y;
	pOutVertexData[1].z = pOutVertexData[0].z = (float)from.Z;
	pOutVertexData[3].z = pOutVertexData[2].z = (float)to.Z;

	pOutVertexData[3].t1u = pOutVertexData[2].t1u = pOutVertexData[1].t1u = pOutVertexData[0].t1u = (float)-normal.X;
	pOutVertexData[3].t1v = pOutVertexData[2].t1v = pOutVertexData[1].t1v = pOutVertexData[0].t1v = (float)-normal.Y;
	pOutVertexData[3].t1w = pOutVertexData[2].t1w = pOutVertexData[1].t1w = pOutVertexData[0].t1w = (float)-normal.Z;

	pOutVertexData[3].t2u = pOutVertexData[2].t2u = pOutVertexData[1].t2u = pOutVertexData[0].t2u = (float)normal.X;
	pOutVertexData[3].t2v = pOutVertexData[2].t2v = pOutVertexData[1].t2v = pOutVertexData[0].t2v = (float)normal.Y;
	pOutVertexData[3].t2w = pOutVertexData[2].t2w = pOutVertexData[1].t2w = pOutVertexData[0].t2w = (float)normal.Z;

	pOutVertexData[2].t0u = pOutVertexData[0].t0u = 1.0f;
	pOutVertexData[3].t0u = pOutVertexData[1].t0u = -1.0f;

	pOutVertexData[3].t0v = pOutVertexData[2].t0v = pOutVertexData[1].t0v = pOutVertexData[0].t0v = 0.0f;

}

void CCampathDrawer::BuildSingleLine(DWORD colorFrom, DWORD colorTo, Vertex * pOutVertexData)
{
	pOutVertexData[1].diffuse = pOutVertexData[0].diffuse = colorFrom;
	pOutVertexData[3].diffuse = pOutVertexData[2].diffuse = colorTo;
}

void CCampathDrawer::RebuildDrawing()
{
	if(!m_Device)
		return;

	double curTime = g_Hook_VClient_RenderView.GetCurTime();
	bool inCampath = 1 <= g_Hook_VClient_RenderView.m_CamPath.GetSize()
		&&	g_Hook_VClient_RenderView.m_CamPath.GetLowerBound() <= curTime
		&& curTime <= g_Hook_VClient_RenderView.m_CamPath.GetUpperBound();
	bool campathCanEval = 4 <= g_Hook_VClient_RenderView.m_CamPath.GetSize();
	bool campathEnabled = g_Hook_VClient_RenderView.m_CamPath.IsEnabled();

	Vertex * lockedLineTriangleListBuffer = 0;
	size_t crossesCount = g_Hook_VClient_RenderView.m_CamPath.GetSize();
	size_t lineTriangleListBufferVertexCount = (crossesCount * 3 +9)* 4;

	if(m_RebuildDrawing)
	{
		UnloadDrawing();

		if(!SUCCEEDED(m_Device->CreateVertexBuffer(
			lineTriangleListBufferVertexCount * sizeof(Vertex),
			D3DUSAGE_WRITEONLY,
			CCampathDrawer_VertexFVF,
			D3DPOOL_DEFAULT,
			&m_LineTriangleListBuffer,
			NULL
		)))
		{
			if(m_LineTriangleListBuffer) m_LineTriangleListBuffer->Release();
			m_LineTriangleListBuffer = 0;
		}

		if(m_LineTriangleListBuffer)
		{
			if(!SUCCEEDED(m_LineTriangleListBuffer->Lock(0, lineTriangleListBufferVertexCount * sizeof(Vertex), (void **)&lockedLineTriangleListBuffer, 0)))
				lockedLineTriangleListBuffer = 0;
		}

		if(lockedLineTriangleListBuffer)
		{
			Vertex * curBuf = lockedLineTriangleListBuffer;

			CamPathIterator it = g_Hook_VClient_RenderView.m_CamPath.GetBegin();

			for(size_t i = 0; i < crossesCount; i++)
			{
				// crosses.

				CamPathValue cpv = it.GetValue();

				// x / forward line:

				curBuf[1].x = curBuf[0].x = (float)cpv.X -c_CampathCrossRadius;
				curBuf[3].x = curBuf[2].x = (float)cpv.X +c_CampathCrossRadius;
				curBuf[3].y = curBuf[2].y = curBuf[1].y = curBuf[0].y = (float)cpv.Y;
				curBuf[3].z = curBuf[2].z = curBuf[1].z = curBuf[0].z = (float)cpv.Z;

				curBuf[3].t1u = curBuf[2].t1u = curBuf[1].t1u = curBuf[0].t1u = -1.0;
				curBuf[3].t1v = curBuf[2].t1v = curBuf[1].t1v = curBuf[0].t1v = 0.0;
				curBuf[3].t1w = curBuf[2].t1w = curBuf[1].t1w = curBuf[0].t1w = 0.0;

				curBuf[3].t2u = curBuf[2].t2u = curBuf[1].t2u = curBuf[0].t2u = 1.0;
				curBuf[3].t2v = curBuf[2].t2v = curBuf[1].t2v = curBuf[0].t2v = 0.0;
				curBuf[3].t2w = curBuf[2].t2w = curBuf[1].t2w = curBuf[0].t2w = 0.0;

				curBuf[2].t0u = curBuf[0].t0u = 1.0;
				curBuf[3].t0u = curBuf[1].t0u = -1.0;

				curBuf[3].t0v = curBuf[2].t0v = curBuf[1].t0v = curBuf[0].t0v = 0.0f;

				curBuf += 4;

				// y / left line:

				curBuf[3].x = curBuf[2].x = curBuf[1].x = curBuf[0].x = (float)cpv.X;
				curBuf[1].y = curBuf[0].y = (float)cpv.Y -c_CampathCrossRadius;
				curBuf[3].y = curBuf[2].y = (float)cpv.Y +c_CampathCrossRadius;
				curBuf[3].z = curBuf[2].z = curBuf[1].z = curBuf[0].z = (float)cpv.Z;

				curBuf[3].t1u = curBuf[2].t1u = curBuf[1].t1u = curBuf[0].t1u = 0.0;
				curBuf[3].t1v = curBuf[2].t1v = curBuf[1].t1v = curBuf[0].t1v = -1.0;
				curBuf[3].t1w = curBuf[2].t1w = curBuf[1].t1w = curBuf[0].t1w = 0.0;

				curBuf[3].t2u = curBuf[2].t2u = curBuf[1].t2u = curBuf[0].t2u = 0.0;
				curBuf[3].t2v = curBuf[2].t2v = curBuf[1].t2v = curBuf[0].t2v = 1.0;
				curBuf[3].t2w = curBuf[2].t2w = curBuf[1].t2w = curBuf[0].t2w = 0.0;

				curBuf[2].t0u = curBuf[0].t0u = 1.0;
				curBuf[3].t0u = curBuf[1].t0u = -1.0;

				curBuf[3].t0v = curBuf[2].t0v = curBuf[1].t0v = curBuf[0].t0v = 0.0f;

				curBuf += 4;

				// z / up line:

				curBuf[3].x = curBuf[2].x = curBuf[1].x = curBuf[0].x = (float)cpv.X;
				curBuf[3].y = curBuf[2].y = curBuf[1].y = curBuf[0].y = (float)cpv.Y;
				curBuf[1].z = curBuf[0].z = (float)cpv.Z -c_CampathCrossRadius;
				curBuf[3].z = curBuf[2].z = (float)cpv.Z +c_CampathCrossRadius;

				curBuf[3].t1u = curBuf[2].t1u = curBuf[1].t1u = curBuf[0].t1u = 0.0;
				curBuf[3].t1v = curBuf[2].t1v = curBuf[1].t1v = curBuf[0].t1v = 0.0;
				curBuf[3].t1w = curBuf[2].t1w = curBuf[1].t1w = curBuf[0].t1w = -1.0;

				curBuf[3].t2u = curBuf[2].t2u = curBuf[1].t2u = curBuf[0].t2u = 0.0;
				curBuf[3].t2v = curBuf[2].t2v = curBuf[1].t2v = curBuf[0].t2v = 0.0;
				curBuf[3].t2w = curBuf[2].t2w = curBuf[1].t2w = curBuf[0].t2w = 1.0;

				curBuf[2].t0u = curBuf[0].t0u = 1.0;
				curBuf[3].t0u = curBuf[1].t0u = -1.0;

				curBuf[3].t0v = curBuf[2].t0v = curBuf[1].t0v = curBuf[0].t0v = 0.0f;

				curBuf += 4;

				// next:

				++it;
			}
		}
	}
	else
	if(m_LineTriangleListBuffer)
	{
		if(!SUCCEEDED(m_LineTriangleListBuffer->Lock(0, lineTriangleListBufferVertexCount * sizeof(Vertex), (void **)&lockedLineTriangleListBuffer, 0)))
			lockedLineTriangleListBuffer = 0;
	}

	if(lockedLineTriangleListBuffer)
	{
		// update colouring:

		Vertex * curBuf = lockedLineTriangleListBuffer;

		CamPathIterator it = g_Hook_VClient_RenderView.m_CamPath.GetBegin();

		for(size_t i = 0; i < crossesCount; i++)
		{
			// crosses.

			CamPathValue cpv = it.GetValue();

			double deltaTime = abs(curTime -it.GetTime());

			DWORD colour;

			if(deltaTime < 1.0)
			{
				// green to yellow:
				double t = (deltaTime -0.0)/1.0;
				colour = D3DCOLOR_RGBA((unsigned char)(255.0*t),255,0,(unsigned char)(127*(1.0-t))+128);
			}
			else
			if(deltaTime < 2.0)
			{
				// yellow to red:
				double t = (deltaTime -1.0)/1.0;
				colour = D3DCOLOR_RGBA(255,(unsigned char)(255.0*(1.0-t)),0,(unsigned char)(64*(1.0-t))+64);
			}
			else
			{
				colour = D3DCOLOR_RGBA(255,0,0,64);
			}

			// x / forward line:

			curBuf[3].diffuse = curBuf[2].diffuse = curBuf[1].diffuse = curBuf[0].diffuse = colour;

			curBuf += 4;

			// y / left line:

			curBuf[3].diffuse = curBuf[2].diffuse = curBuf[1].diffuse = curBuf[0].diffuse = colour;

			curBuf += 4;

			// z / upward line:

			curBuf[3].diffuse = curBuf[2].diffuse = curBuf[1].diffuse = curBuf[0].diffuse = colour;

			curBuf += 4;

			// next:

			++it;
		}

		if(inCampath && campathCanEval)
		{
			// camera.

			DWORD colourCam = campathEnabled ? D3DCOLOR_RGBA(255, 0, 255, 128) : D3DCOLOR_RGBA(255, 255, 255, 128);
			DWORD colourCamUp = campathEnabled ? D3DCOLOR_RGBA(0, 255, 0, 128) : D3DCOLOR_RGBA(0, 0, 0, 128);

			CamPathValue cpv = g_Hook_VClient_RenderView.m_CamPath.Eval(curTime);

			double forward[3], right[3], up[3];
			MakeVectors(cpv.Roll, cpv.Pitch, cpv.Yaw, forward, right, up);

			Vector3 vCp(cpv.X, cpv.Y, cpv.Z);
			Vector3 vForward(forward);
			Vector3 vUp(up);
			Vector3 vRight(right);

			Vector3 vLU = vCp +(double)c_CameraRadius * (vForward -vRight +vUp).Normalize();
			Vector3 vRU = vCp +(double)c_CameraRadius * (vForward +vRight +vUp).Normalize();
			Vector3 vLD = vCp +(double)c_CameraRadius * (vForward -vRight -vUp).Normalize();
			Vector3 vRD = vCp +(double)c_CameraRadius * (vForward +vRight -vUp).Normalize();
			Vector3 vMU = vLU +(vRU -vLU)/2;
			Vector3 vMUU = vMU +(double)c_CameraRadius * vUp;

			BuildSingleLine(vCp, vLD, curBuf);
			BuildSingleLine(colourCam, colourCam, curBuf);
			curBuf += 4;

			BuildSingleLine(vCp, vRD, curBuf);
			BuildSingleLine(colourCam, colourCam, curBuf);
			curBuf += 4;

			BuildSingleLine(vCp, vLU, curBuf);
			BuildSingleLine(colourCam, colourCam, curBuf);
			curBuf += 4;

			BuildSingleLine(vCp, vRU, curBuf);
			BuildSingleLine(colourCam, colourCam, curBuf);
			curBuf += 4;

			BuildSingleLine(vLD, vRD, curBuf);
			BuildSingleLine(colourCam, colourCam, curBuf);
			curBuf += 4;

			BuildSingleLine(vRD, vRU, curBuf);
			BuildSingleLine(colourCam, colourCam, curBuf);
			curBuf += 4;

			BuildSingleLine(vRU, vLU, curBuf);
			BuildSingleLine(colourCam, colourCam, curBuf);
			curBuf += 4;

			BuildSingleLine(vLU, vLD, curBuf);
			BuildSingleLine(colourCam, colourCam, curBuf);
			curBuf += 4;

			BuildSingleLine(vMU, vMUU, curBuf);
			BuildSingleLine(colourCam, colourCamUp, curBuf);
			curBuf += 4;
		}

		//

		m_LineTriangleListBuffer->Unlock();
	}

	m_RebuildDrawing = false;
}

void CCampathDrawer::UnloadDrawing()
{
	if(m_LineTriangleListBuffer) { m_LineTriangleListBuffer->Release(); m_LineTriangleListBuffer = 0; }
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
