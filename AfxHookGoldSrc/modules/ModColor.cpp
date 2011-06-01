#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-16 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "ModColor.h"

ModColor g_ModColor;

// ModColor /////////////////////////////////////////////////////////////////////

ModColor::ModColor() {
	m_Active = false;
	m_Enabled = false;
}


void ModColor::OnGlBegin(GLenum mode) {
	m_Active = m_Enabled;
	
	if(!m_Active) return;

	m_Color.OnGlBegin(mode);
}

void ModColor::OnGlEnd() {
	if(!m_Active) return;
	m_Active = false;
	
	m_Color.OnGlEnd();
}


