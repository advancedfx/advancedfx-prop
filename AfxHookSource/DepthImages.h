#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-04-22 by dominik.matrixstorm.com
//
// First changes:
// 2010-04-22 by dominik.matrixstorm.com

#include <d3d9.h>

class DepthImages
{
public:
	DepthImages();
	
	char const * NextDepthImageFileName();

	void OnEndScene(LPDIRECT3DDEVICE9 device);

	bool WriteDepth_get();

	void WriteDepth_set(bool value);

	char const * FilePrefix_get();

	void FilePrefix_set(char const * filePrefix);

private:
	int m_Counter;
	char m_FilePrefix[100];
	bool m_WriteDepth;
};

extern DepthImages g_DepthImages;
