#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-21 dominik.matrixstorm.com
//
// First changes
// 2009-11-21 dominik.matrixstorm.com

#include <windows.h>
#include <gl\gl.h>


// ModColorMask /////////////////////////////////////////////////////////////////

class ModColorMask {
public:
	ModColorMask();

	void OnGlBegin();
	void OnGlEnd();

	//
	// Properties:

	bool Enabled_get() { return m_Enabled; }
	void Enabled_set(bool value) { m_Enabled = value; }

	// x < 0: disable (block), x == 0: no change, 0 < x: enable (pass)
	int Red_get() { return m_Red; }
	void Red_set(int value) { m_Red = value; }

	int Green_get() { return m_Green; }
	void Green_set(int value) { m_Green = value; }

	int Blue_get() { return m_Blue; }
	void Blue_set(int value) { m_Blue = value; }

	int Alpha_get() { return m_Alpha; }
	void Alpha_set(int value) { m_Alpha = value; }

private:
	bool m_Active;
	bool m_Enabled;
	GLboolean m_Old_Gl_ColorMask[4];

	int m_Red;
	int m_Green;
	int m_Blue;
	int m_Alpha;
};

extern ModColorMask g_ModColorMask;

