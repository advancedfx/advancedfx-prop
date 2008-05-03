#ifndef HLAEOBJ_DATA_H
#define HLAEOBJ_DATA_H

// Last changes:
// 2008-05-03 19:00 UTC by ripieces

// First changes:
// 2008-05-03 19:00 UTC by ripieces

#include "hlaeobj.h"

namespace HLAEOBJ_NAMESPACE
{
	//
	// Declerations:
	//

	class HlaeAbstractData : HlaeObj
	{
	public:
		// Abstract Binary Access:
		virtual BOOL SetData( PTR pointer, DWORD size )=0;
		virtual PTR GetData( DWORD & size )=0;
	};

	template <class T>
	class HlaeData : HlaeAbstractData
	{
	public:
		// Structors:
		HlaeData();
		HlaeData( T default_value );

		// Typed access:
		void Set( T value );
		T Get();

		// Implement Binary access:
		BOOL SetData( PTR pointer, DWORD size );
		PTR GetData( DWORD & size );
	private:
		T _data;
	};

	//
	// Implementation of template functions:
	//

	template <class T>
	HlaeData<T>::HlaeData()
	{
		// does nothing yet
	}

	template <class T>
	HlaeData<T>::HlaeData(T default_value)
	{
		_data = default_value;
	}

	template <class T>
	void HlaeData<T>::Set( T value )
	{
		_data = value;
	}

	template <class T>
	T HlaeData<T>::Get( void )
	{
		return _data;
	}

	template <class T>
	BOOL HlaeData<T>::SetData( PTR pointer, DWORD size )
	{
		if (size != sizeof(T))
			return false;

		_data = *((T *)pointer);
		
		return true;
	}

	template <class T>
	PTR HlaeData<T>::GetData( DWORD & size )
	{
		size = sizeof(T);

		return (PTR)(&_data);
	}

}

#endif
