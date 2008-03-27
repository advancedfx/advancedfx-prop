#ifndef HLAE_SUPPORTRENDER_H
#define HLAE_SUPPORTRENDER_H

// Project :  Half-Life Advanced Effects
// File    :  dll/supportrender.h

// Authors : last change / first change / name
// 2008-03-27 / 2008-03-27 / Dominik Tugend

// Comment:
//   due to the OpenGL PixelOwnership test implementation dependency we have
//   to support rendering to FBO and hidden during recording appart from
//   only rendering to game window (otherwise our users may notice missing
//   aeras in the image dumps)

//#include <gl/gl.h>

class CHlaeSupportRender
{
public:
	enum ERenderTarget
	{
		RT_FAILED,
		RT_GAMEWINDOW,
		RT_HIDDENWINDOW,
		RT_FRAMEBUFFEROBJECT
	};

	CHlaeSupportRender(void *hGameWindow, int iWidth, int iHeight);
	// hGameWindow - 

	~CHlaeSupportRender();
	// also calls CleanUp();

	void CleanUp(bool bFallBackToGame=true);
	// cleans unused render target resources
	// bFallBackToGame - true: set's game window as target before cleanup

	ERenderTarget SetTarget (ERenderTarget newRenderTarget, bool bAutoFallBack=true, bool bAutoCleanUp=true);
	// use this to set a new render target, call only occassionally (overhead)

	// rendering hooks:
	void OnRenderBegin(void);
	// this shall be called before any rendering starts that shall be buffered

	void OnRenderEnd(void);
	// this shall be called after any rendering ends that shall be buffered

private:
	//bool _bInRender; // indicates OnRenderBegin has been called and OnRenderEnd has not been called yet

	bool _bFBOsupported; // EXT_framebuffer_object supported?
	unsigned int _frameBuffer;
	unsigned int _depthRenderBuffer;
	unsigned int _rgbaRenderBuffer;

	ERenderTarget _eCurrentTarget;

	void *_hGameWindow;
	int _iWidth;
	int _iHeight;

	bool _bHaveGameWindow;
	bool _bHaveHiddenWindow;
	bool _bHaveFramBufferObject;

	bool _CreateFrameBuffer();
	bool _ReleaseFrameBuffer();

};

#endif