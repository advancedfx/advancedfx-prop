#include "stdafx.h"

#include "camimport.h"

#include <stdio.h>
#include <windows.h>

#include <hlsdk.h>

#include "filming.h"

#include "cmdregister.h"

extern cl_enginefuncs_s *pEngfuncs;
extern Filming g_Filming;

CCamImport g_CamImport;

REGISTER_CMD_FUNC(camimport_load)
{
	int ic =  pEngfuncs->Cmd_Argc();

	if((2 == ic)||(3 == ic))
	{
		float fBase;

		if(ic==2)
			fBase = g_Filming.GetDebugClientTime();
		else
			fBase = atof(pEngfuncs->Cmd_Argv(2));

		if(g_CamImport.LoadMotionFile(pEngfuncs->Cmd_Argv(1)))
		{
			pEngfuncs->Con_Printf("BVH opened.\n");
			g_CamImport.SetBaseTime(fBase);
		}
		else
			pEngfuncs->Con_Printf("ERROR.\n");
	} else {
		pEngfuncs->Con_Printf("Usage:\n" PREFIX "camimport_load <bvhfile> [<basetime>]\n");
	}
}

REGISTER_CMD_FUNC(camimport_end)
{
	g_CamImport.CloseMotionFile();
}

REGISTER_CMD_FUNC(camimport_basetime)
{
	if(2 == pEngfuncs->Cmd_Argc())
	{
		if(!strcmp(pEngfuncs->Cmd_Argv(1),"current"))
			g_CamImport.SetBaseTime(g_Filming.GetDebugClientTime());
		else
			g_CamImport.SetBaseTime(atof(pEngfuncs->Cmd_Argv(1)));
	} else {
		pEngfuncs->Con_Printf("Usage:\n" PREFIX "camimport_basetime (<basetime>|current)\nCurrent: %f",g_CamImport.GetBaseTime());
	}
}

////////////////////////////////////////////////////////////////////////////////


// CCamImport //////////////////////////////////////////////////////////////////

CCamImport::CCamImport()
{

}

void CCamImport::CloseMotionFile()
{
	m_BvhImport.CloseMotionFile();
}

bool CCamImport::GetCamPositon(float fTimeOfs, float outCamdata[6])
{
	if(m_BvhImport.IsActive())
		return m_BvhImport.GetCamPositon(fTimeOfs -m_BaseTime, outCamdata);

	return false;
}


bool CCamImport::IsActive()
{
	return m_BvhImport.IsActive();
}

bool CCamImport::LoadMotionFile(char const * pszFileName)
{
	return m_BvhImport.LoadMotionFile(pszFileName);
}

float CCamImport::GetBaseTime()
{
	return m_BaseTime;
}

void CCamImport::SetBaseTime(float fBaseTime)
{
	m_BaseTime = fBaseTime;
}

CCamImport::~CCamImport()
{
	m_BvhImport.CloseMotionFile();
}


