// #include <stdafx.h>

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


// BEGIN HLSDK includes
//
// HACK: prevent cldll_int.h from messing the HSPRITE definition,
// HLSDK's HSPRITE --> MDTHACKED_HSPRITE
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <hlsdk/multiplayer/cl_dll/wrect.h>
#include <hlsdk/multiplayer/cl_dll/cl_dll.h>
#include <hlsdk/multiplayer/engine/cdll_int.h>
#include <hlsdk/multiplayer/common/cvardef.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

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

