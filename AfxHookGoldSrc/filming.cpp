#include "stdafx.h"

#include "filming.h"

#include <iomanip>
#include <list>
#include <sstream>

#include <shared/detours.h>
#include <shared/FileTools.h>
#include <shared/StringTools.h>
#include <shared/RawOutput.h>

#include <hlsdk.h>
#include <modules/ModInfo.h>

#include "hooks/user32Hooks.h"
#include "hooks/hw/Mod_LeafPvs.h"

#include "AfxGoldSrcComClient.h"
#include "supportrender.h"
#include "camimport.h"
#include "camexport.h"
#include "cmdregister.h"
#include "hl_addresses.h"
#include "mirv_glext.h"
#include "mirv_scripting.h"


using namespace std;


extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s *pEngStudio;
extern playermove_s *ppmove;

extern float clamp(float, float, float);

REGISTER_DEBUGCVAR(camera_test, "0", 0);


REGISTER_DEBUGCVAR(depth_bpp, "8", 0);
REGISTER_DEBUGCVAR(depth_slice_lo, "0.0", 0);
REGISTER_DEBUGCVAR(depth_slice_hi, "1.0", 0);
REGISTER_DEBUGCVAR(gl_force_noztrick, "1", 0);
REGISTER_DEBUGCVAR(sample_overlap, "0.0", 0);
REGISTER_DEBUGCVAR(sample_overlapfuture, "0.0", 0);
REGISTER_DEBUGCVAR(sample_ffunc, "0", 0);
REGISTER_DEBUGCVAR(sample_smethod, "1", 0);
REGISTER_DEBUGCVAR(print_frame, "0", 0);
REGISTER_DEBUGCVAR(print_pos, "0", 0);

REGISTER_CVAR(camexport_mode, "0", 0);
REGISTER_CVAR(crop_height, "-1", 0);
REGISTER_CVAR(crop_yofs, "-1", 0);
REGISTER_CVAR(depth_streams, "3", 0);
REGISTER_CVAR(fx_lightmap, "0", 0);
REGISTER_CVAR(fx_wh_enable, "0", 0);
REGISTER_CVAR(fx_wh_alpha, "0.5", 0);
REGISTER_CVAR(fx_wh_additive, "1", 0);
REGISTER_CVAR(fx_wh_noquads, "0", 0);
REGISTER_CVAR(fx_wh_tint_enable, "0", 0);
REGISTER_CVAR(fx_wh_xtendvis, "1", 0);
REGISTER_CVAR(fx_xtendvis, "0", 0);

REGISTER_CVAR(matte_entityquads, "2", 0);
REGISTER_CVAR(matte_method, "1", 0);
REGISTER_CVAR(matte_particles, "2", 0);
REGISTER_CVAR(matte_viewmodel, "2", 0);
REGISTER_CVAR(matte_worldmodels, "1", 0);
REGISTER_CVAR(matte_xray, "0", 0);

REGISTER_CVAR(movie_clearscreen, "0", 0);
REGISTER_CVAR(movie_bmp, "1", 0);
REGISTER_CVAR(movie_depthdump, "0", 0);
REGISTER_CVAR(movie_export_sound, "0", 0); // should default to 1, but I don't want to mess up other updates
REGISTER_CVAR(movie_filename, "untitled_rec", 0);
REGISTER_CVAR(movie_fps, "30", 0);
REGISTER_CVAR(movie_separate_hud, "0", 0);
REGISTER_CVAR(movie_simulate, "0", 0);
REGISTER_CVAR(movie_simulate_delay, "0", 0);
REGISTER_CVAR(movie_sound_volume, "0.4", 0); // volume 0.8 is CS 1.6 default
REGISTER_CVAR(movie_stereomode,"0",0);
REGISTER_CVAR(movie_stereo_centerdist,"1.3",0);
REGISTER_CVAR(movie_stereo_yawdegrees,"0.0",0);
REGISTER_CVAR(movie_splitstreams, "0", 0);
REGISTER_CVAR(movie_wireframe, "0", 0);
REGISTER_CVAR(movie_wireframesize, "1", 0);
REGISTER_CVAR(sample_enable, "0", 0);
REGISTER_CVAR(sample_sps, "180", 0);


// Our filming singleton
Filming g_Filming;


FilmingStreamInfo g_Filming_Stream_Infos[] = {
	{L"all", false},
	{L"world", false},
	{L"entity", false},
	{L"depthall", false},
	{L"depthworld", false},
	{L"hudcolor", false},
	{L"hudalpha", false},
	{L"sampled", false},
	0
};

enum FilmingStreamInfoType {
	FSIT_all,
	FSIT_world,
	FSIT_entity,
	FSIT_depthall,
	FSIT_depthworld,
	FSIT_hudcolor,
	FSIT_hudalpha,
	FSIT_sampled
};

//
//
//

typedef void (*R_RenderView__t)( void );

R_RenderView__t	detoured_R_RenderView_=NULL;

// asm related definitons we will use:
#define asmNOP 0x90 // opcode for NOP
#define asmJMP	0xE9 // opcode for JUMP
#define JMP32_SZ 5	// the size of JMP <address>

bool bHudToursInstalled=false;

LPVOID g_lpCode_TourIn_Continue = 0;
LPVOID g_lpCode_TourOut_Continue = 0;
LPVOID g_lpCode_HudTours_Loop = 0;

__declspec(naked) void tour_HudBegin()
{
	__asm
	{
		PUSH ECX ; used by c++ to get g_Filming addr
	}
	g_Filming.OnHudBeginEvent();
	__asm
	{
		POP ECX
		JMP [g_lpCode_TourIn_Continue]
	}
}

__declspec(naked) void tour_HudEnd()
{
	__asm
	{
		PUSH EAX ; c++ ret value
		PUSH ECX ; used by c++ to get g_Filming addr
	}
	if(g_Filming.OnHudEndEvnet())
	{
		__asm {
			POP ECX
			POP EAX
			JMP [g_lpCode_HudTours_Loop]
		}
	} else {
		__asm {
			POP ECX
			POP EAX
			JMP [g_lpCode_TourOut_Continue]
		}
	}
}

// this function installs the tours:
void install_Hud_tours()
// this func also checks if the hook has been already installedo or not.
{
	if (bHudToursInstalled) // don't do anything if already hooked
		return;

	bHudToursInstalled=true;

	unsigned char ucTemp;
	DWORD dwTemp;

	//
	//	Get access to code where detours will be applied:
	//

	MdtMemBlockInfos mbisIn, mbisOut;
	LPVOID pCodeTourIn = (LPVOID)HL_ADDR_GET(HUD_TOURIN);
	LPVOID pCodeTourOut = (LPVOID)HL_ADDR_GET(HUD_TOUROUT);
	size_t dwCodeSizeIn = 0x05;
	size_t dwCodeSizeOut = 0x05;

	MdtMemAccessBegin( pCodeTourIn, dwCodeSizeIn, &mbisIn );
	MdtMemAccessBegin( pCodeTourOut, dwCodeSizeOut, &mbisOut );

	//

	g_lpCode_HudTours_Loop = pCodeTourIn; // FILL IN ADDRESS

	//
	//	detour In:
	//

	// create continue code:

	// get mem that is never freed:
	LPVOID pDetouredCodeIn = (LPVOID)MdtAllocExecuteableMemory(dwCodeSizeIn + JMP32_SZ);

	g_lpCode_TourIn_Continue = pDetouredCodeIn; // FILL IN ADDRESS

	// copy the original mov instruction:
	memcpy(pDetouredCodeIn,pCodeTourIn,dwCodeSizeIn);

	// create jump back to continue in original code:
	ucTemp = asmJMP;
	memcpy((unsigned char *)pDetouredCodeIn + dwCodeSizeIn,&ucTemp,1);
	dwTemp = (DWORD)pCodeTourIn - (DWORD)pDetouredCodeIn  - JMP32_SZ;
	memcpy((unsigned char *)pDetouredCodeIn + dwCodeSizeIn + 1,&dwTemp,4);

	// detour original code to jump to the tour_HudBegin func:
	ucTemp = asmJMP;
	memcpy(pCodeTourIn,&ucTemp,1);
	dwTemp=(DWORD)&tour_HudBegin - (DWORD)pCodeTourIn - JMP32_SZ;
	memcpy((unsigned char *)pCodeTourIn+1,&dwTemp,4);

	//
	//	detour Out:
	//

	// create continue code:

	// get mem that is never freed:
	LPVOID pDetouredCodeOut = (LPVOID)MdtAllocExecuteableMemory(dwCodeSizeOut + JMP32_SZ);

	g_lpCode_TourOut_Continue = pDetouredCodeOut; // FILL IN ADDRESS

	// copy the original function call:
	memcpy(pDetouredCodeOut,pCodeTourOut,dwCodeSizeOut);

	// patch the call address:
	memcpy(&dwTemp,(unsigned char*)pDetouredCodeOut+1,4),
	dwTemp -= (DWORD)pDetouredCodeOut - (DWORD)pCodeTourOut;
	memcpy((unsigned char*)pDetouredCodeOut+1,&dwTemp,4);

	// create jump back to continue in original code:
	ucTemp = asmJMP;
	memcpy((unsigned char *)pDetouredCodeOut + dwCodeSizeOut,&ucTemp,1);
	dwTemp = (DWORD)pCodeTourOut - (DWORD)pDetouredCodeOut  - JMP32_SZ;
	memcpy((unsigned char *)pDetouredCodeOut + dwCodeSizeOut + 1,&dwTemp,4);

	// detour original code to jump to the tour_HudBegin func:
	ucTemp = asmJMP;
	memcpy(pCodeTourOut,&ucTemp,1);
	dwTemp=(DWORD)&tour_HudEnd - (DWORD)pCodeTourOut - JMP32_SZ;
	memcpy((unsigned char *)pCodeTourOut+1,&dwTemp,4);

	//
	//	Restore code access:
	//

	MdtMemAccessEnd(&mbisOut);
	MdtMemAccessEnd(&mbisIn);

	//
	// print Debug info:

	pEngfuncs->Con_DPrintf(
		"Detoured in: 0x%08x\n"
		"Detoured out: 0x%8x\n",
		(DWORD)pCodeTourIn,
		(DWORD)pCodeTourOut
	);
}

