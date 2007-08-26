/*
File        : mdt_media.cpp
Started     : 2007-08-25 21:00:00
Project     : Mirv Demo Tool
Authors     : Dominik Tugend
Description : This file implements the mirv_cameraofs_cs function to allow setting of cameraofs (can be usefull to do stereocaptures by capturing scenes twice i.e.)
*/

// I tried to do this similar to demoeditfix.cpp, but of course it is different heh

#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "com_model.h"
#include "r_studioint.h"
#include "pm_defs.h"
#include "cvardef.h"
#include "entity_types.h"

//
#include <windows.h> // BYTE, ...
#include "cmdregister.h" // functions to help with registering console commands
#include "ref_params.h" // hl1/common/ref_params.h, defines the ref_params_s structure supplied with each V_CalcRefDef call

extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s *pEngStudio;
extern playermove_s *ppmove;

// DetourWrapFunc
/*
void *DetourWrapFunc(BYTE *origFunc, int sizeOrig, BYTE *wrappingFunc)
{
	DWORD dwProt = 0; // here we will store the original protection mode of the memory we have to change

	BYTE *jmp = (BYTE*)malloc(sizeOrig); // get memory where we can store a complete copy of the original function
	VirtualProtect(origFunc, sizeOrig, PAGE_READWRITE, &dwProt); // tell windows to give us access to the bytes of the original function in memory, dwProt will store the protection that was set before
	memcpy(jmp, origFunc, sizeOrig); // copy the original function to our new memory

	memset(origFunc, NOP, sizeOrig); // fill the original memory up with the no operation asm instruction
	// now if the original code gets into the orginial func's memory we want it
	// to think it called our wrapper function, so we just jump onto it
	// I just hope that doesn't mess up any relative pointers (I should check the assembler reference for function calls) :S
	// may be we should change this in the future and do it like this: replacing the orig function with a call to wrapper function
	origFunc[0] = JMP; // place asm JUMP OPCODE (may be see the Intel assembler reference which one of the codes it is heh)
	*(DWORD*)(origFunc+1) = (DWORD)(wrappingFunc - origFunc) - JMP32_SZ; // supply the (relative) address (the argument for the opcode) actually what you do here is not save, relative jumps is not a good idea, to much asumptions for loading order of DLLS (i.e. hook>orig)

	VirtualProtect(origFunc, sizeOrig, dwProt, 0); // Well, try to restore the old memory protection heh
	return jmp; // return base address of the copy of the original func
}*/

void *HookVTentryWrapper(DWORD* tableEntry, BYTE* wrappingFunc)
// tableEntry --> Address(-pointer) of Table entry
// wrappingFUnc --> (Address) of new func
// WARNING: THIS FUNCTION IS ONLY FOR 32 BIT I386 environments!
{
	DWORD dwProt = 0; // here we will store the original protection mode of the memory we have to change

	DWORD oldentry;
	VirtualProtect(tableEntry, sizeof(DWORD), PAGE_READWRITE, &dwProt); // tell windows to give us access to the memory where the entry is stored
	
	oldentry = *tableEntry; // get original entry

	*tableEntry = (DWORD)wrappingFunc; // place the new entry

	pEngfuncs->Con_DPrintf("Hooked:  @0x%08x: 0x%08x --> 0x%08x (==0x%08x)\n",tableEntry,oldentry,*tableEntry,wrappingFunc); // Developer 1 can see this

	VirtualProtect(tableEntry, sizeof(DWORD), dwProt, 0); // try to restore the old memory protection heh
	
	return (BYTE *)oldentry; // return old entry
}

float camerofs_screenofs_x = 0;
float camerofs_screenofs_y = 0;
float camerofs_screenofs_z = 0;

// address from client.dll's export table:
#define ADDRESS_V_CalcRefDef 0x0196db70
#define SIZE_V_CalcRefDef 38
#define VTENTRY_V_CalcRefDef 0x02f6a98c

typedef void (*V_CalcRefDef_t)( struct ref_params_s *pparams );
V_CalcRefDef_t orig_V_CalcRefDef;
// typedef bool (__stdcall  *V_CalcRefDef_t)( struct ref_params_s *pparams ); // why bool?
// V_CalcRefDef_t orig_V_CalcRefDef;

void hooked_V_CalcRefDef( struct ref_params_s *pparams )
{
	// call the original function
	orig_V_CalcRefDef(pparams);

	// apply our values(this code is similar to HL1SDK/multiplayer/cl_dll/view.cpp/V_CalcNormalRefdef):
	for ( int i=0 ; i<3 ; i++ )
	{
		pparams->vieworg[i] += camerofs_screenofs_z*pparams->forward[i] + camerofs_screenofs_x*pparams->right[i] + camerofs_screenofs_y*pparams->up[i];
	}

	// that's it

}

REGISTER_CMD_FUNC(cameraofs_cs)
{
	static bool bHooked_V_CalcRefDef = false;

	if (pEngfuncs->Cmd_Argc() == 4)
	{
		if(!bHooked_V_CalcRefDef)
		{
			// Redirect to hook
			// orig_V_CalcRefDef = (V_CalcRefDef_t) DetourWrapFunc((BYTE *) ADDRESS_V_CalcRefDef, SIZE_V_CalcRefDef, (BYTE *) hooked_V_CalcRefDef);
			orig_V_CalcRefDef = (V_CalcRefDef_t) HookVTentryWrapper((DWORD *)VTENTRY_V_CalcRefDef,(BYTE *)hooked_V_CalcRefDef);

			bHooked_V_CalcRefDef = true;
			pEngfuncs->Con_Printf("Hooked V_CalcRefDef.\n");
		}

		camerofs_screenofs_x = atof(pEngfuncs->Cmd_Argv(1));
		camerofs_screenofs_y = atof(pEngfuncs->Cmd_Argv(2));
		camerofs_screenofs_z = atof(pEngfuncs->Cmd_Argv(3));
		pEngfuncs->Con_Printf("Set new x y and z.\n");
	}
	else
		pEngfuncs->Con_Printf("Usage: " PREFIX "cameraofs_cs <x_ofs> <y_ofs> <z_ofs>\nThis function probably currently only works with Counter-Strike 1.6.\n");
}