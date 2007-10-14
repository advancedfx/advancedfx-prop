
#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
#include "r_efx.h"
#include "com_model.h"
#include "r_studioint.h"
#include "pm_defs.h"
#include "cvardef.h"
#include "entity_types.h"

#include "cmdregister.h"
#include "filming.h"

#include "hl_addresses.h" // we want to access addressese (i.e. R_RenderView)
#include "detours.h" // we want to use Detourapply
#include "in_defs.h" // PITCH YAW ROLL // HL1 sdk


extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s *pEngStudio;
extern playermove_s *ppmove;

extern float clamp(float, float, float);

REGISTER_DEBUGCVAR(depth_bias, "0", 0);
REGISTER_DEBUGCVAR(depth_scale, "1", 0);
REGISTER_DEBUGCVAR(gl_force_noztrick, "1", 0);

//REGISTER_DEBUGCVAR(matte_nointerp, "1", 0);
REGISTER_DEBUGCVAR(movie_oldcapture, "0", 0);

REGISTER_CVAR(crop_height, "-1", 0);
REGISTER_CVAR(crop_yofs, "-1", 0);

REGISTER_CVAR(depth_logarithmic, "32", 0);

REGISTER_CVAR(movie_customdump, "1", 0)
REGISTER_CVAR(movie_clearscreen, "0", 0);
REGISTER_CVAR(movie_depthdump, "0", 0);
REGISTER_CVAR(movie_export_cammotion, "0", 0);
REGISTER_CVAR(movie_filename, "untitled", 0);
REGISTER_CVAR(movie_fps, "30", 0);
REGISTER_CVAR(movie_separate_hud, "0", 0);
REGISTER_CVAR(movie_stereomode,"0",0);
REGISTER_CVAR(movie_stereo_centerdist,"0.0",0);
REGISTER_CVAR(movie_stereo_yawdegrees,"2.0",0);
REGISTER_CVAR(movie_swapdoors, "0", 0);
REGISTER_CVAR(movie_swapweapon, "0", 0);
REGISTER_CVAR(movie_splitstreams, "0", 0);
REGISTER_CVAR(movie_onlyentity, "0", 0);
REGISTER_CVAR(movie_wireframe, "0", 0);
REGISTER_CVAR(movie_wireframesize, "1", 0);

// Our filming singleton
Filming g_Filming;

//>> added 20070922 >>

typedef void (* HL_unknownFunc_t) ( void );
//GL_Set2D_t detoured_GL_Set2D=NULL;
R_RenderView__t	detoured_R_RenderView_=NULL;
//V_RenderView_t	detoured_V_RenderView=NULL;
//SCR_UpdateScreen_t	detoured_SCR_UpdateScreen=NULL;
//HL_unknownFunc_t detoured_unknown=NULL;

//#define ADDRESS_unkown 0x01d50960
//#define DETOURSIZE_unknown 6

#define ADDRESS_R_RenderView_ HL_ADDR_R_RenderView_
#define DETOURSIZE_R_RenderView_ 0x014
#define ADDRESS_r_refdef HL_ADDR_r_refdef

//#define ADDRESS_V_RenderView HL_ADDR_V_RenderView
//#define DETOURSIZE_V_RenderView 0x5
//#define ADDRESS_GL_Set2D HL_ADDR_GL_Set2D
//#define DETOURSIZE_GL_Set2D 9


// >> Hooking the HUD functions  >>

// >> fyi >>

// relevant excerpt from HL's SCR_UpdateScreen after the first GL_Set2D() call

// 01dd0442 7535            jne     launcher!CreateInterface+0x9cf08a (01dd0479)

// Hud Begin

// 01dd0444 a1603fe002      mov     eax,dword ptr [launcher!CreateInterface+0x1a02b71 (02e03f60)]
// 01dd0449 c644240c00      mov     byte ptr [esp+0Ch],0
// 01dd044e 83f801          cmp     eax,1
// 01dd0451 7505            jne     launcher!CreateInterface+0x9cf069 (01dd0458)
// 01dd0453 c644240c01      mov     byte ptr [esp+0Ch],1
// 01dd0458 6a00            push    0

// drawing stuff before ingame hud:

// 01dd045a e80105f8ff      call    launcher!CreateInterface+0x94f571 (01d50960)
// 01dd045f 8b4c2410        mov     ecx,dword ptr [esp+10h]
// 01dd0463 81e1ff000000    and     ecx,0FFh
// 01dd0469 51              push    ecx

// main ingame hud

// 01dd046a e83184f4ff      call    launcher!CreateInterface+0x9174b1 (01d188a0)
// 01dd046f 6a01            push    1

