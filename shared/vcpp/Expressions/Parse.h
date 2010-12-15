#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-17 dominik.matrixstorm.com
//
// First changes
// 2010-11-17 dominik.matrixstorm.com

#include "Compile.h"

#include <vector>

using namespace std;

namespace Afx { namespace Expressions {


class ParseArgs : public Ref
{
public:
	/// <remarks>by default Null is skipped upon parsing, set SetSkipNull</remarks>
	ParseArgs(IArgumentCompiler * argumentCompiler);

	ICompiled * GetArg(int index);

	int GetCount (void);

	bool GetSkipNull (void) const;

	/// <summary>Parses the next argument (not added to list).</summary>
	/// <returns>true if argument was Eof, false otherwise.</returns>
	bool ParseEof (void);

	/// <summary>Parses the next argument, references it in the arg list and returns it.</summary>
	ICompiled * ParseNextArg(void);

	/// <summary>Parses the next argument, references it in the arg list and returns it's type.</summary>
	ICompiled::Type ParseNextArgT (void);

	/// <summary>Parses the next argument, references it in the arg list only if it's not Eof and returns it's type.</summary>
	ICompiled::Type ParseNextArgTE (void);

	/// <summary>Parses the next argument, references it in the arg list and returns if it matches a given type.</summary>
	bool ParseNextArgTC (ICompiled::Type type);

	/// <summary>Parses the next argument, references it in the arg list only if it's not Eof and returns if it matches a given type.</summary>
	bool ParseNextArgTCE (ICompiled::Type type);

	/// <summary>Parses the next argument, references it in the arg list only if it's not Eof.</summary>
	/// <param name="type">type to match</param>
	/// <param name="outMatchedOrEof">if type is matched or Eof</param>
	/// <returns>if not Eof</returns>
	bool ParseNextArgTCEX(ICompiled::Type type, bool & outMatchedOrEof);


	::Afx::IRef * Ref (void);

	void SetArg(int index, ICompiled * value);

	void SetSkipNull (bool value);

protected:
	virtual ~ParseArgs();

private:
	typedef vector<ICompiled *> VectorT;

	IArgumentCompiler * m_ArgumentCompiler;
	VectorT m_Compileds;
	bool m_SkipNull;

	ICompiled * ParseNextArg_Internal (void);
	
};


} } // namespace Afx { namespace Expr {

