/*
Half-Life Advanced Effects project
*/

#include "newsky.h"

// BEGIN HLSDK includes
//
// HACK: prevent cldll_int.h from messing the HSPRITE definition,
// HLSDK's HSPRITE --> MDTHACKED_HSPRITE
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <wrect.h>
#include <cl_dll.h>
#include <cdll_int.h>
#include <cvardef.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include "cmdregister.h"

// from Quake sorce:
//#define	SKY_TEX		2000
// HL:
#define SKY_TEX 0x16a8

extern cl_enginefuncs_s *pEngfuncs;

///////////////////////////////////////////////////////////////////////////////


int skyorder[6] = {0,1,2,3,4,5};
char *suf[6] = {"rt", "bk", "lf", "ft", "up", "dn"};

typedef struct skyimage_s {
	LONG width;
	LONG height;
	void *data;
} skyimage_t;

// this is only useful when GL is working on a 4 byte boundary:
skyimage_t *LoadSky(const char *pszFileName, bool bFlipY)
{
	skyimage_t *pSkyImage;
	void *pTmpLine;
	FILE *pFile;
	BITMAPINFOHEADER bmInfoH;
	BITMAPFILEHEADER bmFileH;
	size_t cbSize, cbImageSize, cbLineSize;

	if(!(pSkyImage = (skyimage_t *)malloc(sizeof(skyimage_t))))
	{
		pEngfuncs->Con_Printf("Error: Out of memory\n",pszFileName);
		return 0;
	}

	if (
		!(pFile = fopen(pszFileName, "rb")) // could not open file
		|| fseek(pFile,0,SEEK_END) // could not seek to file end
		|| !(cbSize = ftell(pFile)) // file size is zero
		|| fseek(pFile,0,SEEK_SET) // could not seek back
	)
	{
		pEngfuncs->Con_Printf("Error: could not open sky image %s.\n",pszFileName);
		free(pSkyImage);
		return 0;
	}

	if(
		1 != fread(&bmFileH,sizeof(BITMAPFILEHEADER),1,pFile) // could not read bitmap header
		|| bmFileH.bfType != 0x4d42 // is not a bitmap "BM"
		|| 1 != fread(&bmInfoH,sizeof(BITMAPINFOHEADER),1,pFile) // could not read bitmap header
		|| 24 != bmInfoH.biBitCount // not 24 bit
		|| cbSize < bmInfoH.biSizeImage // file or imagesize is corrupted (file too short)
		|| fseek(pFile,bmFileH.bfOffBits,SEEK_SET) // could not seek to bitmap data
	)
	{
		pEngfuncs->Con_Printf("Error: unsupported format in sky image %s.\n",pszFileName);
		fclose(pFile);
		free(pSkyImage);
		return 0;
	}

	pSkyImage->width = bmInfoH.biWidth;
	pSkyImage->height = bmInfoH.biHeight;

	cbImageSize = bmInfoH.biSizeImage;

	if(!cbImageSize)
		cbImageSize = cbSize-bmFileH.bfOffBits;

	cbLineSize = pSkyImage->height ? cbImageSize / pSkyImage->height : 0;

	if(cbImageSize <= 0 || cbImageSize > cbSize || !cbLineSize)
	{
		pEngfuncs->Con_Printf("Error: Image empty or probably corrupted.\n",pszFileName);
		fclose(pFile);
		return 0;
	}

	pSkyImage->data = (void *)malloc(cbImageSize);
	pTmpLine =  (void *)malloc(cbLineSize);
	if(!pSkyImage->data || !pTmpLine)
	{
		pEngfuncs->Con_Printf("Error: Out of memory\n",pszFileName);
		fclose(pFile);
		free(pTmpLine);
		free(pSkyImage);
		return 0;
	}

	if(cbImageSize != fread(pSkyImage->data,1,cbImageSize,pFile))
	{
		pEngfuncs->Con_Printf("Error: failed reading imagedate from sky image %s.\n",pszFileName);
		free(pSkyImage->data);
		fclose(pFile);
		free(pTmpLine);
		free(pSkyImage);
		return 0;
	}

	fclose(pFile);

	if(bFlipY)
	{
		unsigned char bytes[3];
		unsigned int iH = pSkyImage->height; 
		for(int iY=0; iY<(iH>>1); iY++)
		{
			void *pUp = (void *)((unsigned char *)(pSkyImage->data) + iY*cbLineSize);
			void *pDn = (void *)((unsigned char *)(pSkyImage->data) + (iH-iY-1)*cbLineSize);
			memcpy(pTmpLine, pUp, cbLineSize);
			memcpy(pUp, pDn, cbLineSize);
			memcpy(pDn, pTmpLine, cbLineSize);
		}
	}

	free(pTmpLine);

	return pSkyImage;
}


///////////////////////////////////////////////////////////////////////////////
// CNewSky

CNewSky::CNewSky()
{
	_iSkyQuadsCount = 7; // no sky processing in progress
	_bWantCustomSky = false; // user does not want CustomSky (or s.th. went rong)
	_bWantReload = true; // by default we need to load
	_bOldFormat = true;

	memset(_SkyTextures,0,6*sizeof(GLuint)); // no textures yet
}

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
	
	skyimage_s *SkyTextures[6];

	memset(SkyTextures,0,6* sizeof(skyimage_t *));

	SkyTextures[skyorder[0]]=LoadSky("mdtskyrt.bmp", !_bOldFormat);
	SkyTextures[skyorder[1]]=LoadSky("mdtskybk.bmp", !_bOldFormat);
	SkyTextures[skyorder[2]]=LoadSky("mdtskylf.bmp", !_bOldFormat);
	SkyTextures[skyorder[3]]=LoadSky("mdtskyft.bmp", !_bOldFormat);
	SkyTextures[skyorder[4]]=LoadSky("mdtskyup.bmp", !_bOldFormat);
	SkyTextures[skyorder[5]]=LoadSky("mdtskydn.bmp", !_bOldFormat);

	bRes = SkyTextures[0] && SkyTextures[1] && SkyTextures[2] && SkyTextures[3] && SkyTextures[4] && SkyTextures[5];

	if (bRes)
	{
		// every sky was loaded successfully

		GLint iGLcurrBind;
		glGetIntegerv(GL_TEXTURE_BINDING_2D,&iGLcurrBind); // save curr bind

		for (int i=0;i<6;i++)
		{
			glBindTexture(GL_TEXTURE_2D, _SkyTextures[i]);

			glTexImage2D(GL_TEXTURE_2D, 0, 3, SkyTextures[i]->width, SkyTextures[i]->height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, SkyTextures[i]->data);

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


void CNewSky::User_ForceReload(bool bEnableCustomSky, bool bOldFormat)
{
	_bOldFormat = bOldFormat;
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
		pEngfuncs->Con_Printf("Usage: " PREFIX "fx_skyhd 0|1|2\nSee manual for more information.");
		return;
	}

	int iarg = atoi(pEngfuncs->Cmd_Argv(1));

	g_NewSky.User_ForceReload(iarg != 0, 2==iarg);
};