// << Hooking the HUD functions  <<

// BEGIN from ID Software's Quake 1 Source:

// q1source/QW/client/mathlib.h
// our hl includes already give us that:
//typedef float vec_t;
//typedef vec_t vec3_t[3];

// q1source/QW/client/vid.h
typedef struct vrect_s
{
	int				x,y,width,height;
	struct vrect_s	*pnext;
} vrect_t;

// q1source/QW/client/render.h
// !!! if this is changed, it must be changed in asm_draw.h too !!!
typedef struct
{
	vrect_t		vrect;				// subwindow in video for refresh
									// FIXME: not need vrect next field here?
	vrect_t		aliasvrect;			// scaled Alias version
	int			vrectright, vrectbottom;	// right & bottom screen coords
	int			aliasvrectright, aliasvrectbottom;	// scaled Alias versions
	float		vrectrightedge;			// rightmost right edge we care about,
										//  for use in edge list
	float		fvrectx, fvrecty;		// for floating-point compares
	float		fvrectx_adj, fvrecty_adj; // left and top edges, for clamping
	int			vrect_x_adj_shift20;	// (vrect.x + 0.5 - epsilon) << 20
	int			vrectright_adj_shift20;	// (vrectright + 0.5 - epsilon) << 20
	float		fvrectright_adj, fvrectbottom_adj;
										// right and bottom edges, for clamping
	float		fvrectright;			// rightmost edge, for Alias clamping
	float		fvrectbottom;			// bottommost edge, for Alias clamping
	float		horizontalFieldOfView;	// at Z = 1.0, this many X is visible 
										// 2.0 = 90 degrees
	float		xOrigin;			// should probably allways be 0.5
	float		yOrigin;			// between be around 0.3 to 0.5

	vec3_t		vieworg;
	vec3_t		viewangles;

	float		fov_x, fov_y;
	
	int			ambientlight;
} refdef_t;

// END from ID Software's Quake 1 Source.

// from HL1SDK/multiplayer/common/mathlib.h:
#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

void touring_R_RenderView_(void)
// this is our R_RemderView hook
// pay attention, cuz it will have heavy interaction with our filming singelton!
{

	refdef_t* p_r_refdef=(refdef_t*)HL_ADDR_GET(r_refdef); // pointer to r_refdef global struct

	static vec3_t oldorigin;
	static vec3_t oldangles;
	
	// save original values
	memcpy (oldorigin,p_r_refdef->vieworg,3*sizeof(float));
	memcpy (oldangles,p_r_refdef->viewangles,3*sizeof(float));

	g_Filming.OnR_RenderView(p_r_refdef->vieworg, p_r_refdef->viewangles);

	bool bLoop;
	do {
		bLoop = false;
		if(ScriptEvent_OnRenderViewBegin()) {
			//
			// call original R_RenderView_
			//

			g_ModInfo.SetIn_R_Renderview(true);
			detoured_R_RenderView_();
			g_ModInfo.SetIn_R_Renderview(false);

			bLoop = ScriptEvent_OnRenderViewEnd();
		}

		// restore original values
		memcpy (p_r_refdef->vieworg,oldorigin,3*sizeof(float));
		memcpy (p_r_refdef->viewangles,oldangles,3*sizeof(float));
	} while(bLoop);
}




//
// // // //
//

REGISTER_CMD_FUNC(cameraofs_cs)
{
	if (!detoured_R_RenderView_)
	{
		pEngfuncs->Con_Printf("Usage: " PREFIX "cameraofs_cs will only work when R_RenderView is available (you have to record one time first)\n");
	}

	if (pEngfuncs->Cmd_Argc() == 4)
		g_Filming.SetCameraOfs(atof(pEngfuncs->Cmd_Argv(1)),atof(pEngfuncs->Cmd_Argv(2)),atof(pEngfuncs->Cmd_Argv(3)));
	else
		pEngfuncs->Con_Printf("Usage: " PREFIX "cameraofs_cs <right> <up> <forward>\nNot neccessary for stereo mode, use mirv_movie_stereo instead\n");
}

void Filming::EnsureStreamDirectory(FilmingStreamInfo * streamInfo) {
	if(!streamInfo->dirCreated &&!_bSimulate2)
	{
		std::wstring strDir(m_TakeDir);
		strDir += L"\\";
		strDir += streamInfo->name;

		if(!CreatePath(strDir.c_str(), strDir))
			pEngfuncs->Con_Printf("ERROR: could not create \"%s\"\n", strDir.c_str());

		streamInfo->dirCreated = true;
	}
}


//bool Filming::bNoMatteInterpolation()
//{ return _bNoMatteInterpolation; }

//void Filming::bNoMatteInterpolation (bool bSet)
//{ if (m_iFilmingState == FS_INACTIVE) _bNoMatteInterpolation = bSet; }


void do_camera_test(vec3_t & vieworg, vec3_t & viewangles) {
	static unsigned int state = 0;
	static float angles[3];
	static float ofs[3];

	switch(state) {
	case 1:
		ofs[0] += 2.0f;
		if(360.0f <= ofs[0]) {
			state++;
			ofs[0] = 0;
		}
		break;
	case 2:
		ofs[1] += 2.0f;
		if(360.0f <= ofs[1]) {
			state++;
			ofs[1] = 0;
		}
		break;
	case 3:
		ofs[2] += 2.0f;
		if(360.0f <= ofs[2]) {
			state++;
			ofs[2] = 0;
		}
		break;
	case 4:
		angles[0] += 2.0f;
		if(360.0f <= angles[0]) {
			state++;
			angles[0] = 0;
		}
		break;
	case 5:
		angles[1] += 2.0f;
		if(360.0f <= angles[1]) {
			state++;
			angles[1] = 0;
		}
		break;
	case 6:
		angles[2] += 2.0f;
		if(360.0f <= angles[2]) {
			state++;
			angles[2] = 0;
		}
		break;
	case 7:
		angles[0] -= 2.0f;
		if(-360.0f >= angles[0]) {
			state++;
			angles[0] = 0;
		}
		break;
	case 8:
		angles[1] -= 2.0f;
		if(-360.0f >= angles[1]) {
			state++;
			angles[1] = 0;
		}
		break;
	case 9:
		angles[2] -= 2.0f;
		if(-360.0f >= angles[2]) {
			state++;
			angles[2] = 0;
		}
		break;
	default:
		angles[0] = 0;
		angles[1] = 0;
		angles[2] = 0;
		ofs[0] = 0;
		ofs[0] = 0;
		ofs[0] = 0;

		state = 1;
	};

	viewangles[0] = angles[0];
	viewangles[1] = angles[1];
	viewangles[2] = angles[2];
	vieworg[0] += ofs[0];
	vieworg[1] += ofs[1];
	vieworg[2] += ofs[2];

	if(state < 10) {
		static char szInfo[100];

		if(state < 4)
			_snprintf(szInfo, 100, "ofs=(%f, %f, %f)", ofs[0], ofs[1], ofs[2]);
		else if(state < 10)
			_snprintf(szInfo, 100, "ang=(%f, %f, %f)", angles[0], angles[1], angles[2]);

		pEngfuncs->pfnCenterPrint(szInfo);
	}
	else
		pEngfuncs->pfnCenterPrint("zZz");
}


