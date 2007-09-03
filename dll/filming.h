
#ifndef FILMING_H
#define FILMING_H

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>

#include "mdt_media.h" // We Mant RAWGLPICS and other media interfaces

class Filming
{
private:
	#define MIN_FRAME_DURATION 0.001f

public:
	enum DRAW_RESULT { DR_NORMAL, DR_HIDE, DR_MASK };

private:
	enum MATTE_STAGE { MS_ALL, MS_WORLD, MS_ENTITY };
	enum BUFFER { COLOR, DEPTH };
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

	float m_MatteColour[3];

	FILMING_STATE m_iFilmingState;

	DRAW_RESULT shouldDrawDuringWorldMatte(GLenum mode);
	DRAW_RESULT shouldDrawDuringEntityMatte(GLenum mode);

	bool m_bInWireframe;
	GLenum m_iLastMode;

public:
	Filming()
	{
		m_bInWireframe = false;
	}
	~Filming()
	{
	}

	void Capture(const char *pszFileTag, int iFileNumber, BUFFER iBuffer);
	DRAW_RESULT shouldDraw(GLenum mode);
	void Start();
	void Stop();
	void recordBuffers();	// call to record from the currently selected buffers
	void clearBuffers();	// call this (i.e. after Swapping) when we can prepare (clear) our buffers for the next frame

	void setScreenSize(GLint w, GLint h);

	bool isFilming() { return (m_iFilmingState != FS_INACTIVE); }
	bool checkClear(GLbitfield mask);

	Filming::DRAW_RESULT doWireframe(GLenum mode);

	void setMatteColour(float r, float g, float b)
	{
		m_MatteColour[0] = r;
		m_MatteColour[1] = g;
		m_MatteColour[2] = b;
	}
};

#endif