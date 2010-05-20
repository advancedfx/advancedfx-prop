#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-05-20 by dominik.matrixstorm.com
//
// First changes:
// 2009-08-31 by dominik.matrixstorm.com

//#include <string>

#include <stdio.h>
#include <windows.h>


// BvhExport ///////////////////////////////////////////////////////////////////

class BvhExport
{
public:
	/// <summary> Creates a new BVH file </summary>
	BvhExport(wchar_t const * fileName, char const * rootName, float frameTime);

	/// </summary> Closes the BVH file </summary>
	~BvhExport();

	void WriteFrame(
		float Xposition, float Yposition, float Zposition,
		float Zrotation, float Xrotation, float Yrotation
	);

private:
	unsigned int m_FrameCount;
	FILE * m_pMotionFile;
	long m_lMotionTPos;

	void BeginContent(FILE *pFile ,char const * pRootName, float frameTime, long &ulTPos);

};


