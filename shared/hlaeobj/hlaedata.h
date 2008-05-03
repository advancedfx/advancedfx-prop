#ifndef HLAEOBJ_DATA_H
#define HLAEOBJ_DATA_H

#include "hlaeobj.h"

namespace HLAEOBJ_NAMESPACE
{
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
		HlaeData();
		HlaeData( T default_value );

		// Typed access:
		void Set( T value );
		T Get( void );

		// Implement Binary access:
		BOOL SetData( PTR pointer, DWORD size );
		PTR GetData( DWORD & size );
	private:
		T _data;
	};
}

#endif
