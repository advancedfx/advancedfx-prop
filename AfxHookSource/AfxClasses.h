#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-06-26 dominik.matrixstorm.com
//
// First changes:
// 2015-06-26 dominik.matrixstorm.com

#include "AfxInterfaces.h"
#include "SourceInterfaces.h"

#include <list>
#include <map>
#include <atomic>
#include <shared_mutex>


// CAfxFreeable ////////////////////////////////////////////////////////////////

class CAfxFreeable
: public IAfxFreeable
{
public:
	CAfxFreeable();
	CAfxFreeable(IAfxFreeMaster * parent);

	virtual ~CAfxFreeable();

	/// <remarks>
	/// Implementations must a) make sure that they can be called multiple times
	/// and b) call the base class' AfxFree and c) call this in the destructor
	/// of every derived class that overrides AfxFree (that's why there is b)
	/// </remarks>
	virtual void AfxFree(void);

protected:
	IAfxFreeMaster * GetParentFreeMaster() const;

private:
	IAfxFreeMaster * m_Parent;
};

// CAfxFreeMaster //////////////////////////////////////////////////////////////

class CAfxFreeMaster
: public CAfxFreeable
, public IAfxFreeMaster
{
public:
	CAfxFreeMaster();
	CAfxFreeMaster(IAfxFreeMaster * parent);

	virtual ~CAfxFreeMaster();

	virtual void AfxFree(void);

	virtual void AfxFreeable_Register(IAfxFreeable * value);
	virtual void AfxFreeable_Unregister(IAfxFreeable * value);

private:
	std::list<IAfxFreeable *> m_Childs;
};


// CAfxMaterial ////////////////////////////////////////////////////////////////

class CAfxMaterial
: public CAfxFreeable
, public IAfxMaterial
{
public:
	CAfxMaterial();

	/// <remarks>
	/// This should only be used, when you have proper control about destruction
	//  at the right time.
	/// Normally you should use the other constructors instead.
	/// </remarks>
	CAfxMaterial(SOURCESDK::IMaterial_csgo * material);

	CAfxMaterial(IAfxFreeMaster * freeMaster, SOURCESDK::IMaterial_csgo * material);

	/// <remarks>
	/// The copy will use the same freeMaster, meaning the copy's material
	/// usually won't live longer than the original (except if freeMaster is 0).
	/// </remarks>
	CAfxMaterial(const CAfxMaterial & x);

	virtual ~CAfxMaterial();

	virtual SOURCESDK::IMaterial_csgo * GetMaterial() const;

	virtual void AfxFree(void);

protected:
	static std::map<SOURCESDK::IMaterial_csgo *, std::atomic_int> m_KnownMaterials;
	static std::shared_timed_mutex m_KnownMaterialsMutex;

	static void AddRef(SOURCESDK::IMaterial_csgo * material);
	static void Release(SOURCESDK::IMaterial_csgo * material);

	SOURCESDK::IMaterial_csgo * m_Material;
};

// CAfxMaterialKey /////////////////////////////////////////////////////////////

class CAfxMaterialKey : public CAfxMaterial
{
public:
	/// <remarks>
	/// This should only be used, when you have proper control about destruction
	//  at the right time.
	/// Normally you should use the other constructors instead.
	/// </remarks>
	CAfxMaterialKey(SOURCESDK::IMaterial_csgo * material);

	CAfxMaterialKey(IAfxFreeMaster * freeMaster, SOURCESDK::IMaterial_csgo * material);

	/// <remarks>
	/// The copy will use the same freeMaster, meaning the copy's material
	/// usually won't live longer than the original (except if freeMaster is 0).
	/// </remarks>
	CAfxMaterialKey(const CAfxMaterialKey & x);


	/// <remarks>
	/// This is object pointer based, meaning different references on the same material can have different keys
	/// </remarks>
	bool operator < (const CAfxMaterialKey & y) const;
};
