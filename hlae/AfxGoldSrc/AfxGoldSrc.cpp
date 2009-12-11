#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-12-06T15:58Z by dominik.matrixstorm.com
//
// First changes:
// 2009-12-06T15:58Z by dominik.matrixstorm.com

#include "AfxGoldSrc.h"

// AfxGoldSrc //////////////////////////////////////////////////////////////////

AfxGoldSrc::AfxGoldSrc() {
	m_Running = false;
	m_Settings = gcnew AfxGoldSrcSettings(this);
}

AfxGoldSrc::~AfxGoldSrc() {
}

bool AfxGoldSrc::Launch() {
	return false;
}


// AfxGoldSrcSettings //////////////////////////////////////////////////////////

bool AfxGoldSrcSettings::Running() {
	return m_AfxGoldSrc->Running;
}