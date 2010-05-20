#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-01-09 dominik.matrixstorm.com
//
// First changes
// 2010-01-09 dominik.matrixstorm.com

#include <streams.h>
#include <initguid.h>

// The current Implementation has
// performance quirks, due to
// the synchronization with
// FillBuffer, which is actually
// a waste of the 2nd buffer.
// CSourceStream might be simply
// not a good class to derive from.
class DsAfxImagePin : public CSourceStream
{
protected:
    BITMAPINFO * m_pBmi;
    DWORD m_cbBmi;
	DWORD m_cbData;
	REFERENCE_TIME m_rtFrameLength;
	int m_iFrameNumber;
	BYTE * m_pImage;
	CAMEvent *m_CanFillBuffer;
	CAMEvent *m_ContinueFillBuffer;

public:

	DsAfxImagePin(HRESULT *phr, CSource *pFilter, BITMAPINFO *pBmi, float avgFps);
	~DsAfxImagePin();

	HRESULT GetMediaType(CMediaType *pMediaType);
	HRESULT DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest);
	HRESULT FillBuffer(IMediaSample *pSample);

	STDMETHODIMP Notify(IBaseFilter *pSelf, Quality q)
	{
		return E_FAIL;
	}

	void AfxSupplyImage(BYTE *pImage);
	
	void AfxSupplyImageEnd();

};