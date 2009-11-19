//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-19 dominik.matrixstorm.com
//
// First changes
// 2009-11-19 dominik.matrixstorm.com

#include "FxHide.h"

FxHide g_FxHide;

// FxHide /////////////////////////////////////////////////////////////////////

FxHide::FxHide() {
	m_Enabled = false;
}


bool FxHide::OnGlBegin() {
	return !m_Enabled;

}



