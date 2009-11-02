#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-09-11 by dominik.matrixstorm.com
//
// First changes:
// 2009-08-31 by dominik.matrixstorm.com

#include <string>

#include <hooks/shared/bvhexport.h>

#include <stdio.h>
#include <windows.h>


// CCamExport //////////////////////////////////////////////////////////////////

class CCamExport {
public:
	CCamExport();
	~CCamExport();

	void BeginFileLeft(char const * filePrefix, unsigned int take, float frameTime);
	void BeginFileMain(char const * filePrefix, unsigned int take, float frameTime);
	void BeginFileRight(char const * filePrefix, unsigned int take, float frameTime);

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

	BvhExport * BeginFile(char const * filePrefix, unsigned int take, char const * fileSuffix, char const * rootName, float frameTime);

	void EndFile(BvhExport * & bvhFile);

	std::string MakeFileName(char const * filePrefix, unsigned int take, char const * fileSuffix);

	void WriteFrame(BvhExport * bvhFile, float Xposition, float Yposition, float Zposition, float Zrotation, float Xrotation, float Yrotation);
};

extern CCamExport g_CamExport;

