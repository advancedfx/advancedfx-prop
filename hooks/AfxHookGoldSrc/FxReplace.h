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


// FxReplace //////////////////////////////////////////////////////////////////

class FxReplace {
public:
	FxReplace();

	void OnGlBegin();
	void OnGlEnd();

	//
	// Properties:


	bool Blue_get() { return m_Blue; }
	void Blue_set(bool value) { m_Blue = value; }

	bool Enabled_get() { return m_Enabled; }
	void Enabled_set(bool value) { m_Enabled = value; }

	bool Green_get() { return m_Green; }
	void Green_set(bool value) { m_Green = value; }

	bool Red_get() { return m_Red; }
	void Red_set(bool value) { m_Red = value; }

	bool Supported_get();

private:
	bool m_Active;
	bool m_Enabled;
	GLint m_Old_Gl_Active_Texture_Arb;
	GLboolean m_Old_Gl_Texture2d;
	GLint m_Old_Gl_Texture_Env_Mode;
	GLint m_Old_Gl_TextureBinding2d;
	bool m_Blue;
	bool m_Green;
	bool m_Red;

};

extern FxReplace g_FxReplace;

