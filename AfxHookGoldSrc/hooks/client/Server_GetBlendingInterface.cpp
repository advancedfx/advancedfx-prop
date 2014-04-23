#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2014-04-22 dominik.matrixstorm.com
//
// First changes
// 2014-04-22 dominik.matrixstorm.com


// Helpful documentation:
// 
// http://www.mail-archive.com/hlcoders@list.valvesoftware.com/msg07085.html
// http://www.mail-archive.com/hlcoders@list.valvesoftware.com/msg01224.html
// http://www.mail-archive.com/hlcoders@list.valvesoftware.com/msg02724.html


#include "Server_GetBlendingInterface.h"

#include <hlsdk.h>

#include <halflife/common/r_studioint.h>

#include "../../sv_hitboxes.h"


sv_blending_interface_s * OldBlendingInterface;
float (*g_pBoneTransform)[MAXSTUDIOBONES][3][4];
server_studio_api_t IEngineStudio;

void SV_StudioSetupBones( struct model_s *pModel, float frame, int sequence, const 
vec3_t angles, const vec3_t origin, const byte *pcontroller, const byte *pblending, 
int iBone, const edict_t *pEdict )
{
	if(OldBlendingInterface)
		OldBlendingInterface->SV_StudioSetupBones(pModel,frame,sequence,angles,
		origin, pcontroller, pblending, iBone, pEdict);

	FetchHitboxes(&IEngineStudio, g_pBoneTransform, pModel, pEdict);
}

// The simple blending interface we'll pass back to the engine
sv_blending_interface_t svBlending = 
{
        SV_BLENDING_INTERFACE_VERSION,
        SV_StudioSetupBones
};

typedef int(*Server_GetBlendingInterface_t)( int version, 
struct sv_blending_interface_s **ppinterface, struct server_studio_api_s *pstudio, 
float (*rotationmatrix)[3][4], float (*bonetransform)[MAXSTUDIOBONES][3][4] );

Server_GetBlendingInterface_t Old_Server_GetBlendingInterface;

int New_Server_GetBlendingInterface( int version, 
struct sv_blending_interface_s **ppinterface, struct server_studio_api_s *pstudio, 
float (*rotationmatrix)[3][4], float (*bonetransform)[MAXSTUDIOBONES][3][4] )
{
	OldBlendingInterface = 0;

	if(Old_Server_GetBlendingInterface)
	{
		Old_Server_GetBlendingInterface(version, &OldBlendingInterface, pstudio,
			rotationmatrix, bonetransform);
	}

	*ppinterface = &svBlending;

    // Copy in engine helper functions
    memcpy( &IEngineStudio, pstudio, sizeof( IEngineStudio ) );

	g_pBoneTransform = bonetransform;

	return 1;
}

FARPROC Hook_ServerGetBlendingInterface(FARPROC oldProc)
{
	Old_Server_GetBlendingInterface = (Server_GetBlendingInterface_t)oldProc;

	return (FARPROC)&New_Server_GetBlendingInterface;
}