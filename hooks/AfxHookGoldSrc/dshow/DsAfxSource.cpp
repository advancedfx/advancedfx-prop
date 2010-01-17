//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-01-09 dominik.matrixstorm.com
//
// First changes
// 2010-01-09 dominik.matrixstorm.com

#include "DsAfxSource.h"

DsAfxSource::DsAfxSource(IUnknown *pUnk, HRESULT *phr)
: CSource(NAME("DsAfxSource"), pUnk, CLSID_DsAfxSource)
{
	if(phr)
	{
		*phr = S_OK;
	}
}


DsAfxSource::~DsAfxSource()
{
}


CUnknown * WINAPI DsAfxSource::CreateInstance(IUnknown *pUnk, HRESULT *phr)
{
    DsAfxSource *pNewFilter = new DsAfxSource(pUnk, phr );

    if (phr)
	{
        if (pNewFilter == NULL) 
            *phr = E_OUTOFMEMORY;
        else
            *phr = S_OK;
    }

    return pNewFilter;
}