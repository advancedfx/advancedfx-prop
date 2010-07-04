#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-11-01 by dominik.matrixstorm.com
//
// First changes:
// 2009-08-31 by dominik.matrixstorm.com

#include "bvhexport.h"

#include <sstream>

using namespace std;


// BvhExport //////////////////////////////////////////////////////////////////

BvhExport::BvhExport(wchar_t const * fileName, char const * rootName, float frameTime)
{
	m_FrameCount = 0;
	m_pMotionFile = _wfopen(fileName, L"wb");

	if (m_pMotionFile != NULL)
		BeginContent(m_pMotionFile, rootName, frameTime, m_lMotionTPos);
}

BvhExport::~BvhExport()
{
	char pTmp[100];

	if (m_pMotionFile) {
		fseek(m_pMotionFile, m_lMotionTPos, SEEK_SET);
		_snprintf(pTmp, sizeof(pTmp)-1, "Frames: %11i", m_FrameCount);
		fputs(pTmp, m_pMotionFile);
		fclose(m_pMotionFile);
		m_pMotionFile = NULL;
	}
}

void BvhExport::BeginContent(FILE *pFile, char const * pRootName, float frameTime, long &ulTPos)
{
	char szTmp[196];

	fputs("HIERARCHY\n",pFile);

	fputs("ROOT ",pFile);
	fputs(pRootName,pFile);
	fputs("\n{\n\tOFFSET 0.00 0.00 0.00\n\tCHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n\tEnd Site\n\t{\n\t\tOFFSET 0.00 0.00 -1.00\n\t}\n}\n",pFile);

	fputs("MOTION\n",pFile);
	ulTPos = ftell(pFile);
	fputs("Frames: 0123456789A\n",pFile);

	_snprintf(szTmp, sizeof(szTmp) - 1,"Frame Time: %f\n", frameTime);
	fputs(szTmp,pFile);
}

void BvhExport::WriteFrame(float Xposition, float Yposition, float Zposition, float Zrotation, float Xrotation, float Yrotation) {
	char pszT[249];

	_snprintf(pszT, sizeof(pszT)-1, "%f %f %f %f %f %f\n", Xposition, Yposition, Zposition, Zrotation, Xrotation, Yrotation);

	if (m_pMotionFile) fputs(pszT, m_pMotionFile);

	m_FrameCount++;
}



