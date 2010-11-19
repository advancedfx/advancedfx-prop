#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-16 dominik.matrixstorm.com
//
// First changes
// 2010-10-24 dominik.matrixstorm.com


#include "Types.h"


using namespace Afx;
using namespace Afx::Expressions;


// Eof /////////////////////////////////////////////////////////////////////////

::Afx::IRef * Eof::Ref (void)
{
	return dynamic_cast<::Afx::IRef *>(this);
}


// Error ///////////////////////////////////////////////////////////////////////

Error::Error()
{
}

Error::Error(ErrorCode errorCode, Cursor & cur)
{
/*
	ostringstream errText("Error #");

	errText << (int)errorCode;

	errText << " at ";

	errText << (int)cur.GetPos();

	errText << " (...\"";

	for(int i=-16; i<0; i++)
	{
		char val = cur.Get(i);

		if(!Cursor::IsNull(val)) errText << val;
	}

	errText << "\" [> \"";

	if(!cur.IsNull()) errText << cur.Get();

	errText << "\" <] \"";

	for(int i=1; i<16; i++)
	{
		char val = cur.Get(i);

		if(!Cursor::IsNull(val)) errText << val;
	}


	errText << "\"...).";

	MessageBox(0, errText.str().c_str(), "Afx::Expression::Error(ErrorCode, Cursor &)", MB_OK|MB_ICONINFORMATION);
*/
}

::Afx::IRef * Error::Ref (void) {
	return dynamic_cast<::Afx::IRef *>(this);
}