void Filming::OnR_RenderView(Vector & vieworg, Vector & viewangles) {
	//
	// override by cammotion import:
	if(g_CamImport.IsActive())
	{
		static float ftmp[6];
	
		if(g_CamImport.GetCamPositon(g_Filming.GetDebugClientTime(),ftmp))
		{
			vieworg[1] = -ftmp[0];
			vieworg[2] = +ftmp[1];
			vieworg[0] = -ftmp[2];
			viewangles[ROLL] = -ftmp[3];
			viewangles[PITCH] = -ftmp[4];
			viewangles[YAW] = +ftmp[5];
		}
	}

	// >> begin calculate transform vectors
	// we have to calculate our own transformation vectors from the angles and can not use pparams->forward etc., because in spectator mode they might be not present:
	// (adapted from HL1SDK/multiplayer/pm_shared.c/AngleVectors) and modified for quake order of angles:

	float *angles;
	float forward[3],right[3],up[3];

	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	angles = viewangles;

	angle = angles[YAW] * ((float)M_PI*2 / 360);
	sy = sin((float)angle);
	cy = cos((float)angle);
	angle = angles[PITCH] * ((float)M_PI*2 / 360);
	sp = sin((float)angle);
	cp = cos((float)angle);
	angle = angles[ROLL] * ((float)M_PI*2 / 360);
	sr = sin((float)angle);
	cr = cos((float)angle);

	forward[0] = cp*cy;
	forward[1] = cp*sy;
	forward[2] = -sp;

	right[0] = (-1*sr*sp*cy+-1*cr*-sy);
	right[1] = (-1*sr*sp*sy+-1*cr*cy);
	right[2] = -1*sr*cp;

	up[0] = (cr*sp*cy+-sr*-sy);
	up[1] = (cr*sp*sy+-sr*cy);
	up[2] = cr*cp;

	// << end calculate transform vectors

	// (this code is similar to HL1SDK/multiplayer/cl_dll/view.cpp/V_CalcNormalRefdef)

	//
	// apply displacement :
	{
		float fDispRight, fDispUp, fDispForward;

		g_Filming.GetCameraOfs(&fDispRight,&fDispUp,&fDispForward);

		for ( int i=0 ; i<3 ; i++ )
		{
			vieworg[i] += fDispForward*forward[i] + fDispRight*right[i] + fDispUp*up[i];
		}
	}

	// camera test:
	if(camera_test->value)
		do_camera_test(vieworg, viewangles);

	// export:

	if(g_CamExport.HasFileMain()
		&& m_LastCamFrameMid != m_nFrames
	) {
		m_LastCamFrameMid = m_nFrames;

		g_CamExport.WriteMainFrame(
			-vieworg[1], +vieworg[2], -vieworg[0],
			-viewangles[ROLL], -viewangles[PITCH], +viewangles[YAW]
		);
	}

	//
	// Apply Stereo mode:

	// one note on the stereoyawing:
	// it may look simple, but it is only simple since the H-L engine carrys the yawing out as last rotation and after that translates
	// if this wouldn't be the case, then we would have a bit more complicated calculations to make sure the camera is always rotated around the up axis!

	if (g_Filming.bEnableStereoMode()&&(g_Filming.isFilming()))
	{
		float fDispRight;
		Filming::STEREO_STATE sts =g_Filming.GetStereoState(); 

		if(sts ==Filming::STS_LEFT)
		{
			// left
			fDispRight = movie_stereo_centerdist->value; // left displacement
			viewangles[YAW] -= movie_stereo_yawdegrees->value; // turn right
		}
		else
		{
			// right
			fDispRight = movie_stereo_centerdist->value; // right displacement
			viewangles[YAW] += movie_stereo_yawdegrees->value; // turn left
		}

		// apply displacement:
		for ( int i=0 ; i<3 ; i++ )
		{
			vieworg[i] += fDispRight*right[i];
		}

		// export:
		if(sts == Filming::STS_LEFT
			&& g_CamExport.HasFileLeft()
			&& m_LastCamFrameLeft != m_nFrames
		) {
			m_LastCamFrameLeft = m_nFrames;

			g_CamExport.WriteLeftFrame(
				-vieworg[1], +vieworg[2], -vieworg[0],
				-viewangles[ROLL], -viewangles[PITCH], +viewangles[YAW]
			);
		}
		else if(g_CamExport.HasFileRight()
			&& m_LastCamFrameRight != m_nFrames
		) {
			m_LastCamFrameRight = m_nFrames;

			g_CamExport.WriteRightFrame(
				-vieworg[1], +vieworg[2], -vieworg[0],
				-viewangles[ROLL], -viewangles[PITCH], +viewangles[YAW]
			);
		}

	}


	if(print_pos->value!=0.0)
		pEngfuncs->Con_Printf("(%f,%f,%f) (%f,%f,%f)\n",
			vieworg[0],
			vieworg[1],
			vieworg[2],
			viewangles[PITCH],
			viewangles[YAW],
			viewangles[ROLL]
		);
}



void Filming::SupplyZClipping(GLdouble zNear, GLdouble zFar) {
	m_ZNear = zNear;
	m_ZFar = zFar;
}



bool Filming::bEnableStereoMode()
{ return _bEnableStereoMode; }

void Filming::bEnableStereoMode(bool bSet)
{ if (m_iFilmingState == FS_INACTIVE) _bEnableStereoMode = bSet; }

void Filming::SetCameraOfs(float right, float up, float forward)
{
	if (m_iFilmingState == FS_INACTIVE)
	{
		_cameraofs.right = right;
		_cameraofs.up = up;
		_cameraofs.forward = forward;
	}
}

void Filming::SetStereoOfs(float left_and_rightofs)
{
	if (m_iFilmingState == FS_INACTIVE) _fStereoOffset = left_and_rightofs;
}

Filming::Filming()
// constructor
{
		m_bInWireframe = false;

		// added 20070922:
		//_bNoMatteInterpolation = true;
		_bEnableStereoMode = false;
		_cameraofs.right = 0;
		_cameraofs.up = 0;
		_cameraofs.forward = 0;
		_fStereoOffset = (float)1.27;
		
		// this is currently done globally: _detoured_R_RenderView = NULL; // only hook when requested

		_bRecordBuffers_FirstCall = true; //has to be set here cause it is checked by isFinished()

		// (will be set in Start again)
		_stereo_state = STS_LEFT;

		bWantsHudCapture = false;

		_HudRqState = HUDRQ_NORMAL;

		_bSimulate = false;
		_bSimulate2 = false;

	_fx_whRGBf[0]=0.0f;
	_fx_whRGBf[1]=0.5f;	
	_fx_whRGBf[2]=1.0f;

	 bRequestingMatteTextUpdate=false;

	 matte_entities_r.bNotEmpty=false; // by default empty

	 m_sampling.bEnable = false; // not enabled by default
}

Filming::~Filming()
// destructor
{
}

void Filming::GetCameraOfs(float *right, float *up, float *forward)
{
	*right = _cameraofs.right;
	*up = _cameraofs.up;
	*forward = _cameraofs.forward;
}

float Filming::GetDebugClientTime()
{
	if(m_iFilmingState != FS_INACTIVE)
		return m_StartClientTime + m_time;

	return pEngfuncs->GetClientTime();
}

Filming::MATTE_METHOD Filming::GetMatteMethod()
{
	return m_MatteMethod;
}

float Filming::GetStereoOffset()
{
	return _fStereoOffset;
}

Filming::STEREO_STATE Filming::GetStereoState()
{
	return _stereo_state;
}

Filming::HUD_REQUEST_STATE Filming::giveHudRqState()
{
	if (!isFilming())
		return HUDRQ_NORMAL;

	return _HudRqState;
}

Filming::MATTE_STAGE Filming::GetMatteStage()
{
	return m_iMatteStage;
}

void Filming::OnHudBeginEvent()
{
	//MessageBox(NULL,"IN","HUD Event",MB_OK);
	switch(giveHudRqState())
	{
	case HUDRQ_CAPTURE_COLOR:
		glClearColor(0.0f,0.0f,0.0f, 1.0f); // don't forget to set our clear color
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		break;
	case HUDRQ_CAPTURE_ALPHA:
		glClearColor(0.0f,0.0f,0.0f, 0.0f); // don't forget to set our clear color
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		break;
	case HUDRQ_HIDE:
		glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
		break;
	}
}

