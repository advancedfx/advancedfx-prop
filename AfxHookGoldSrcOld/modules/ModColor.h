#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-16 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com


#include "GlPrimMods.h"


// ModColor /////////////////////////////////////////////////////////////////////

class ModColor {
public:
	ModColor();

	void OnGlBegin(GLenum mode);
	void OnGlEnd();

	//
	// Properties:

	bool Enabled_get() { return m_Enabled; }
	void Enabled_set(bool value) { m_Enabled = value; }

	// x < 0: no change, [0.0 - 1.0]: enable (set color channel value)
	GLfloat Red_get() { return m_Color.GetRed(); }
	void Red_set(GLfloat value) { m_Color.SetRed(value); }

	GLfloat Green_get() { return m_Color.GetGreen(); }
	void Green_set(GLfloat value) { m_Color.SetGreen(value); }

	GLfloat Blue_get() { return m_Color.GetBlue(); }
	void Blue_set(GLfloat value) { m_Color.SetBlue(value); }

	GLfloat Alpha_get() { return m_Color.GetAlpha(); }
	void Alpha_set(GLfloat value) { m_Color.SetAlpha(value); }

private:
	bool m_Active;
	bool m_Enabled;

	GlPrimMod::Color m_Color;

};

extern ModColor g_ModColor;