// past main hud

// 01dd0471 e8ea04f8ff      call    launcher!CreateInterface+0x94f571 (01d50960)
// 01dd0476 83c40c          add     esp,0Ch

// Hud End

// 01dd0479 a1b00c8302      mov     eax,dword ptr [launcher!CreateInterface+0x142f8c1 (02830cb0)]

// << fyi <<

// okay,´this is what we do:
//
// we place two __declspec(naked) detours:
// 1st: on 01dd0444
// 2nd: on 01dd0479
//
// the first will care about calling an g_Filming.Onxxx Event Function
// the 2nd will also call an event function and depending on it's result either jump to the first one (doing a loop) or just continue normal op

// asm related definitons we will use:
#define asmNOP 0x90 // opcode for NOP
#define asmJMP	0xE9 // opcode for JUMP
#define JMP32_SZ 5	// the size of JMP <address>

// addresses:
#define ADDRESS_SCR_UpdateScreen HL_ADDR_SCR_UpdateScreen
// 
// 0x01dd0444:
#define ADDRESS_HUD_TOURIN (HL_ADDR_SCR_UpdateScreen + 0xD4)
// 0x01dd0479:
#define ADDRESS_HUD_TOUROUT (ADDRESS_HUD_TOURIN + 0x35)

bool bHudToursInstalled=false;
char pHudTours_begin[5+JMP32_SZ]; // for detouring before jumping into tour_HudBegin
char pHudTours_end[5+JMP32_SZ];	// for detouring on finally leaving tour_HudEnd (when no loop was requested)

DWORD dwAddress_TourIn_Back = 0;
DWORD dwAddress_HudTours_begin=(DWORD)pHudTours_begin;
DWORD dwAddress_HudTours_end=(DWORD)pHudTours_end;

bool bHudBeginCalled=false;

__declspec(naked) void tour_HudBegin()
{
	bHudBeginCalled=true;
	g_Filming.OnHudBeginEvent();
	__asm
	{
		JMP [dwAddress_TourIn_Back]
	}
}

