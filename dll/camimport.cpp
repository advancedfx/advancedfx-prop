#include "camimport.h"

#include <stdio.h>
#include <windows.h>

// BEGIN HLSDK includes
//
// HACK: prevent cldll_int.h from messing the HSPRITE definition,
// HLSDK's HSPRITE --> MDTHACKED_HSPRITE
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <wrect.h>
#include <cl_dll.h>
#include <cdll_int.h>
#include <cvardef.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include "filming.h"

#include "cmdregister.h"

extern cl_enginefuncs_s *pEngfuncs;
extern Filming g_Filming;

CCamImport g_CamImport;

REGISTER_DEBUGCMD_FUNC(beta_camimport_load)
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
		pEngfuncs->Con_Printf("Usage:\n" DEBUG_PREFIX "camimport_load <bvhfile> [<basetime>]\n");
	}
}

REGISTER_DEBUGCMD_FUNC(beta_camimport_end)
{
	g_CamImport.CloseMotionFile();
}

REGISTER_DEBUGCMD_FUNC(beta_camimport_basetime)
{
	if(2 == pEngfuncs->Cmd_Argc())
	{
		if(!strcmp(pEngfuncs->Cmd_Argv(1),"current"))
			g_CamImport.SetBaseTime(g_Filming.GetDebugClientTime());
		else
			g_CamImport.SetBaseTime(atof(pEngfuncs->Cmd_Argv(1)));
	} else {
		pEngfuncs->Con_Printf("Usage:\n" DEBUG_PREFIX "camimport_load (<basetime>|current)\nCurrent: %f",g_CamImport.GetBaseTime());
	}
}


// CCamImport //////////////////////////////////////////////////////////////////

CCamImport::CCamImport()
{
	m_Active = false;
	m_BaseTime = 0;
}

void CCamImport::CloseMotionFile()
{
	if(m_Active)
	{
		m_Active = false;
		fclose(m_File);
	}
}

int CCamImport::DecodeBvhChannel(char * pszRemainder, char * & aoutNewRemainder)
{
	aoutNewRemainder = pszRemainder;
	if(!pszRemainder)
		return -1;

	int iret = -1;

	while(0 != *pszRemainder && (' ' == *pszRemainder || '\t' == *pszRemainder) )
	{
		pszRemainder++;
	}

	if(pszRemainder == strstr(pszRemainder,"Xposition")) iret = BC_Xposition;
	if(pszRemainder == strstr(pszRemainder,"Yposition")) iret = BC_Yposition;
	if(pszRemainder == strstr(pszRemainder,"Zposition")) iret = BC_Zposition;
	if(pszRemainder == strstr(pszRemainder,"Zrotation")) iret = BC_Zrotation;
	if(pszRemainder == strstr(pszRemainder,"Xrotation")) iret = BC_Xrotation;
	if(pszRemainder == strstr(pszRemainder,"Yrotation")) iret = BC_Yrotation;

	if(0 <= iret)
		aoutNewRemainder = pszRemainder + 9;

	return iret;
}