bool Filming::OnHudEndEvnet()
{
	//MessageBox(NULL,"OUT","HUD Event",MB_OK);
	switch(giveHudRqState())
	{
	case HUDRQ_CAPTURE_COLOR:
		if (!_bSimulate2)Capture(&g_Filming_Stream_Infos[FSIT_hudcolor] ,m_nFrames, COLOR);
		if (movie_separate_hud->value!=2.0)
		{
			// we want alpha too in this case
			_HudRqState = HUDRQ_CAPTURE_ALPHA; // set ALPHA mode!
			return true; // we need loop
		}
		break;
	case HUDRQ_CAPTURE_ALPHA:
		if(!_bSimulate2)Capture(&g_Filming_Stream_Infos[FSIT_hudalpha], m_nFrames, ALPHA);
		break;
	}
	return false; // do not loop
}


void OnPrintFrame(unsigned char * data, int number)
{
	g_Filming.OnPrintFrame(data, number);
}

void Filming::setScreenSize(GLint w, GLint h)
{
	if (m_iWidth < w) m_iWidth = w;
	if (m_iHeight < h) m_iHeight = h;
}

void Filming::Start()
{
	_bSimulate = (movie_simulate->value != 0.0);
	_bSimulate2 = _bSimulate && (movie_simulate->value != 2.0);

	// Prepare directories and directory infos:

	for(int i=0; 0 != g_Filming_Stream_Infos[i].name; i++)
		g_Filming_Stream_Infos[i].dirCreated = false;

	if(!_bSimulate2)
	{
		if(AnsiStringToWideString(movie_filename->string, m_TakeDir)
			&& (m_TakeDir.append(L"\\take"), SuggestTakePath(m_TakeDir.c_str(), 4, m_TakeDir))
			&& CreatePath(m_TakeDir.c_str(), m_TakeDir)
		)
		{
			std::string ansiTakeDir;

			pEngfuncs->Con_Printf("Recoding to \"%s\".\n", WideStringToAnsiString(m_TakeDir.c_str(), ansiTakeDir) ? ansiTakeDir.c_str() : "?");

		}
		else
		{
			pEngfuncs->Con_Printf("Error: Failed to create directories for \"%s\".\n", movie_filename->string);
			return; //abort
		}
	}
	else
	{
		pEngfuncs->Con_Printf("Recording (simulated).");
	}

	ScriptEvent_OnRecordStarting();

	g_AfxGoldSrcComClient.OnRecordStarting();

	if(g_AfxGoldSrcComClient.GetOptimizeCaptureVis())
		UndockGameWindowForCapture();

	m_MatteMethod = 2 == matte_method->value ? MM_ALPHA : MM_KEY;

	if(MM_ALPHA == m_MatteMethod && !g_Has_GL_ARB_multitexture)
	{
		pEngfuncs->Con_Printf("ERROR: Alpha mattes not supported by your setup.\n");
		m_MatteMethod = MM_KEY;
	}

	m_fps = max(movie_fps->value,1.0f);
	m_time = 0;

	if (_pSupportRender)
		_pSupportRender->hlaeOnFilmingStart();	

	m_nFrames = 0;
	
	m_LastCamFrameMid = -1;
	m_LastCamFrameLeft = -1;
	m_LastCamFrameRight = -1;

	m_StartClientTime = pEngfuncs->GetClientTime();
	m_iFilmingState = FS_STARTING;
	m_iMatteStage = MS_WORLD;

	// retrive some cvars:
	_fStereoOffset = movie_stereo_centerdist->value;
	_bEnableStereoMode = (movie_stereomode->value != 0.0); // we also have to be able to use R_RenderView
	//_bNoMatteInterpolation = (matte_nointerp->value == 0.0);


	// Mod_LeafPvs (WallHack related):
	g_Mod_LeafPvs_NoVis = ( fx_wh_enable->value == 0.0f && fx_xtendvis->value != 0.0f)||(fx_wh_enable->value != 0.0f && fx_wh_xtendvis->value != 0.0f);


	// setup camexport:
	if (!_bSimulate2) {
		float fc = camexport_mode->value;
		float frameTime = 1.0f / m_fps;

		if(fc && fc != 2.0f) g_CamExport.BeginFileMain(m_TakeDir.c_str(), frameTime);
		if(fc && fc >= 2.0f) g_CamExport.BeginFileLeft(m_TakeDir.c_str(), frameTime);
		if(fc && fc >= 2.0f) g_CamExport.BeginFileRight(m_TakeDir.c_str(), frameTime);
	}

	// if we want to use R_RenderView and we have not already done that, we need to set it up now:

	if ( !detoured_R_RenderView_ )
	{
		// we don't have it yet and the addres is not NULL (which might be an intended cfg setting)
		detoured_R_RenderView_ = (R_RenderView__t) DetourApply((BYTE *)HL_ADDR_GET(R_RenderView), (BYTE *)touring_R_RenderView_, (int)HL_ADDR_GET(DTOURSZ_R_RenderView));
		install_Hud_tours(); // wil automaticall check if already installed or not
	}

	// make sure some states used in recordBuffers are set properly:
	_HudRqState = HUDRQ_NORMAL;
	_stereo_state = STS_LEFT;
	_bRecordBuffers_FirstCall = true;
	bWantsHudCapture = false;

	// Init Cropping:
	// retrive cropping settings and make sure the values are in bounds we can work with:
	// -1 means the code will try to act as if this value was default (default ofs / default max height)

	int iTcrop_yofs=(int)crop_yofs->value;

	m_iCropHeight = (int)crop_height->value;
	if (m_iCropHeight == -1) m_iCropHeight = m_iHeight; // default is maximum height we know, doh :O
	else
	{
		// make sure we are within valid bounds to avoid mem acces errors and potential problems with badly coded loops :P
		// may be someone can optimize this:
		if (m_iCropHeight > m_iHeight) m_iCropHeight=m_iHeight;
		else if (m_iCropHeight < 2)  m_iCropHeight=2;
	}

	if (iTcrop_yofs==-1) m_iCropYOfs = (m_iHeight-m_iCropHeight)/2; // user wants that we center the height-crop (this way we preffer cutting off top lines (OpenGL y-axis!) if the number of lines is uneven)
	else {
		int iTHeightDiff = (m_iHeight - m_iCropHeight);
		// user specified an offset, we will transform the values for him, so he sees Yofs/-axis as top->down while OpenGL handles it down->up
		m_iCropYOfs  =  iTHeightDiff - iTcrop_yofs; //GL y-axis is down->up

		// may be someone can optimize this:
		if (m_iCropYOfs > iTHeightDiff) m_iCropYOfs=iTHeightDiff;
		else if (m_iCropYOfs<0) m_iCropYOfs=0;
	}


	// prepare (and force) some HL engine settings:

	// Clear up the screen
	if (movie_clearscreen->value != 0.0f)
	{
		pEngfuncs->Cvar_SetValue("hud_draw", 0);
		pEngfuncs->Cvar_SetValue("crosshair", 0);
	}

	//
	// gl_ztrick:
	// we force it to 0 by default, cause otherwise it could suppress gl_clear and mess up, see ID SOftware's Quake 1 source for more info why this has to be done
	if (gl_force_noztrick->value)
		pEngfuncs->Cvar_SetValue("gl_ztrick", 0);

	// well for some reason gavin forced gl_clear 1, but we don't relay on it anyways (which is good, cause the in ineye demo mode the engine will reforce it to 0 anyways):
	pEngfuncs->Cvar_SetValue("gl_clear", 1); // this needs should be reforced somwhere since in ineydemo mode the engine might force it to 0

	// indicate sound export if requested:
	_bExportingSound = !_bSimulate2 && (movie_export_sound->value!=0.0f);

	
	// start up sampling system:
	m_sampling.bEnable = 1 == sample_enable->value && 0 == movie_splitstreams->value && 0 == movie_depthdump->value && 0 == movie_separate_hud->value;
	if( m_sampling.bEnable )
	{
		m_fps = max(sample_sps->value,1.0f);
		m_sampling.out_fps = max(movie_fps->value,1.0f);

		float frameDuration = 1.0f / m_sampling.out_fps; 

		m_sampling.sampler = new EasyBgrSampler(
			m_iWidth,
			m_iCropHeight,
			movie_bmp->value ? 4 : 1,
			0 == sample_smethod->value ? EasyBgrSampler::ESM_Rectangle : EasyBgrSampler::ESM_Trapezoid,
			0 == sample_ffunc->value ? EasyBgrSampler::RectangleWeighter : EasyBgrSampler::GaussWeighter,
			-sample_overlapfuture->value * frameDuration, +sample_overlap->value * frameDuration,
			&::OnPrintFrame,
			frameDuration
		);
	}

	g_ModInfo.SetRecording(true);
}

