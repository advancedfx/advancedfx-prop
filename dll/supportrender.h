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

#include <windows.h>

class CHlaeSupportRender
// please read the comment at the top of this file for further usuage instructions
{
public:
	enum ERenderTarget
	{
		RT_NULL, // not set yet
		RT_GAMEWINDOW, // read comment at top of file
		RT_OWNCONTEXT, // .
		RT_FRAMEBUFFEROBJECT // .
	};

	enum EFboSupport
	{
		FBOS_UNKNOWN, // could not be determined (yet)
		FBOS_NO, // EXT_framebuffer_object is not supported
		FBOS_YES // EXT_framebuffer_object is supported
	};

	CHlaeSupportRender(HWND hGameWindow, int iWidth, int iHeight);
	// defaults RenderTarget to RT_NULL
	// hGameWindow - HWND Window Handle of the GameWindow
	// iWidth - total width in pixels of the image data (GameResolution)
	// iHeight - total height in pixels of the image data (GameResolution)

	~CHlaeSupportRender();
	// if RenderTarget is different from RT_NULL hlaeDeleteContext will be implecitly called on destroy

	EFboSupport Has_EXT_FrameBufferObject();
	// will only return s.th. useful when a RT_FRAMEBUFFEROBJECT target was created or the creation attempt at least advanced far enough
	// returns
    //   FBOS_YES     : if EXT_framebuffer_object supported (doesn't mean it supports all we will need though)
	//   FBOS_NO      : if not supported 
	//   FBOS_UNKNOWN : the support may not be determined before the context has been created

	ERenderTarget GetRenderTarget();
	// returns the current RenderTarget

	HGLRC GetHGLRC();
	// returns handle to the current HGLRC or NULL on error

	HDC	GetOwnContextHDC();
	// only valid when RenderTarget is RT_OWNCONTEXT
	// returns the handle to the internal HDC or NULL on error

	HGLRC hlaeCreateContext (ERenderTarget eRenderTarget, HDC hGameWindowDC);
	// this should be placed in a wglCreateContext hook
	// ATTENTION: Make sure you use this for the right HDC and the right CreateContext call only!
	// ATTENTION: If there is already a managed render target, the call will fail!
	// returns NULL on fail, otherwise the OpenGLContext device handle
	// eRenderTarget -> read the comment at the top of this file
	// hGameWindowDC -> DC That we shall derive from
	//
	// To speed up rendering you can:
	// - avoid using RT_OWNCONTEXT (use RT_FRAMEBUFFEROBJECT or RT_GAMEWINDOW instead whenever possible)
	// - reduce calls to DisplayRenderTarget in case you don't need to display every frame
	
	BOOL hlaeDeleteContext (HGLRC hGlRc);
	// this should be placed in a wglDelteContext hook
	// ATTENTION: If no render target is present (RT_NULL) or hGlRc is not the one managed by this class, this call will fail.
	// may also be called ~CHlaeSupportRender() is RenderTarget is different from RT_NULL

	BOOL hlaeMakeCurrent(HDC hGameWindowDC, HGLRC hGlRc);
	// this should be placed in a wglMakeCurrent hook
	// ATTENTION: If no render target is present (RT_NULL) or hGlRc is not the one managed by this class, this call will fail.

	BOOL hlaeSwapBuffers(HDC hGameWindowDC);
	// this should be placed in a [wgl]SwapBuffers hook
	// ATTENTION: If no render target is present (RT_NULL) this call will fail.
	// ATTENTION: the caller has to make sure, that the context that is to be swapped is the right one

private:
	ERenderTarget _eRenderTarget;

	HWND _hGameWindow;
	int _iWidth;
	int _iHeight;

	EFboSupport _eFBOsupported; // EXT_framebuffer_object support check

	// shared:
	HGLRC	_ownHGLRC;

	// for FrameBufferObject only:
	struct _FrameBufferObject_s
	{
		unsigned int _frameBuffer;
		unsigned int _depthRenderBuffer;
		unsigned int _rgbaRenderBuffer;
	} _FrameBufferObject_r;

	// for OwnContext only:
	struct _OwnContext_s
	{
		HDC		ownHDC;
		HBITMAP	ownHBITMAP;
	} _OwnContext_r;

	// functions:

	HGLRC	_Create_RT_GAMEWINDOW (HDC hGameWindowDC);
	BOOL	_Delete_RT_GAMEWINDOW ();
	BOOL	_MakeCurrent_RT_GAMEWINDOW (HDC hGameWindowDC);

	HGLRC	_Create_RT_OWNCONTEXT (HDC hGameWindowDC);
	BOOL	_Delete_RT_OWNCONTEXT ();
	BOOL	_MakeCurrent_RT_OWNCONTEXT (HDC hGameWindowDC);
	BOOL	_SwapBuffers_RT_GAMEWINDOW (HDC hGameWindowDC);

	HGLRC	_Create_RT_FRAMEBUFFEROBJECT (HDC hGameWindowDC);
	BOOL	_Delete_RT_FRAMEBUFFEROBJECT ();
	BOOL	_MakeCurrent_RT_FRAMEBUFFEROBJECT (HDC hGameWindowDC);
};

#endif