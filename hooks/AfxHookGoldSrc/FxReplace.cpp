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


// FxReplace //////////////////////////////////////////////////////////////////

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
				texmem[3*i  ] = r;
				texmem[3*i+1] = g;
				texmem[3*i+2] = b;
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


// FxReplace //////////////////////////////////////////////////////////////////

FxReplace::FxReplace() {
	m_Active = false;
	m_Enabled = false;
	m_Blue = true;
	m_Green = true;
	m_Red = true;
}

bool FxReplace::Supported_get() {
	return g_Has_GL_ARB_multitexture;
}

void FxReplace::OnGlBegin() {
	m_Active = g_Has_GL_ARB_multitexture && m_Enabled;
	if(!m_Active) return;

	EnsureTextures();

	glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &m_Old_Gl_Active_Texture_Arb);

	glActiveTextureARB(GL_TEXTURE2_ARB);

	m_Old_Gl_Texture2d = glIsEnabled(GL_TEXTURE_2D);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &m_Old_Gl_TextureBinding2d);
	glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &m_Old_Gl_Texture_Env_Mode);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_Textures[
		(m_Red ? 0x01 : 0)
		| (m_Green ? 0x02 : 0)
		| (m_Blue ? 0x04 : 0)
	]);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void FxReplace::OnGlEnd() {
	if(!m_Active) return;
	m_Active = false;

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, m_Old_Gl_Texture_Env_Mode);
	glBindTexture(GL_TEXTURE_2D, m_Old_Gl_TextureBinding2d);
	if(!m_Old_Gl_Texture2d) glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(m_Old_Gl_Active_Texture_Arb);
}