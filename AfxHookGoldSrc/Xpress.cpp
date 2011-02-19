#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2011-01-03 dominik.matrixstorm.com
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

	m_Bubbles.Root = IBubble::New();
	m_Bubbles.Root->Ref()->AddRef();

	IBubble * events = IBubble::New();
	events->Ref()->AddRef();

	IBubble * info = IBubble::New();
	info->Ref()->AddRef();


	// Extend Mod Bubble:

	Mod->GetBubble()->Add(StringValue::CopyFrom("./"), FunctionCompilers::Do(m_Bubbles.Root->Compiler()));
	Mod->GetBubble()->Add(StringValue::CopyFrom(".."), FunctionCompilers::Do(m_Bubbles.Root->Compiler()));

	// Populate Root bubble:
	{
//TODO:		ICompiler * defCompiler;
//TODO:		ICompileable * defCompileable = NewCompileableFnDef(m_Bubbles.Root->Compiler(), defCompiler);
		
//TODO:		defCompileable->Ref()->AddRef();
//TODO:		defCompiler->Ref()->AddRef();

		IBubble::AddStandardFunctions(m_Bubbles.Root);
		m_Bubbles.Root->Add(StringValue::CopyFrom("./"), FunctionCompilers::Do(m_Bubbles.Root->Compiler()));
		m_Bubbles.Root->Add(StringValue::CopyFrom(".."), FunctionCompilers::Do(m_Bubbles.Root->Compiler()));
//TODO:		m_Bubbles.Root->Add(".def", FunctionCompilers::Do(defCompiler));	
			m_Bubbles.Root->Add(StringValue::CopyFrom(".events"), FunctionCompilers::Do(events->OuterCompiler()));
			m_Bubbles.Root->Add(StringValue::CopyFrom(".info"), FunctionCompilers::Do(info->OuterCompiler()));
			m_Bubbles.Root->Add(StringValue::CopyFrom(".mod"), FunctionCompilers::Do(Mod->GetBubble()->OuterCompiler()));
//TODO:		m_Bubbles.Root->Add("Def", defCompileable);
		m_Bubbles.Root->Add(StringValue::CopyFrom("Exec"), Delegate::New(
			this,
			(VoidFunction)&Xpress::XExec,
			ArgumentsT::New(1, A_String)
		));

//TODO:		defCompiler->Ref()->Release();
//TODO:		defCompileable->Ref()->Release();
	}

	// Populate Events bubble:
	{
		IBubble::AddStandardFunctions(events);
		events->Add(StringValue::CopyFrom("./"), FunctionCompilers::Do(m_Bubbles.Root->Compiler()));
		events->Add(StringValue::CopyFrom(".."), FunctionCompilers::Do(m_Bubbles.Root->Compiler()));
		events->Add(StringValue::CopyFrom("GlBegin"), Events.GlBegin = new VoidEvent(events->Compiler()));
		events->Add(StringValue::CopyFrom("GlEnd"), Events.GlEnd = new VoidEvent(events->Compiler()));
		events->Add(StringValue::CopyFrom("FilmingStart"), Events.FilmingStart = new VoidEvent(events->Compiler()));
		events->Add(StringValue::CopyFrom("FilmingStop"), Events.FilmingStop = new VoidEvent(events->Compiler()));
		events->Add(StringValue::CopyFrom("Matte"), Events.Matte = new IntEvent(events->Compiler(), -1));
		events->Add(StringValue::CopyFrom("RenderViewBegin"), Events.RenderViewBegin = new BoolEvent(events->Compiler(), true));
		events->Add(StringValue::CopyFrom("RenderViewEnd"), Events.RenderViewEnd = new BoolEvent(events->Compiler(), false));
	}

	// Populate Info bubble:
	{
		IBubble::AddStandardFunctions(info);
		info->Add(StringValue::CopyFrom("./"), FunctionCompilers::Do(m_Bubbles.Root->Compiler()));
		info->Add(StringValue::CopyFrom(".."), FunctionCompilers::Do(m_Bubbles.Root->Compiler()));
		info->Add(StringValue::CopyFrom("CurrentGlMode"), Delegate::New(this, (IntFunction)&Xpress::XCurrentGlMode, ArgumentsT::New()));
		info->Add(StringValue::CopyFrom("CurrentStreamIndex"), Delegate::New(this, (IntFunction)&Xpress::XCurrentStreamIndex, ArgumentsT::New()));
		info->Add(StringValue::CopyFrom("GetCurrentEntityIndex"), Delegate::New(this, (IntFunction)&Xpress::XGetCurrentEntityIndex, ArgumentsT::New()));
		info->Add(StringValue::CopyFrom("GetIsPlayer"), Delegate::New(this, (BoolFunction)&Xpress::XGetIsPlayer, ArgumentsT::New(1, A_Int)));
		info->Add(StringValue::CopyFrom("IsFilming"), Delegate::New(this, (BoolFunction)&Xpress::XIsFilming, ArgumentsT::New()));
		info->Add(StringValue::CopyFrom("InRDrawEntitiesOnList"), Delegate::New(this, (BoolFunction)&Xpress::XInRDrawEntitiesOnList, ArgumentsT::New()));
		info->Add(StringValue::CopyFrom("InRDrawParticles"), Delegate::New(this, (BoolFunction)&Xpress::XInRDrawParticles, ArgumentsT::New()));
		info->Add(StringValue::CopyFrom("InRDrawViewModel"), Delegate::New(this, (BoolFunction)&Xpress::XInRDrawViewModel, ArgumentsT::New()));
		info->Add(StringValue::CopyFrom("InRRenderView"), Delegate::New(this, (BoolFunction)&Xpress::XInRRenderView, ArgumentsT::New()));
	}

	//

	events->Ref()->Release();
	info->Ref()->Release();
}

Xpress::~Xpress()
{
	Mod->Ref()->Release();
	m_Bubbles.Root->Ref()->Release();
}



Xpress * Xpress::Get (void) {
	if(!m_Xpress) m_Xpress = new Xpress();
	return m_Xpress;
}

VoidT Xpress::XExec(Arguments args)
{
	StringValueRef strVal(args[0].String->EvalString());

	pEngfuncs->pfnClientCmd(
		const_cast<char *>(strVal.getData())
	);
}

IntT Xpress::XGetCurrentEntityIndex(Arguments args)
{
	cl_entity_t *ce = pEngStudio->GetCurrentEntity();

	if(ce)
		return ce->index;

	return -1;
}

BoolT Xpress::XGetIsPlayer(Arguments args)
{
	int index = args[0].Int->EvalInt();

	cl_entity_t * e = pEngfuncs->GetEntityByIndex(index);

	return e && e->player;	
}


////////////////////////////////////////////////////////////////////////////////


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
			double dResult = compiled->GetFloat()->EvalFloat();
			pEngfuncs->Con_Printf(
				"Result: Float = %f\n",
				dResult
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
