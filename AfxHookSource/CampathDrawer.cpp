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

#define _USE_MATH_DEFINES
#include <math.h>

#include <stdio.h>
#include <string>

/// <remarks>Must be at least 4.</remarks>
const UINT c_VertexBufferVertexCount = 200;

const FLOAT c_CampathCrossRadius = 36.0f;
const FLOAT c_CampathCrossPixelWidth = 4.0f;

const FLOAT c_CameraRadius = c_CampathCrossRadius / 2.0f;
const FLOAT c_CameraPixelWidth = 4.0f;

const FLOAT c_CameraTrajectoryPixelWidth = 8.0f;

/// <summary>Epsilon for point reduction in world units (inch).</summary>
/// <remarks>Must be at least 0.0.</remarks>
const double c_CameraTrajectoryEpsilon = 1.0f;

/// <remarks>Must be at least 2.</remarks>
const size_t c_CameraTrajectoryMaxPointsPerInterval = 1024;

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
, m_VertexBuffer(0)
, m_VertexBufferVertexCount(0)
, m_LockedVertexBuffer(0)
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

void CCampathDrawer::AutoPolyLineStart()
{
	m_PolyLineStarted = true;
}

void CCampathDrawer::AutoPolyLinePoint(Vector3 previous, Vector3 current, DWORD colorCurrent, Vector3 next)
{
	bool isFirst = 0 == m_VertexBufferVertexCount;
	UINT vertexCount = isFirst ? 4 : 2;

	// make sure we have enough space:
	if(c_VertexBufferVertexCount < m_VertexBufferVertexCount+vertexCount)
		AutoPolyLineFlush();

	if(!m_LockedVertexBuffer)
	{
		bool locked = LockVertexBuffer();
		if(!locked)
			return;
	}

	Vertex * curBuf = &(m_LockedVertexBuffer[m_VertexBufferVertexCount]);
	if(isFirst && !m_PolyLineStarted)
	{
		BuildPolyLinePoint(m_OldPreviousPolyLinePoint, previous, m_OldCurrentColor, current, curBuf);
		curBuf += 2;
		m_VertexBufferVertexCount += 2;
	}

	BuildPolyLinePoint(previous, current, colorCurrent, next, curBuf);
	m_VertexBufferVertexCount += 2;

	m_PolyLineStarted = false;
	m_OldCurrentColor = colorCurrent;
	m_OldPreviousPolyLinePoint = previous;
}

void CCampathDrawer::AutoPolyLineFlush()
{
	if(!m_LockedVertexBuffer)
		return;

	UnlockVertexBuffer();

	m_Device->SetStreamSource(0, m_VertexBuffer, 0, sizeof(Vertex));

	UINT startVertex = 0;
	UINT primitiveCount = m_VertexBufferVertexCount -2;

	// Draw lines:
	m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, startVertex, primitiveCount);
	startVertex += 2*(primitiveCount+1);

	m_VertexBufferVertexCount = 0;
}

void CCampathDrawer::AutoSingleLine(Vector3 from, DWORD colorFrom, Vector3 to, DWORD colorTo)
{
	// make sure we have space for another line:
	if(c_VertexBufferVertexCount < m_VertexBufferVertexCount+4)
		AutoSingleLineFlush();

	if(!m_LockedVertexBuffer)
	{
		bool locked = LockVertexBuffer();
		if(!locked)
			return;
	}

	Vertex * curBuf = &(m_LockedVertexBuffer[m_VertexBufferVertexCount]);
	BuildSingleLine(from, to, curBuf);
	BuildSingleLine(colorFrom, colorTo, curBuf);
	m_VertexBufferVertexCount += 4;
}

