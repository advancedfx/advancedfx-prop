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

// {e069b892-479c-49ef-b6d6-1023d1728cfc}
DEFINE_GUID(CLSID_DsAfxSource, 
0xe069b892, 0x479c, 0x49ef, 0xb6, 0xd6, 0x10, 0x23, 0xd1, 0x72, 0x8c, 0xfc);

class DsAfxSource : public CSource
{

private:
    DsAfxSource(IUnknown *pUnk, HRESULT *phr);
    ~DsAfxSource();

public:
    static CUnknown * WINAPI CreateInstance(IUnknown *pUnk, HRESULT *phr);  

};

