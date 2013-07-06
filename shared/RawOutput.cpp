#include "stdafx.h"

#include "RawOutput.h"

#include <windows.h>
#include <stdio.h>

#include <libpng/png.h>
#include <zlib/zlib.h>

int CalcPitch(int width, unsigned char bytePerPixel, int byteAlignment)
{
	if(byteAlignment < 1)
		return 0;

	int pitch = 
		width * (int)bytePerPixel;

	if(0 != pitch % byteAlignment)
		pitch = (1+(pitch / byteAlignment))*byteAlignment;

	return pitch;
}

// see RawOutput.h
bool WriteRawBitmap(
	unsigned char const * pData,
	wchar_t const * fileName,
	unsigned short usWidth,
	unsigned short usHeight,
	unsigned char ucBpp,
	int pitch
)
{
	if(ucBpp > 24) return false;

	FILE *pFile;
	if (!(pFile = _wfopen(fileName, L"wb"))) return false;

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

	LONG realLineSize = bmInfoH.biSizeImage / usHeight;

	if(pitch == realLineSize)
	{
		fwrite(pData, sizeof(unsigned char), bmInfoH.biSizeImage, pFile);
		fclose(pFile);
		return true;
	}
	else if(pitch <  realLineSize)
	{
		int iPaddings = realLineSize-pitch;
		char pad=0x00;

		for( LONG line=0; line<usHeight; line++)
		{
			fwrite(pData, sizeof(unsigned char), pitch, pFile);
			pData += pitch;
			
			for(int i=0;i<iPaddings;i++)
				fwrite(&pad, 1, 1, pFile);
		}

		fclose(pFile);
		return true;
	}

	fclose(pFile);
	return false;
}

// see RawOutput.h
bool WriteRawTarga(
	unsigned char const * pData,
	wchar_t const * fileName,
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

	if ((pFile = _wfopen(fileName, L"wb")) != NULL)
	{
		fwrite(szTgaheader, sizeof(unsigned char), 12, pFile);
		fwrite(szHeader, sizeof(unsigned char), 6, pFile);

		fwrite(pData, sizeof(unsigned char), usWidth * usHeight * ucBppCeilDiv8, pFile);

		fclose(pFile);

		return true;
	}

	return false;
}

bool WriteRawPng (
	wchar_t const * fileName,
	unsigned char const * data,
	unsigned short width,
	unsigned short height,
	unsigned char bpp,
	int pitch,
	bool grayScale
)
{
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t y;
    png_byte ** row_pointers = NULL;

    bool status = false;
    int depth = 8;
    
    fp = _wfopen(fileName, L"wb");
    if (! fp) {
        goto fopen_failed;
    }

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto png_create_write_struct_failed;
    }
    
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        goto png_create_info_struct_failed;
    }
    
    // Set up error handling:

    if (setjmp (png_jmpbuf (png_ptr))) {
        goto png_failure;
    }
    
    // Set image attributes:

	png_set_IHDR (png_ptr,
		info_ptr,
		width,
		height,
		grayScale ? bpp : bpp/3,
		grayScale ? PNG_COLOR_TYPE_GRAY : PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);

	if(!grayScale) png_set_bgr(png_ptr);

	// setting better compression is not really worth it, filesize is 95%
	// and it takes 5 times longer compared to default settings:
//	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

    // Initialize rows of PNG:

    row_pointers = (png_byte **)png_malloc (png_ptr, height * sizeof (png_byte *));
    for (y = 0; y < height; y++)
	{
        png_byte *row = (png_byte *)data + (height -1 -y)*pitch;
        row_pointers[y] = row;
    }
    
	// Write the image data to "fp":

    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

	// Success:

    status = true;
        
png_failure:
	png_free (png_ptr, row_pointers);
png_create_info_struct_failed:
	png_destroy_write_struct (&png_ptr, &info_ptr);
png_create_write_struct_failed:
	fclose (fp);
fopen_failed:
	return status;
}