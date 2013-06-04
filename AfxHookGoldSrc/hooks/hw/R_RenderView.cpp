#include "stdafx.h"

#include "R_RenderView.h"

#include <shared/detours.h>
#include <hl_addresses.h>

#include "../../filming.h"

typedef void (*R_RenderView_t)( void );

R_RenderView_t g_Old_R_RenderView = 0;


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



void New_R_RenderView(void)
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

		//
		// call original R_RenderView_
		//

		g_Old_R_RenderView();

		// restore original values
		memcpy (p_r_refdef->vieworg,oldorigin,3*sizeof(float));
		memcpy (p_r_refdef->viewangles,oldangles,3*sizeof(float));
	} while(bLoop);
}

void Hook_R_RenderView()
{
	static bool firstRun = true;
	if(!firstRun) return;
	firstRun = false;

	if(!HL_ADDR_GET(R_RenderView)) return;

	g_Old_R_RenderView = (R_RenderView_t)DetourApply((BYTE *)HL_ADDR_GET(R_RenderView), (BYTE *)New_R_RenderView, (int)HL_ADDR_GET(DTOURSZ_R_RenderView));
}
