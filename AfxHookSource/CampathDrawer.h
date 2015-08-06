#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-08-03 dominik.matrixstorm.com
//
// First changes:
// 2010-04-22 dominik.matrixstorm.com

#include "SourceInterfaces.h"
#include <shared/CamPath.h>
#include <d3d9.h>

#define CCampathDrawer_VertexFVF D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX0 | D3DFVF_TEXCOORDSIZE2(0) | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(1) | D3DFVF_TEX2 | D3DFVF_TEXCOORDSIZE3(2)

class CCampathDrawer
: public ICamPathChanged
{
public:
	CCampathDrawer();
	~CCampathDrawer();

	void Draw_set(bool value);
	bool Draw_get(void);

	void BeginDevice(IDirect3DDevice9 * device);
	void EndDevice();

	void OnPostRenderAllTools();
	void OnSetMaterial(CONST D3DMATERIAL9* pMaterial);
	void OnSetRenderState(D3DRENDERSTATETYPE State, DWORD Value);
	void OnSetupEngineView();

	virtual void CamPathChanged(CamPath * obj);

private:
	struct Vertex
	{
		FLOAT x, y, z; // Position of current line point
		DWORD diffuse; // Diffuse color of current line point
		FLOAT t0u, t0v; // Extrusion direction from current line point (-1/1), reserved
		FLOAT t1u, t1v, t1w; // Unit vector pointing to previous line point
		FLOAT t2u, t2v, t2w; // Unit vector pointing to next line point
	};

	IDirect3DDevice9 * m_Device;
	bool m_Draw;
	DWORD m_OldColorWriteEnable;
	D3DMATERIAL9 m_OldMaterial;
	IDirect3DPixelShader9 * m_PixelShader;
	char * m_PsoFileName;
	bool m_RebuildDrawing;
	bool m_ReloadPixelShader;
	bool m_ReloadVertexShader;
	IDirect3DVertexShader9 * m_VertexShader;
	char * m_VsoFileName;
	VMatrix m_WorldToScreenMatrix;
	IDirect3DVertexBuffer9 * m_CrossesVertexBuffer;

	void LoadPso(char const * fileName);
	void LoadVso(char const * fileName);

	DWORD * LoadShaderFileInMemory(char const * fileName);

	void LoadShader();
	void LoadPixelShader();
	void LoadVertexShader();

	void RebuildDrawing();
	void UnloadDrawing();
	
	void UnloadShader();
	void UnloadPixelShader();
	void UnloadVertexShader();

};

extern CCampathDrawer g_CampathDrawer;
