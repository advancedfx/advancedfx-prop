#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-16 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include <windows.h>
#include <gl\gl.h>


// FxColor /////////////////////////////////////////////////////////////////////

class FxColor {
public:
	FxColor();

	void OnGlBegin(GLenum mode);
	void OnGlEnd();

	//
	// Properties:

	bool Enabled_get() { return m_Enabled; }
	void Enabled_set(bool value) { m_Enabled = value; }

	GLfloat Red_get() { return m_Red; }
	void Red_set(GLfloat value) { m_Red = value; }

	GLfloat Green_get() { return m_Green; }
	void Green_set(GLfloat value) { m_Green = value; }

	GLfloat Blue_get() { return m_Blue; }
	void Blue_set(GLfloat value) { m_Blue = value; }

	GLfloat Alpha_get() { return m_Alpha; }
	void Alpha_set(GLfloat value) { m_Alpha = value; }

private:
	bool m_Active;
	bool m_Enabled;
	GLfloat m_Old_Gl_Color[4];

	GLfloat m_Red;
	GLfloat m_Green;
	GLfloat m_Blue;
	GLfloat m_Alpha;
};

extern FxColor g_FxColor;

