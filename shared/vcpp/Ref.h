#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-04-24 dominik.matrixstorm.com
//
// First changes
// 2010-04-24 dominik.matrixstorm.com

namespace Afx {

/// <summary>
/// Object which's life-time is managed using
/// Reference Counting.
/// </summary>
struct __declspec(novtable) IRef abstract
{
	virtual void AddRef (void) abstract = 0;
	virtual void Release (void) abstract = 0;
};

/// <summary>
/// Base class implementation of IRef.
/// </summary>
class Ref :
	public IRef
{
public:
	Ref();

	virtual void AddRef (void);
	virtual void Release (void);

protected:
	unsigned int m_RefCount;

	virtual ~Ref();

private:

};

} // namespace Afx {