void Filming::Stop()
{
	g_ModInfo.SetRecording(false);

	if (_pSupportRender)
		_pSupportRender->hlaeOnFilmingStop();

	// stop camera motion export:
	g_CamExport.EndFiles();
	
	// stop sound system if exporting sound:
	if (_bExportingSound)
	{
		_FilmSound.Stop();
		_bExportingSound = false;
	}

	// shutdown sampling system if it was enabled:
	if( m_sampling.bEnable )
	{
		pEngfuncs->Con_Printf("Peak count of frames lingering in sampler: %i\n", m_sampling.sampler->GetPeakFrameCount());

		delete m_sampling.sampler;
		m_sampling.bEnable = false;
	}

	m_iFilmingState = FS_INACTIVE;
	_HudRqState=HUDRQ_NORMAL;

	// Need to reset this otherwise everything will run crazy fast
	pEngfuncs->Cvar_SetValue("host_framerate", 0);

	// in case our code is broken [again] we better also reset the mask here: : )
	glColorMask(TRUE, TRUE, TRUE, TRUE);

	if(g_AfxGoldSrcComClient.GetOptimizeCaptureVis())
		RedockGameWindow();

	g_AfxGoldSrcComClient.OnRecordEnded();

	ScriptEvent_OnRecordEnded();
}

void Filming::OnPrintFrame(unsigned char * data, int number)
{
	FilmingStreamInfo * streamInfo = &g_Filming_Stream_Infos[FSIT_sampled];

	bool bBMP = 0.0f != movie_bmp->value;

	// only supports BGR data atm!:
	char cDepth = 2;
	int nBits = 24;

	EnsureStreamDirectory(streamInfo);

	std::wostringstream os;
	os << m_TakeDir << L"\\"  << streamInfo->name << L"\\" << setfill(L'0') << setw(5) << number << setw(0) << (bBMP ? L".bmp" : L".tga");

	
	if( bBMP )
		WriteRawBitmap((unsigned char *)data, os.str().c_str(), m_iWidth, m_iCropHeight, nBits,
			CalcPitch(m_iWidth, 3, 4)); // align is still 4 byte probably
	else
		WriteRawTarga((unsigned char *)data, os.str().c_str(), m_iWidth, m_iCropHeight, nBits,
			false); // always color
}


void LinearizeFloatDepthBuffer(GLfloat *pBuffer, unsigned int count, GLdouble zNear, GLdouble zFar) {

	GLfloat f = (GLfloat)zFar;
	GLfloat n = (GLfloat)zNear;
	GLfloat w = 1.0f;

	GLfloat f1 = (-1)*f*n*w;
	GLfloat f2 = f-n;

	for(; count; count--) {
		float t = f1/((*pBuffer)*f2-f);
		*pBuffer = (t-n)/f2;
		pBuffer++;
	}
}

void LogarithmizeDepthBuffer(GLfloat *pBuffer, unsigned int count, GLdouble zNear, GLdouble zFar) {
	GLfloat N  = (GLfloat)zNear;
	GLfloat yL = log((GLfloat)zNear);
	GLfloat yD = log((GLfloat)zFar) -yL;
	GLfloat xD = (GLfloat)zFar - (GLfloat)zNear;

	for(; count; count--) {
		*pBuffer = (log(xD*(*pBuffer) + N) -yL)/yD;
		pBuffer++;
	}

}

void DebugDepthBuffer(GLfloat *pBuffer, unsigned int count) {
	for(; count; count--) {
		float t = *pBuffer;
		if(t<0.0f) *pBuffer = 0.0f;
		else if(1.0f < t) *pBuffer = 1.0f;
		else *pBuffer = 0.5f;
		pBuffer++;
	}
}

void SliceDepthBuffer(GLfloat *pBuffer, unsigned int count, GLfloat sliceLo, GLfloat sliceHi) {

	if(!(
		0.0f <= sliceLo
		&& sliceLo < sliceHi
		&& sliceHi <= 1.0f
		&& (0.0f != sliceLo || 1.0f != sliceHi)
	))
		return; // no valid slicing range

	GLfloat s = 1.0f/(sliceHi - sliceLo);

	for(; count; count--) {
		GLfloat t = (*pBuffer);
		
		// clamp
		if(t<sliceLo) t = sliceLo;
		else if(sliceHi < t) t = sliceHi;
		
		*pBuffer = s*(t-sliceLo); // and scale
		
		pBuffer++;
	}
}

// Constraints: 
// - assumes the GLfloat buffer to contain values in [0.0f,1.0f] v
// - assumes GLfloat to conform with IEEE 754-2008 binary32
// - componentBytes \in 1,2,3
void GLfloatArrayToXByteArray(GLfloat *pBuffer, unsigned int width, unsigned int height, unsigned char componentBytes) {
	__asm {
		MOV  EBX, height
		TEST EBX, EBX
		JZ   __Done ; height 0

		MOV  EBX, width
		TEST EBX, EBX
		JZ   __Done ; width 0

		MOV  ESI, pBuffer
		MOV  EDI, ESI

		JMP  __SelectLoop

		__Next8:
			ADD  ESI, 4
			INC  EDI
			DEC  EBX
			JZ   __LineDone

		__Loop8:
			MOV  EAX, [ESI]
			TEST EAX, EAX
			JZ   __Zero8

			MOV  ECX, EAX
			SHR  ECX, 23
			SUB  CL, 127
			NEG  CL
			JZ   __One8

			; value in (0.0f, 1.0f)
			;
			AND  EAX, 0x7FFFFF
			OR   EAX, 0x800000
			SHR  EAX, 15
			SHR  EAX, CL
			MOV  [EDI], AL

			JMP  __Next8

		__Zero8:
			MOV  BYTE PTR [EDI], 0x00
			JMP  __Next8

		__One8:
			MOV  BYTE PTR [EDI], 0xFF
			JMP  __Next8

		__Next16:
			ADD  ESI, 4
			ADD  EDI, 2
			DEC  EBX
			JZ   __LineDone

		__Loop16:
			MOV  EAX, [ESI]
			TEST EAX, EAX
			JZ   __Zero16

			MOV  ECX, EAX
			SHR  ECX, 23
			SUB  CL, 127
			NEG  CL
			JZ   __One16

			; value in (0.0f, 1.0f)
			;
			AND  EAX, 0x7FFFFF
			OR   EAX, 0x800000
			SHR  EAX, 7
			SHR  EAX, CL
			MOV  [EDI], AX

			JMP  __Next16

		__Zero16:
			MOV  WORD PTR [EDI], 0x0000
			JMP  __Next16

		__One16:
			MOV  WORD PTR [EDI], 0xFFFF
			JMP  __Next16

		__Next24:
			ADD  ESI, 4
			ADD  EDI, 3
			DEC  EBX
			JZ   __LineDone

		__Loop24:
			MOV  EAX, [ESI]
			TEST EAX, EAX
			JZ   __Zero24

			MOV  ECX, EAX
			SHR  ECX, 23
			SUB  CL, 127
			NEG  CL
			JZ   __One24

			; value in (0.0f, 1.0f)
			;
			AND  EAX, 0x7FFFFF
			OR   EAX, 0x800000
			SHL  EAX, 1
			SHR  EAX, CL
			MOV  [EDI], AX
			SHR  EAX, 16
			MOV  [EDI+2], AL
			JMP  __Next24

		__Zero24:
			MOV  WORD PTR [EDI], 0x0000
			MOV  BYTE PTR [EDI+2], 0x0000
			JMP  __Next24

		__One24:
			MOV  WORD PTR [EDI], 0xFFFF
			MOV  BYTE PTR [EDI+2], 0xFF
			JMP  __Next24

		__LineDone:
			AND EBX, 0x00000003
			JZ   __LineDoneContinue

			; fix align:
			NEG BL
			ADD BL, 4
			ADD	EDI, EBX

		__LineDoneContinue:
			; check linecount:
			MOV  EBX, height
			DEC  EBX
			JZ   __Done

			MOV  height, ebx
			MOV  EBX, width

		__SelectLoop:
			MOV  AL, componentBytes
			CMP  AL, 1
			JLE  __Loop8
			CMP  AL, 2
			JLE  __Loop16
			JMP  __Loop24

			
		__Done:
	};
}


