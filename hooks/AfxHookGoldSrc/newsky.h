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
	void DetectAndProcessSky(GLenum mode); // this should be called from glBegin, so our class has a chance to detect the

	void User_ForceReload(bool bEnableCustomSky, bool bOldFormat);
private:
	int _iSkyQuadsCount;
	bool _bOldFormat;
	bool _bWantCustomSky;
	bool _bWantReload;

	GLuint _SkyTextures[6];

	void EnsureGLTextureIndices();
	bool ReloadTexturesFromFile();
};

extern CNewSky g_NewSky; // our global sky singleton

#endif