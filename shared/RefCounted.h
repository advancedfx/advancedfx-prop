#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-03-22 dominik.matrixstorm.com
//
// First changes:
// 2016-03-22 dominik.matrixstorm.com

namespace Afx {

class IRefCounted abstract
{
public:
	virtual void AddRef(void) = 0;
	virtual void Release(void) = 0;
};

class CRefCounted
: public IRefCounted
{
public:
	CRefCounted();

	virtual void AddRef(void);
	virtual void Release(void);

protected:
	virtual ~CRefCounted();

private:
	int m_RefCount;
};

} // namespace Afx {
