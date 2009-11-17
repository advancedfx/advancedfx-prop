#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-14 dominik.matrixstorm.com
//
// First changes
// 2009-11-14 dominik.matrixstorm.com

#include <windows.h>
#include <gl\gl.h>


// FxRgbMask //////////////////////////////////////////////////////////////////

class FxRgbMask {
public:
	FxRgbMask();

	bool Supported_get();

	void OnGlBegin(GLenum mode);
	void OnGlEnd();

	//
	// Properties:

	bool Enabled_get() { return m_Enabled; }
	void Enabled_set(bool value) { m_Enabled = value; }

	int OpRed_get() { return m_OpRed; }
	void OpRed_set(int value) { m_OpRed = value; }

	int OpGreen_get() { return m_OpGreen; }
	void OpGreen_set(int value) { m_OpGreen = value; }

	int OpBlue_get() { return m_OpBlue; }
	void OpBlue_set(int value) { m_OpBlue = value; }

private:
	bool m_Active;
	bool m_Enabled;
	GLint m_Old_Gl_Active_Texture_Arb;
	GLfloat m_Old_Gl_Color[4];
	GLboolean m_Old_Gl_ColorMask[4];
	GLboolean m_Old_Gl_Texture2d;
	GLint m_Old_Gl_Texture_Env_Mode;
	GLint m_Old_Gl_TextureBinding2d;
	int m_OpBlue;
	int m_OpGreen;
	int m_OpRed;

};

extern FxRgbMask g_FxRgbMask;

