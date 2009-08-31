#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-08-31 by dominik.matrixstorm.com
//
// First changes:
// 2009-08-31 by dominik.matrixstorm.com

#include <string>

#include <stdio.h>
#include <windows.h>


// BvhExport ///////////////////////////////////////////////////////////////////

class BvhExport
{
public:
	/// <summary> Creates a new BVH file </summary>
	BvhExport(char const * fileName, char const * rootName, float frameTime);

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

	void BeginContent(FILE *pFile ,char const * pAdditonalTag, float frameTime, long &ulTPos);

};


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