__declspec(naked) void tour_HudEnd()
{
	// I am not sure, may be we should adjust the stackspace here, acutally some of our function might operate in the HL stack space lol :O
	__asm
	{
		PUSH eax
	}
	if(g_Filming.OnHudEndEvnet())
	{
		__asm {
			POP EAX
			JMP [dwAddress_HudTours_begin]
		}
	} else {
		__asm {
			POP EAX
			JMP [ dwAddress_HudTours_end]
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

	// fill tourin return address:
	dwAddress_TourIn_Back = (DWORD)ADDRESS_HUD_TOURIN +5;
    
	// get access top the code in the code segment:
	char *pCodeAccess=(char *)ADDRESS_HUD_TOURIN;
	DWORD dwOldProtect;
	VirtualProtect(pCodeAccess,(ADDRESS_HUD_TOUROUT - ADDRESS_HUD_TOURIN)+5,PAGE_READWRITE,&dwOldProtect);

	DWORD dwTemp;

	// Detour of HudIn:

	// copy the in code we want to detour and place a jump onto the inhook func:
	memcpy(pHudTours_begin,pCodeAccess,5); // copy the mov
	pHudTours_begin[5]=asmJMP; // place JMP opcode
	dwTemp=(DWORD)tour_HudBegin - ((DWORD)pHudTours_begin+5) - JMP32_SZ; // and supply the address
	memcpy(pHudTours_begin+6,&dwTemp,sizeof(DWORD));// .

	// replace the mov with a jump on our code hook:
	pCodeAccess[0]=asmJMP;
	dwTemp=(DWORD)pHudTours_begin - (DWORD)pCodeAccess - JMP32_SZ;
	memcpy(pCodeAccess+1,&dwTemp,sizeof(DWORD));

	// Detour HudOut:
	char *pOutAccess=(char *)ADDRESS_HUD_TOUROUT;

	// save the code (the 5 byte move eax,[addr]) we have to exec later when leaving our loop:
	memcpy(pHudTours_end,pOutAccess,5);
	// and make it Jump back into the original Code:
	pHudTours_end[5]=asmJMP;
	dwTemp=((DWORD)pOutAccess+5) - ((DWORD)pHudTours_end+5) - JMP32_SZ;
	memcpy(pHudTours_end+6,&dwTemp,sizeof(DWORD));

	//memset(pOutAccess,0,5);
	//memcpy(pOutAccess,pHudTours_end,5);

	// now place the JMP onto our introducing end hook tour into the original code:
	pOutAccess[0]=asmJMP;
	dwTemp=(DWORD)tour_HudEnd  - (DWORD)pOutAccess - JMP32_SZ;
	memcpy(pOutAccess+1,&dwTemp,sizeof(DWORD));

	//
	// that's it.

	// restore olde code access:
	VirtualProtect(pCodeAccess,(ADDRESS_HUD_TOUROUT - ADDRESS_HUD_TOURIN)+5,dwOldProtect,NULL);
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
	refdef_t* p_r_refdef=(refdef_t*)ADDRESS_r_refdef; // pointer to r_refdef global struct

	vec3_t oldorigin = p_r_refdef->vieworg; // save old

	// >> begin calculate transform vectors
	// we have to calculate our own transformation vectors from the angles and can not use pparams->forward etc., because in spectator mode they might be not present:
	// (adapted from HL1SDK/multiplayer/pm_shared.c/AngleVectors) and modified for quake order of angles:

	vec3_t angles;
	float forward[3],right[3],up[3];

	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	angles = p_r_refdef->viewangles;

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

	// apply our displacement (this code is similar to HL1SDK/multiplayer/cl_dll/view.cpp/V_CalcNormalRefdef):
	float fDispRight, fDispUp, fDispForward;
	float fYaw=0;

	g_Filming.GetCameraOfs(&fDispRight,&fDispUp,&fDispForward);

	if (g_Filming.bEnableStereoMode()&&(g_Filming.isFilming()))
	{
		if (g_Filming.GetStereoState()==Filming::STS_LEFT)
		{
			// left
			fDispRight-=movie_stereo_centerdist->value; // left displacement
			p_r_refdef->viewangles[YAW]+= movie_stereo_yawdegrees->value; // right turn
		}
		else
		{
			// right
			fDispRight+=movie_stereo_centerdist->value; // right displacement
			p_r_refdef->viewangles[YAW]-= movie_stereo_yawdegrees->value; // left turn
		}
	}

	for ( int i=0 ; i<3 ; i++ )
	{
		p_r_refdef->vieworg[i] += fDispForward*forward[i] + fDispRight*right[i] + fDispUp*up[i];
	}

	g_Filming.SupplyCamMotion(p_r_refdef->vieworg[0],p_r_refdef->vieworg[1],p_r_refdef->vieworg[2],p_r_refdef->viewangles[ROLL],p_r_refdef->viewangles[PITCH],p_r_refdef->viewangles[YAW]);

	detoured_R_RenderView_();

	p_r_refdef->vieworg = oldorigin; // restore old (is this necessary? I don't know if the values are used for interpolations later or not)
}
/*
void touring_unknown(void)
{
	static int ifakeframe=0;
	static char tmptag[10];

	//pEngfuncs->Con_Printf("Called GL_Set2d (%i times) during tracking.\n",GL_2DCalls);
	if (g_Filming.isFilming()&&g_Filming.bWantsHudCapture)
	{
		GL_2DCalls++;
		sprintf(tmptag,"dbg%i",GL_2DCalls);
		g_Filming.Capture(tmptag,ifakeframe,g_Filming.BUFFER::COLOR);
	}
	detoured_unknown();
	if (g_Filming.isFilming()&&g_Filming.bWantsHudCapture)
	{
		GL_2DCalls++;
		sprintf(tmptag,"dbg%i",GL_2DCalls);
		g_Filming.Capture(tmptag,ifakeframe,g_Filming.BUFFER::COLOR);
	}
}*/

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

bool Filming::bNewRequestMethod()
{ return _bNewRequestMethod; }

void Filming::bNewRequestMethod(bool bSet)
{ if (m_iFilmingState == FS_INACTIVE) _bNewRequestMethod = bSet; };

//bool Filming::bNoMatteInterpolation()
//{ return _bNoMatteInterpolation; }

//void Filming::bNoMatteInterpolation (bool bSet)
//{ if (m_iFilmingState == FS_INACTIVE) _bNoMatteInterpolation = bSet; }

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
		_bNewRequestMethod = true;
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

		_bCamMotion = false;
		pMotionFile = NULL;
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
float Filming::GetStereoOffset()
{
	return _fStereoOffset;
}

Filming::STEREO_STATE Filming::GetStereoState()
{
	return _stereo_state;
}

bool Filming::bCustomDump()
{
	return (movie_customdump->value != 0.0);
}

Filming::HUD_REQUEST_STATE Filming::giveHudRqState()
{
	if (!isFilming())
		return HUDRQ_NORMAL;

	return _HudRqState;
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
		Capture("hudcolor",m_nFrames,COLOR);
		if (movie_separate_hud->value!=2.0)
		{
			// we want alpha too in this case
			_HudRqState = HUDRQ_CAPTURE_ALPHA; // set ALPHA mode!
			return true; // we need loop
		}
		break;
	case HUDRQ_CAPTURE_ALPHA:
		Capture("hudalpha",m_nFrames,ALPHA);
		break;
	}
	return false; // do not loop
}


//<< added 

void Filming::setScreenSize(GLint w, GLint h)
{
	if (m_iWidth < w) m_iWidth = w;
	if (m_iHeight < h) m_iHeight = h;
}

void Filming::Start()
{
	// Different filename, so save it and reset the take count
	if (strncmp(movie_filename->string, m_szFilename, sizeof(m_szFilename) - 1) != 0)
	{
		strncpy(m_szFilename, movie_filename->string, sizeof(m_szFilename) - 1);
		m_nTakes = 0;
	}

	m_nFrames = 0;
	m_iFilmingState = FS_STARTING;
	m_iMatteStage = MS_WORLD;

	// retrive some cvars:
	_fStereoOffset = movie_stereo_centerdist->value;
	_bNewRequestMethod = (movie_oldcapture->value != 1.0)&&(movie_customdump->value != 0.0);
	if (!_bNewRequestMethod) pEngfuncs->Con_Printf("Warning: Using old RequestMethod!\nThe new method is only available with __mirv_movie_oldcapture 0 and mirv_movie_customdump 1.\n");
	_bEnableStereoMode = (movie_stereomode->value != 0.0) && _bNewRequestMethod; // we also have to be able to use R_RenderView
	//_bNoMatteInterpolation = (matte_nointerp->value == 0.0);
	_bCamMotion = (movie_export_cammotion->value != 0.0) && _bNewRequestMethod;

	if (_bCamMotion) MotionFile_Begin();

	// if we want to use R_RenderView and we have not already done that, we need to set it up now:
	if (_bNewRequestMethod)
	{
		// we want to use it, so make sure we have it:
		if (!detoured_R_RenderView_&&((ADDRESS_R_RenderView_)!=NULL))
		{
			// we don't have it yet and the addres is not NULL (which might be an intended cfg setting)
			detoured_R_RenderView_ = (R_RenderView__t) DetourApply((BYTE *)ADDRESS_R_RenderView_, (BYTE *)touring_R_RenderView_, (int)DETOURSIZE_R_RenderView_);
			install_Hud_tours(); // wil automaticall check if already installed or not

			//detoured_V_RenderView = (V_RenderView_t) DetourApply((BYTE *)ADDRESS_V_RenderView, (BYTE *)touring_V_RenderView, (int)DETOURSIZE_V_RenderView);
			//detoured_SCR_UpdateScreen = (SCR_UpdateScreen_t) DetourApply((BYTE *)ADDRESS_SCR_UpdateScreen, (BYTE *)touring_SCR_UpdateScreen, (int)DETOURSIZE_SCR_UpdateScreen);
			//detoured_GL_Set2D = (GL_Set2D_t) DetourApply((BYTE *)ADDRESS_GL_Set2D, (BYTE *)touring_GL_Set2D, (int)DETOURSIZE_GL_Set2D);
			//detoured_unknown = (HL_unknownFunc_t) DetourApply((BYTE *)ADDRESS_unkown, (BYTE *)touring_unknown, (int)DETOURSIZE_unknown);
		}
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
}

void Filming::Stop()
{
	if (_bCamMotion) MotionFile_End();
	
	m_nFrames = 0;
	m_iFilmingState = FS_INACTIVE;
	m_nTakes++;
	_HudRqState=HUDRQ_NORMAL;

	// Need to reset this otherwise everything will run crazy fast
	pEngfuncs->Cvar_SetValue("host_framerate", 0);
}

void Filming::Capture(const char *pszFileTag, int iFileNumber, BUFFER iBuffer)
{
	char cDepth = (iBuffer == COLOR ? 2 : 3); //? problem if depth caputre and bits >8?
	int iMovieBitDepth = (int)(movie_depthdump->value);

	GLenum eGLBuffer = (iBuffer == COLOR ? GL_BGR_EXT : (iBuffer == ALPHA ? GL_ALPHA : GL_DEPTH_COMPONENT));
	GLenum eGLtype = ((iBuffer == COLOR)||(iBuffer == ALPHA)? GL_UNSIGNED_BYTE : GL_FLOAT);
	int nBits = ((iBuffer == COLOR ) ? 3 : (iBuffer == ALPHA ? 1:(iMovieBitDepth==32?4:(iMovieBitDepth==24?3:(iMovieBitDepth==16?2:1)))));

	char* pszStereotag="";
	if (_bEnableStereoMode&&!bWantsHudCapture)
	{
		// if we are not capturing the hud and are in stereo mode add the proper tag:
		if (_stereo_state==STS_LEFT) pszStereotag="_left"; else pszStereotag="_right";
	}

	char szFilename[196];
	_snprintf(szFilename, sizeof(szFilename) - 1, "%s_%02d_%s%s_%05d.tga", m_szFilename, m_nTakes, pszFileTag, pszStereotag, iFileNumber);

	unsigned char szTgaheader[12] = { 0, 0, cDepth, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char szHeader[6] = { (int) (m_iWidth % 256), (int) (m_iWidth / 256), (int) (m_iCropHeight % 256), (int) (m_iCropHeight / 256), 8 * nBits, 0 };

	FILE *pImage;

	//// in case we want to check if the buffer's are set:
	//GLint iTemp,iTemp2; glGetIntegerv(GL_READ_BUFFER,&iTemp); glGetIntegerv(GL_DRAW_BUFFER,&iTemp2); pEngfuncs->Con_Printf(">>Read:  0x%08x, Draw:  0x%08x \n",iTemp,iTemp2);

	bool bReadOk = m_GlRawPic.DoGlReadPixels(0, m_iCropYOfs, m_iWidth, m_iCropHeight, eGLBuffer, eGLtype);
	if (!bReadOk)
	{
		pEngfuncs->Con_Printf("MDT ERROR: failed to capture take %05d, Errorcode: %d.\n",m_nTakes,m_GlRawPic.GetLastUnhandledError());
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
		int iSize=m_iWidth*m_iCropHeight;
		unsigned char* t_pBuffer=m_GlRawPic.GetPointer();	// the pointer where we write
		unsigned char* t_pBuffer2=t_pBuffer;				// the pointer where we read
		
		float tfloat;
		unsigned int tuint;

		unsigned int mymax=(1<<(8*nBits))-1;

		float logscale=depth_logarithmic->value;

		// these values are needed in order to work around in bugs i.e. of the NVIDIA Geforce 5600, since it doesn't use the default values for some reason:
		static float fHard_GL_DEPTH_BIAS  = 0.0; // OGL reference says default is 0.0
		static float fHard_GL_DEPTH_SCALE = 1.0; // OGL reference says default is 1.0	
		glGetFloatv(GL_DEPTH_BIAS,&fHard_GL_DEPTH_BIAS);
		glGetFloatv(GL_DEPTH_SCALE,&fHard_GL_DEPTH_SCALE);

		//pEngfuncs->Con_Printf("Depth: Scale: %f, Bias: %f, Firstpixel: %f\n",fHard_GL_DEPTH_SCALE,fHard_GL_DEPTH_BIAS,(float)*(float *)t_pBuffer2);

		for (int i=0;i<iSize;i++)
		{
			memmove(&tfloat,t_pBuffer2,sizeof(float));

            tfloat=tfloat*(depth_scale->value)+(depth_bias->value); // allow custom scale and offset
			tfloat=tfloat/fHard_GL_DEPTH_SCALE-fHard_GL_DEPTH_BIAS; // fix the range for card's that don't do it their selfs, although the OpenGL reference says so.

			if (logscale!=0)
			{
				tfloat = (exp(tfloat*logscale)-1)/(exp((float)1*logscale)-1);
			}
			tfloat*=mymax; // scale to int's max. value
			tuint = max(min((unsigned int)tfloat,mymax),0); // floor,clamp and convert to the desired data type

			memmove(t_pBuffer,&tuint,nBits);
				
			t_pBuffer+=nBits;
			t_pBuffer2+=sizeof(float);
		}
	}

	if ((pImage = fopen(szFilename, "wb")) != NULL)
	{
		fwrite(szTgaheader, sizeof(unsigned char), 12, pImage);
		fwrite(szHeader, sizeof(unsigned char), 6, pImage);

		fwrite(m_GlRawPic.GetPointer(), sizeof(unsigned char), m_iWidth * m_iCropHeight * nBits, pImage);

		fclose(pImage);
	}
}

Filming::DRAW_RESULT Filming::shouldDraw(GLenum mode)
{
	if (m_iMatteStage == MS_ALL)
		return DR_NORMAL;

	else if (m_iMatteStage == MS_ENTITY)
		return shouldDrawDuringEntityMatte(mode);

	else 
		return shouldDrawDuringWorldMatte(mode);
}

Filming::DRAW_RESULT Filming::shouldDrawDuringEntityMatte(GLenum mode)
{
	bool bSwapWeapon = (movie_swapweapon->value != 0);
	bool bSwapDoors = (movie_swapdoors->value != 0);
	int iOnlyActor = (int) movie_onlyentity->value;

	// GL_POLYGON is a worldbrush
	if (mode == GL_POLYGON)
	{
		cl_entity_t *ce = pEngStudio->GetCurrentEntity();

		// This is a polygon func_ something, so probably a door or a grill
		// We don't touch doors here ol' chap if swapdoors is on
		if (bSwapDoors && ce && ce->model && ce->model->type == mod_brush && strncmp(ce->model->name, "maps/", 5) != 0)
			return DR_NORMAL;

		return DR_MASK;
	}

	// Sprites and sky are just removed completely
	else if (mode == GL_QUADS)
		return DR_HIDE;

	// Entities
	else if (mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN)
	{
		cl_entity_t *ce = pEngStudio->GetCurrentEntity();

		// Studio models
		if (ce && ce->model && ce->model->type == mod_studio)
		{
			// This is the viewmodel so hide it from ent-only if they want it to be shown as normal
			// However hide it via a mask so it still covers stuff
			// Actually do we want to do that?
			// edit: No, not for now as it breaks
			if (bSwapWeapon && strncmp("models/v_", ce->model->name, 9) == 0)
				return DR_HIDE;

			// We have selected 1 ent only to be visible and its not this
			if (iOnlyActor != 0 && iOnlyActor != ce->index)
				return DR_HIDE;
		}	
		// This is some sort of func thing so matte effect it
		// TODO: why is this doing MATTE_COLOUR instea of masking?
		else
			//glColor3f(MATTE_COLOUR);
			return DR_MASK;
	}

	return DR_NORMAL;
}

Filming::DRAW_RESULT Filming::shouldDrawDuringWorldMatte(GLenum mode)
{
	bool bSwapWeapon = (movie_swapweapon->value != 0);
	bool bSwapDoors = (movie_swapdoors->value != 0);
	int iOnlyActor = (int) movie_onlyentity->value;

	// Worldbrush stuff
	if (mode == GL_POLYGON)
	{
		cl_entity_t *ce = pEngStudio->GetCurrentEntity();

		// This is a polygon func_ something, so probably a door or a grill
		if (bSwapDoors && ce && ce->model && ce->model->type == mod_brush && strncmp(ce->model->name, "maps/", 5) != 0)
			return DR_HIDE;
	}

	// Entities...
	// We remove stuff rather than hide it, because in world only they probably
	// want the depth dump to just be the world!
	else if (mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN)
	{
		cl_entity_t *ce = pEngStudio->GetCurrentEntity();

		// Studio models need only apply
		if (ce && ce->model && ce->model->type == mod_studio)
		{
			bool bKeepDueToSpecialCondition = false;

			// This is the viewmodel so hide it from ent-only if they want it to be shown as normal
			if (bSwapWeapon && strncmp("models/v_", ce->model->name, 9) == 0)
				return DR_NORMAL;

			// We have selected 1 ent only to be on the ent only layer and its not this
			if (iOnlyActor != 0 && iOnlyActor != ce->index)
				return DR_NORMAL;

			if (!bKeepDueToSpecialCondition)
				return DR_HIDE;
		}	
	}

	return DR_NORMAL;
}

void Filming::_old_recordBuffers()
{
	// If this is a none swapping one then force to the correct stage.
	// Otherwise continue working wiht the stage that this frame has
	// been rendered with.
	if (movie_splitstreams->value < 3.0f)
		m_iMatteStage = (MATTE_STAGE) ((int) MS_ALL + (int) max(movie_splitstreams->value, 0.0f));

	// If we've only just started, delay until the next scene so that
	// the first frame is drawn correctly
	if (m_iFilmingState == FS_STARTING)
	{
		glClearColor(m_MatteColour[0], m_MatteColour[1], m_MatteColour[2], 1.0f); // don't forget to set our clear color
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		m_iFilmingState = FS_ACTIVE;
		return;
	}

	bool bSplitting = (movie_splitstreams->value == 3.0f);
	float flTime = 1.0f / max(movie_fps->value, 1.0f);

	static char *pszTitles[] = { "all", "world", "entity" };
	static char *pszDepthTitles[] = { "depthall", "depthworld", "depthall" };

	// Are we doing our own screenshot stuff
	bool bCustomDumps = (movie_customdump->value != 0);
	bool bDepthDumps = (movie_depthdump->value != 0);

	if (bCustomDumps)
		Capture(pszTitles[m_iMatteStage], m_nFrames, COLOR);

	if (bDepthDumps)
		Capture(pszDepthTitles[m_iMatteStage], m_nFrames, DEPTH);

	float flNextFrameDuration = flTime;

	// If splitting, fill out the rest of the fps
	if (bSplitting)
	{
		// We want as little time to pass as possible until the next frame
		// is drawn, so the difference is tiny.
		if (m_iMatteStage == MS_WORLD)
		{
			flNextFrameDuration = MIN_FRAME_DURATION;
			m_iMatteStage = MS_ENTITY;
		}
		// Make up the rest of the time so that their fps is met.
		// Also increase the frame count
		else
		{
			flNextFrameDuration = flTime - MIN_FRAME_DURATION;
			m_iMatteStage = MS_WORLD;
			m_nFrames++;
		}
	}
	else
		m_nFrames++;

	// Make sure the next frame time isn't invalid
	flNextFrameDuration = max(flNextFrameDuration, MIN_FRAME_DURATION);

	pEngfuncs->Cvar_SetValue("host_framerate", flNextFrameDuration);
}

bool Filming::recordBuffers(HDC hSwapHDC,BOOL *bSwapRes)
// be sure to read the comments to _bRecordBuffers_FirstCall in filming.h, because this is fundamental for undertanding what the **** is going on here
// currently like the old code we relay on some user changable values, however we should lock those during filming to avoid crashes caused by the user messing around (not implemented yet)
{
	if (!_bNewRequestMethod)
	{
		_old_recordBuffers();
		return false;
	} else if (!_bRecordBuffers_FirstCall)
	{
		pEngfuncs->Con_Printf("WARNING: Unexpected recordBuffers request, this should not happen!");
	}
	_bRecordBuffers_FirstCall = false;

	// If this is a none swapping one then force to the correct stage.
	// Otherwise continue working wiht the stage that this frame has
	// been rendered with.
	if ((0.0f <= movie_splitstreams->value)&&(movie_splitstreams->value < 3.0f))
		m_iMatteStage = (MATTE_STAGE) ((int) MS_ALL + (int) max(movie_splitstreams->value, 0.0f));
	else
		m_iMatteStage = MS_WORLD;

	// If we've only just started, delay until the next scene so that
	// the first frame is drawn correctly
	if (m_iFilmingState == FS_STARTING)
	{
		if (movie_separate_hud->value!=0.0)
		{
			bWantsHudCapture = true; // signal for R_RenderView
			m_iMatteStage = MS_ALL; // override matte stage
			_HudRqState = HUDRQ_CAPTURE_COLOR; // signal we want an color capture
		}
		
		glClearColor(m_MatteColour[0], m_MatteColour[1], m_MatteColour[2], 1.0f); // don't forget to set our clear color
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		m_iFilmingState = FS_ACTIVE;

		_bRecordBuffers_FirstCall = true;
		return false;
	}

	bool bSplitting = (movie_splitstreams->value == 3.0f);
	float flTime = 1.0f / max(movie_fps->value, 1.0f);

	static char *pszTitles[] = { "all", "world", "entity" };
	static char *pszDepthTitles[] = { "depthall", "depthworld", "depthall" };

	// Are we doing our own screenshot stuff
	bool bCustomDumps = (movie_customdump->value != 0);
	bool bDepthDumps = (movie_depthdump->value != 0);

	if (bWantsHudCapture)
	{
		// currently we waste a whole frame cuz I want to get this done, so rerquest it
		_HudRqState = HUDRQ_NORMAL;
		bWantsHudCapture=false;
		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
		glClearColor(m_MatteColour[0], m_MatteColour[1], m_MatteColour[2], 1.0f); // don't forget to set our clear color
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		touring_R_RenderView_(); // rerender frame instant!!!!
	}

	do
	{
		// capture stage:
		if (bCustomDumps) Capture(pszTitles[m_iMatteStage], m_nFrames, COLOR);
		if (bDepthDumps) Capture(pszDepthTitles[m_iMatteStage], m_nFrames, DEPTH);
		*bSwapRes=SwapBuffers(hSwapHDC);

		if (bSplitting)
		{
			m_iMatteStage = MS_ENTITY;

			g_Filming.clearBuffers();
			touring_R_RenderView_(); // rerender frame instant!!!!

			// capture stage:
			if (bCustomDumps) Capture(pszTitles[m_iMatteStage], m_nFrames, COLOR);
			if (bDepthDumps) Capture(pszDepthTitles[m_iMatteStage], m_nFrames, DEPTH);
			*bSwapRes=SwapBuffers(hSwapHDC); // well let's count the last on, ok? :)

			m_iMatteStage = MS_WORLD;
		}

		if (_bCamMotion) MotionFile_Frame();
	
		if (_bEnableStereoMode && (_stereo_state==STS_LEFT))
		{
			_stereo_state=STS_RIGHT;
			g_Filming.clearBuffers();
			touring_R_RenderView_(); // rerender frame instant!!!!
		} else _stereo_state=STS_LEFT;

	} while (_stereo_state!=STS_LEFT);

	float flNextFrameDuration = flTime;
	m_nFrames++;
	
	// Make sure the next frame time isn't invalid
	flNextFrameDuration = max(flNextFrameDuration, MIN_FRAME_DURATION);
	pEngfuncs->Cvar_SetValue("host_framerate", flNextFrameDuration);

	_bRecordBuffers_FirstCall = true;

	if ((movie_separate_hud->value!=0)&&isFilming())
	{
		bWantsHudCapture = true; // signal for R_RenderView
		m_iMatteStage = MS_ALL; // override matte stage
		_HudRqState = HUDRQ_CAPTURE_COLOR; // signal we want an color capture
	}
	return true;
}

void Filming::clearBuffers()
{
	// Now we do our clearing!
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

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

void Filming::SupplyCamMotion(float Xposition,float Yposition,float Zposition,float Zrotation,float Xrotation,float Yrotation)
{
	_cammotion.Xposition=Xposition;
	_cammotion.Yposition=Yposition;
	_cammotion.Zposition=Zposition;
	_cammotion.Zrotation=Zrotation;
	_cammotion.Xrotation=Xrotation;
	_cammotion.Yrotation=Yrotation;
}

void Filming::MotionFile_Begin()
{
	char szTmp[196];
	_snprintf(szTmp, sizeof(szTmp) - 1, "%s_%02d_cammotion.bvh", m_szFilename, m_nTakes);

	if ((pMotionFile = fopen(szTmp, "wb")) != NULL)
	{
		fputs("HIERARCHY\n",pMotionFile);

		if (_bEnableStereoMode)
		{
			fputs("ROOT MdtCamLeft\n{\n\tOFFSET 0.00 0.00 0.00\n\tCHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n}\n",pMotionFile);
			fputs("ROOT MdtCamRight\n{\n\tOFFSET 0.00 0.00 0.00\n\tCHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n}\n",pMotionFile);
		} else {
			fputs("ROOT MdtCam\n{\n\tOFFSET 0.00 0.00 0.00\n\tCHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation\n}\n",pMotionFile);
		}

		fputs("MOTION\n",pMotionFile);
		_lMotionTPos = ftell(pMotionFile);
		fputs("Frames: 0123456789A\n",pMotionFile);

		float flTime = max(1.0f / max(movie_fps->value, 1.0f), MIN_FRAME_DURATION);
		_snprintf(szTmp, sizeof(szTmp) - 1,"Frame Time: %f\n",flTime);
		fputs(szTmp,pMotionFile);

	}
}
void Filming::MotionFile_Frame()
{
	if (!pMotionFile) return;

	char pszT[249];

	_snprintf(pszT,sizeof(pszT)-1,"%f %f %f %f %f %f",_cammotion.Xposition,_cammotion.Yposition,_cammotion.Zposition,_cammotion.Zrotation,_cammotion.Xrotation,_cammotion.Yrotation);
	fputs(pszT,pMotionFile);
	if (!_bEnableStereoMode || _stereo_state == STS_RIGHT) fputs("\n",pMotionFile);

}
void Filming::MotionFile_End()
{
	if (pMotionFile){
		char pTmp[100];
		fseek(pMotionFile,_lMotionTPos,SEEK_SET);
		_snprintf(pTmp,sizeof(pTmp)-1,"Frames: %11i",m_nFrames);
		fputs(pTmp,pMotionFile);
		fclose(pMotionFile);
	}
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
		pEngfuncs->Con_Printf("Useage: " PREFIX "matte_setcolour <red: 0-255> <green: 0-255> <blue: 0-255>\n");
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
		pEngfuncs->Con_Printf("Useage: " PREFIX "matte_setcolourf <red: 0.0-1.0> <green: 0.0-1.0> <blue: 0.0-1.0>\n");
		return;
	}

	float flRed = (float) atof(pEngfuncs->Cmd_Argv(1));
	float flGreen = (float) atof(pEngfuncs->Cmd_Argv(2));
	float flBlue = (float) atof(pEngfuncs->Cmd_Argv(3));

	_mirv_matte_setcolorf(flRed, flBlue, flGreen);
}