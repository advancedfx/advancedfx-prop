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


char const * g_StrForMoreXpressInfo = "For more information on expressions please refer to the manual.";


int Xpress_t :: FnGetCurrentEntityIndex :: EvalInt (void)
{
	cl_entity_t *ce = pEngStudio->GetCurrentEntity();

	if(ce)
		return ce->index;

	return -1;
}


Xpress_t g_Xpress;



REGISTER_CMD_FUNC(xp_matte)
{
	if(2 == pEngfuncs->Cmd_Argc())
	{
		bool isOk = g_Xpress.CompileMatteEx( pEngfuncs->Cmd_Argv(1) );

		if(isOk)
			pEngfuncs->Con_Printf("Expression compiled successfully.\n");
		else
			pEngfuncs->Con_Printf("Expression failed to compile, using standard behaviour.\n");

		return;
	}

	pEngfuncs->Con_Printf(
		"WARNING: This command is yet untested and might behave unexpected.\n"
		"Usage: " PREFIX "xpress_matte <code>\n"
		"%s\n",
		g_StrForMoreXpressInfo
	);
}
