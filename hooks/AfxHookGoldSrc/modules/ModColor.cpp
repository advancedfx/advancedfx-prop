//#include "stdafx.h"

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
	m_Red = -1;
	m_Green = -1;
	m_Blue = -1;
	m_Alpha = -1;
}


void ModColor::OnGlBegin() {
	m_Active = m_Enabled;
	
	if(!m_Active) return;

	glGetFloatv(GL_CURRENT_COLOR, m_Old_Gl_Color);

	glColor4f(
		(0 <= m_Red ? m_Red : m_Old_Gl_Color[0]),
		(0 <= m_Green ? m_Green : m_Old_Gl_Color[1]),
		(0 <= m_Blue ? m_Blue : m_Old_Gl_Color[2]),
		(0 <= m_Alpha ? m_Alpha : m_Old_Gl_Color[3])
	);
}

void ModColor::OnGlEnd() {
	if(!m_Active) return;
	m_Active = false;
	
	glColor4f(m_Old_Gl_Color[0], m_Old_Gl_Color[1], m_Old_Gl_Color[2], m_Old_Gl_Color[3]);	
}


