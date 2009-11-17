//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-16 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "MirvInfo.h"


MirvInfo g_MirvInfo;


// MirvInfo //////////////////////////////////////////////////////////////////

MirvInfo::MirvInfo() {
	m_Recording = false;
}

void MirvInfo::SetRecording(bool value) {
	m_Recording = value;
}