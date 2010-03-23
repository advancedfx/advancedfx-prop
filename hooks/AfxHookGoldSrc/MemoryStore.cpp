#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-23 dominik.matrixstorm.com
//
// First changes
// 2010-03-23 dominik.matrixstorm.com

#include "MemoryStore.h"

#include <stdexcept>

using namespace std;

////////////////////////////////////////////////////////////////////////////////

class FrequentMemoryStoreItem :
	public IMemoryStoreItem
{
public:
	FrequentMemoryStoreItem * PreviousFree;
	FrequentMemoryStoreItem * NextFree;

	FrequentMemoryStoreItem(FrequentMemoryStoreManager * manager);
	~FrequentMemoryStoreItem();

	void Aquire();

	virtual void * Memory();

	virtual void Release();
private:
	bool m_Aquired;
	FrequentMemoryStoreManager * m_Manager;
	void * m_Memory;

	void Delist();
	void Enlist();
};

class FrequentMemoryStoreManager
{
public:
	int ItemCount;
	FrequentMemoryStoreItem * FreeItem;

	FrequentMemoryStoreManager(size_t itemSize);
	~FrequentMemoryStoreManager();

	FrequentMemoryStoreItem * GetItem(void);
	size_t GetItemSize();

	void Pack(void);

private:
	size_t m_ItemSize;

};


// FrequentMemoryStore /////////////////////////////////////////////////////////

FrequentMemoryStore::FrequentMemoryStore(size_t itemSize)
{
	m_Manager = new FrequentMemoryStoreManager(itemSize);
}
FrequentMemoryStore::~FrequentMemoryStore()
{
	delete m_Manager;
}

IMemoryStoreItem * FrequentMemoryStore::GetItem(void)
{
	return m_Manager->GetItem();
}

size_t FrequentMemoryStore::GetItemSize()
{
	return m_Manager->GetItemSize();
}

void FrequentMemoryStore::Pack(void)
{
	m_Manager->Pack();
}

// FrequentMemoryStoreManager //////////////////////////////////////////////////

FrequentMemoryStoreManager::FrequentMemoryStoreManager(size_t itemSize)
{
	ItemCount = 0;
	FreeItem = 0;
	m_ItemSize = itemSize;
}

FrequentMemoryStoreManager::~FrequentMemoryStoreManager()
{
	while(FreeItem)
		delete FreeItem;

	if(ItemCount)
		throw logic_error("");
}

FrequentMemoryStoreItem * FrequentMemoryStoreManager::GetItem(void)
{
	if(!FreeItem)
		new FrequentMemoryStoreItem(this);

	FreeItem->Aquire();

	return FreeItem;
}

size_t FrequentMemoryStoreManager::GetItemSize()
{
	return m_ItemSize;
}

void FrequentMemoryStoreManager::Pack(void)
{
	while(FreeItem)
		delete FreeItem;
}



// FrequentMemoryStoreItem /////////////////////////////////////////////////////

FrequentMemoryStoreItem::FrequentMemoryStoreItem(FrequentMemoryStoreManager * manager)
{
	PreviousFree = 0;
	NextFree = 0;
	m_Aquired = false;
	m_Manager = manager;
	m_Memory = malloc(manager->GetItemSize());

	if(!m_Memory)
		throw runtime_error("Out of memory.");

	manager->ItemCount++;
	Enlist();
}

FrequentMemoryStoreItem::~FrequentMemoryStoreItem()
{
	if(m_Aquired)
		throw logic_error("");

	Delist();
	m_Manager->ItemCount--;

	free(m_Memory);
}

void FrequentMemoryStoreItem::Aquire()
{
	if(m_Aquired)
		throw logic_error("");

	Delist();
	m_Aquired = true;
}

void FrequentMemoryStoreItem::Delist()
{
	if(PreviousFree)
		PreviousFree->NextFree = NextFree;

	if(NextFree)
		NextFree->PreviousFree = PreviousFree;

	if(this == m_Manager->FreeItem)
		m_Manager->FreeItem = 0;
}

void FrequentMemoryStoreItem::Enlist()
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

void * FrequentMemoryStoreItem::Memory()
{
	return m_Memory;
}

void FrequentMemoryStoreItem::Release()
{
	if(!m_Aquired)
		throw logic_error("");

	m_Aquired = false;
	Enlist();
}
