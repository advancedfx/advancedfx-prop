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

CAfxMaterial::CAfxMaterial()
: CAfxFreeable()
, m_Material(0)
{
}

CAfxMaterial::CAfxMaterial(SOURCESDK::IMaterial_csgo * material)
: CAfxFreeable()
, m_Material(material)
{
	if(m_Material) m_Material->IncrementReferenceCount();
}

CAfxMaterial::CAfxMaterial(IAfxFreeMaster * freeMaster, SOURCESDK::IMaterial_csgo * material)
: CAfxFreeable(freeMaster)
, m_Material(material)
{
	if(m_Material) m_Material->IncrementReferenceCount();
}

CAfxMaterial::CAfxMaterial(const CAfxMaterial & x)
: CAfxFreeable(x.GetParentFreeMaster())
, m_Material(x.GetMaterial())
{
	if(m_Material) m_Material->IncrementReferenceCount();
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
	if(m_Material) { m_Material->DecrementReferenceCount(); m_Material = 0; }
	
	CAfxFreeable::AfxFree();
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
	int cmp1 = strcmp(this->m_Material->GetTextureGroupName(), y.m_Material->GetTextureGroupName());
	if(cmp1 < 0)
		return true;
	else
	if(cmp1 > 0)
		return false;

	return strcmp(this->m_Material->GetName(), y.m_Material->GetName()) < 0;
}
