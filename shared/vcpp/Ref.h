#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-10 dominik.matrixstorm.com
//
// First changes
// 2010-04-24 dominik.matrixstorm.com

#ifdef _DEBUG
#define AFX_DEBUG_REF
#endif


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

#ifdef AFX_DEBUG_REF
	/// <remarks>Results might inaccurate due to threading.</remarks>
	static unsigned int DEBUG_GetGlobalRefCount (void);
#endif

	virtual void Release (void);

	/// <summary>AddRef, Release</summary>
	void TouchRef (void);

	/// <summary>AddRef, Release</summary>
	static void TouchRef (IRef * ref);

protected:
	unsigned int m_RefCount;

	virtual ~Ref();

private:

#ifdef AFX_DEBUG_REF
	static unsigned int m_GlobalRefCount;
#endif

};

} // namespace Afx {
