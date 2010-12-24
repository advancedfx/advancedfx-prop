/*
Half-Life Advanced Effects project
*/

#ifndef MDT_NEWSKY_H
#define MDT_NEWSKY_H

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

class CNewSky
{
public:
	CNewSky();

	void User_ForceReload(bool bEnableCustomSky, bool bOldFormat);

private:
	int _iSkyQuadsCount;

	GLuint _SkyTextures[6];

	void EnsureGLTextureIndices();
	bool ReloadTexturesFromFile(bool oldFormat);
};

extern CNewSky g_NewSky; // our global sky singleton

#endif