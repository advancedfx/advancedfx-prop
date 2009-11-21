//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-16 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "FxColorMask.h"

FxColorMask g_FxColorMask;

// FxColorMask /////////////////////////////////////////////////////////////////

FxColorMask::FxColorMask() {
	m_Active = false;
	m_Enabled = false;
	m_Red = 0;
	m_Green = 0;
	m_Blue = 0;
	m_Alpha = 0;
}


void FxColorMask::OnGlBegin() {
	m_Active = m_Enabled;
	
	if(!m_Active) return;

	glGetBooleanv(GL_COLOR_WRITEMASK, m_Old_Gl_ColorMask);

	glColorMask(
		0 == m_Red ? m_Old_Gl_ColorMask[0] : (0 <= m_Red ? GL_TRUE: GL_FALSE),
		0 == m_Green ? m_Old_Gl_ColorMask[1] : (0 <= m_Green ? GL_TRUE: GL_FALSE),
		0 == m_Blue ? m_Old_Gl_ColorMask[2] : (0 <= m_Blue ? GL_TRUE: GL_FALSE),
		0 == m_Alpha ? m_Old_Gl_ColorMask[3] : (0 <= m_Alpha ? GL_TRUE: GL_FALSE)
	);
}

void FxColorMask::OnGlEnd() {
	if(!m_Active) return;
	m_Active = false;
	
	glColorMask(m_Old_Gl_ColorMask[0], m_Old_Gl_ColorMask[1], m_Old_Gl_ColorMask[2], m_Old_Gl_ColorMask[3]);
}


