#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-23 dominik.matrixstorm.com
//
// First changes
// 2010-03-23 dominik.matrixstorm.com

#include "Store.h"

#include <stdexcept>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

class FrequentStoreItem :
	public IStoreItem
{
public:
	FrequentStoreItem * PreviousFree;
	FrequentStoreItem * NextFree;

	FrequentStoreItem(FrequentStoreManager * manager);
	~FrequentStoreItem();

	void Aquire();

	virtual StoreValue GetValue();

	virtual void Release();

private:
	bool m_Aquired;
	FrequentStoreManager * m_Manager;
	StoreValue m_Value;

	void Delist();
	void Enlist();
};

class FrequentStoreManager
{
public:
	int ItemCount;
	FrequentStoreItem * FreeItem;

	FrequentStoreManager(IStoreFactory * factory);
	~FrequentStoreManager();

	IStoreItem * Aquire(void);

	IStoreFactory * GetFactory();

	void Pack(void);

private:
	IStoreFactory * m_Factory;

};


// FrequentStore /////////////////////////////////////////////////////////

FrequentStore::FrequentStore(IStoreFactory * factory)
{
	m_Manager = new FrequentStoreManager(factory);
}
FrequentStore::~FrequentStore()
{
	delete m_Manager;
}

IStoreItem * FrequentStore::Aquire(void)
{
	return m_Manager->Aquire();
}

void FrequentStore::Pack(void)
{
	m_Manager->Pack();
}


// FrequentStoreManager //////////////////////////////////////////////////

FrequentStoreManager::FrequentStoreManager(IStoreFactory * factory)
{
	ItemCount = 0;
	FreeItem = 0;
	m_Factory = factory;
}

FrequentStoreManager::~FrequentStoreManager()
{
	while(FreeItem)
		delete FreeItem;

	if(ItemCount)
		throw logic_error("");
}

IStoreItem * FrequentStoreManager::Aquire(void)
{
	if(!FreeItem)
		new FrequentStoreItem(this);

	FreeItem->Aquire();

	return FreeItem;
}

IStoreFactory *  FrequentStoreManager::GetFactory()
{
	return m_Factory;
}

void FrequentStoreManager::Pack(void)
{
	while(FreeItem)
		delete FreeItem;
}



// FrequentStoreItem /////////////////////////////////////////////////////

FrequentStoreItem::FrequentStoreItem(FrequentStoreManager * manager)
{
	PreviousFree = 0;
	NextFree = 0;
	m_Aquired = false;
	m_Manager = manager;
	m_Value = manager->GetFactory()->ContructValue();

	manager->ItemCount++;
	Enlist();
}

FrequentStoreItem::~FrequentStoreItem()
{
	if(m_Aquired)
		throw logic_error("");

	Delist();
	m_Manager->ItemCount--;

	m_Manager->GetFactory()->DestructValue(m_Value);
}

void FrequentStoreItem::Aquire()
{
	if(m_Aquired)
		throw logic_error("");

	Delist();
	m_Aquired = true;
}

void FrequentStoreItem::Delist()
{
	if(PreviousFree)
		PreviousFree->NextFree = NextFree;

	if(NextFree)
		NextFree->PreviousFree = PreviousFree;

	if(this == m_Manager->FreeItem)
		m_Manager->FreeItem = 0;
}

void FrequentStoreItem::Enlist()
{
	if(!m_Manager->FreeItem)
		m_Manager->FreeItem = this;
	else
	{
		NextFree = m_Manager->FreeItem;
		
		if(NextFree)
			NextFree->PreviousFree = this;
		m_Manager->FreeItem = this;
	}	
}

StoreValue FrequentStoreItem::GetValue()
{
	return m_Value;
}

void FrequentStoreItem::Release()
{
	if(!m_Aquired)
		throw logic_error("");

	m_Aquired = false;
	Enlist();
}

