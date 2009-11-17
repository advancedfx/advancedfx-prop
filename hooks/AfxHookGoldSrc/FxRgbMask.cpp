//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-14 dominik.matrixstorm.com
//
// First changes
// 2009-11-14 dominik.matrixstorm.com

#include "FxRgbMask.h"

#include "mirv_glext.h"


FxRgbMask g_FxRgbMask;


// FxRgbMask //////////////////////////////////////////////////////////////////

GLuint g_Textures[8];


void EnsureTextures() {
	static bool firstRun = true;

	if(firstRun) {
		firstRun = false;

		unsigned char texmem[48];
		GLint oldtex;

		glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldtex);
		glGenTextures(8, g_Textures);

		for(unsigned char t=0; t<8; t++) {
			unsigned char r = 0x01 & t ? 0xFF: 0x00;
			unsigned char g = 0x02 & t ? 0xFF: 0x00;
			unsigned char b = 0x04 & t ? 0xFF: 0x00;

			for(int i=0; i<16; i++) {
				texmem[i  ] = r;
				texmem[i+1] = g;
				texmem[i+2] = b;
			}

			glBindTexture(GL_TEXTURE_2D, g_Textures[t]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_UNSIGNED_BYTE, texmem);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		}

		glBindTexture(GL_TEXTURE_2D, oldtex);
	}

}


// FxRgbMask //////////////////////////////////////////////////////////////////

FxRgbMask::FxRgbMask() {
	m_Active = false;
	m_Enabled = false;
	m_OpBlue = 1;
	m_OpGreen = 1;
	m_OpRed = 1;
}

bool FxRgbMask::Supported_get() {
	return g_Has_GL_ARB_multitexture;
}

void FxRgbMask::OnGlBegin(GLenum mode) {
	m_Active = g_Has_GL_ARB_multitexture && m_Enabled;
	if(!m_Active) return;

	EnsureTextures();

	glGetFloatv(GL_CURRENT_COLOR, m_Old_Gl_Color);
	glGetBooleanv(GL_COLOR_WRITEMASK, m_Old_Gl_ColorMask);
	glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &m_Old_Gl_Active_Texture_Arb);

	glColorMask(
		m_OpRed ? GL_TRUE: GL_FALSE,
		m_OpGreen ? GL_TRUE: GL_FALSE,
		m_OpBlue ? GL_TRUE: GL_FALSE,
		GL_TRUE
	);

	glActiveTextureARB(GL_TEXTURE1_ARB);

	m_Old_Gl_Texture2d = glIsEnabled(GL_TEXTURE_2D);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_Old_Gl_TextureBinding2d);
	glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &m_Old_Gl_Texture_Env_Mode);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_Textures[
		(0 < m_OpRed ? 0x01 : 0)
		| (0 < m_OpGreen ? 0x02 : 0)
		| (0 < m_OpBlue ? 0x04 : 0)
	]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glColor4f(
		(0 < m_OpRed ? 1 : 0),
		(0 < m_OpGreen ? 1 : 0),
		(0 < m_OpBlue ? 1 : 0),
		m_Old_Gl_Color[3]
	);
}

void FxRgbMask::OnGlEnd() {
	if(!m_Active) return;
	m_Active = false;

	glColor4f(m_Old_Gl_Color[0], m_Old_Gl_Color[1], m_Old_Gl_Color[2], m_Old_Gl_Color[3]);	

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_Old_Gl_Texture_Env_Mode);
	glBindTexture(GL_TEXTURE_2D, m_Old_Gl_TextureBinding2d);
	if(!m_Old_Gl_Texture2d) glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(m_Old_Gl_Active_Texture_Arb);

	glColorMask(m_Old_Gl_ColorMask[0], m_Old_Gl_ColorMask[1], m_Old_Gl_ColorMask[2], m_Old_Gl_ColorMask[3]);
}