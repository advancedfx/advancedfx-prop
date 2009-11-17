#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-16 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include <windows.h>
#include <gl\gl.h>


// MirvInfo //////////////////////////////////////////////////////////////////

class MirvInfo {
public:
	MirvInfo();

	void SetRecording(bool value);

	//
	// Properties:

	bool Recording_get() { return m_Recording; }

private:
	bool m_Recording;


};

extern MirvInfo g_MirvInfo;

