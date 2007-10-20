/*
File        : mdt_gltools.cpp
Started     : 2007-008-02 11:56:21
Project     : Mirv Demo Tool
Description : see mdt_gltools.h
*/
#ifndef FILMING_H
#define FILMING_H

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

#include "mdt_media.h" // We Mant RAWGLPICS and other media interfaces

// added 20070922:

// I could have also moved this into the class, but I didn't want to mess around with static members and stuff etc.
typedef void (*R_RenderView__t)( void );
extern R_RenderView__t detoured_R_RenderView_; // filming.cpp
typedef void (*V_RenderView_t)( void );
extern V_RenderView_t detoured_V_RenderView; // filming.cpp
typedef void (*SCR_UpdateScreen_t)( void );
extern SCR_UpdateScreen_t detoured_SCR_UpdateScreen; // filming.cpp

class Filming
{
private:
	#define MIN_FRAME_DURATION 0.001f

public:
	enum DRAW_RESULT { DR_NORMAL, DR_HIDE, DR_MASK };
	enum STEREO_STATE { STS_LEFT, STS_RIGHT };
	enum BUFFER { COLOR, DEPTH, ALPHA };
	enum HUD_REQUEST_STATE { HUDRQ_NORMAL,HUDRQ_HIDE,HUDRQ_CAPTURE_COLOR,HUDRQ_CAPTURE_ALPHA };

private:
	enum MATTE_STAGE { MS_ALL, MS_WORLD, MS_ENTITY };
	enum FILMING_STATE { FS_INACTIVE, FS_STARTING, FS_ACTIVE };

private:
	char m_szFilename[256];

	unsigned int m_nTakes;
	unsigned int m_nFrames;

	int m_iWidth, m_iHeight;
	int m_iCropYOfs,m_iCropHeight; // used to determine the output params (after cropping)

	bool m_bActive;

	CMdt_Media_RAWGLPIC m_GlRawPic;

	MATTE_STAGE m_iMatteStage;

	FILMING_STATE m_iFilmingState;

	DRAW_RESULT shouldDrawDuringWorldMatte(GLenum mode);
	DRAW_RESULT shouldDrawDuringEntityMatte(GLenum mode);

	bool m_bInWireframe;
	GLenum m_iLastMode;

	// added 20070922:
	struct _cameraofs_s { float right; float up; float forward; } _cameraofs;
	bool	_bNewRequestMethod; // if we have R_RenderView etc available
	//bool	_bNoMatteInterpolation;
	bool	_bEnableStereoMode;
	float	_fStereoOffset;

	STEREO_STATE _stereo_state;

	// it is very important to understand this and the things connected to it right:
	bool _bRecordBuffers_FirstCall; 
	// On the one hand Filming::recordBuffres() can get called because the engine advanced in time and rendered an new frame, in this case _bRecordBuffers_FirstCall is true
	// On the other hand we might have triggered a new frame our self by doing an manual call to R_RenderView, in that case _bRecordBuffers_FirstCall is false!!
	// The second case usually can only happen when we have the R_RenderView hook and therefore the code connected to it enabled (which is the defualt).

	void _old_recordBuffers(); // unused

	HUD_REQUEST_STATE _HudRqState;

	struct _cammotion_s
	{
		float Xposition;
		float Yposition;
		float Zposition;
		float Zrotation;
		float Xrotation;
		float Yrotation;
	} _cammotion;

	bool _bCamMotion;
	FILE *pMotionFile, *pMotionFile2;
	long _lMotionTPos,_lMotionTPos2;

	void _MotionFile_BeginContent(FILE *pFile,char *pAdditonalTag,long &ulTPos);
	void MotionFile_Begin();
	void MotionFile_Frame();
	void MotionFile_End();

	bool _bSimulate;

public:
	Filming();
	~Filming();

	void Capture(const char *pszFileTag, int iFileNumber, BUFFER iBuffer);
	DRAW_RESULT shouldDraw(GLenum mode);
	void Start();
	void Stop();
	bool recordBuffers(HDC hSwapHDC,BOOL *bSwapRes);	// call to record from the currently selected buffers, returns true if it already swapped itself, in this case also bSwapRes is the result of SwapBuffers
	void clearBuffers();	// call this (i.e. after Swapping) when we can prepare (clear) our buffers for the next frame

	void setScreenSize(GLint w, GLint h);

	bool isFilming() { return (m_iFilmingState != FS_INACTIVE); }
	bool isFinished() { return !_bRecordBuffers_FirstCall; }
	bool checkClear(GLbitfield mask);

	Filming::DRAW_RESULT doWireframe(GLenum mode);

	float m_MatteColour[3];

	void setMatteColour(float r, float g, float b)
	{
		m_MatteColour[0] = r;
		m_MatteColour[1] = g;
		m_MatteColour[2] = b;
	}

	//
	HUD_REQUEST_STATE giveHudRqState();


	// those are used by R_RenderView, since it doesn't sit in the class yet ( I didn't want to mess around with static properties and this pointers etc.)
	void GetCameraOfs(float *right, float *up, float *forward); // will copy the current camera ofs to the supplied addresses
	float GetStereoOffset(); // returns current stereoofs
	bool bEnableStereoMode();
	STEREO_STATE GetStereoState();
	void SupplyCamMotion(float Xposition,float Yposition,float Zposition,float Zrotation,float Xrotation,float Yrotation); // used by R_RenderView_ to supply Camera Motion Data

	bool bWantsHudCapture; // used by R_RenderView to prepare HUD Captures
	bool bCustomDump();

	void OnHudBeginEvent(); // called by Hud Begin tour
	bool OnHudEndEvnet(); // called by Hud End tour, if pDoLoop is true the toor will cause an loop, otherwise it will continue normal HL code operation

	// I don't know why I wrote these, may be remove them again if u want (but I like em heh):
	bool bNewRequestMethod();
	void bNewRequestMethod(bool bSet);	// if R_RenderView is not available set this to false, cannot be set during filming
	//bool bNoMatteInterpolation();
	//void bNoMatteInterpolation (bool bSet); // if the Interpolation should check for Matte Color on Entity streams (if enabled Matte Color will not be blended, instead it will be overwritten)
	void bEnableStereoMode(bool bSet); // if you enable stereo mode MDT will take left and right images using the stereofs you set

	void SetCameraOfs(float right, float up, float forward); // you can set an static cameraofs here, however during stereomode it should be 0
	void SetStereoOfs(float left_and_rightofs); // will be used in stereo mode to displace the camera left and right, suggested values are between 1.0 - 1.4, value should be positive, otherewise you would switch left and right cam
};

#endif