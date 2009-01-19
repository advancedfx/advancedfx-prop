#ifndef MDT_RAW_OUTPUT_H
#define MDT_RAW_OUTPUT_H

// Temporary raw and dirty raw output functions
// to allow getting the existing code more structured and readable
// and also to reduce code duplication.
//
// Raw means dumb, no checks etc..

#include "mdt_debug.h"

#include <windows.h>
#include <winbase.h>

//	WriteRawTarga
//
//	Writes out prepared data to an tga file:
//	This function assumes that the data is already coorectly
//	packed and alligned in memory and that the given
//	parameters are correct, no additional consistency checks
//	are performed.
//
//	Params: (not checked for consistency)
//	pData - pointer on Data in the format of (B,G,R),(B,G,R),....
//		from bottom-left to top-right
//	pszFileName - [path] and filename of the output file
//	usWidth - Width in pixels
//	usHeight - Height in pixels
//	ucBpp - BitDepth
//	bGrayScale - if this image is GrayScale or color
bool WriteRawTarga(
	unsigned char *pData, const char *pszFileName,
	unsigned short usWidth, unsigned short usHeight,
	unsigned char ucBpp, bool bGrayScale
);

//	WriteRawBitmap
//
//	pData - bottom-left -> top-right, 4 Byte (32Bit DWORD) alligned
//	ucBpp <= 24
//	bRestoreAlign // hack: 1 Byte Row Alignment (SAMPLER) -> 4 Byte Row Alignment (BMP)
bool WriteRawBitmap(
	unsigned char *pData,
	const char *pszFileName,
	unsigned short usWidth,
	unsigned short usHeight,
	unsigned char ucBpp,
	bool bRestoreAlign
);

#endif // #ifndef MDT_RAW_OUTPUT_H