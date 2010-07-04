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

#include "../GlPrimMods.h"


// ModReplace //////////////////////////////////////////////////////////////////

class ModReplace {
public:
	ModReplace();

	void OnGlBegin(GLenum mode);
	void OnGlEnd();

	//
	// Properties:


	bool Blue_get() { return 0 != m_Replace.GetBlue(); }
	void Blue_set(bool value) { m_Replace.SetBlue(value ? 0 : 0xFF); }

	bool Enabled_get() { return m_Enabled; }
	void Enabled_set(bool value) { m_Enabled = value; }

	bool Green_get() { return 0 != m_Replace.GetGreen(); }
	void Green_set(bool value) { m_Replace.SetGreen(value ? 0 : 0xFF); }

	bool Red_get() { return 0 != m_Replace.GetRed(); }
	void Red_set(bool value) { m_Replace.SetRed(value ? 0 : 0xFF); }

	bool Supported_get() { return m_Replace.IsSupported(); }

private:
	bool m_Active;
	bool m_Enabled;

	GlPrimMod::Replace m_Replace;
};

extern ModReplace g_ModReplace;