void Filming::Capture(FilmingStreamInfo * streamInfo, int iFileNumber, BUFFER iBuffer)
{
	if(print_frame->value)
	{
		char stmp[7+33]="Frame: 01234567890123456789012345678901";
		itoa(m_nFrames,stmp+7,10);
		pEngfuncs->pfnCenterPrint(stmp);
	}

	bool bBMP = 0.0f != movie_bmp->value;
	GLenum eGLBuffer;
	GLenum eGLtype;
	unsigned char ucComponentBytes;

	switch(iBuffer) {
	case ALPHA:
		eGLBuffer = GL_ALPHA;
		eGLtype = GL_UNSIGNED_BYTE;
		ucComponentBytes = 1;
		break;

	case DEPTH:
		eGLBuffer = GL_DEPTH_COMPONENT;
		eGLtype = GL_FLOAT;
		switch(unsigned char ucDepthBpp = (unsigned char)depth_bpp->value) {
		case 16:
			ucComponentBytes = 2;
			break;
		case 24:
			ucComponentBytes = 3;
			break;
		default:
			ucComponentBytes = 1;
		}
		break;

	case COLOR:
	default:
		eGLBuffer = GL_BGR_EXT;
		eGLtype = GL_UNSIGNED_BYTE;
		ucComponentBytes = 3;
	};

	bool bSampledStream = m_iMatteStage==MS_ALL && iBuffer == COLOR && m_sampling.bEnable;

	bool bReadOk = m_GlRawPic.DoGlReadPixels(0, m_iCropYOfs, m_iWidth, m_iCropHeight, eGLBuffer, eGLtype, !bBMP);
	if (!bReadOk)
	{
		pEngfuncs->Con_Printf("MDT ERROR: failed to capture frame %05d, Errorcode: %d.\n", m_nFrames, m_GlRawPic.GetLastUnhandledError());
		return; // may be we should code some better error handling here heh
	}

	// apply postprocessing to the depthbuffer:
	// the following code should be replaced completly later, cause it's rather dependent
	// on sizes of unsigned int and float and stuff, although it should be somewhat
	// save from acces violations (only corrupted pixel data):
	// also the GL_UNSIGNED_INT FIX is somewhat slow by now, code has to be optimized
	if (iBuffer==DEPTH)
	{
		// user wants 24 Bit output, we need to cut off
		unsigned int uiCount = (unsigned int)m_iWidth * (unsigned int)m_iCropHeight;
		void * pBuffer=m_GlRawPic.GetPointer();	// the pointer where we write

		unsigned char ucMethod = (unsigned char)movie_depthdump->value;

		if(1==ucMethod||2==ucMethod) LinearizeFloatDepthBuffer((GLfloat *)pBuffer, uiCount, m_ZNear, m_ZFar);
		if(2==ucMethod) LogarithmizeDepthBuffer((GLfloat *)pBuffer, uiCount, m_ZNear, m_ZFar);
		if(0x4 & ucMethod) DebugDepthBuffer((GLfloat *)pBuffer, uiCount);

		float sliceLo = depth_slice_lo->value;
		float sliceHi = depth_slice_hi->value;

		if( 0.0f<=sliceLo
			&& sliceLo<sliceHi
			&& sliceHi<=1.0f
			&& (0.0f != sliceLo || 1.0f != sliceHi)
			)
			SliceDepthBuffer((GLfloat *)pBuffer, uiCount, sliceLo, sliceHi);

		GLfloatArrayToXByteArray((GLfloat *)pBuffer, m_iWidth, m_iCropHeight, ucComponentBytes);
	}

	if( bSampledStream )
	{
		// pass on to sampling system:

		m_sampling.sampler->Sample(
			(unsigned char const *)m_GlRawPic.GetPointer(),
			1.0f / m_fps
		);
	}
	else
	{
		// write out directly:

		bool bColor = iBuffer == COLOR;

		// construct filename:
		wchar_t * pszStereotag = L"";
		if (_bEnableStereoMode&&!bWantsHudCapture)
		{
			// if we are not capturing the hud and are in stereo mode add the proper tag:
			if (_stereo_state==STS_LEFT) pszStereotag=L"left_"; else pszStereotag=L"right_";
		}
		
		EnsureStreamDirectory(streamInfo);

		std::wostringstream os;
		os << m_TakeDir << L"\\"  << streamInfo->name << L"\\" << pszStereotag << setfill(L'0') << setw(5) << iFileNumber << setw(0) << (bBMP ? L".bmp" : L".tga");
	
		if( bBMP )
			WriteRawBitmap(m_GlRawPic.GetPointer(), os.str().c_str(), m_iWidth, m_iCropHeight, ucComponentBytes<<3,
				CalcPitch(m_iWidth, ucComponentBytes, 4)); // align is still 4 byte probably
		else
			WriteRawTarga(m_GlRawPic.GetPointer(), os.str().c_str(), m_iWidth, m_iCropHeight, ucComponentBytes<<3, !bColor);
	}
}

Filming::DRAW_RESULT Filming::shouldDraw(GLenum mode)
{
	bool bMatteXray = matte_xray->value ;
	bool bFilterEntities = matte_entities_r.bNotEmpty;

	int iMatteParticles = (int)matte_particles->value;
	bool bParticleWorld  = 0x01 & iMatteParticles;
	bool bParticleEntity = 0x02 & iMatteParticles;

	int iMatteViewModel = (int)matte_viewmodel->value;
	bool bViewModelWorld  = 0x01 & iMatteViewModel;
	bool bViewModelEntity = 0x02 & iMatteViewModel;

	int iMatteWmodels = (int)matte_worldmodels->value;
	bool bWmodelWorld  = 0x01 & iMatteWmodels;
	bool bWmodelEntity = 0x02 & iMatteWmodels;

	int iMatteEntityQuads = (int)matte_entityquads->value;
	bool bEntityQuadWorld  = 0x01 & iMatteEntityQuads;
	bool bEntityQuadEntity = 0x02 & iMatteEntityQuads;

	// in R_Particles:
	if(g_ModInfo.In_R_DrawParticles_get()) {
		switch(m_iMatteStage) {
		case MS_WORLD:
			return bParticleWorld ? DR_NORMAL : DR_HIDE;
		case MS_ENTITY:
			return bParticleEntity ? DR_NORMAL : (bMatteXray ? DR_HIDE : DR_MASK );
		};
		return bParticleWorld | bParticleEntity ? DR_NORMAL : DR_HIDE;
	}

	// in R_DrawEntitiesOnList:
	else if(g_ModInfo.In_R_DrawEntitiesOnList_get()) {
		cl_entity_t *ce = pEngStudio->GetCurrentEntity();

		if(!ce)
			return DR_NORMAL;

		// Apply entity Filter list:
		if (bFilterEntities) {
			bool bActive = _InMatteEntities(ce->index);
			switch(m_iMatteStage) {
			case MS_WORLD:
				return !bActive ? DR_NORMAL : DR_HIDE;
			case MS_ENTITY:
				return bActive ? DR_NORMAL : (bMatteXray ? DR_HIDE : DR_MASK );
			};
			return bActive ? DR_NORMAL : DR_HIDE;
		}

		// w_* models_:
		else if(ce->model && ce->model->type == mod_brush && strncmp(ce->model->name, "maps/", 5) != 0) {
			switch(m_iMatteStage) {
			case MS_WORLD:
				return bWmodelWorld ? DR_NORMAL : DR_HIDE;
			case MS_ENTITY:
				return bWmodelEntity ? DR_NORMAL : (bMatteXray ? DR_HIDE : DR_MASK );
			};
			return bWmodelWorld | bWmodelEntity ? DR_NORMAL : DR_HIDE;
		}

		// QUADS, such as blood sprites:
		else if(mode == GL_QUADS) {
			switch(m_iMatteStage) {
			case MS_WORLD:
				return bEntityQuadWorld ? DR_NORMAL : DR_HIDE;
			case MS_ENTITY:
				return bEntityQuadEntity ? DR_NORMAL : DR_HIDE;
			};
			return bEntityQuadWorld | bEntityQuadEntity ? DR_NORMAL : DR_HIDE;
		}

		// Everything else in the entity list:
		switch(m_iMatteStage) {
		case MS_WORLD:
			return DR_HIDE;
		case MS_ENTITY:
			return DR_NORMAL;
		};
		return DR_NORMAL;
	}

	// in R_DrawViewModel
	else if(g_ModInfo.In_R_DrawViewModel_get()) {
		switch(m_iMatteStage) {
		case MS_WORLD:
			return bViewModelWorld ? DR_NORMAL : DR_HIDE;
		case MS_ENTITY:
			return bViewModelEntity ? DR_NORMAL : (bMatteXray ? DR_HIDE : DR_MASK );
		};
		return bViewModelWorld | bViewModelEntity ? DR_NORMAL : DR_HIDE;
	}

	// Everything else:
	switch(m_iMatteStage) {
	case MS_WORLD:
		return DR_NORMAL;
	case MS_ENTITY:
		return (bMatteXray ? DR_HIDE : DR_MASK );
	};
	return DR_NORMAL;
}

