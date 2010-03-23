#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-23 dominik.matrixstorm.com
//
// First changes
// 2010-03-23 dominik.matrixstorm.com

class IMemoryStoreItem abstract
{
	virtual void * Memory() abstract = 0;
	virtual void Release() abstract = 0;
};

class IMemoryStore
{
	virtual IMemoryStoreItem * GetItem(void) abstract = 0;
};

/*
template <class T>
class MemoryStoreItem
{
public:
	MemoryStoreItem(IMemoryStore * store)
	{
		m_Item = store->GetItem();
	}

	~MemoryStoreItem()
	{
		m_Item->Release();
	}

	T * Value()
	{
		return (T *)m_Item,
	}

private:
	IMemoryStoreItem * m_Item;	
}
*/

class FrequentMemoryStoreManager;

class FrequentMemoryStore
{
public:
	FrequentMemoryStore(size_t itemSize);
	~FrequentMemoryStore();

	virtual IMemoryStoreItem * GetItem(void);

	size_t GetItemSize();

	void Pack(void);

private:
	FrequentMemoryStoreManager * m_Manager;

};
