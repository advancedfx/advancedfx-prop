#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-10 dominik.matrixstorm.com
//
// First changes
// 2010-04-24 dominik.matrixstorm.com

#include "Ref.h"

#include <assert.h>

using namespace Afx;

Ref::Ref()
{
	m_RefCount = 0;
}

Ref::~Ref()
{
	assert( 0 == m_RefCount );
}

void Ref::AddRef()
{
    m_RefCount++;
}

void Ref::Release()
{
	m_RefCount--;
	assert( 0 <= m_RefCount );

    if (0 == m_RefCount)
    {
        delete this;
    }
}

void Ref::TouchRef (void)
{
	TouchRef(this);
}

void Ref::TouchRef (IRef * ref)
{
	if(ref)
	{
		ref->AddRef();
		ref->Release();
	}
}
