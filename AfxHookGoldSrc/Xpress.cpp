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


hud_player_info_t g_hpinfo;


Xpress * Xpress::m_Xpress;


Xpress::Xpress()
{	
	Mod = XmodFactory::Create();
	Mod->Ref()->AddRef();

	m_Bubbles.Root = Tools::StandardBubble();
	m_Bubbles.Root->Ref()->AddRef();

	IBubble * events = Tools::StandardBubble();
	events->Ref()->AddRef();

	IBubble * info = Tools::StandardBubble();
	info->Ref()->AddRef();

	//

	Mod->GetBubble()->Add("./", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
	Mod->GetBubble()->Add("..", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));

	m_Bubbles.Root->Add("./", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
	m_Bubbles.Root->Add("..", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
	m_Bubbles.Root->Add(".events", Tools::FnDoCompileable(events->Compiler()));
	m_Bubbles.Root->Add(".info", Tools::FnDoCompileable(info->Compiler()));
	m_Bubbles.Root->Add(".mod", Tools::FnDoCompileable(Mod->GetBubble()->Compiler()));
	m_Bubbles.Root->Add("Exec", Delegate::New(
		m_Bubbles.Root->Compiler(),
		this,
		(VoidFunction)&Xpress::Exec,
		ArgumentsT::New(1, A_String)
	));

	events->Add("./", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
	events->Add("..", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
	events->Add("GlBegin", Events.GlBegin = new VoidEvent(events->Compiler()));
	events->Add("GlEnd", Events.GlEnd = new VoidEvent(events->Compiler()));
	events->Add("FilmingStart", Events.FilmingStart = new VoidEvent(events->Compiler()));
	events->Add("FilmingStop", Events.FilmingStop = new VoidEvent(events->Compiler()));
	events->Add("Matte", Events.Matte = new IntEvent(events->Compiler(), -1));
	events->Add("RenderViewBegin", Events.RenderViewBegin = new BoolEvent(events->Compiler(), true));
	events->Add("RenderViewEnd", Events.RenderViewEnd = new BoolEvent(events->Compiler(), false));

	info->Add("./", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
	info->Add("..", Tools::FnDoCompileable(m_Bubbles.Root->Compiler()));
	info->Add("CurrentGlMode", Info.CurrentGlMode = new IntVariable(info->Compiler(), IntVariable::CA_Getter, -1));
	info->Add("CurrentStreamIndex", Info.CurrentStreamIndex = new IntVariable(info->Compiler(), IntVariable::CA_Getter, -1));
	info->Add("GetCurrentEntityIndex", Delegate::New(
		info->Compiler(),
		this,
		(IntFunction)&Xpress::GetCurrentEntityIndex,
		ArgumentsT::New()
	));
	info->Add("GetIsPlayer", Delegate::New(
		info->Compiler(),
		this,
		(BoolFunction)&Xpress::GetIsPlayer,
		ArgumentsT::New(1, A_Int)
	));
	info->Add("IsFilming", Info.IsFilming = new BoolVariable(info->Compiler(), BoolVariable::CA_Getter, false));
	info->Add("InRDrawEntitiesOnList", Info.InRDrawEntitiesOnList = new BoolVariable(info->Compiler(), BoolVariable::CA_Getter, false));
	info->Add("InRDrawParticles", Info.InRDrawParticles = new BoolVariable(info->Compiler(), BoolVariable::CA_Getter, false));
	info->Add("InRDrawViewModel", Info.InRDrawViewModel = new BoolVariable(info->Compiler(), BoolVariable::CA_Getter, false));
	info->Add("InRRenderView", Info.InRRenderView = new BoolVariable(info->Compiler(), BoolVariable::CA_Getter, false));

	//

	events->Ref()->Release();
	info->Ref()->Release();
}

Xpress::~Xpress()
{
	Mod->Ref()->Release();
	m_Bubbles.Root->Ref()->Release();
}

VoidT Xpress::Exec(Arguments args)
{
	StringValueRef strVal(args[0].String->EvalString());

	pEngfuncs->pfnClientCmd(
		const_cast<char *>(strVal.getData())
	);
}

Xpress * Xpress::Get (void) {
	if(!m_Xpress) m_Xpress = new Xpress();
	return m_Xpress;
}

IntT Xpress::GetCurrentEntityIndex(Arguments args)
{
	cl_entity_t *ce = pEngStudio->GetCurrentEntity();

	if(ce)
		return ce->index;

	return -1;
}

BoolT Xpress::GetIsPlayer(Arguments args)
{
	int index = args[0].Int->EvalInt();

	cl_entity_t * e = pEngfuncs->GetEntityByIndex(index);

	return e && e->player;	
}

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

void xpress_execute(char const * textCode)
{
	if(!textCode)
	{
		pEngfuncs->Con_Printf("Error: 0 code pointer.");
		return;
	}

	ICompiled * compiled = Xpress::Get()->CompileEx( textCode );

	compiled->Ref()->AddRef();

	switch(compiled->GetType())
	{
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

	case ICompiled::T_Float:
		{
			int iResult = compiled->GetFloat()->EvalFloat();
			pEngfuncs->Con_Printf(
				"Result: Float = %f\n",
				iResult
			);			
		}
		break;

	case ICompiled::T_String:
		{
			IStringValue * result = compiled->GetString()->EvalString();

			result->Ref()->AddRef();

			pEngfuncs->Con_Printf(
				"Result: String = \"%s\"\n",
				result->GetData()
			);			

			result->Ref()->Release();
		}
		break;

	default:
		pEngfuncs->Con_Printf("Error: Unknown expression type.");
		break;
	}

	compiled->Ref()->Release();
}

_REGISTER_CMD("xpress", xpress_cmd)
void xpress_cmd()
{
	if(2 <= pEngfuncs->Cmd_Argc())
	{
		char * textCode = New_CodeFromEngArgs();

		xpress_execute(textCode);

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
