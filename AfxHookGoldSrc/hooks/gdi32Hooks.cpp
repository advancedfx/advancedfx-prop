#include "stdafx.h"

#include "gdi32Hooks.h"

#include "../AfxGoldSrcComClient.h"

void MbPrintPixelFormatDescriptor(char const * title, PIXELFORMATDESCRIPTOR const * pfd)
{
	char szTmp[1000];
	_snprintf(szTmp, sizeof(szTmp) - 1,
		"ColorBits: %u\nDepthBits: %u\nRedBits: %u, RedShift: %u\nGreenBits: %u, GreenShift: %u\nBlueBits: %u, BlueShift: %u\nAlphaBits: %u, AlphaShift: %u",
		pfd->cColorBits,
		pfd->cDepthBits,
		pfd->cRedBits, pfd->cRedShift,
		pfd->cGreenBits, pfd->cGreenShift,
		pfd->cBlueBits, pfd->cBlueShift,
		pfd->cAlphaBits, pfd->cAlphaShift
	);

	MessageBox(0, szTmp, title, MB_OK|MB_ICONINFORMATION);
}

BOOL  WINAPI NewSetPixelFormat(__in HDC hdc, __in int format, __in CONST PIXELFORMATDESCRIPTOR * ppfd)
{
	if (!g_AfxGoldSrcComClient.GetForceAlpha8())
		return SetPixelFormat(hdc, format, ppfd);

	PIXELFORMATDESCRIPTOR *myppfd = const_cast<PIXELFORMATDESCRIPTOR *>(ppfd);

	// we intentionally void the const paradigm here:
	myppfd->cAlphaBits = 8; // request alpha bit planes (generic implementation doesn't support that)
	myppfd->cAlphaShift = 24;

	return SetPixelFormat(hdc, ChoosePixelFormat(hdc, myppfd), myppfd);
}