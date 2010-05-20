#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-04-22 by dominik.matrixstorm.com
//
// First changes:
// 2010-04-22 by dominik.matrixstorm.com

#include "DepthImages.h"

#include <stdio.h>
#include <string.h>

#include <shared/RawOutput.h>

DepthImages g_DepthImages;


DepthImages::DepthImages()
{
	m_WriteDepth = false;

	FilePrefix_set("unnamed");
}

char const * DepthImages::NextDepthImageFileName()
{
	static char fileName[100+1+33+10+1];

	sprintf_s(fileName, "%s_%05d_depth.bmp", m_FilePrefix, m_Counter);

	m_Counter++;

	return fileName;
}

void DepthImages::OnEndScene(LPDIRECT3DDEVICE9 device)
{
	if(!m_WriteDepth)
		return;

	LPDIRECT3DDEVICE9 dev = device;
	LPDIRECT3DSURFACE9 vidSrf = NULL;
	LPDIRECT3DSURFACE9 memSrf = NULL;

	D3DSURFACE_DESC desc;
	D3DLOCKED_RECT lockedRect;
	D3DFORMAT targetFormat;

	unsigned char depthShr;
	unsigned __int32 depthMask;
	unsigned char depthBytes;

	bool bOk;

	if(bOk = SUCCEEDED(dev->GetDepthStencilSurface(&vidSrf))
		&& SUCCEEDED(vidSrf->GetDesc(&desc)))
	{
/*switch(desc.Format)
		{
		case D3DFMT_D16_LOCKABLE:
			depthBytes = 2;
			depthMask = 0xFFFF;
			depthShr = 0;
			targetFormat = D3DFMT_G16R16;
			break;
		case D3DFMT_D32:
			depthMask = 0xFFFFFFFF;
			depthBytes = 4;
			depthShr = 0;
			targetFormat = D3DFMT_A8R8G8B8;
			break;
		case D3DFMT_D15S1:
			depthMask = 0xFFFE;
			depthBytes = 2;
			depthShr = 0;
			targetFormat = D3DFMT_G16R16;
			break;
		case D3DFMT_D24S8:
			depthMask = 0xFFFFFF00;
			depthBytes = 4;
			targetFormat = D3DFMT_A8R8G8B8;
			break;
		case D3DFMT_D24X8:
			depthMask = 0xFFFFFF00;
			depthBytes = 4;
			targetFormat = D3DFMT_A8R8G8B8;
			break;
		case D3DFMT_D24X4S4:
			depthMask = 0xFFFFFF00;
			depthBytes = 4;
			targetFormat = D3DFMT_A8R8G8B8;
			break;
		case D3DFMT_D32_LOCKABLE:
			depthMask = 0xFFFFFFFF;
			depthBytes = 4;
			targetFormat = D3DFMT_A8R8G8B8;
			break;
		case D3DFMT_D16:
			depthMask = 0xFFFF;
			depthBytes = 2;
			targetFormat = D3DFMT_G16R16;
			break;
		default:
			bOk = false;
		}
*/
	}

	if(bOk)
	{
		bOk =
			SUCCEEDED(dev->CreateOffscreenPlainSurface(
			desc.Width, desc.Height, D3DFMT_D16_LOCKABLE, D3DPOOL_DEFAULT, &memSrf, NULL))
			&& (MessageBox(0, "1", "!!!", MB_OK), SUCCEEDED(dev->GetRenderTargetData(vidSrf, memSrf)))
			&& (MessageBox(0, "2", "!!!", MB_OK), SUCCEEDED(memSrf->LockRect(&lockedRect, NULL, D3DLOCK_READONLY)))
		;
	}

	if(bOk)
	{
		///WriteRawBitmap((unsigned char const *)lockedRect.pBits, NextDepthImageFileName(), desc.Width, desc.Height, 24, CalcPitch(desc.Width, 3, 1));
		memSrf->UnlockRect();
	}

	if(memSrf) memSrf->Release();
	if(vidSrf) vidSrf->Release();
}

bool DepthImages::WriteDepth_get()
{
	return m_WriteDepth;
}

void DepthImages::WriteDepth_set(bool value)
{
	m_WriteDepth = value;
}

char const * DepthImages::FilePrefix_get()
{
	return m_FilePrefix;
}

void DepthImages::FilePrefix_set(char const * filePrefix)
{
	strncpy_s(m_FilePrefix, filePrefix, 100);

	m_Counter = 0;
}
