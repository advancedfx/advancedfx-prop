#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2009-09-11 by dominik.matrixstorm.com
//
// First changes:
// 2009-08-31 by dominik.matrixstorm.com

#include "camexport.h"

#include <sstream>

using namespace std;


#include <hlsdk.h>

#include "filming.h"

#include "cmdregister.h"

extern cl_enginefuncs_s *pEngfuncs;
extern Filming g_Filming;

CCamExport g_CamExport;


REGISTER_CMD_FUNC(camexport_timeinfo)
{
	pEngfuncs->Con_Printf("Current (filming)ClientTime: %f\n", g_Filming.GetDebugClientTime());
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

BvhExport * CCamExport::BeginFile(char const * folder, char const * fileName, char const * rootName, float frameTime) {
	
	std::string strFileName(folder);
	if(folder && 0 < strlen(folder))
		strFileName.append("\\");
	strFileName.append(fileName);

	BvhExport * bvhFile = new BvhExport(
		strFileName.c_str(),
		rootName,
		frameTime
	);

	return bvhFile;
}

void CCamExport::BeginFileLeft(char const * folder, float frameTime) {
	m_BvhFileLeft = BeginFile(folder, "motion_left.bvh", "MdtCamLeft", frameTime);
}

void CCamExport::BeginFileMain(char const * folder, float frameTime) {
	m_BvhFileMain = BeginFile(folder, "motion.bvh", "MdtCam", frameTime);
}

void CCamExport::BeginFileRight(char const * folder, float frameTime) {
	m_BvhFileRight = BeginFile(folder, "motion_right.bvh", "MdtCamRight", frameTime);
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

