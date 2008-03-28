#ifndef HLAE_SUPPORTRENDER_H
#define HLAE_SUPPORTRENDER_H

// Project :  Half-Life Advanced Effects
// File    :  dll/supportrender.h

// Authors : last change / first change / name
// 2008-03-28 / 2008-03-27 / Dominik Tugend

// Comment:
//   Due to the OpenGL FrameBuffer contents depending on the Pixel OwnerShip
//   Test implementation of the used rendering hardware / software, we cannot
//   safely read from the usual GameWindow with glReadPixels, because window
//   parts that are not on-screen or covered by other windows may cause
//   those implementations to discard fragments for these areas, which can
//   result in black or random areas.
//
//   The CHlaeSupportRender class has been created to deal with this problem
//   by supplying additional render targets. But not all of these targets can
//   be considered to be supported by the customer's GL implementation or to
//   work as expected.
//
//   The idea is to switch the target during recording only, so that the user
//   still has some of the performance he is used to when not recroding.
//
//   The following targets are supported by the class:
//
//   RT_GAMEWINDOW:
//     This is the default render target, and means using the original game
//     window, which we want to avoid of course. This will most likely cause
//     problems on NVIDIA cards which may omit covered window parts in the
//     fram buffer.
//     However in case of problems with the other target this is our last
//     resort the class may fall back to.
//    
//  RT_OWNCONTEXT:
//    In the current implementation this is actually not a hidden window, but
//    instead a memory Device Context to which we render. This may work for
//    many users, but might also not work all.
//    But still better than the game window in case other methods are not
//    available.
//
//  RT_FRAMEBUFFEROBJECT:
//    This is actually what we are longing for, however this requires the
//    EXT_framebuffers_object OpenGL extension (implemented at least to fit
//    our needs to render to RenderBuffer Objects), which is not present in
//    older implementations.
//    If present this will render to a FrameBuffer Object (to which we assign
//    a GL_RGBA and a GL_DEPTH_COMPONENT RenderBuffer Object, so no stenceling
//    is buffered at the moment).

class CHlaeSupportRender
{
public:
	enum ERenderTarget
	{
		RT_GAMEWINDOW,
		RT_OWNCONTEXT,
		RT_FRAMEBUFFEROBJECT
	};

	CHlaeSupportRender(void *hGameWindow, int iWidth, int iHeight);
	// prepares the targets:
	//   DisplayTarget - always RT_GAMEWINDOW
	//   RenderTarget - defaults to RT_GAMEWINDOW
	// hGameWindow - HWND Window Handle of the GameWindow
	// iWidth - total width in pixels of the image data (GameResolution)
	// iHeight - total height in pixels of the image data (GameResolution)

	~CHlaeSupportRender();
	// will switch to DisplayTarget, restore render target to RT_GAMEWINDOW and clean up.

	bool Has_EXT_FrameBufferObject();
	// returns true if EXT_framebuffer_object supported (doesn't mean it supports all we will need though), otherwise false

	ERenderTarget SetRenderTarget (ERenderTarget newRenderTarget, bool bSoftFallBack=true);
	// use this to set a new render target, call only occassionally (overhead),
	// Make sure it's safe to call it (don't call while things are still rendered (i.e. between glBegin and glEnd))
	// Returns: the new render target (if s.th. went wrong this can be different from the requested target)
	// newRenderTarget - the desired target, for more info read the comments at the top of this file
	// bSoftFallBack - if true and newRenderTarget is RT_FRAMEBUFFEROBJECT and it is not available, SetTarget will try RT_OWNCONTEXT before falling back to the default target

	//
	// rendering control:
	//

	// To speed up rendering you can:
	// - avoid unnecessary switches between RenderTarget and DisplayTarget,
	//   especially with RT_OWNCONTEXT these can be a bit expensive
	// - reduce calls to DisplayRenderTarget in case you don't need to display every frame

	void SwitchToRenderTarget(void);
	// Shall be called before any rendering starts that shall be carried out on the rendertarget
	// Make sure it's safe to call it (don't call while things are still rendered (i.e. between glBegin and glEnd))

	void SwitchToDisplayTarget(void);
	// Shall be called before any rendering starts that shall be carried out on the DisplayTarget.
	// The DisplayTarget is always RT_GAMEWINDOW
	// Make sure it's safe to call it (don't call while things are still rendered (i.e. between glBegin and glEnd))

	void DisplayRenderTarget(void);
	// Displays the (color buffer) contents of RenderTarget on the DisplayTarget.
	// The DisplayTarget is always RT_GAMEWINDOW
	// Make sure it's safe to call it (don't call while things are still rendered (i.e. between glBegin and glEnd))

private:
	ERenderTarget _eRenderTarget;

	void *_hGameWindow;
	int _iWidth;
	int _iHeight;

	bool _bFBOsupported; // EXT_framebuffer_object supported?
	
	// for FrameBufferObject:
	bool _bHaveFramBufferObject;
	unsigned int _frameBuffer;
	unsigned int _depthRenderBuffer;
	unsigned int _rgbaRenderBuffer;

	// for OwnContext:
	bool _bHaveOwnContext;
	void *_hOwnContextDC;
	void *_hOwnContextGLRC;
	void *_hOwnContextBITMAP;
	void *_hGameContextDC;
	void *_hGameContextGLRC;

	bool _CreateFrameBuffer();
	bool _ReleaseFrameBuffer();

	bool _CreateOwnContext();
	bool _ReleaseOwnContext();

	void _CleanUp(void);
};

#endif