#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-17 by dominik.matrixstorm.com
///
// First changes:
// 2010-03-17 by dominik.matrixstorm.com

#include "InitScript.h"

#include "AfxGoldSrcComClient.h"

#include "mirv_scripting.h"

#include <shared/vcpp/AfxAddr.h>
#include <shared/vcpp/Expressions.h>

#include <string>


using namespace Afx;
using namespace Afx::Expressions;


//#define INIT_SCRIPT_FILE "AfxHookGoldSrc_init.js"
#define INIT_SCRIPT_FILE "AfxHookGoldSrc_init.x.txt"
#define SCRIPT_FOLDER "scripts\\"
#define DLL_NAME	"AfxHookGoldSrc.dll"


class AddressProperty : public IntProperty
{
public:
	AddressProperty(ICompiler * compiler, AfxAddr * addr)
	: IntProperty(compiler, CA_Property), m_Addr(addr)
	{

	}

	virtual IntT Get (void) {
		return *m_Addr;
	}

	virtual void Set (IntT value) {
		*m_Addr = value;
	}

private:
	AfxAddr * m_Addr;
};


class GoldSrcAddresses
{
public:
	GoldSrcAddresses()
	{
		m_Bubble = Tools::StandardBubble();
		m_Bubble->Ref()->AddRef();

		m_Bubble->Add("OnHwDllLoaded", (m_OnHwDllLoaded = new VoidEvent(m_Bubble->Compiler())));
		m_Bubble->Add("OnClientDllLoaded", (m_OnClientDllLoaded = new VoidEvent(m_Bubble->Compiler())));
	}

	~GoldSrcAddresses()
	{
		m_Bubble->Ref()->Release();
	}

	void BubbleAddresses()
	{
		for(unsigned int ui = 0; ui < AfxAddr_Debug_GetCount(); ui++)
		{
			AfxAddr addr;
			char const * name;

			if(AfxAddr_Debug_GetAt(ui, addr, name))
			{
				AfxAddr * padr = AfxAddr_GetByName(name);

				if(padr)
				{
					m_Bubble->Add(name, new AddressProperty(m_Bubble->Compiler(), padr));
				}
			}
		}
	}

	bool BubbleCode(char const * code)
	{
		ICompiled * compiled = m_Bubble->Compile(code);

		compiled->Ref()->AddRef();

		bool bOk =
			0 != compiled->GetVoid()
		;

		if(bOk) compiled->GetVoid()->EvalVoid();

		compiled->Ref()->Release();

		return bOk;
	}

	bool OnHwDllLoaded()
	{
		if(!m_OnHwDllLoaded->HasEvent())
			return false;

		m_OnHwDllLoaded->CallEvent();

		return true;
	}

	bool OnClientDllLoaded()
	{
		if(!m_OnClientDllLoaded->HasEvent())
			return false;

		m_OnClientDllLoaded->CallEvent();

		return true;
	}

private:
	IBubble * m_Bubble;
	VoidEvent * m_OnHwDllLoaded;
	VoidEvent * m_OnClientDllLoaded;

} g_GoldSrcAddresses;


bool InitEvent_OnHwDllLoaded()
{
	return g_GoldSrcAddresses.OnHwDllLoaded();
}


bool InitEvent_OnClientDllLoaded()
{
	return g_GoldSrcAddresses.OnClientDllLoaded();
}


/*
bool RunInitScript()
{
	char hookPath[1025];
	bool bCfgres = false;
	HMODULE hHookDll = GetModuleHandle(DLL_NAME);

	hookPath[0]=NULL;
	
	if (hHookDll)
	{
		GetModuleFileName(hHookDll, hookPath, sizeof(hookPath)/sizeof(*hookPath) -1);

		std::string strFolder(hookPath);
		size_t fp = strFolder.find_last_of('\\');
		if(std::string::npos != fp)
		{
			strFolder.resize(fp+1);
		}

		strFolder += SCRIPT_FOLDER;

		JsSetScriptFolder(strFolder.c_str());

		bCfgres = JsExecute("load('" INIT_SCRIPT_FILE "');");

		if(!bCfgres) {
			strFolder = "Failed to load:\n\""
				+strFolder +INIT_SCRIPT_FILE "\"."
			;

			MessageBox(0, strFolder.c_str(), "MDT_ERROR",MB_OK|MB_ICONHAND);
		}
	}

	return bCfgres;
}
*/


bool RunInitScript()
{
	char hookPath[1025];
	bool bCfgres = false;
	HMODULE hHookDll = GetModuleHandle(DLL_NAME);

	hookPath[0]=NULL;

#ifdef _DEBUG
	MessageBox(0, "RunInitScript", "MDT_DEBUG",MB_OK|MB_ICONINFORMATION);
#endif

	g_GoldSrcAddresses.BubbleAddresses();
	
	if (hHookDll)
	{
		GetModuleFileName(hHookDll, hookPath, sizeof(hookPath)/sizeof(*hookPath) -1);

		std::string strFolder(hookPath);
		size_t fp = strFolder.find_last_of('\\');
		if(std::string::npos != fp)
		{
			strFolder.resize(fp+1);
		}

		strFolder += SCRIPT_FOLDER;

		std::string scriptFile(strFolder);
		scriptFile += INIT_SCRIPT_FILE;

		std::string code("(compile (stringFromFile (s ");

		for(std::string::iterator it = scriptFile.begin(); it != scriptFile.end(); it++)
		{
			char val = (*it);

			switch(val)
			{
			case '\\':
			case '(':
			case ')':
				code += "\\";
				code += val;
				break;
			default:
				code += val;
				break;
			}
		}

		code += ")))";

		bCfgres = g_GoldSrcAddresses.BubbleCode(code.c_str());

		if(!bCfgres) {
			strFolder = "Failed to load:\n\"" +scriptFile +"\".";

			MessageBox(0, strFolder.c_str(), "MDT_ERROR",MB_OK|MB_ICONHAND);
		}
	}

	return bCfgres;
}