#include "RawOutput.h"

#include "mdt_debug.h"

#include <windows.h>
#include <gl\gl.h>
//#include <winbase.h>
#include <stdio.h>

//
//	TODO: lot's of, this is just another mad hack
//


/*
//
//	TODO: GlGetError handling is bad, may result in lost errors!
//

// commonly used properties that cause problems for us:
struct GLStorageProperties_s
{
	GLint alignment;
};

void GetStorageProperties(GLStorageProperties_s *pHere)
{
	glGetIntegerv(GL_PACK_ALIGNMENT,&(pHere->alignment));
}

void SetStorageProperties(GLStorageProperties_s *pHere)
{
	glPixelStorei(GL_PACK_ALIGNMENT, pHere->alignment);
}

//	if it fails it restores the old ones
bool SafeSetStorageProperties(GLStorageProperties_s *pHere)
{
	GLStorageProperties_s backup;
	GetStorageProperties(backup);
	glGetError(); // unset any old error (get's lost, bad coding)
	SetStorageProperties(pHere);
	if(glGetError())
	{
		glGetError(); // unset new error
		SetStorageProperties(backup);
	}

}


bool SafeSetupGLForTarga()
{




	return false;
}

bool SafeSetupGLForBitmap()
{
	return false;
}
*/

// see RawOutput.h
bool WriteRawBitmap(
	unsigned char *pData,
	const char *pszFileName,
	unsigned short usWidth,
	unsigned short usHeight,
	unsigned char ucBpp,
	bool bRestoreAlign
)
{
	if(ucBpp > 24) return false;

	FILE *pFile;
	if (!(pFile = fopen(pszFileName, "wb"))) return false;

	BITMAPINFOHEADER bmInfoH;
	BITMAPFILEHEADER bmFileH;

	//
	// Construct the BITMAPINFOHEADER:

	bmInfoH.biSize = sizeof(bmInfoH);
	bmInfoH.biWidth = (LONG)usWidth;
	bmInfoH.biHeight = (LONG)usHeight;
	bmInfoH.biPlanes = 1;
	bmInfoH.biBitCount = ucBpp;
	bmInfoH.biCompression = BI_RGB;

	bmInfoH.biSizeImage =
		// biHeight * 4 * ceil( cClrBits / 8 ) * biWidth
		(((bmInfoH.biWidth * ucBpp +31) & ~31)>>3) * (LONG)usHeight; 


	bmInfoH.biXPelsPerMeter = 0; // dunno
	bmInfoH.biYPelsPerMeter = 0; // dunno
	
	bmInfoH.biClrUsed = 0;
	if( ucBpp < 24 ) bmInfoH.biClrUsed = 1 << ucBpp;

	bmInfoH.biClrImportant = 0; // all color indexies important lol

	//
	// construct the BITMAPFILEHEADER:

	bmFileH.bfType = 0x4d42; // 0x42='B', 0x4d = 'M'
	bmFileH.bfSize = bmInfoH.biSize
		+ bmInfoH.biClrUsed * sizeof(RGBQUAD) + bmInfoH.biSizeImage;

    bmFileH.bfReserved1 = 0; 
    bmFileH.bfReserved2 = 0;
 
    bmFileH.bfOffBits =
		(DWORD)sizeof(BITMAPFILEHEADER) + 
        bmInfoH.biSize + bmInfoH.biClrUsed * sizeof (RGBQUAD);

	//
	//	write out headers:

	fwrite(&bmFileH, sizeof(BITMAPFILEHEADER), 1, pFile);
	fwrite(&bmInfoH, sizeof(BITMAPINFOHEADER), 1, pFile);

	//
	//	write out fake pallete if required:

	RGBQUAD rgbquad;
	rgbquad.rgbReserved = 0;
	if( bmInfoH.biClrUsed <= 256)
	{
		// gray fade (okay may have some rounding errors hehe):
		float tmpf = (BYTE)(255.0f / (bmInfoH.biClrUsed-1));
		for( DWORD cols = 0; cols<bmInfoH.biClrUsed; cols++)
		{
			rgbquad.rgbRed = (float)cols * tmpf;
			rgbquad.rgbGreen = rgbquad.rgbRed;
			rgbquad.rgbBlue = rgbquad.rgbRed;
			fwrite(&rgbquad,sizeof(rgbquad),1,pFile);
		}
	} else {
		// simply encode it into RGB:
		for( DWORD cols = 0; cols<bmInfoH.biClrUsed; cols++)
		{
			rgbquad.rgbRed = (BYTE)(cols & 0xFF0000);
			rgbquad.rgbGreen = (BYTE)(cols & 0x00FF00);
			rgbquad.rgbBlue = (BYTE)(cols & 0x0000FF);
			fwrite(&rgbquad,sizeof(rgbquad),1,pFile);
		}
	}

	//
	//	write out image data:

	if(	bRestoreAlign ) // bRestoreAlign may change in this sub code!
	{
		LONG inLineSize = ucBpp >> 3;
		if( ucBpp & 0x7) inLineSize++;
		inLineSize *= usWidth;
		LONG realLineSize = bmInfoH.biSizeImage / usHeight;
		int iPaddings = realLineSize-inLineSize;
		char pad='P';

		if(iPaddings>0)
		{

			for( LONG line=0; line<usHeight; line++)
			{
				fwrite(pData, sizeof(unsigned char), bmInfoH.biSizeImage, pFile);
				pData += inLineSize;
				
				for(int i=0;i<iPaddings;i++) fwrite(&pad, sizeof(char), 1 ,pFile);
			}

		}
		else bRestoreAlign=false; // !!!
	}

	if(!bRestoreAlign)
	{
		fwrite(pData, sizeof(unsigned char), bmInfoH.biSizeImage, pFile);
	}

	fclose(pFile);

	return true;
}

// see RawOutput.h
bool WriteRawTarga(
	unsigned char *pData,
	const char *pszFileName,
	unsigned short usWidth,
	unsigned short usHeight,
	unsigned char ucBpp,
	bool bGrayScale
)
{
	unsigned char ucBppCeilDiv8 =  (ucBpp & 0x07) ? (ucBpp >> 3)+1 : (ucBpp >> 3);
	unsigned char ucGray = (bGrayScale ? 3 : 2);
	unsigned char szTgaheader[12] = { 0, 0, ucGray, 0, 0, 0, 0, 24, 0, 0, 0, 0 };
	unsigned char szHeader[6] = { (unsigned char)(usWidth & 0xFF), (unsigned char)(usWidth >> 8), (unsigned char)(usHeight & 0xFF), (unsigned char)(usHeight >> 8), ucBpp, 0 };
	FILE *pFile;

	if ((pFile = fopen(pszFileName, "wb")) != NULL)
	{
		fwrite(szTgaheader, sizeof(unsigned char), 12, pFile);
		fwrite(szHeader, sizeof(unsigned char), 6, pFile);

		fwrite(pData, sizeof(unsigned char), usWidth * usHeight * ucBppCeilDiv8, pFile);

		fclose(pFile);

		return true;
	}

	return false;
}