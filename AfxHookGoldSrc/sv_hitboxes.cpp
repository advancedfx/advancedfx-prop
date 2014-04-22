#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2014-04-22 dominik.matrixstorm.com
//
// First changes
// 2014-04-21 dominik.matrixstorm.com

#include "sv_hitboxes.h"

#include <hlsdk.h>

#include "hooks/HookHw.h"
#include "hl_addresses.h"

#include "cmdregister.h"

REGISTER_CVAR(draw_sv_hitboxes, "0", 0);

#include <GL/GL.h>

#include <list>

typedef struct temp_box_s {
	float data[8][3];
} temp_box_t;

std::list<temp_box_t> tempBoxes;

//enginefuncs_t * g_pSvEnginefuncs;

bool Install_Hooks(void)
{
	static bool firstRun = true;
	static bool firstResult = false;
	if(!firstRun) return firstResult;
	firstRun = false;

//	g_pSvGlobals = (globalvars_t *)AFXADDR_GET(p_sv_globals);
//	g_pSvEnginefuncs = (enginefuncs_t *)AFXADDR_GET(p_sv_enginefuncs);
//	g_pBoneTransform = (BoneTransform_t *)AFXADDR_GET(p_sv_bonetransform);

	firstResult = true;
	return firstResult;
}


void DrawBox(temp_box_t p, double red, double green, double blue)
{
	glColor3d(red, green, blue);

	glBegin(GL_LINES);

	glVertex3f(p.data[0][0], p.data[0][1], p.data[0][2]);
	glVertex3f(p.data[1][0], p.data[1][1], p.data[1][2]);

	glVertex3f(p.data[1][0], p.data[1][1], p.data[1][2]);
	glVertex3f(p.data[3][0], p.data[3][1], p.data[3][2]);

	glVertex3f(p.data[3][0], p.data[3][1], p.data[3][2]);
	glVertex3f(p.data[2][0], p.data[2][1], p.data[2][2]);

	glVertex3f(p.data[2][0], p.data[2][1], p.data[2][2]);
	glVertex3f(p.data[0][0], p.data[0][1], p.data[0][2]);

	glVertex3f(p.data[4][0], p.data[4][1], p.data[4][2]);
	glVertex3f(p.data[5][0], p.data[5][1], p.data[5][2]);

	glVertex3f(p.data[5][0], p.data[5][1], p.data[5][2]);
	glVertex3f(p.data[7][0], p.data[7][1], p.data[7][2]);

	glVertex3f(p.data[7][0], p.data[7][1], p.data[7][2]);
	glVertex3f(p.data[6][0], p.data[6][1], p.data[6][2]);

	glVertex3f(p.data[6][0], p.data[6][1], p.data[6][2]);
	glVertex3f(p.data[4][0], p.data[4][1], p.data[4][2]);

	glVertex3f(p.data[0][0], p.data[0][1], p.data[0][2]);
	glVertex3f(p.data[4][0], p.data[4][1], p.data[4][2]);

	glVertex3f(p.data[1][0], p.data[1][1], p.data[1][2]);
	glVertex3f(p.data[5][0], p.data[5][1], p.data[5][2]);

	glVertex3f(p.data[3][0], p.data[3][1], p.data[3][2]);
	glVertex3f(p.data[7][0], p.data[7][1], p.data[7][2]);

	glVertex3f(p.data[2][0], p.data[2][1], p.data[2][2]);
	glVertex3f(p.data[6][0], p.data[6][1], p.data[6][2]);

	glEnd();	
}

//
// >>>> math functions
// Copyright (c) by Valve Software.

#define DotProduct(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])

void VectorTransform (const float *in1, float in2[3][4], float *out)
{
	out[0] = DotProduct(in1, in2[0]) + in2[0][3];
	out[1] = DotProduct(in1, in2[1]) + in2[1][3];
	out[2] = DotProduct(in1, in2[2]) + in2[2][3];
}

// <<<< math functions
//

void Draw_SV_Hitboxes(void)
{
	if(!draw_sv_hitboxes->value) return;

	GLboolean depth = glIsEnabled(GL_DEPTH_TEST);
	GLboolean text = glIsEnabled(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);

	for(std::list<temp_box_t>::iterator it = tempBoxes.begin(); it != tempBoxes.end(); it++)
	{
		DrawBox(*it, 1.0, 0.0, 0.0);
	}

	tempBoxes.clear();

	if(text) glEnable(GL_TEXTURE_2D);
	if(!depth) glDisable(GL_DEPTH_TEST);
}

void FetchHitboxes(struct server_studio_api_s *pstudio, float (*bonetransform)[MAXSTUDIOBONES][3][4], struct model_s *pModel, const edict_t *pEdict)
{
	Install_Hooks();

	if(!draw_sv_hitboxes->value) return;

	studiohdr_t *pstudiohdr = (studiohdr_t *)pstudio->Mod_Extradata(pModel);

	if(!pstudiohdr)
	{
		pEngfuncs->Con_Printf("Error: no model for pEdict (serialnumber=%i)\n",pEdict->serialnumber);
		return;
	}
	
	mstudiobbox_t *pbbox;
	vec3_t tmp;
	temp_box_t p;
	int i,j;

	pbbox = (mstudiobbox_t *)((byte *)pstudiohdr+ pstudiohdr->hitboxindex);

	for (i = 0; i < pstudiohdr->numhitboxes; i++)
	{
		/*
		pEngfuncs->Con_Printf("((%f, %f, %f),(%f, %f, %f))\n",
			pbbox[i].bbmin[0], pbbox[i].bbmin[1], pbbox[i].bbmin[2],
			pbbox[i].bbmax[0], pbbox[i].bbmax[1], pbbox[i].bbmax[2]
		);
		*/

		//get the vector positions of the 8 corners of the bounding box
		for (j = 0; j < 8; j++)
		{
			tmp[0] = (j & 1) ? pbbox[i].bbmin[0] : pbbox[i].bbmax[0];
			tmp[1] = (j & 2) ? pbbox[i].bbmin[1] : pbbox[i].bbmax[1];
			tmp[2] = (j & 4) ? pbbox[i].bbmin[2] : pbbox[i].bbmax[2];

			VectorTransform( tmp, (*bonetransform)[pbbox[i].bone], p.data[j] );
		}

		tempBoxes.push_back(p);
	}
}