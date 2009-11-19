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


// FxReplace ///////////////////////////////////////////////////////////////////

class FxReplace {
public:
	FxReplace();

	void OnGlBegin(GLenum mode);
	void OnGlEnd();

	//
	// Properties:

	GLfloat Blue_get() { return m_Blue; }

	// -1 = disabled or [0-1] value
	void Blue_set(GLfloat value) { m_Blue = value; }
	bool Enabled_get() { return m_Enabled; }
	void Enabled_set(bool value) { m_Enabled = value; }

	GLfloat Green_get() { return m_Green; }

	// -1 = disabled or [0-1] value
	void Green_set(GLfloat value) { m_Green = value; }

	GLfloat Red_get() { return m_Red; }

	// -1 = disabled or [0-1] value
	void Red_set(GLfloat value) { m_Red = value; }


	bool Supported_get();

private:
	bool m_Active;
	GLfloat m_Blue;
	bool m_Enabled;
	GLfloat m_Green;
	GLint m_Old_Gl_Active_Texture_Arb;
	GLfloat m_Old_Gl_Color[4];
	GLboolean m_Old_Gl_ColorMask[4];
	GLboolean m_Old_Gl_Texture2d;
	GLint m_Old_Gl_Texture_Env_Mode;
	GLint m_Old_Gl_TextureBinding2d;
	GLfloat m_Red;

};

extern FxReplace g_FxReplace;