void CCampathDrawer::AutoSingleLineFlush()
{
	if(!m_LockedVertexBuffer)
		return;

	UnlockVertexBuffer();

	m_Device->SetStreamSource(0, m_VertexBuffer, 0, sizeof(Vertex));

	UINT startVertex = 0;
	UINT lineCount = m_VertexBufferVertexCount / 4;
	for(UINT i=0; i<lineCount; ++i)
	{
		// Draw line:
		m_Device->DrawPrimitive(D3DPT_TRIANGLESTRIP, startVertex, 2);

		startVertex += 4;
	}

	m_VertexBufferVertexCount = 0;
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

void CCampathDrawer::BuildPolyLinePoint(Vector3 previous, Vector3 current, DWORD currentColor, Vector3 next, Vertex * pOutVertexData)
{
	Vector3 toPrevious = (previous-current).Normalize();
	Vector3 toNext = (next-current).Normalize();
	
	pOutVertexData[1].x = pOutVertexData[0].x = (float)current.X;
	pOutVertexData[1].y = pOutVertexData[0].y = (float)current.Y;
	pOutVertexData[1].z = pOutVertexData[0].z = (float)current.Z;

	pOutVertexData[3].t1u = pOutVertexData[2].t1u = pOutVertexData[1].t1u = pOutVertexData[0].t1u = (float)toPrevious.X;
	pOutVertexData[3].t1v = pOutVertexData[2].t1v = pOutVertexData[1].t1v = pOutVertexData[0].t1v = (float)toPrevious.Y;
	pOutVertexData[3].t1w = pOutVertexData[2].t1w = pOutVertexData[1].t1w = pOutVertexData[0].t1w = (float)toPrevious.Z;

	pOutVertexData[3].t2u = pOutVertexData[2].t2u = pOutVertexData[1].t2u = pOutVertexData[0].t2u = (float)toNext.X;
	pOutVertexData[3].t2v = pOutVertexData[2].t2v = pOutVertexData[1].t2v = pOutVertexData[0].t2v = (float)toNext.Y;
	pOutVertexData[3].t2w = pOutVertexData[2].t2w = pOutVertexData[1].t2w = pOutVertexData[0].t2w = (float)toNext.Z;

	pOutVertexData[0].t0u = 1.0f;
	pOutVertexData[1].t0u = -1.0f;

	pOutVertexData[1].t0v = pOutVertexData[0].t0v = 0.0f;

	pOutVertexData[1].diffuse = pOutVertexData[0].diffuse = currentColor;
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

	UnloadVertexBuffer();

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

#define ValToUCCondInv(value,invert) ((invert) ? 0xFF -(unsigned char)(value) : (unsigned char)(value) )

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

	DWORD oldCullMode;
	m_Device->GetRenderState(D3DRS_CULLMODE, &oldCullMode);

	// Draw:
	{
		double curTime = g_Hook_VClient_RenderView.GetCurTime();
		bool inCampath = 1 <= g_Hook_VClient_RenderView.m_CamPath.GetSize()
			&&	g_Hook_VClient_RenderView.m_CamPath.GetLowerBound() <= curTime
			&& curTime <= g_Hook_VClient_RenderView.m_CamPath.GetUpperBound();
		bool campathCanEval = 4 <= g_Hook_VClient_RenderView.m_CamPath.GetSize();
		bool campathEnabled = g_Hook_VClient_RenderView.m_CamPath.IsEnabled();
		bool cameraMightBeSelected = false;

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
		m_Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		m_Device->SetVertexShader(m_VertexShader);
	
		m_Device->SetVertexShaderConstantF(8, m_WorldToScreenMatrix.m[0], 4);

		m_Device->SetPixelShader(m_PixelShader);

		m_Device->SetFVF(CCampathDrawer_VertexFVF);

		int screenWidth, screenHeight;
		g_VEngineClient->GetScreenSize(screenWidth, screenHeight);
		FLOAT newCScreenInfo[4] = { 0 != screenWidth ? 1.0f / screenWidth : 0.0f, 0 != screenHeight ? 1.0f / screenHeight : 0.0f, 0.0, 0.0f};

		// Draw trajectory:
		if(2 <= g_Hook_VClient_RenderView.m_CamPath.GetSize() && campathCanEval)
		{
			if(m_RebuildDrawing)
			{
				// Rebuild trajectory points.
				// This operation can be quite expensive (up to O(N^2)),
				// so it should be done only when s.th.
				// changed (which is what we do here).

				m_TrajectoryPoints.clear();
				
				CamPathIterator last = g_Hook_VClient_RenderView.m_CamPath.GetBegin();				
				CamPathIterator it = last;

				TempPoint * pts = new TempPoint[c_CameraTrajectoryMaxPointsPerInterval];

				for(++it; it != g_Hook_VClient_RenderView.m_CamPath.GetEnd(); ++it)
				{
					double delta = it.GetTime() -last.GetTime();

					for(size_t i = 0; i<c_CameraTrajectoryMaxPointsPerInterval; i++)
					{
						double t = last.GetTime() + delta*((double)i/(c_CameraTrajectoryMaxPointsPerInterval-1));

						CamPathValue cpv = g_Hook_VClient_RenderView.m_CamPath.Eval(t);

						pts[i].t = t;
						pts[i].y = Vector3(cpv.X, cpv.Y, cpv.Z);
						pts[i].nextPt = i+1 <c_CameraTrajectoryMaxPointsPerInterval ? &(pts[i+1]) : 0;
					}

					RamerDouglasPeucker(&(pts[0]), &(pts[c_CameraTrajectoryMaxPointsPerInterval-1]), c_CameraTrajectoryEpsilon);

					// add all points except the last one (to avoid duplicates):
					for(TempPoint * pt = &(pts[0]); pt && pt->nextPt; pt = pt->nextPt)
					{
						m_TrajectoryPoints.push_back(pt->t);
					}

					last = it;
				}

				// add last point:
				m_TrajectoryPoints.push_back(pts[c_CameraTrajectoryMaxPointsPerInterval-1].t);

				delete pts;

				m_RebuildDrawing = false;
			}

			newCScreenInfo[2] = c_CameraTrajectoryPixelWidth;
			m_Device->SetVertexShaderConstantF(48, newCScreenInfo, 1);

			AutoPolyLineStart();

			std::list<double>::iterator itPts = m_TrajectoryPoints.begin();
	
			bool hasLastPt = false;
			bool hasNextPt = false;
			bool hasCurPt = false;
			
			double lastPtTime;
			Vector3 lastPtValue;
			double curPtTime;
			Vector3 curPtValue;
			double nextPtTime;
			Vector3 nextPtValue;

			do
			{
				if(hasNextPt)
				{
					hasLastPt = true;
					lastPtTime = curPtTime;
					lastPtValue = curPtValue;

					hasCurPt = true;
					curPtTime = nextPtTime;
					curPtValue = nextPtValue;

					hasNextPt = false;
				}
				else
				{
					hasCurPt = true;
					curPtTime = *itPts;
					CamPathValue curValue = g_Hook_VClient_RenderView.m_CamPath.Eval(curPtTime);
					curPtValue = Vector3(curValue.X, curValue.Y, curValue.Z);
					++itPts;
				}

				if(itPts != m_TrajectoryPoints.end())
				{
					hasNextPt = true;
					nextPtTime = *itPts;
					CamPathValue nextValue = g_Hook_VClient_RenderView.m_CamPath.Eval(nextPtTime);
					nextPtValue = Vector3(nextValue.X, nextValue.Y, nextValue.Z);
					++itPts;
				}
				else
				{
					// current point is last point.
					hasNextPt = false;
					nextPtValue = curPtValue +(curPtValue -lastPtValue);
				}

				if(!hasLastPt)
				{
					// current point is first point.
					lastPtValue = curPtValue +(curPtValue -nextPtValue);
				}

				// emit current point:
				{
					double deltaTime = abs(curTime -curPtTime);

					bool selected = false; //lastKeySelected && nextKeySelected;

					DWORD colour;

					// determine colour:
					if(deltaTime < 1.0)
					{
						double t = (deltaTime -0.0)/1.0;
						colour = D3DCOLOR_RGBA(
							ValToUCCondInv(255.0*t, selected),
							ValToUCCondInv(255, selected),
							ValToUCCondInv(0, selected),
							(unsigned char)(127*(1.0-t))+128
						);
					}
					else
					if(deltaTime < 2.0)
					{
						double t = (deltaTime -1.0)/1.0;
						colour = D3DCOLOR_RGBA(
							ValToUCCondInv(255, selected),
							ValToUCCondInv(255.0*(1.0-t), selected),
							ValToUCCondInv(0, selected),
							(unsigned char)(64*(1.0-t))+64
						);
					}
					else
					{
						colour = D3DCOLOR_RGBA(
							ValToUCCondInv(255, selected),
							ValToUCCondInv(0, selected),
							ValToUCCondInv(0, selected),
							64
						);
					}

					AutoPolyLinePoint(lastPtValue, curPtValue, colour, nextPtValue);
				}
			}
			while(hasNextPt);

			AutoPolyLineFlush();
		}

		// Draw keyframes:
		{
			newCScreenInfo[2] = c_CampathCrossPixelWidth;
			m_Device->SetVertexShaderConstantF(48, newCScreenInfo, 1);

			CamPathIterator last = g_Hook_VClient_RenderView.m_CamPath.GetEnd();
		
			for(CamPathIterator it = g_Hook_VClient_RenderView.m_CamPath.GetBegin(); it != g_Hook_VClient_RenderView.m_CamPath.GetEnd(); ++it)
			{
				CamPathValue cpv = it.GetValue();

				if(last != g_Hook_VClient_RenderView.m_CamPath.GetEnd())
				{
					cameraMightBeSelected = cameraMightBeSelected || last.IsSelected() && it.IsSelected() && last.GetTime() <= curTime && curTime <= it.GetTime();
				}
				last = it;

				double deltaTime = abs(curTime -it.GetTime());

				bool selected = it.IsSelected();

				DWORD colour;

				// determine colour:
				if(deltaTime < 1.0)
				{
					double t = (deltaTime -0.0)/1.0;
					colour = D3DCOLOR_RGBA(
						ValToUCCondInv(255.0*t, selected),
						ValToUCCondInv(255, selected),
						ValToUCCondInv(0, selected),
						(unsigned char)(127*(1.0-t))+128
					);
				}
				else
				if(deltaTime < 2.0)
				{
					double t = (deltaTime -1.0)/1.0;
					colour = D3DCOLOR_RGBA(
						ValToUCCondInv(255, selected),
						ValToUCCondInv(255.0*(1.0-t), selected),
						ValToUCCondInv(0, selected),
						(unsigned char)(64*(1.0-t))+64
					);
				}
				else
				{
					colour = D3DCOLOR_RGBA(
						ValToUCCondInv(255, selected),
						ValToUCCondInv(0, selected),
						ValToUCCondInv(0, selected),
						64
					);
				}

				// x / forward line:

				AutoSingleLine(
					Vector3(cpv.X -c_CampathCrossRadius, cpv.Y, cpv.Z),
					colour,
					Vector3(cpv.X +c_CampathCrossRadius, cpv.Y, cpv.Z),
					colour
				);

				// y / left line:

				AutoSingleLine(
					Vector3(cpv.X, cpv.Y -c_CampathCrossRadius, cpv.Z),
					colour,
					Vector3(cpv.X, cpv.Y +c_CampathCrossRadius, cpv.Z),
					colour
				);

				// z / up line:

				AutoSingleLine(
					Vector3(cpv.X, cpv.Y, cpv.Z -c_CampathCrossRadius),
					colour,
					Vector3(cpv.X, cpv.Y, cpv.Z +c_CampathCrossRadius),
					colour
				);
			}

			AutoSingleLineFlush();
		}

		// Draw wireframe camera:
		if(inCampath && campathCanEval)
		{
			newCScreenInfo[2] = c_CameraPixelWidth;
			m_Device->SetVertexShaderConstantF(48, newCScreenInfo, 1);

			DWORD colourCam = campathEnabled
				? D3DCOLOR_RGBA(
					ValToUCCondInv(255,cameraMightBeSelected),
					ValToUCCondInv(0,cameraMightBeSelected),
					ValToUCCondInv(255,cameraMightBeSelected),
					128)
				: D3DCOLOR_RGBA(
					ValToUCCondInv(255,cameraMightBeSelected),
					ValToUCCondInv(255,cameraMightBeSelected),
					ValToUCCondInv(255,cameraMightBeSelected),
					128);
			DWORD colourCamUp = campathEnabled
				? D3DCOLOR_RGBA(
					ValToUCCondInv(0,cameraMightBeSelected),
					ValToUCCondInv(255,cameraMightBeSelected),
					ValToUCCondInv(0,cameraMightBeSelected),
					128)
				: D3DCOLOR_RGBA(
					ValToUCCondInv(0,cameraMightBeSelected),
					ValToUCCondInv(0,cameraMightBeSelected),
					ValToUCCondInv(0,cameraMightBeSelected),
					128);

			CamPathValue cpv = g_Hook_VClient_RenderView.m_CamPath.Eval(curTime);

			double forward[3], right[3], up[3];
			MakeVectors(cpv.Roll, cpv.Pitch, cpv.Yaw, forward, right, up);

			Vector3 vCp(cpv.X, cpv.Y, cpv.Z);
			Vector3 vForward(forward);
			Vector3 vUp(up);
			Vector3 vRight(right);

			double a = sin(cpv.Fov * M_PI / 360.0) * c_CameraRadius;
			double b = a;

			int screenWidth, screenHeight;
			g_VEngineClient->GetScreenSize(screenWidth, screenHeight);

			double aspectRatio = screenWidth ? (double)screenHeight / (double)screenWidth : 1.0;

			b *= aspectRatio;

			Vector3 vLU = vCp +(double)c_CameraRadius * vForward -a * vRight +b * vUp;
			Vector3 vRU = vCp +(double)c_CameraRadius * vForward +a * vRight +b * vUp;
			Vector3 vLD = vCp +(double)c_CameraRadius * vForward -a * vRight -b * vUp;
			Vector3 vRD = vCp +(double)c_CameraRadius * vForward +a * vRight -b * vUp;
			Vector3 vMU = vLU +(vRU -vLU)/2;
			Vector3 vMUU = vMU +(double)c_CameraRadius * vUp;

			AutoSingleLine(vCp, colourCam, vLD, colourCam);

			AutoSingleLine(vCp, colourCam, vRD, colourCam);

			AutoSingleLine(vCp, colourCam, vLU, colourCam);

			AutoSingleLine(vCp, colourCam, vRU, colourCam);

			AutoSingleLine(vLD, colourCam, vRD, colourCam);

			AutoSingleLine(vRD, colourCam, vRU, colourCam);

			AutoSingleLine(vRU, colourCam, vLU, colourCam);

			AutoSingleLine(vLU, colourCam, vLD, colourCam);

			AutoSingleLine(vMU, colourCam, vMUU, colourCamUp);

			AutoSingleLineFlush();
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

	m_Device->SetRenderState(D3DRS_CULLMODE, oldCullMode);
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

bool CCampathDrawer::LockVertexBuffer()
{
	if(m_VertexBuffer)
	{
		if(!SUCCEEDED(m_VertexBuffer->Lock(0, c_VertexBufferVertexCount * sizeof(Vertex), (void **)&m_LockedVertexBuffer, 0)))
		{
			m_LockedVertexBuffer = 0;
			return false;
		}
		return true;
	}

	UnlockVertexBuffer();

	if(!SUCCEEDED(m_Device->CreateVertexBuffer(
		c_VertexBufferVertexCount * sizeof(Vertex),
		D3DUSAGE_WRITEONLY,
		CCampathDrawer_VertexFVF,
		D3DPOOL_DEFAULT,
		&m_VertexBuffer,
		NULL
	)))
	{
		if(m_VertexBuffer) m_VertexBuffer->Release();
		m_VertexBuffer = 0;
		return false;
	}

	if(m_VertexBuffer)
	{
		if(!SUCCEEDED(m_VertexBuffer->Lock(0, c_VertexBufferVertexCount * sizeof(Vertex), (void **)&m_LockedVertexBuffer, 0)))
		{
			m_LockedVertexBuffer = 0;
			return 0;
		}
		return true;
	}

	return false;
}

void CCampathDrawer::UnlockVertexBuffer()
{
	if(m_VertexBuffer && m_LockedVertexBuffer)
	{
		m_VertexBuffer->Unlock();
		m_LockedVertexBuffer = 0;
	}
}

void CCampathDrawer::UnloadVertexBuffer()
{
	if(m_VertexBuffer) { m_VertexBuffer->Release(); m_VertexBuffer = 0; }
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

void CCampathDrawer::RamerDouglasPeucker(TempPoint * start, TempPoint * end, double epsilon)
{
	double dmax = 0;
	TempPoint * index = start;

	for(TempPoint * i = start->nextPt; i && i != end; i = i->nextPt)
	{
		double d = ShortestDistanceToSegment(i, start, end);
		if(d > dmax)
		{
			index = i;
			dmax = d;
		}
	}

	// If max distance is greater than epsilon, recursively simplify
	if ( dmax > epsilon )
	{
		RamerDouglasPeucker(start, index, epsilon);
		RamerDouglasPeucker(index, end, epsilon);
	} else {
		start->nextPt = end;
	}
}

double CCampathDrawer::ShortestDistanceToSegment(TempPoint * pt, TempPoint * start, TempPoint * end)
{
	double ESx = end->y.X - start->y.X;
	double ESy = end->y.Y - start->y.Y;
	double ESz = end->y.Z - start->y.Z;
	double dESdES = ESx*ESx + ESy*ESy + ESz*ESz;
	double t = dESdES ? (
		(pt->y.X-start->y.X)*ESx +(pt->y.Y -start->y.Y)*ESy + (pt->y.Z -start->y.Z)*ESz
	) / dESdES : 0.0;

	if(t <= 0.0)
		return (start->y -pt->y).Length();
	else
	if(1.0 <= t)
		return (pt->y -end->y).Length();

	return (pt->y -(start->y +t*(end->y -start->y))).Length();
}