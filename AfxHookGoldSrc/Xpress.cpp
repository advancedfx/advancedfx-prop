#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-11-10 dominik.matrixstorm.com
//
// First changes
// 2010-11-10 dominik.matrixstorm.com

#include "Xpress.h"

#include "hooks/HookHw.h"
#include "cmdregister.h"


Xpress_t::Xpress_t()
{	
	m_MatteEx = 0;

	m_Bubbles.Info = Tools::StandardBubble();
	m_Bubbles.Info->Ref()->AddRef();

	m_Bubbles.Root = Tools::StandardBubble();
	m_Bubbles.Root->Ref()->AddRef();

	m_Bubbles.Root->Add("./", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
	m_Bubbles.Root->Add("..", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
	m_Bubbles.Root->Add(".info", Tools::FnDoCompileable(m_Bubbles.Info->Compiler()));

	m_Bubbles.Info->Add("./", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
	m_Bubbles.Info->Add("..", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
	m_Bubbles.Info->Add("CurrentGlMode", CurrentGlMode = new IntVariable(m_Bubbles.Info->Compiler(), IntVariable::CA_Getter, -1));
	m_Bubbles.Info->Add("CurrentStreamIndex", CurrentStreamIndex = new IntVariable(m_Bubbles.Info->Compiler(), IntVariable::CA_Getter, -1));
	m_Bubbles.Info->Add("GetCurrentEntityIndex", new FnGetCurrentEntityIndex(m_Bubbles.Info->Compiler()));
	m_Bubbles.Info->Add("IsFilming", IsFilming = new BoolVariable(m_Bubbles.Info->Compiler(), BoolVariable::CA_Getter, false));
	m_Bubbles.Info->Add("InRDrawEntitiesOnList", InRDrawEntitiesOnList = new BoolVariable(m_Bubbles.Info->Compiler(), BoolVariable::CA_Getter, false));
	m_Bubbles.Info->Add("InRDrawParticles", InRDrawParticles = new BoolVariable(m_Bubbles.Info->Compiler(), BoolVariable::CA_Getter, false));
	m_Bubbles.Info->Add("InRDrawViewModel", InRDrawViewModel = new BoolVariable(m_Bubbles.Info->Compiler(), BoolVariable::CA_Getter, false));
	m_Bubbles.Info->Add("InRRenderView", InRRenderView = new BoolVariable(m_Bubbles.Info->Compiler(), BoolVariable::CA_Getter, false));
}

Xpress_t::~Xpress_t()
{
	m_Bubbles.Info->Ref()->Release();
	m_Bubbles.Root->Ref()->Release();

	if(m_MatteEx) m_MatteEx->Ref()->Release();
}


IntT Xpress_t :: FnGetCurrentEntityIndex :: Get (void)
{
	cl_entity_t *ce = pEngStudio->GetCurrentEntity();

	if(ce)
		return ce->index;

	return -1;
}


Xpress_t g_Xpress;


char * New_CodeFromEngArgs (void)
{
	int argc = pEngfuncs->Cmd_Argc();

	if(argc < 2)
	{
		return 0;
	}

	//
	// concat arguments to full string:

	char *ttt, *ct;
	unsigned int len=0;

	// calculate required space:
	for(int i=0; i<argc; i++) len += strlen(pEngfuncs->Cmd_Argv(i))+1;
	if(len<1) len=1;

	ct = ttt = (char *)malloc(sizeof(char)*len);

	if(!ct) {
		return 0;
	}

	for(int i=1; i<argc; i++) {
		char const * cur = pEngfuncs->Cmd_Argv(i);
		unsigned int lcur = strlen(cur);
		
		if(1<i) {
			strcpy(ct, " ");
			ct++;
		}

		strcpy(ct, cur);
		ct += lcur;
	}
	*ct = 0; // Term

	return ttt;
}

_REGISTER_CMD("xpress", xpress_cmd)
void xpress_cmd()
{
	if(2 <= pEngfuncs->Cmd_Argc())
	{
		char * textCode = New_CodeFromEngArgs();

		if(!textCode)
		{
			pEngfuncs->Con_Printf("Error: New_CodeFromEngArgs failed.");
			return;
		}

		ICompiled * compiled = g_Xpress.CompileEx( textCode );

		compiled->Ref()->AddRef();

		switch(compiled->GetType())
		{
		case ICompiled::T_Bool:
			{
				bool bResult = compiled->GetBool()->EvalBool();
				pEngfuncs->Con_Printf(
					"Result: Bool = %s\n",
					bResult ? "true" : "false"
				);			
			}
			break;

		case ICompiled::T_Int:
			{
				int iResult = compiled->GetInt()->EvalInt();
				pEngfuncs->Con_Printf(
					"Result: Int = %i\n",
					iResult
				);			
			}
			break;

		case ICompiled::T_Error:
			{
				pEngfuncs->Con_Printf("Result: Error\n");
			}
			break;

		case ICompiled::T_Void:
			{
				compiled->GetVoid()->EvalVoid();
				pEngfuncs->Con_Printf(
					"Result: Void\n"				
				);			
			}
			break;

		default:
			pEngfuncs->Con_Printf("Error: Unknown expression type.");
			break;
		}

		compiled->Ref()->Release();

		free(textCode);

		return;
	}

	pEngfuncs->Con_Printf(
		"Usage: xpress ...\n"
		"For more information on HLAE XPress contact the manual / HLAEwiki.\n"
		"WARNING: This command is for experts only.\n"
		"WARNING: This command is yet untested and might behave unexpected.\n"
	);
}
