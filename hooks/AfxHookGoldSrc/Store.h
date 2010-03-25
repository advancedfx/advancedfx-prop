#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-23 dominik.matrixstorm.com
//
// First changes
// 2010-03-23 dominik.matrixstorm.com


typedef void * StoreValue;


class IStoreFactory abstract
{
public:
	virtual StoreValue ConstructValue() abstract = 0;
	virtual void DestructValue(StoreValue value) abstract = 0;
};


class IStoreItem abstract
{
public:
	virtual StoreValue GetValue() abstract = 0;
	virtual void Release() abstract = 0;
};


class IStore
{
public:
	virtual IStoreItem * Aquire(void) abstract = 0;
};


class FrequentStoreManager;

class FrequentStore :
	public IStore
{
public:
	FrequentStore(IStoreFactory * factory);
	virtual ~FrequentStore();

	virtual IStoreItem * Aquire(void);

	void Pack(void);

private:
	FrequentStoreManager * m_Manager;
};
