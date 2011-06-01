#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-19 dominik.matrixstorm.com
//
// First changes
// 2009-11-19 dominik.matrixstorm.com

#include "ModHide.h"

ModHide g_ModHide;

// ModHide /////////////////////////////////////////////////////////////////////

ModHide::ModHide() {
	m_Enabled = false;
}


bool ModHide::OnGlBegin(GLenum mode) {
	return !m_Enabled;

}



