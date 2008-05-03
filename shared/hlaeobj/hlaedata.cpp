#include "hlaeobj.h"

#include "hlaedata.h"

namespace HLAEOBJ_NAMESPACE
{
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
	}

	template <class T>
	PTR HlaeData<T>::GetData( DWORD & size )
	{
		size = sizeof(T);

		return (PTR)(&_data);
	}

}