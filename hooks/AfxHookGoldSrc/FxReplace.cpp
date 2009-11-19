//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-14 dominik.matrixstorm.com
//
// First changes
// 2009-11-14 dominik.matrixstorm.com

#include "FxReplace.h"

#include "mirv_glext.h"


FxReplace g_FxReplace;

GLuint g_WhiteTexture;

void EnsureTextures() {
	static bool firstRun = true;

	if(firstRun) {
		firstRun = false;

		unsigned char texmem[48];
		GLint oldtex;

		glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldtex);
		glGenTextures(1, &g_WhiteTexture);

		memset(texmem, 0xFF, 48);

		glBindTexture(GL_TEXTURE_2D, g_WhiteTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_UNSIGNED_BYTE, texmem);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

		glBindTexture(GL_TEXTURE_2D, oldtex);
	}

}


// FxReplace //////////////////////////////////////////////////////////////////

FxReplace::FxReplace() {
	m_Active = false;
	m_Enabled = false;
	m_Red = 1;
	m_Green = 1;
	m_Blue = 1;
}

bool FxReplace::Supported_get() {
	return g_Has_GL_ARB_multitexture;
}

void FxReplace::OnGlBegin(GLenum mode) {
	m_Active = g_Has_GL_ARB_multitexture && m_Enabled;
	if(!m_Active) return;

	EnsureTextures();

	glGetFloatv(GL_CURRENT_COLOR, m_Old_Gl_Color);
	glGetBooleanv(GL_COLOR_WRITEMASK, m_Old_Gl_ColorMask);
	glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &m_Old_Gl_Active_Texture_Arb);

	bool bRed = 0.0f <= m_Red && m_Red <= 1.0f;
	bool bGreen = 0.0f <= m_Green && m_Green <= 1.0f;
	bool bBlue = 0.0f <= m_Blue && m_Blue <= 1.0f;

	glColorMask(
		bRed ? GL_TRUE: GL_FALSE,
		bGreen ? GL_TRUE: GL_FALSE,
		bBlue ? GL_TRUE: GL_FALSE,
		m_Old_Gl_ColorMask[3]
	);

	glActiveTextureARB(GL_TEXTURE1_ARB);

	m_Old_Gl_Texture2d = glIsEnabled(GL_TEXTURE_2D);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_Old_Gl_TextureBinding2d);
	glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &m_Old_Gl_Texture_Env_Mode);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_WhiteTexture);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glColor4f(
		(bRed ? m_Red : 0),
		(bGreen ? m_Green : 0),
		(bBlue ? m_Blue : 0),
		m_Old_Gl_Color[3]
	);
}

void FxReplace::OnGlEnd() {
	if(!m_Active) return;
	m_Active = false;

	glColor4f(m_Old_Gl_Color[0], m_Old_Gl_Color[1], m_Old_Gl_Color[2], m_Old_Gl_Color[3]);	

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_Old_Gl_Texture_Env_Mode);
	glBindTexture(GL_TEXTURE_2D, m_Old_Gl_TextureBinding2d);
	if(!m_Old_Gl_Texture2d) glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(m_Old_Gl_Active_Texture_Arb);

	glColorMask(m_Old_Gl_ColorMask[0], m_Old_Gl_ColorMask[1], m_Old_Gl_ColorMask[2], m_Old_Gl_ColorMask[3]);
}