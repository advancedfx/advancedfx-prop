//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-01-09 dominik.matrixstorm.com
//
// First changes
// 2010-01-09 dominik.matrixstorm.com

#include "DsAfxImagePin.h"

BITMAPINFO * AllocPackedBitMapInfoCopy(BITMAPINFO * pBmi, DWORD &outSize) {
	size_t bmiSize = sizeof(BITMAPINFO) +min(1,sizeof(RGBQUAD)*(pBmi->bmiHeader.biClrUsed)) -1;
	BITMAPINFO * pOutBmi = (BITMAPINFO *)malloc(bmiSize);
	if(pBmi) {
		memcpy(
			&pOutBmi->bmiHeader,
			&pBmi->bmiHeader,
			min(sizeof(BITMAPINFOHEADER), pBmi->bmiHeader.biSize)
		);
		memcpy(
			pOutBmi->bmiColors,
			pBmi->bmiColors,
			sizeof(RGBQUAD)*(pBmi->bmiHeader.biClrUsed)
		);
		outSize = (DWORD)bmiSize;
	}

	return pBmi;
}

DsAfxImagePin::DsAfxImagePin(HRESULT *phr, CSource *pFilter, BITMAPINFO *pBmi, float avgFps)
: CSourceStream(NAME("DsAfxImagePin"), phr, pFilter, L"Out")
{
	m_pBmi = NULL;
	m_cbBmi = 0;
	m_cbData = 0;
	m_rtFrameLength = (REFERENCE_TIME)((float)UNITS/avgFps +0.5f);
	m_iFrameNumber = 0;
    m_pImage = NULL;

	m_CanFillBuffer = new CAMEvent(FALSE, phr);
	if(!m_CanFillBuffer)
		return;

	m_ContinueFillBuffer = new CAMEvent(FALSE, phr);
	if(!m_ContinueFillBuffer)
		return;

	m_pBmi = AllocPackedBitMapInfoCopy(pBmi, m_cbData);
	if(!m_pBmi) {
		if(phr) *phr = E_OUTOFMEMORY;
		return;
	}
}


DsAfxImagePin::~DsAfxImagePin()
{
	delete m_pBmi;
	delete m_ContinueFillBuffer;
	delete m_CanFillBuffer;
}

void DsAfxImagePin::AfxSupplyImage(BYTE *pImage)
{
	m_CanFillBuffer->Wait();
	memcpy(m_pImage, pImage, m_cbData);
	m_ContinueFillBuffer->Set();
}

void DsAfxImagePin::AfxSupplyImageEnd()
{
	m_CanFillBuffer->Wait();
	m_pImage = NULL;
	m_ContinueFillBuffer->Set();
}

HRESULT DsAfxImagePin::GetMediaType(CMediaType *pMediaType)
{
    CAutoLock cAutoLock(m_pFilter->pStateLock());

    CheckPointer(pMediaType, E_POINTER);

    // Allocate enough room for the VIDEOINFOHEADER and the color tables
    VIDEOINFOHEADER *pvi = 
        (VIDEOINFOHEADER*)pMediaType->AllocFormatBuffer(SIZE_PREHEADER +m_cbBmi);
    if (pvi == 0) 
        return(E_OUTOFMEMORY);

    // Initialize the video info header
    ZeroMemory(pvi, pMediaType->cbFormat);   
    pvi->AvgTimePerFrame = m_rtFrameLength;

    // Copy the header info from the current bitmap
    memcpy(&(pvi->bmiHeader), m_pBmi, m_cbBmi);

    // Set image size for use in FillBuffer
    pvi->bmiHeader.biSizeImage  = GetBitmapSize(&pvi->bmiHeader);

    // Clear source and target rectangles
    SetRectEmpty(&(pvi->rcSource)); // we want the whole image area rendered
    SetRectEmpty(&(pvi->rcTarget)); // no particular destination rectangle

    pMediaType->SetType(&MEDIATYPE_Video);
    pMediaType->SetFormatType(&FORMAT_VideoInfo);
    pMediaType->SetTemporalCompression(FALSE);

    // Work out the GUID for the subtype from the header info.
    const GUID SubTypeGUID = GetBitmapSubtype(&pvi->bmiHeader);
    pMediaType->SetSubtype(&SubTypeGUID);
    pMediaType->SetSampleSize(pvi->bmiHeader.biSizeImage);

    return S_OK;
}


HRESULT DsAfxImagePin::DecideBufferSize(IMemAllocator *pAlloc, ALLOCATOR_PROPERTIES *pRequest)
{
    HRESULT hr;
    CAutoLock cAutoLock(m_pFilter->pStateLock());

    CheckPointer(pAlloc, E_POINTER);
    CheckPointer(pRequest, E_POINTER);

    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER*) m_mt.Format();
    
    // Ensure a minimum number of buffers
    if (pRequest->cBuffers == 0)
    {
        pRequest->cBuffers = 2;
    }
    pRequest->cbBuffer = pvi->bmiHeader.biSizeImage;

    ALLOCATOR_PROPERTIES Actual;
    hr = pAlloc->SetProperties(pRequest, &Actual);
    if (FAILED(hr)) 
    {
        return hr;
    }

    // Is this allocator unsuitable?
    if (Actual.cbBuffer < pRequest->cbBuffer) 
    {
        return E_FAIL;
    }

    return S_OK;
}


HRESULT DsAfxImagePin::FillBuffer(IMediaSample *pSample)
{
    CheckPointer(pSample, E_POINTER);

    VIDEOINFOHEADER *pVih = (VIDEOINFOHEADER*)m_mt.pbFormat;

	// Check that we're still using video
    ASSERT(m_mt.formattype == FORMAT_VideoInfo);

    pSample->GetPointer(&m_pImage);
	m_cbData = min(pVih->bmiHeader.biSizeImage, pSample->GetSize());


	m_CanFillBuffer->Set();

    m_ContinueFillBuffer->Wait();

	if(!m_pImage)
		// no more images.
		return S_FALSE;

    REFERENCE_TIME rtStart = m_iFrameNumber * m_rtFrameLength;
    REFERENCE_TIME rtStop  = rtStart + m_rtFrameLength;

    pSample->SetTime(&rtStart, &rtStop);
    m_iFrameNumber++;

    pSample->SetSyncPoint(TRUE); // set TRUE for uncompressed frame.

    return S_OK;
}