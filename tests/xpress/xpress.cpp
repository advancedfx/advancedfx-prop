#include "stdafx.h"

#include <shared/vcpp/Expressions.h>

#include <iostream>
#include <string>

using namespace std;
using namespace Afx;
using namespace Afx::Expressions;


class TestIntProperty : public IntProperty
{
public:
	TestIntProperty(ICompiler * compiler, IntT val)
	: IntProperty(compiler, CA_Property), m_Val(val)
	{

	}

	virtual IntT Get (void) {
		cout << "TestIntProperty::Get = " << m_Val << endl;

		return m_Val;
	}

	virtual void Set (IntT value) {
		cout << "TestIntProperty::Set(" << value <<")" << endl;

		m_Val = value;
	}

private:
	IntT m_Val;
};


class Xpress
{
public:
	Xpress()
	{
		m_Bubble = Tools::StandardBubble();
		m_Bubble->Ref()->AddRef();

		m_Bubble->Add("quit", m_FnQuit = new FnQuit(m_Bubble->Compiler()));
		m_Bubble->Add("iprop", new TestIntProperty(m_Bubble->Compiler(), 0));
	}

	~Xpress()
	{
		m_Bubble->Ref()->Release();
	}

	void Execute(char const * code)
	{
		ICompiled * compiled = m_Bubble->Compile(code);

		compiled->Ref()->AddRef();

		switch(compiled->GetType())
		{
		case ICompiled::T_Error:
			{
				cout << "Result: Error";
			}
			break;
		case ICompiled::T_Null:
			{
				cout << "Result: Null";
			}
			break;
		case ICompiled::T_Void:
			{
				compiled->GetVoid()->EvalVoid();
				cout << "Result: Void";
			}
			break;
		case ICompiled::T_Bool:
			{
				cout << "Result: Bool = " << compiled->GetBool()->EvalBool();
			}
			break;
		case ICompiled::T_Int:
			{
				cout << "Result: Int = " << compiled->GetInt()->EvalInt();
			}
			break;
		case ICompiled::T_Float:
			{
				cout << "Result: Float = " << compiled->GetFloat()->EvalFloat();
			}
			break;
		case ICompiled::T_String:
			{
				IStringValue * val = compiled->GetString()->EvalString();
				val->Ref()->AddRef();

				cout << "Result: String = \"" << val->GetData() << "\"";

				val->Ref()->Release();
			}
			break;
		default:
			{
				cout << "ERROR: Unknown Type";
			}
			break;
		}

		cout << "." << endl;

		compiled->Ref()->Release();

#ifdef AFX_DEBUG_REF
		cout << "GlobalRefCount: " << Ref::DEBUG_GetGlobalRefCount() << endl;
#endif
	}

	bool GetQuit() const {
		return m_FnQuit->GetQuit();
	}

private:
	class FnQuit : public VoidFunction
	{
	public:
		FnQuit(ICompiler * compiler) : VoidFunction(compiler), m_Quit(false) {
		}

		bool GetQuit (void) const {
			return m_Quit;
		}

		virtual VoidT EvalVoid (void) {
			m_Quit = true;
		}

	private:
		bool m_Quit;
	} * m_FnQuit;

	IBubble * m_Bubble;

} g_Express;


int main(int argc, char * argv[])
{
	string strInp;

	cout << "Enter \"quit\" to quit." << endl;

	do
	{
		cout << ">";
		getline(cin, strInp);

		g_Express.Execute(strInp.c_str());
		if(0 >= strInp.length()) cout << "Hint: Enter \"quit\" to quit." << endl;
	} while(!g_Express.GetQuit());

	return 0;
}

