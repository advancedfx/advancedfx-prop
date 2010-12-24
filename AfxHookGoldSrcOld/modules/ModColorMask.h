#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-06-29 dominik.matrixstorm.com
//
// First changes
// 2009-11-21 dominik.matrixstorm.com

#include <windows.h>
#include <gl\gl.h>

#include "../GlPrimMods.h"

// ModColorMask /////////////////////////////////////////////////////////////////

class ModColorMask {
public:
	ModColorMask();

	void OnGlBegin(GLenum mode);
	void OnGlEnd();

	//
	// Properties:

	bool Enabled_get() { return m_Enabled; }
	void Enabled_set(bool value) { m_Enabled = value; }

	// x < 0: disable (block), x == 0: no change, 0 < x: enable (pass)
	int Red_get() { return GlPrimMod::ToInt(m_ColorMask.GetRed()); }
	void Red_set(int value) { m_ColorMask.SetRed(GlPrimMod::FromInt(value)); }

	int Green_get() { return GlPrimMod::ToInt(m_ColorMask.GetGreen()); }
	void Green_set(int value) { m_ColorMask.SetGreen(GlPrimMod::FromInt(value)); }

	int Blue_get() { return GlPrimMod::ToInt(m_ColorMask.GetBlue()); }
	void Blue_set(int value) { m_ColorMask.SetBlue(GlPrimMod::FromInt(value)); }

	int Alpha_get() { return GlPrimMod::ToInt(m_ColorMask.GetAlpha()); }
	void Alpha_set(int value) { m_ColorMask.SetAlpha(GlPrimMod::FromInt(value)); }

private:
	bool m_Active;
	bool m_Enabled;

	GlPrimMod::ColorMask m_ColorMask;
};

extern ModColorMask g_ModColorMask;

