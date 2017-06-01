#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-06-26 dominik.matrixstorm.com
//
// First changes:
// 2015-06-26 dominik.matrixstorm.com

#include "AfxClasses.h"

#include <string.h>


// CAfxFreeable ////////////////////////////////////////////////////////////////

CAfxFreeable::CAfxFreeable()
: m_Parent(0)
{
}
CAfxFreeable::CAfxFreeable(IAfxFreeMaster * parent)
: m_Parent(parent)
{
	if(m_Parent) m_Parent->AfxFreeable_Register(this);
}

CAfxFreeable::~CAfxFreeable()
{
	AfxFree();
}

void CAfxFreeable::AfxFree(void)
{
	if(m_Parent) { m_Parent->AfxFreeable_Unregister(this); m_Parent = 0; }
}

IAfxFreeMaster * CAfxFreeable::GetParentFreeMaster() const
{
	return m_Parent;
}

// CAfxFreeMaster //////////////////////////////////////////////////////////////

CAfxFreeMaster::CAfxFreeMaster()
: CAfxFreeable()
{
}
CAfxFreeMaster::CAfxFreeMaster(IAfxFreeMaster * parent)
: CAfxFreeable(parent)
{
}

CAfxFreeMaster::~CAfxFreeMaster()
{
	AfxFree();
}

void CAfxFreeMaster::AfxFree(void)
{
	std::list<IAfxFreeable *>::iterator it;
	IAfxFreeable * last = 0;
	while((it = m_Childs.begin()) != m_Childs.end())
	{
		IAfxFreeable * current = *it;

		if(current == last) throw "CAfxFreeMaster::AfxFree: Child failed to unregister.";

		current->AfxFree();

		last = current;
	}

	CAfxFreeable::AfxFree();
}

void CAfxFreeMaster::AfxFreeable_Register(IAfxFreeable * value)
{
	m_Childs.push_back(value);
}

void CAfxFreeMaster::AfxFreeable_Unregister(IAfxFreeable * value)
{
	// we could use m_Childs.remove here, but I want to make sure that regardless
	// of implementation the first elements are scanned first:
	for(std::list<IAfxFreeable *>::iterator it = m_Childs.begin(); it != m_Childs.end(); ++it)
	{
		if(*it == value)
		{
			m_Childs.erase(it);
			break;
		}
	}
}

// CAfxMaterial ////////////////////////////////////////////////////////////////

std::map<SOURCESDK::IMaterial_csgo *, std::atomic_int> CAfxMaterial::m_KnownMaterials;
std::shared_timed_mutex CAfxMaterial::m_KnownMaterialsMutex;

CAfxMaterial::CAfxMaterial()
: CAfxFreeable()
, m_Material(0)
{
}

CAfxMaterial::CAfxMaterial(SOURCESDK::IMaterial_csgo * material)
: CAfxFreeable()
, m_Material(material)
{
	if(m_Material) AddRef(m_Material);
}

CAfxMaterial::CAfxMaterial(IAfxFreeMaster * freeMaster, SOURCESDK::IMaterial_csgo * material)
: CAfxFreeable(freeMaster)
, m_Material(material)
{
	if (m_Material) AddRef(m_Material);
}

CAfxMaterial::CAfxMaterial(const CAfxMaterial & x)
: CAfxFreeable(x.GetParentFreeMaster())
, m_Material(x.GetMaterial())
{
	if (m_Material) AddRef(m_Material);
}

CAfxMaterial::~CAfxMaterial()
{
	AfxFree();
}

SOURCESDK::IMaterial_csgo * CAfxMaterial::GetMaterial() const
{
	return m_Material;
}

void CAfxMaterial::AfxFree(void)
{
	if(m_Material) { Release(m_Material); m_Material = 0; }
	
	CAfxFreeable::AfxFree();
}

void CAfxMaterial::AddRef(SOURCESDK::IMaterial_csgo * material)
{
	m_KnownMaterialsMutex.lock_shared();

	std::map<SOURCESDK::IMaterial_csgo *, std::atomic_int>::iterator it = m_KnownMaterials.find(material);

	if (it != m_KnownMaterials.end())
	{
		++(it->second);
		m_KnownMaterialsMutex.unlock_shared();
	}
	else
	{
		m_KnownMaterialsMutex.unlock_shared();
		m_KnownMaterialsMutex.lock();

		const std::pair<const std::map<SOURCESDK::IMaterial_csgo *,std::atomic_int>::iterator, bool> & emp = m_KnownMaterials.try_emplace(material, 1);

		if (!emp.second)
		{
			++(emp.first->second);
		}
		else
		{
			material->IncrementReferenceCount(); // expensive operation!
		}

		m_KnownMaterialsMutex.unlock();
	}
}

void CAfxMaterial::Release(SOURCESDK::IMaterial_csgo * material)
{
	m_KnownMaterialsMutex.lock_shared();

	std::map<SOURCESDK::IMaterial_csgo *, std::atomic_int>::iterator it = m_KnownMaterials.find(material);

	if (it != m_KnownMaterials.end())
	{
		int lastValue = --(it->second);
		m_KnownMaterialsMutex.unlock_shared();

		if (0 == lastValue)
		{
			m_KnownMaterialsMutex.lock();

			std::map<SOURCESDK::IMaterial_csgo *, std::atomic_int>::iterator it = m_KnownMaterials.find(material);

			if (it != m_KnownMaterials.end())
			{
				if (0 == it->second)
				{
					m_KnownMaterials.erase(it);
					material->DecrementReferenceCount(); // expensive operation!
				}
			}

			m_KnownMaterialsMutex.unlock();
		}
	}
	else
	{
		m_KnownMaterialsMutex.unlock_shared();
		throw "void CAfxMaterial::Release(SOURCESDK::IMaterial_csgo * material): Unexpected call.";
	}
}


// CAfxMaterialKey /////////////////////////////////////////////////////////////

CAfxMaterialKey::CAfxMaterialKey(SOURCESDK::IMaterial_csgo * material)
: CAfxMaterial(material)
{
}

CAfxMaterialKey::CAfxMaterialKey(IAfxFreeMaster * freeMaster, SOURCESDK::IMaterial_csgo * material)
: CAfxMaterial(freeMaster, material)
{
}

CAfxMaterialKey::CAfxMaterialKey(const CAfxMaterialKey & x)
: CAfxMaterial(x)
{
}

bool CAfxMaterialKey::operator < (const CAfxMaterialKey & y) const
{
	/*
	int cmp1 = strcmp(this->m_Material->GetTextureGroupName(), y.m_Material->GetTextureGroupName());
	if(cmp1 < 0)
		return true;
	else
	if(cmp1 > 0)
		return false;

	return strcmp(this->m_Material->GetName(), y.m_Material->GetName()) < 0;
	*/

	return this->m_Material < y.m_Material;
}
