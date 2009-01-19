/*
Half-Life Advanced Effects project
*/

#include "newsky.h"

#include <wrect.h>
#include <cl_dll.h>
#include <cdll_int.h>
#include <cvardef.h>

#include "cmdregister.h"

// from Quake sorce:
//#define	SKY_TEX		2000
// HL:
#define SKY_TEX 0x16a8

extern cl_enginefuncs_s *pEngfuncs;

///////////////////////////////////////////////////////////////////////////////
// CNewSky

CNewSky::CNewSky()
{
	_iSkyQuadsCount = 7; // no sky processing in progress
	_bWantCustomSky = false; // user does not want CustomSky (or s.th. went rong)
	_bWantReload = true; // by default we need to load

	memset(_SkyTextures,0,6*sizeof(GLuint)); // no textures yet
}

int	skytexorder[6] = {0,1,2,3,4,5};
char *suf[6] = {"rt", "bk", "lf", "ft", "up", "dn"};

void CNewSky::DetectAndProcessSky(GLenum mode)
{
	if (!_bWantCustomSky || mode!=GL_QUADS) return;

	GLint iGLcurrBind;
	glGetIntegerv(GL_TEXTURE_BINDING_2D,&iGLcurrBind);

	if ((iGLcurrBind<SKY_TEX)||(SKY_TEX +6 <= iGLcurrBind)) return;

	EnsureGLTextureIndices(); // make sure we have the indices
			
	if (_bWantReload)
	{
		_bWantCustomSky = ReloadTexturesFromFile();
		_bWantReload = false;
	}

	// if we still have s.th. we can use as sky lol, then use it:
	if (_bWantCustomSky)
		glBindTexture(GL_TEXTURE_2D,_SkyTextures[iGLcurrBind-SKY_TEX]);

	//pEngfuncs->Con_Printf("QUAD: %i\n",iGLcurrBind-SKY_TEX);
}

bool CNewSky::ReloadTexturesFromFile()
{
	bool bRes=false;
	
	AUX_RGBImageRec *SkyTextures[6];

	memset(SkyTextures,0,6* sizeof(AUX_RGBImageRec *));

	SkyTextures[skytexorder[0]]=auxDIBImageLoad("mdtskyrt.bmp");
	SkyTextures[skytexorder[1]]=auxDIBImageLoad("mdtskybk.bmp");
	SkyTextures[skytexorder[2]]=auxDIBImageLoad("mdtskylf.bmp");
	SkyTextures[skytexorder[3]]=auxDIBImageLoad("mdtskyft.bmp");
	SkyTextures[skytexorder[4]]=auxDIBImageLoad("mdtskyup.bmp");
	SkyTextures[skytexorder[5]]=auxDIBImageLoad("mdtskydn.bmp");

	bRes = SkyTextures[0] && SkyTextures[1] && SkyTextures[2] && SkyTextures[3] && SkyTextures[4] && SkyTextures[5];

	if (bRes)
	{
		// every sky was loaded successfully

		GLint iGLcurrBind;
		glGetIntegerv(GL_TEXTURE_BINDING_2D,&iGLcurrBind); // save curr bind

		for (int i=0;i<6;i++)
		{
			glBindTexture(GL_TEXTURE_2D, _SkyTextures[i]);

			glTexImage2D(GL_TEXTURE_2D, 0, 3, SkyTextures[i]->sizeX, SkyTextures[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, SkyTextures[i]->data);

			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		}

		glBindTexture(GL_TEXTURE_2D, iGLcurrBind); // restore old bind


	}

	// free data:

	if (SkyTextures[0]) free(SkyTextures[0]->data); free(SkyTextures[0]);
	if (SkyTextures[1]) free(SkyTextures[1]->data); free(SkyTextures[1]);
	if (SkyTextures[2]) free(SkyTextures[2]->data); free(SkyTextures[2]);
	if (SkyTextures[3]) free(SkyTextures[3]->data); free(SkyTextures[3]);
	if (SkyTextures[4]) free(SkyTextures[4]->data); free(SkyTextures[4]);
	if (SkyTextures[5]) free(SkyTextures[5]->data); free(SkyTextures[5]);

	if (bRes) pEngfuncs->Con_Printf("Loaded custom sky textures.\n");
	else pEngfuncs->Con_Printf("Loading custom sky textures FAILED.\n");

	return bRes;

}

void CNewSky::EnsureGLTextureIndices()
{
	if (_SkyTextures[0]==0) glGenTextures(6,_SkyTextures);
}


void CNewSky::User_ForceReload(bool bEnableCustomSky)
{
	_bWantReload = true;
	_bWantCustomSky = bEnableCustomSky;
}

///////////////////////////////////////////////////////////////////////////////
// g_NewSky global singelton

CNewSky g_NewSky; // our global sky singleton

///////////////////////////////////////////////////////////////////////////////

REGISTER_CMD_FUNC(fx_skyhd)
{
	if (pEngfuncs->Cmd_Argc() != 2)
	{
		pEngfuncs->Con_Printf("Usage: " PREFIX "fx_skyhd 0|1\nPlease refer to the changelog / HLAEwiki for more information.");
		return;
	}

	g_NewSky.User_ForceReload(atoi(pEngfuncs->Cmd_Argv(1))!=0);
};