bool Filming::_InMatteEntities(int iid)
{
	bool bFound=false;

	std::list<int>::iterator iterend = matte_entities_r.ids.end();
	for (std::list<int>::iterator iter = matte_entities_r.ids.begin(); iter != iterend; iter++)
	{
		if (*iter == iid)
		{
			bFound=true;
			break;
		}
	}

	return bFound;
}


bool Filming::recordBuffers(HDC hSwapHDC,BOOL *bSwapRes)
// be sure to read the comments to _bRecordBuffers_FirstCall in filming.h, because this is fundamental for undertanding what the **** is going on here
// currently like the old code we relay on some user changable values, however we should lock those during filming to avoid crashes caused by the user messing around (not implemented yet)
{
	if (!_bRecordBuffers_FirstCall)
	{
		pEngfuncs->Con_Printf("WARNING: Unexpected recordBuffers request, this should not happen!");
	}
	_bRecordBuffers_FirstCall = false;

	m_iMatteStage = MS_ALL;

	if( movie_splitstreams->value >= 1)
		m_iMatteStage = MS_WORLD;

	// If we've only just started, delay until the next scene so that
	// the first frame is drawn correctly
	if (m_iFilmingState == FS_STARTING)
	{
		// we drop this frame and prepare the next one:

		if (movie_separate_hud->value!=0.0)
		{
			bWantsHudCapture = true; // signal for R_RenderView
			m_iMatteStage = MS_ALL; // override matte stage
			_HudRqState = HUDRQ_CAPTURE_COLOR; // signal we want an color capture
		}

		// execute swap already (support render might need preparations for first frame):
		if (_pSupportRender)
			*bSwapRes = _pSupportRender->hlaeSwapBuffers(hSwapHDC);
		else
			*bSwapRes=SwapBuffers(hSwapHDC);
		
		// prepare and clear for render:
		glClearColor(m_MatteColour[0], m_MatteColour[1], m_MatteColour[2], 1.0f); // don't forget to set our clear color
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		m_iFilmingState = FS_ACTIVE;

		_bRecordBuffers_FirstCall = true;
		return true;
	}

	// no sound updates during re-renders:
	FilmSound_BlockChannels(true);

	float flTime = ((m_nFrames+1)/m_fps)-(m_nFrames/m_fps); // pay attention when changing s.th. here because of handling of precision errors!

	if(print_frame->value)
		pEngfuncs->Con_Printf("MDT: capturing engine frame #%i (mdt time: %f, client time: %f)\n", m_nFrames, flTime, pEngfuncs->GetClientTime());

	FilmingStreamInfoType infos[] = { FSIT_all, FSIT_world, FSIT_entity };
	FilmingStreamInfoType depthInfos[] = { FSIT_depthall, FSIT_depthworld, FSIT_depthall };

	// Are we doing our own screenshot stuff
	bool bDepthDumps = (movie_depthdump->value != 0);
	int iDepthStreams = (int)depth_streams->value;

	if (bWantsHudCapture)
	{
		// currently we waste a whole frame cuz I want to get this done, so rerquest it
		_HudRqState = HUDRQ_NORMAL;
		bWantsHudCapture=false;

		if (_bSimulate && movie_simulate_delay->value > 0) Sleep((DWORD)movie_simulate_delay->value);

		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
		glClearColor(m_MatteColour[0], m_MatteColour[1], m_MatteColour[2], 1.0f); // don't forget to set our clear color
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		touring_R_RenderView_(); // rerender frame instant!!!!
	}

	do
	{
		// capture ALL OR WORLD:
		if( movie_splitstreams->value != 2)
		{
			if (!_bSimulate) Capture(&g_Filming_Stream_Infos[infos[m_iMatteStage]], m_nFrames, COLOR);
			if (bDepthDumps && !_bSimulate2 && iDepthStreams & 0x1)
				Capture(&g_Filming_Stream_Infos[depthInfos[m_iMatteStage]], m_nFrames, DEPTH);

			if (_pSupportRender) 
				*bSwapRes = _pSupportRender->hlaeSwapBuffers(hSwapHDC);
			else
				*bSwapRes=SwapBuffers(hSwapHDC);

			if (_bSimulate && movie_simulate_delay->value > 0) Sleep((DWORD)movie_simulate_delay->value);
		}

		if( movie_splitstreams->value >= 2)
		{
			m_iMatteStage = MS_ENTITY;

			g_Filming.clearBuffers();
			touring_R_RenderView_(); // rerender frame instant!!!!

			// capture stage:
			if (!_bSimulate) Capture(&g_Filming_Stream_Infos[infos[m_iMatteStage]], m_nFrames, COLOR);
			if (bDepthDumps && !_bSimulate2 && iDepthStreams & 0x2)
				Capture(&g_Filming_Stream_Infos[depthInfos[m_iMatteStage]], m_nFrames, DEPTH);
			if (_pSupportRender)
				*bSwapRes = _pSupportRender->hlaeSwapBuffers(hSwapHDC);
			else
				*bSwapRes=SwapBuffers(hSwapHDC); // well let's count the last on, ok? :)

			if (_bSimulate && movie_simulate_delay->value > 0) Sleep((DWORD)movie_simulate_delay->value);

			m_iMatteStage = MS_WORLD;
		}

		if (_bEnableStereoMode && (_stereo_state==STS_LEFT))
		{
			_stereo_state=STS_RIGHT;
			g_Filming.clearBuffers();
			touring_R_RenderView_(); // rerender frame instant!!!!
		} else _stereo_state=STS_LEFT;

	} while (_stereo_state!=STS_LEFT);

	// update time past the frame time:
	m_time += flTime;
	
	//
	// Sound system handling:
	//

	if (_bExportingSound)
	{
		// is requested or active
		
		if (m_nFrames==0)
		{
			// first frame, start sound system!

			// try to init sound filming system:
			std::wstring fileName(m_TakeDir);

			fileName.append(L"\\sound.wav");

			_bExportingSound = _FilmSound.Start(fileName.c_str() ,m_time,movie_sound_volume->value);
			// the soundsystem will get deactivated here, if it fails
			//pEngfuncs->Con_Printf("sound t: %f\n",flTime);

			if (!_bExportingSound) pEngfuncs->Con_Printf("ERROR: Starting MDT Sound Recording System failed!\n");

		} else {
			// advancing frame, update sound system

			_FilmSound.AdvanceFrame(m_time);
		}
	}

	m_nFrames++;
	
	float flNextFrameDuration = flTime;
	pEngfuncs->Cvar_SetValue("host_framerate", flNextFrameDuration);

	_bRecordBuffers_FirstCall = true;

	if ((movie_separate_hud->value!=0)&&isFilming())
	{
		bWantsHudCapture = true; // signal for R_RenderView
		m_iMatteStage = MS_ALL; // override matte stage
		_HudRqState = HUDRQ_CAPTURE_COLOR; // signal we want an color capture
	}

	// allow sound updates again:
	FilmSound_BlockChannels(false);

	return true;
}

void Filming::clearBuffers()
{
	// Make sure the mask colour is still correct
	glClearColor(m_MatteColour[0], m_MatteColour[1], m_MatteColour[2], 1.0f);

	// Now we do our clearing!
	if(m_iMatteStage!=MS_ENTITY || matte_xray->value)
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT);

	// well for some reason gavin forced gl_clear 1, but we don't relay on it anyways (which is good, cause the in ineye demo mode the engine will reforce it to 0 anyways):
	pEngfuncs->Cvar_SetValue("gl_clear", 1); // reforce (I am not sure if this is a good position)
}

bool Filming::checkClear(GLbitfield mask)
{
	// we now want coll app controll
	// Don't clear unless we specify
	if (isFilming() && (mask & GL_COLOR_BUFFER_BIT || mask & GL_DEPTH_BUFFER_BIT))
		return false;

	// Make sure the mask colour is still correct
	glClearColor(m_MatteColour[0], m_MatteColour[1], m_MatteColour[2], 1.0f);
	// we could also force glDepthRange here, but I preffer relaing on that forcing ztrick 0 worked
	return true;
}

