#ifndef HLAEOBJ_HUMANDATA_H
#define HLAEOBJ_HUMANDATA_H

// Last changes:
// 2008-05-03 19:00 UTC by ripieces

// First changes:
// 2008-05-03 19:00 UTC by ripieces

#include "hlaeobj.h"
#include "hlaedata.h"

namespace HLAEOBJ_NAMESPACE
{
	//
	// Declerations:
	//

	class HlaeAbstractHuman : HlaeObj
	{
	public:
		// Abstract Human Access:
		virtual BOOL SetHuman( STRING value )=0;
		virtual STRING GetHuman( void )=0;
	};

	template <class T>
	class HlaeHumanData : HlaeData<T>, HlaeAbstractHuman
	{
	public:
		// Structors:
		HlaeHumanData() : HlaeData();
		HlaeHumanData( T default_value ) : HlaeData( T default_value );

		// Implement Human Access:
		BOOL SetHuman( STRING value )=0;
		STRING GetHuman( void )=0;
	};

	//
	// Implementation of template functions:
	//

}

#endif
