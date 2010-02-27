#include "stdafx.h"

#include "HltvFix.h"

#include "cmdregister.h"
#include "hooks/HookHw.h"


#include <hlsdk.h>

REGISTER_DEBUGCVAR(fixforcehltv, "1", 0);
REGISTER_DEBUGCVAR(force_thirdperson, "0", 0);

bool g_FixForceHltvEnabled = false;

typedef int (*CL_IsThirdPerson_t)( void );
CL_IsThirdPerson_t OldClientCL_IsThirdPerson;


//	bool bNotInEye = ppmove->iuser1 != 4;
//	int iwatched = ppmove->iuser2;

int NewClientCL_IsThirdPerson( void )
{
	if(force_thirdperson->value)
		return 1 == force_thirdperson->value ? 1 : 0;

	if(fixforcehltv->value
		&& pEngfuncs->IsSpectateOnly()
		&& g_FixForceHltvEnabled
		&& (
			ppmove->iuser1 != 4 // not in-eye
			|| ppmove->iuser2 != pEngfuncs->GetLocalPlayer()->index // not watching ourselfs
		)
	) 
		return 1;

	return OldClientCL_IsThirdPerson();
}

xcommand_t OldClientCmdDemForceHltv = NULL;


void NewClientCmdDemForceHltv(void)
{
	char *ptmp="";
	if (pEngfuncs->Cmd_Argc()>=1) ptmp=pEngfuncs->Cmd_Argv(1);
	g_FixForceHltvEnabled = (0 != atof(ptmp));
	OldClientCmdDemForceHltv();
}

// << dem_forcehltv fix