Filming::DRAW_RESULT Filming::doWireframe(GLenum mode)
{
	// Wireframe turned off
	if (m_bInWireframe && movie_wireframe->value == 0)
	{
		glLineWidth(1.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		m_bInWireframe = false;
		return DR_NORMAL;
	}
	
	if (movie_wireframe->value == 0)
		return DR_NORMAL;

	m_bInWireframe = true;

	// Keep the same mode as before
	if (mode == m_iLastMode)
		return DR_NORMAL;

	// Record last mode, but record STRIPS for FANS (since they imply the same
	// things in terms of wireframeness.
	m_iLastMode = (mode == GL_TRIANGLE_FAN ? GL_TRIANGLE_STRIP : mode);

	if (movie_wireframe->value == 1 && mode == GL_QUADS)
		return DR_HIDE;
	
	if (movie_wireframe->value == 1 && mode == GL_POLYGON)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (movie_wireframe->value == 2 && (mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (movie_wireframe->value == 3)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	pEngfuncs->Cvar_SetValue("gl_clear", 1);
	glLineWidth(movie_wireframesize->value);

	return DR_NORMAL;
}

void Filming::DoWorldFxBegin(GLenum mode)
{
	static float psCurColor[4];

	// those are wh effects

	// only if transparency is enabled world and not in entitymatte
	if ( fx_wh_enable->value==0.0f || mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN || m_iMatteStage == Filming::MS_ENTITY )
	{
		_bWorldFxDisableBlend=false;
		_bWorldFxEnableDepth=false;
		return;
	}

	if (fx_wh_tint_enable->value==0.0f)
	{
		glGetFloatv(GL_CURRENT_COLOR,psCurColor);
		glColor4f(psCurColor[0],psCurColor[1],psCurColor[2],fx_wh_alpha->value);
	}
	else
		glColor4f(_fx_whRGBf[0],_fx_whRGBf[1],_fx_whRGBf[2],fx_wh_alpha->value);


	glDisable(GL_DEPTH_TEST);
	_bWorldFxEnableDepth=true;
	if (mode==GL_QUADS)
	{
		if (fx_wh_noquads->value==1.0f)
			glBlendFunc(GL_ZERO,GL_ONE);
		//else default blend func
	} else 	{
		if (fx_wh_additive->value==1.0f)
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		else
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}

	glEnable(GL_BLEND);
	_bWorldFxDisableBlend=true;
}

void Filming::DoWorldFxEnd()
{
	// those are wh effects

	if (_bWorldFxDisableBlend)
	{
		glDisable(GL_BLEND);
		_bWorldFxDisableBlend = false;
	}
	if (_bWorldFxEnableDepth)
	{
		glEnable(GL_DEPTH_TEST);
		_bWorldFxEnableDepth = false;
	}
}

void Filming::setWhTintColor(float r, float g, float b)
{
	_fx_whRGBf[0]=r;
	_fx_whRGBf[1]=g;	
	_fx_whRGBf[2]=b;
}

void Filming::DoWorldFx2(GLenum mode)
{
	// only if enabled and world and not in entity matte
	if ( fx_lightmap->value==0.0f || mode != GL_POLYGON || m_iMatteStage == Filming::MS_ENTITY )
		return;

	if (fx_lightmap->value==2.0f)
		glBindTexture(GL_TEXTURE_2D,0);
	else
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
}

/////

REGISTER_CMD_FUNC_BEGIN(recordmovie)
{
	if (g_Filming.isFilming())
	{
		pEngfuncs->Con_Printf("Already recording!\n");
		return;
	}

	g_Filming.Start();
}

REGISTER_CMD_FUNC_END(recordmovie)
{
	g_Filming.Stop();
}

REGISTER_CMD_FUNC(recordmovie_start)
{
	CALL_CMD_BEGIN(recordmovie);
}

REGISTER_CMD_FUNC(recordmovie_stop)
{
	CALL_CMD_END(recordmovie);
}

void _mirv_matte_setcolorf(float flRed, float flBlue, float flGreen)
{
	// ensure that the values are within the falid range
	clamp(flRed, 0.0f, 1.0f);
	clamp(flGreen, 0.0f, 1.0f);	
	clamp(flBlue, 0.0f, 1.0f);
	// store matte values.
	g_Filming.setMatteColour(flRed, flGreen, flBlue);
}

// that's not too nice, may be someone can code it more efficient (but still readable please):
// also I think you can retrive it directly as float or even dot it as an cvars
REGISTER_CMD_FUNC(matte_setcolor)
{
	if (pEngfuncs->Cmd_Argc() != 4)
	{
		pEngfuncs->Con_Printf("Useage: " PREFIX "matte_setcolor <red: 0-255> <green: 0-255> <blue: 0-255>\n");
		return;
	}

	float flRed = (float) atoi(pEngfuncs->Cmd_Argv(1)) / 255.0f;
	float flGreen = (float) atoi(pEngfuncs->Cmd_Argv(2)) / 255.0f;
	float flBlue = (float) atoi(pEngfuncs->Cmd_Argv(3)) / 255.0f;

	_mirv_matte_setcolorf(flRed, flBlue, flGreen);
}

REGISTER_CMD_FUNC(matte_setcolorf)
{
	if (pEngfuncs->Cmd_Argc() != 4)
	{
		pEngfuncs->Con_Printf("Useage: " PREFIX "matte_setcolorf <red: 0.0-1.0> <green: 0.0-1.0> <blue: 0.0-1.0>\n");
		return;
	}

	float flRed = (float) atof(pEngfuncs->Cmd_Argv(1));
	float flGreen = (float) atof(pEngfuncs->Cmd_Argv(2));
	float flBlue = (float) atof(pEngfuncs->Cmd_Argv(3));

	_mirv_matte_setcolorf(flRed, flBlue, flGreen);
}

REGISTER_CMD_FUNC(fx_wh_tint_colorf)
{
	if (pEngfuncs->Cmd_Argc() != 4)
	{
		pEngfuncs->Con_Printf("Useage: " PREFIX "wh_tint_colorf <red: 0.0-1.0> <green: 0.0-1.0> <blue: 0.0-1.0>\n");
		return;
	}

	float flRed = (float) atof(pEngfuncs->Cmd_Argv(1));
	float flGreen = (float) atof(pEngfuncs->Cmd_Argv(2));
	float flBlue = (float) atof(pEngfuncs->Cmd_Argv(3));

	// ensure that the values are within the falid range
	g_Filming.setWhTintColor(clamp(flRed, 0.0f, 1.0f),clamp(flGreen, 0.0f, 1.0f),clamp(flBlue, 0.0f, 1.0f));
}

REGISTER_CMD_FUNC(matte_entities)

{
	bool bShowHelp=true;
	int icarg=pEngfuncs->Cmd_Argc();

	if (icarg>=2)
	{
		char *pcmd = pEngfuncs->Cmd_Argv(1);

		if (!lstrcmp(pcmd ,"list") && icarg==2)
		{
			// list
			pEngfuncs->Con_Printf("Ids: ");
			std::list<int>::iterator iterend = g_Filming.matte_entities_r.ids.end();
			for (std::list<int>::iterator iter = g_Filming.matte_entities_r.ids.begin(); iter != iterend; iter++)
			{
				pEngfuncs->Con_Printf("%i, ",*iter);
			}
			pEngfuncs->Con_Printf("\n");
			bShowHelp=false;
		}
		else if (!lstrcmp(pcmd ,"add") && icarg==3)
		{
			// add
			int iid = atoi(pEngfuncs->Cmd_Argv(2));
			g_Filming.matte_entities_r.ids.push_front(iid);
			g_Filming.matte_entities_r.bNotEmpty=true;
			bShowHelp=false;
		}
		else if (!lstrcmp(pcmd ,"del") && icarg==3)
		{
			// del
			int iid = atoi(pEngfuncs->Cmd_Argv(2));
			g_Filming.matte_entities_r.ids.remove(iid);
			g_Filming.matte_entities_r.bNotEmpty=!(g_Filming.matte_entities_r.ids.empty());
			bShowHelp=false;
		}
		else if (!lstrcmp(pcmd ,"clear") && icarg==2)
		{
			// clear
			g_Filming.matte_entities_r.ids.clear();
			g_Filming.matte_entities_r.bNotEmpty=false;
			bShowHelp=false;
		}
	}

	if (bShowHelp)
	{
		pEngfuncs->Con_Printf(
			"Allows to restrict the matte to a list of entities only.\n"
			"If the list is empty (default) all show up.\n"
			"\n"
			"Commands:\n"
			"\t" PREFIX "matte_entities list - displays current list\n"
			"\t" PREFIX "matte_entities add <id> - adds entity with id <id> to the list\n"
			"\t" PREFIX "matte_entities del <id> - removes <id> from the list\n"
			"\t" PREFIX "matte_entities clear - clears all items from the list\n"
		);
	}
}


REGISTER_DEBUGCMD_FUNC(depth_info) {
	float N = g_Filming.GetZNear();
	float F = g_Filming.GetZFar();
	float E = (F-N)/256.0f;
	float P = (F-N) ? 100*E/(F-N) : 0;
	pEngfuncs->Con_Printf("zNear: %f\nzFar: %f\nMax linear error (8bit): %f (%f %%)\n", N, F, E, P);
}

