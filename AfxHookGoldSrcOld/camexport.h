#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2010-05-20 by dominik.matrixstorm.com
//
// First changes:
// 2009-08-31 by dominik.matrixstorm.com


#include <shared/bvhexport.h>


// CCamExport //////////////////////////////////////////////////////////////////

class CCamExport {
public:
	CCamExport();
	~CCamExport();

	void BeginFileLeft(wchar_t const * folder, float frameTime);
	void BeginFileMain(wchar_t const * folder, float frameTime);
	void BeginFileRight(wchar_t const * folder, float frameTime);

	void EndFileLeft();
	void EndFileMain();
	void EndFileRight();
	void EndFiles();

	bool HasFileLeft();
	bool HasFileMain();
	bool HasFileRight();

	void WriteLeftFrame(float Xposition, float Yposition, float Zposition, float Zrotation, float Xrotation, float Yrotation);
	void WriteMainFrame(float Xposition, float Yposition, float Zposition, float Zrotation, float Xrotation, float Yrotation);
	void WriteRightFrame(float Xposition, float Yposition, float Zposition, float Zrotation, float Xrotation, float Yrotation);

private:
	BvhExport * m_BvhFileLeft;
	BvhExport * m_BvhFileMain;
	BvhExport * m_BvhFileRight;
	unsigned int m_FrameCount;

	BvhExport * BeginFile(wchar_t const * folder, wchar_t const * fileName, char const * rootName, float frameTime);

	void EndFile(BvhExport * & bvhFile);

	void WriteFrame(BvhExport * bvhFile, float Xposition, float Yposition, float Zposition, float Zrotation, float Xrotation, float Yrotation);
};

extern CCamExport g_CamExport;

