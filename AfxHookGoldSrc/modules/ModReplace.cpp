#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-14 dominik.matrixstorm.com
//
// First changes
// 2009-11-14 dominik.matrixstorm.com

#include "ModReplace.h"


ModReplace g_ModReplace;


// ModReplace //////////////////////////////////////////////////////////////////

ModReplace::ModReplace() {
	m_Active = false;
	m_Enabled = false;
}

void ModReplace::OnGlBegin(GLenum mode) {
	m_Active = m_Enabled;
	if(!m_Active) return;

	m_Replace.OnGlBegin(mode);
}

void ModReplace::OnGlEnd() {
	if(!m_Active) return;
	m_Active = false;

	m_Replace.OnGlEnd();
}