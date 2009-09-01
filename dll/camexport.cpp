// #include <stdafx.h>

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-08-31 by dominik.matrixstorm.com
//
// First changes:
// 2009-08-31 by dominik.matrixstorm.com

#include "camexport.h"

#include <sstream>

using namespace std;


// BEGIN HLSDK includes
//
// HACK: prevent cldll_int.h from messing the HSPRITE definition,
// HLSDK's HSPRITE --> MDTHACKED_HSPRITE
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <wrect.h>
#include <cl_dll.h>
#include <cdll_int.h>
#include <cvardef.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include "filming.h"

#include "cmdregister.h"

extern cl_enginefuncs_s *pEngfuncs;
extern Filming g_Filming;

CCamExport g_CamExport;


// BvhExport //////////////////////////////////////////////////////////////////

BvhExport::BvhExport(char const * fileName, char const * rootName, float frameTime)
{
	m_FrameCount = 0;
	m_pMotionFile = fopen(fileName, "wb");

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

	//if (movie_fpscap->value) flTime = max(flTime, MIN_FRAME_DURATION);
	_snprintf(szTmp, sizeof(szTmp) - 1,"Frame Time: %f\n", frameTime);
	fputs(szTmp,pFile);
}

void BvhExport::WriteFrame(float Xposition, float Yposition, float Zposition, float Zrotation, float Xrotation, float Yrotation) {
	char pszT[249];

	_snprintf(pszT, sizeof(pszT)-1, "%f %f %f %f %f %f\n", Xposition, Yposition, Zposition, Zrotation, Xrotation, Yrotation);

	if (m_pMotionFile) fputs(pszT, m_pMotionFile);

	m_FrameCount++;
}


// CCamExport //////////////////////////////////////////////////////////////////

CCamExport::CCamExport() {
	m_BvhFileLeft = NULL;
	m_BvhFileMain = NULL;
	m_BvhFileRight = NULL;
}

CCamExport::~CCamExport() {
	EndFiles();
}

bool CCamExport::HasFileLeft() {
	return NULL != m_BvhFileLeft;
}

bool CCamExport::HasFileMain() {
	return NULL != m_BvhFileMain;
}

bool CCamExport::HasFileRight() {
	return NULL != m_BvhFileRight;
}

void CCamExport::EndFile(BvhExport * & bvhFile) {
	if(NULL != bvhFile) delete bvhFile;
	bvhFile = NULL;
}

BvhExport * CCamExport::BeginFile(char const * filePrefix, unsigned int take, char const * fileSuffix, char const * rootName, float frameTime) {
	BvhExport * bvhFile = new BvhExport(
		MakeFileName(filePrefix, take, fileSuffix).c_str(),
		rootName,
		frameTime
	);

	return bvhFile;
}

void CCamExport::BeginFileLeft(char const * filePrefix, unsigned int take, float frameTime) {
	m_BvhFileLeft = BeginFile(filePrefix, take, "_motion_left.bvh", "MdtCamLeft", frameTime);
}

void CCamExport::BeginFileMain(char const * filePrefix, unsigned int take, float frameTime) {
	m_BvhFileMain = BeginFile(filePrefix, take, "_motion.bvh", "MdtCam", frameTime);
}

void CCamExport::BeginFileRight(char const * filePrefix, unsigned int take, float frameTime) {
	m_BvhFileRight = BeginFile(filePrefix, take, "_motion_right.bvh", "MdtCamRight", frameTime);
}


void CCamExport::EndFileLeft() {
	EndFile(m_BvhFileLeft);
}

void CCamExport::EndFileMain() {
	EndFile(m_BvhFileMain);
}

void CCamExport::EndFileRight() {
	EndFile(m_BvhFileRight);
}


void CCamExport::EndFiles() {
	EndFileLeft();
	EndFileMain();
	EndFileRight();
}

std::string CCamExport::MakeFileName(char const * filePrefix, unsigned int take, char const * fileSuffix) {
	ostringstream os(ostringstream::out);

	os.fill('0');
	os << filePrefix << '_' ;
	os.width(2);
	os << std::right << take;
	os << fileSuffix;

	return os.str();
}


void CCamExport::WriteFrame(BvhExport * bvhFile, float Xposition, float Yposition, float Zposition, float Zrotation, float Xrotation, float Yrotation) {
	if(bvhFile)
		bvhFile->WriteFrame(Xposition, Yposition, Zposition, Zrotation, Xrotation, Yrotation);
}

void CCamExport::WriteLeftFrame(float Xposition, float Yposition, float Zposition, float Zrotation, float Xrotation, float Yrotation) {
	WriteFrame(m_BvhFileLeft, Xposition, Yposition, Zposition, Zrotation, Xrotation, Yrotation);
}

void CCamExport::WriteMainFrame(float Xposition, float Yposition, float Zposition, float Zrotation, float Xrotation, float Yrotation) {
	WriteFrame(m_BvhFileMain, Xposition, Yposition, Zposition, Zrotation, Xrotation, Yrotation);
}

void CCamExport::WriteRightFrame(float Xposition, float Yposition, float Zposition, float Zrotation, float Xrotation, float Yrotation) {
	WriteFrame(m_BvhFileRight, Xposition, Yposition, Zposition, Zrotation, Xrotation, Yrotation);
}



// MdtCam..

