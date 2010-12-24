#include "stdafx.h"

#include "gdi32Hooks.h"

#include "../AfxGoldSrcComClient.h"

int WINAPI NewChoosePixelFormat(HDC hdc, CONST PIXELFORMATDESCRIPTOR *ppfd)
{
	// itentionally void const paradigm:
	PIXELFORMATDESCRIPTOR *myppfd = const_cast<PIXELFORMATDESCRIPTOR *>(ppfd);

	if (g_AfxGoldSrcComClient.GetForceAlpha8())
	{
		// we intentionally void the const paradigm here:
		myppfd->cAlphaBits = 8; // request alpha bit planes (generic implementation doesn't support that)
		myppfd->cAlphaShift = 24;
	}

	return ChoosePixelFormat(hdc,myppfd);
}

