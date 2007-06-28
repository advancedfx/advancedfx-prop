
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

extern cl_enginefuncs_s *pEngfuncs;
extern engine_studio_api_s *pEngStudio;
extern playermove_s *ppmove;

extern float clamp(float, float, float);

REGISTER_CVAR(movie_customdump, "1", 0);
REGISTER_CVAR(movie_depthdump, "0", 0);
REGISTER_CVAR(movie_filename, "untitled", 0);
REGISTER_CVAR(movie_splitstreams, "0", 0);
REGISTER_CVAR(movie_swapweapon, "0", 0);
REGISTER_CVAR(movie_swapdoors, "0", 0);
REGISTER_CVAR(movie_onlyentity, "0", 0);
REGISTER_CVAR(movie_clearscreen, "0", 0);
REGISTER_CVAR(movie_fps, "30", 0);
REGISTER_CVAR(movie_wireframe, "0", 0);
REGISTER_CVAR(movie_wireframesize, "1", 0);

// Our filming singleton
Filming g_Filming;

void Filming::setScreenSize(GLint w, GLint h)
{
	if (m_pBuffer == NULL || m_iWidth < w || m_iHeight < h)
	{
		m_pBuffer = (unsigned char *) realloc(m_pBuffer, w * h * 3);
		m_iWidth = w;
		m_iHeight = h;
	}
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
}

void Filming::Stop()
{
	m_nFrames = 0;
	m_iFilmingState = FS_INACTIVE;
	m_nTakes++;

	// Need to reset this otherwise everything will run crazy fast
	pEngfuncs->Cvar_SetValue("host_framerate", 0);
}

void Filming::Capture(const char *pszFileTag, int iFileNumber, BUFFER iBuffer)
{
	char cDepth = (iBuffer == COLOR ? 2 : 3);
	int iGlBuffer = (iBuffer == COLOR ? GL_BGR_EXT : GL_DEPTH_COMPONENT);
	int nBits = (iBuffer == COLOR ? 3 : 1);

	char szFilename[128];
	_snprintf(szFilename, sizeof(szFilename) - 1, "%s_%s_%02d_%05d.tga", m_szFilename, pszFileTag, m_nTakes, iFileNumber);

	unsigned char szTgaheader[12] = { 0, 0, cDepth, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	unsigned char szHeader[6] = { (int) (m_iWidth % 256), (int) (m_iWidth / 256), (int) (m_iHeight % 256), (int) (m_iHeight / 256), 8 * nBits, 0 };

	FILE *pImage;

	glReadPixels(0, 0, m_iWidth, m_iHeight, iGlBuffer, GL_UNSIGNED_BYTE, m_pBuffer);

	if ((pImage = fopen(szFilename, "wb")) != NULL)
	{
		fwrite(szTgaheader, sizeof(unsigned char), 12, pImage);
		fwrite(szHeader, sizeof(unsigned char), 6, pImage);
		fwrite(m_pBuffer, sizeof(unsigned char), m_iWidth * m_iHeight * nBits, pImage);

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

void Filming::recordBuffers()
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
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		m_iFilmingState = FS_ACTIVE;
		return;
	}

	bool bSplitting = (movie_splitstreams->value == 3.0f);
	float flTime = 1.0f / max(movie_fps->value, 1.0f);

	static char *pszTitles[] = { "all", "world", "entity" };
	static char *pszDepthTitles[] = { "depthall", "depthworld", "depthall" };

	// Clear up the screen
	if (movie_clearscreen->value != 0.0f)
	{
		pEngfuncs->Cvar_SetValue("hud_draw", 0);
		pEngfuncs->Cvar_SetValue("crosshair", 0);
	}

	pEngfuncs->Cvar_SetValue("gl_clear", 1);

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

	// Now we do our clearing!
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

bool Filming::checkClear(GLbitfield mask)
{
	// Don't clear unless we specify
	if (isFilming() && (mask & GL_COLOR_BUFFER_BIT || mask & GL_DEPTH_BUFFER_BIT))
		return false;

	// Make sure the mask colour is still correct
	glClearColor(m_MatteColour[0], m_MatteColour[1], m_MatteColour[2], 1.0f);
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

REGISTER_CMD_FUNC(matte_setcolour)
{
	if (pEngfuncs->Cmd_Argc() != 4)
	{
		pEngfuncs->Con_Printf("Useage: " PREFIX "entity_setcolourjump <red: 0-100> <green: 0-100> <blue: 0-100>\n");
		return;
	}

	float flRed = (float) atoi(pEngfuncs->Cmd_Argv(1)) / 100.0f;
	float flGreen = (float) atoi(pEngfuncs->Cmd_Argv(2)) / 100.0f;
	float flBlue = (float) atoi(pEngfuncs->Cmd_Argv(3)) / 100.0f;

	clamp(flRed, 0.0f, 1.0f);
	clamp(flGreen, 0.0f, 1.0f);
	clamp(flBlue, 0.0f, 1.0f);

	g_Filming.setMatteColour(flRed, flGreen, flBlue);
}