bool CCamImport::GetCamPositon(float fTimeOfs, float outCamdata[6])
{
	char * pc;

	if(!m_Active || !outCamdata)
		return false; // not active

	fTimeOfs -= m_BaseTime;

	// calc targetframe:
	int iCurFrame = (int)(fTimeOfs / m_FrameTime);
	if(iCurFrame < 0 || iCurFrame >= m_Frames)
		return false; // out of range

	if(	iCurFrame == m_LastFrame)
	{
		pEngfuncs->Con_DPrintf("Using cached cam motion frame: %i (%f)\n",iCurFrame,fTimeOfs);
		memcpy(outCamdata,m_Cache,sizeof(m_Cache));
		return true;
	}

	if(0 == iCurFrame)
	{
		// we start at the first frame
		if(fseek(m_File,m_MotionFPos,SEEK_SET))
		{
			// read error
			CloseMotionFile();
			return false;
		}
	} else {
		// seek to frame position:
		pc = 0;
		
		int iSeekFrames;
		if(0 <= m_LastFrame)
			iSeekFrames = iCurFrame - m_LastFrame -1;
		else
			iSeekFrames = iCurFrame;
		
		while(iSeekFrames)
		{
			if(0<iSeekFrames)
			{
				// seek forward
				pc = fgets(ms_readbuff,sizeof(ms_readbuff)/sizeof(char),m_File);
				if(!pc)
				{
					// read error
					CloseMotionFile();
					return false;
				}
				iSeekFrames--;
			}
			else
			{
				// seek backward
				char ac=0;
				bool bReadErr = false;
				int iReadBreaks = 2;

				while(!bReadErr && 0<iReadBreaks)
				{
					if(!bReadErr)
						bReadErr = 0 != fseek(m_File,-1,SEEK_CUR);

					if(!bReadErr)
						bReadErr = 1 != fread(&ac,sizeof(char),1,m_File);

					bReadErr = bReadErr || ftell(m_File)<m_MotionFPos;

					if('\n' == ac)
						iReadBreaks--;

					if(!bReadErr)
						bReadErr = 0 != fseek(m_File,-1,SEEK_CUR);
				}

				bReadErr = bReadErr || 0 != fseek(m_File,+1,SEEK_CUR);

				if(bReadErr)
				{
					// read error
					CloseMotionFile();
					return false;
				}

				iSeekFrames++;
			}
		}
	}

	// read current frame:
	pc = fgets(ms_readbuff,sizeof(ms_readbuff)/sizeof(char),m_File);
	if(!pc)
	{
		// read error
		CloseMotionFile();
		return false;
	}

	m_LastFrame = iCurFrame;

	// decode frame:
	int ichan;
	char tc = 0;
	char * pc2;
	float fff;

	pc = ms_readbuff;
	for(ichan = 0; ichan < 6; ichan++)
	{
		pc2 = strchr(pc,' ');
		if(!pc2)
			pc2 = strchr(pc,'\t');
		if(!pc2)
			pc2 = pc + strlen(pc);

		tc = *pc2;
		*pc2 = 0;

		fff = 0;
		fff = atof(pc);

		m_Cache[channelcode[ichan]] = fff;

		*pc2 = tc;
		pc = pc2;

		while(0 != *pc && (' ' == *pc || '\t' == *pc)) pc++;
	}

	pEngfuncs->Con_DPrintf("Imported cam motion frame: %i (%f)\n",iCurFrame,fTimeOfs);
	memcpy(outCamdata,m_Cache,sizeof(m_Cache));
	return true;
}


bool CCamImport::IsActive()
{
	return m_Active;
}

bool CCamImport::LoadMotionFile(char const * pszFileName)
{
	char * pc;
	char * pc2;

	if(m_Active)
		CloseMotionFile();

	m_File = fopen(pszFileName,"rb+");

	if(!m_File)
		return false;

	// check if this could be a valid BVH file:
	pc = fgets(ms_readbuff,sizeof(ms_readbuff)/sizeof(char),m_File);
	if(!pc || strcmp(ms_readbuff,"HIERARCHY\n"))
	{
		fclose(m_File);
		return false;
	}

	// skip till first channels entry:
	pc2 = 0;
	while(!pc2)
	{
		pc = fgets(ms_readbuff,sizeof(ms_readbuff)/sizeof(char),m_File);
		if(!pc)
		{
			fclose(m_File);
			return false;
		}

		pc2 = strstr(ms_readbuff,"CHANNELS 6 ");
	}

	// determine channel assignment:
	pc2 += strlen("CHANNELS 6 ");

	for(int i=0; i<6; i++) channelcode[i]=-1;

	for(int i=0; i <6; i++)
	{
		int icode = DecodeBvhChannel(pc2,pc2);

		if(0 <= icode)
			channelcode[i]=icode;
	}

	for(int i=0; i<6; i++)
	{
		if(channelcode[i]<0)
		{
			fclose(m_File);
			return false;
		}
	}

	// skip till MOTION entry:
	pc2 = 0;
	while(!pc2)
	{
		pc = fgets(ms_readbuff,sizeof(ms_readbuff)/sizeof(char),m_File);
		if(!pc)
		{
			fclose(m_File);
			return false;
		}

		pc2 = strstr(ms_readbuff,"MOTION\n");
	}

	// read frames:
	pc = fgets(ms_readbuff,sizeof(ms_readbuff)/sizeof(char),m_File);
	if(!pc || strcmp(ms_readbuff,"Frames:") <= 0)
	{
		fclose(m_File);
		return false;
	}
	pc += strlen("Frames:");
	m_Frames = atoi(pc);

	// read frame time:
	pc = fgets(ms_readbuff,sizeof(ms_readbuff)/sizeof(char),m_File);
	if(!pc || strcmp(ms_readbuff,"Frame Time:") <= 0)
	{
		fclose(m_File);
		return false;
	}
	pc += strlen("Frame Time:");
	m_FrameTime = atof(pc);
	if(m_FrameTime <= 0)
	{
		fclose(m_File);
		return false;
	}

	m_MotionFPos = ftell(m_File);
	m_LastFrame = -1;
	m_Active = true;

	return true;
}

float CCamImport::GetBaseTime()
{
	return m_BaseTime;
}

void CCamImport::SetBaseTime(float fBaseTime)
{
	m_BaseTime = fBaseTime;
}

char CCamImport::ms_readbuff[200];

CCamImport::~CCamImport()
{
	CloseMotionFile();
}


