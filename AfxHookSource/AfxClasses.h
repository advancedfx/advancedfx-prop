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


// CAfxFreeable ////////////////////////////////////////////////////////////////

class CAfxFreeable
: public IAfxFreeable
{
public:
	CAfxFreeable();
	CAfxFreeable(IAfxFreeMaster * parent);
	~CAfxFreeable();

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
	CAfxMaterial(IMaterial_csgo * material);

	CAfxMaterial(IAfxFreeMaster * freeMaster, IMaterial_csgo * material);

	/// <remarks>
	/// The copy will use the same freeMaster, meaning the copy's material
	/// usually won't live longer than the original (except if freeMaster is 0).
	/// </remarks>
	CAfxMaterial(const CAfxMaterial & x);

	virtual IMaterial_csgo * GetMaterial() const;

	virtual void AfxFree(void);

protected:
	IMaterial_csgo * m_Material;
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
	CAfxMaterialKey(IMaterial_csgo * material);

	CAfxMaterialKey(IAfxFreeMaster * freeMaster, IMaterial_csgo * material);

	/// <remarks>
	/// The copy will use the same freeMaster, meaning the copy's material
	/// usually won't live longer than the original (except if freeMaster is 0).
	/// </remarks>
	CAfxMaterialKey(const CAfxMaterialKey & x);

	bool operator < (const CAfxMaterialKey & y) const;
};

// CAfxMaterialCache ///////////////////////////////////////////////////////////

class CAfxMaterialCache
{
public:
	CAfxMaterial * Lookup(CAfxMaterialKey & key);

	CAfxMaterial * Insert(CAfxMaterialKey & key, CAfxMaterial & value);

	void Ivalidate(void);

private:
	std::map<CAfxMaterialKey, CAfxMaterial> m_Map;

};
