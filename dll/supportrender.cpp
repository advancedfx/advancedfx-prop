// Project :  Half-Life Advanced Effects
// File    :  dll/supportrender.h

// Authors : last change / first change / name
// 2008-03-28 / 2008-03-27 / Dominik Tugend

// Comment: see supportrender.h

#include <windows.h>

#include <gl\gl.h>
#include <gl\glu.h>
#include "../shared\ogl\glext.h"

#include "wrect.h"
#include "cl_dll.h"
#include "cdll_int.h"
//#include "r_efx.h"
//#include "com_model.h"
//#include "r_studioint.h"
//#include "pm_defs.h"
//#include "cvardef.h"
//#include "entity_types.h"

#include "supportrender.h"

extern cl_enginefuncs_s *pEngfuncs;

//#define ERROR_MESSAGE(errmsg) pEngfuncs->Con_Printf("SupportRender: " errmsg "\n");

char g_ErrorMessageBuffer[300];

#define ERROR_MESSAGE(errmsg) MessageBoxA(0, errmsg, "SupportRender:",MB_OK|MB_ICONERROR);
#define ERROR_MESSAGE_LE(errmsg) \
	{ \
		_snprintf(g_ErrorMessageBuffer,sizeof(g_ErrorMessageBuffer)-1,"SupportRender:" errmsg "\nGetLastError: %i",::GetLastError()); \
		g_ErrorMessageBuffer[sizeof(g_ErrorMessageBuffer)-1]=0; \
		MessageBoxA(0, g_ErrorMessageBuffer, "SupportRender:",MB_OK|MB_ICONERROR); \
	}


//
// EXT_framebuffer_object
//
//
// http://www.opengl.org/registry/specs/EXT/framebuffer_object.txt
//

bool g_bEXT_framebuffer_object=false;

PFNGLISRENDERBUFFEREXTPROC glIsRenderbufferEXT = NULL;
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT = NULL;
PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT = NULL;
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT = NULL;
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT = NULL;
PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC glGetRenderbufferParameterivEXT = NULL;
PFNGLISFRAMEBUFFEREXTPROC glIsFramebufferEXT = NULL;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT = NULL;
PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT = NULL;
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT = NULL;
PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE1DEXTPROC glFramebufferTexture1DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT = NULL;
PFNGLFRAMEBUFFERTEXTURE3DEXTPROC glFramebufferTexture3DEXT = NULL;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameterivEXT = NULL;
PFNGLGENERATEMIPMAPEXTPROC glGenerateMipmapEXT = NULL;

CHlaeSupportRender::EFboSupport InstallFrameBufferExtentsion(void)
// do not install before a context has been created, because
// in this case glGetString will not work (return 0)
{
	if (g_bEXT_framebuffer_object) return CHlaeSupportRender::FBOS_YES;

	if (!wglGetCurrentContext())
	{
		ERROR_MESSAGE("InstallFrameBufferExtentsion called before context was installed")
		return CHlaeSupportRender::FBOS_UNKNOWN;
	}

	CHlaeSupportRender::EFboSupport retTemp=CHlaeSupportRender::FBOS_NO;
	char *pExtStr = (char *)(glGetString( GL_EXTENSIONS ));
	if (!pExtStr)
	{
		retTemp = CHlaeSupportRender::FBOS_UNKNOWN;
		ERROR_MESSAGE("glGetString failed!")
	}
	else if (strstr( pExtStr, "EXT_framebuffer_object" ))
	{
		glIsRenderbufferEXT = (PFNGLISRENDERBUFFEREXTPROC)wglGetProcAddress("glIsRenderbufferEXT");
		glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
		glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
		glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
		glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
		glGetRenderbufferParameterivEXT = (PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC)wglGetProcAddress("glGetRenderbufferParameterivEXT");
		glIsFramebufferEXT = (PFNGLISFRAMEBUFFEREXTPROC)wglGetProcAddress("glIsFramebufferEXT");
		glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
		glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
		glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
		glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
		glFramebufferTexture1DEXT = (PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)wglGetProcAddress("glFramebufferTexture1DEXT");
		glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
		glFramebufferTexture3DEXT = (PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)wglGetProcAddress("glFramebufferTexture3DEXT");
		glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
		glGetFramebufferAttachmentParameterivEXT = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC)wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
		glGenerateMipmapEXT = (PFNGLGENERATEMIPMAPEXTPROC)wglGetProcAddress("glGenerateMipmapEXT");

		if (
			glIsRenderbufferEXT && glBindRenderbufferEXT && glDeleteRenderbuffersEXT && glGenRenderbuffersEXT
			&& glRenderbufferStorageEXT && glGetRenderbufferParameterivEXT && glIsFramebufferEXT && glBindFramebufferEXT
			&& glDeleteFramebuffersEXT && glGenFramebuffersEXT && glCheckFramebufferStatusEXT && glFramebufferTexture1DEXT
			&& glFramebufferTexture2DEXT && glFramebufferTexture3DEXT && glFramebufferRenderbufferEXT && glGetFramebufferAttachmentParameterivEXT
			&& glGenerateMipmapEXT
		) retTemp = CHlaeSupportRender::FBOS_YES;


	}

	g_bEXT_framebuffer_object = CHlaeSupportRender::FBOS_YES == retTemp;

	return retTemp;
}

//
//  CHlaeSupportRender
//

CHlaeSupportRender::CHlaeSupportRender(HWND hGameWindow, int iWidth, int iHeight)
{
	_hGameWindow = hGameWindow;
	_iWidth = iWidth;
	_iHeight = iHeight;

	_eRenderTarget = RT_NULL;
	_eFBOsupported = FBOS_UNKNOWN;

	_ownHGLRC = NULL;
}

CHlaeSupportRender::~CHlaeSupportRender()
{
	if (RT_NULL != _eRenderTarget) hlaeDeleteContext (_ownHGLRC);
	_eRenderTarget = RT_NULL;
}

CHlaeSupportRender::EFboSupport CHlaeSupportRender::Has_EXT_FrameBufferObject()
{
	return _eFBOsupported;
}

CHlaeSupportRender::ERenderTarget CHlaeSupportRender::GetRenderTarget()
{
	return _eRenderTarget;
}

HGLRC CHlaeSupportRender::GetHGLRC()
{
	return _ownHGLRC;
}

HDC	CHlaeSupportRender::GetOwnContextHDC()
{
	if (RT_OWNCONTEXT != _eRenderTarget)
		return NULL;

	return _OwnContext_r.ownHDC;
}

HGLRC CHlaeSupportRender::hlaeCreateContext (ERenderTarget eRenderTarget, HDC hGameWindowDC)
{
	if(RT_NULL != _eRenderTarget)
	{
		ERROR_MESSAGE("already using a target")
		return NULL;
	}

	switch (eRenderTarget)
	{
	case RT_NULL:
		ERROR_MESSAGE("cannot Create RT_NULL target")
		return NULL;
	case RT_GAMEWINDOW:
		return _Create_RT_GAMEWINDOW (hGameWindowDC);
	case RT_OWNCONTEXT:
		return _Create_RT_OWNCONTEXT (hGameWindowDC);
	case RT_FRAMEBUFFEROBJECT:
		return _Create_RT_FRAMEBUFFEROBJECT (hGameWindowDC);
	}

	ERROR_MESSAGE("cannot Create unknown target")
	return NULL;
}

BOOL CHlaeSupportRender::hlaeDeleteContext (HGLRC hGlRc)
{
	if (RT_NULL==_eRenderTarget)
	{
		ERROR_MESSAGE("cannot Delete RT_NULL target")
		return FALSE;
	}
	else if (hGlRc != _ownHGLRC)
	{
		ERROR_MESSAGE("cannot Delete, hGlRc is not managed by this class")
		return FALSE;
	}

	switch (_eRenderTarget)
	{
	case RT_GAMEWINDOW:
		return _Delete_RT_GAMEWINDOW ();
	case RT_OWNCONTEXT:
		return _Delete_RT_OWNCONTEXT ();
	case RT_FRAMEBUFFEROBJECT:
		return _Delete_RT_FRAMEBUFFEROBJECT ();
	}

	ERROR_MESSAGE("cannot delete unknown target")
	return FALSE;
}

BOOL CHlaeSupportRender::hlaeMakeCurrent(HDC hGameWindowDC, HGLRC hGlRc)
{
	if (RT_NULL==_eRenderTarget)
	{
		ERROR_MESSAGE("cannot MakeCurrent RT_NULL target")
		return FALSE;
	}
	else if (hGlRc != _ownHGLRC)
	{
		ERROR_MESSAGE("cannot MakeCurrent, hGlRc is not managed by this class")
		return FALSE;
	}

	switch (_eRenderTarget)
	{
	case RT_GAMEWINDOW:
		return _MakeCurrent_RT_GAMEWINDOW (hGameWindowDC);
	case RT_OWNCONTEXT:
		return _MakeCurrent_RT_OWNCONTEXT (hGameWindowDC);
	case RT_FRAMEBUFFEROBJECT:
		return _MakeCurrent_RT_FRAMEBUFFEROBJECT (hGameWindowDC);
	}

	ERROR_MESSAGE("cannot MakeCurrent unknown target")
	return FALSE;
}

BOOL CHlaeSupportRender::hlaeSwapBuffers(HDC hGameWindowDC)
{
	if (RT_NULL==_eRenderTarget)
	{
		ERROR_MESSAGE("cannot SwapBuffers RT_NULL target")
		return FALSE;
	}

	switch (_eRenderTarget)
	{
	case RT_GAMEWINDOW:
		return SwapBuffers (hGameWindowDC);
	case RT_OWNCONTEXT:
		return _SwapBuffers_RT_GAMEWINDOW (hGameWindowDC);
	case RT_FRAMEBUFFEROBJECT:
		return SwapBuffers (hGameWindowDC);
	}

	ERROR_MESSAGE("cannot SwapBuffers unknown target")
	return FALSE;
}

/*void CHlaeSupportRender::SwitchToRenderTarget(void)
{
	switch(_eRenderTarget)
	{
	case RT_OWNCONTEXT:
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		wglMakeCurrent((HDC)_hOwnContextDC,(HGLRC)_hOwnContextGLRC);
		glPopAttrib();
		break;
	case RT_FRAMEBUFFEROBJECT:
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT,_frameBuffer );
		break;
	};
}

void CHlaeSupportRender::SwitchToDisplayTarget(void)
{
	switch(_eRenderTarget)
	{
	case RT_OWNCONTEXT:
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		wglMakeCurrent((HDC)_hGameContextDC,(HGLRC)_hGameContextGLRC);
		glPopAttrib();
		break;
	case RT_FRAMEBUFFEROBJECT:
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
		break;
	};
}*/

HGLRC CHlaeSupportRender::_Create_RT_GAMEWINDOW (HDC hGameWindowDC)
{
	_ownHGLRC = wglCreateContext(hGameWindowDC);

	if(_ownHGLRC) _eRenderTarget=RT_GAMEWINDOW;

	return _ownHGLRC;
}

BOOL CHlaeSupportRender::_Delete_RT_GAMEWINDOW ()
{
	BOOL wbRet = wglDeleteContext(_ownHGLRC);

	if (TRUE != wbRet)
		return wbRet;

	_eRenderTarget=RT_NULL;
	_ownHGLRC=NULL;
	
	return wbRet;
}

BOOL CHlaeSupportRender::_MakeCurrent_RT_GAMEWINDOW (HDC hGameWindowDC)
{
	return wglMakeCurrent(hGameWindowDC,_ownHGLRC);
}

HGLRC CHlaeSupportRender::_Create_RT_OWNCONTEXT (HDC hGameWindowDC)
{
	_OwnContext_r.ownHDC = CreateCompatibleDC(hGameWindowDC);
	if (!_OwnContext_r.ownHDC)
	{
		ERROR_MESSAGE_LE("could not create compatible context")
		return NULL;
	}

    _OwnContext_r.ownHBITMAP = CreateCompatibleBitmap ( hGameWindowDC, _iWidth, _iHeight );
	if (!_OwnContext_r.ownHBITMAP)
	{
		ERROR_MESSAGE_LE("could not create compatible bitmap")
		DeleteDC(_OwnContext_r.ownHDC);
		return NULL;
	}

    HGDIOBJ tobj = SelectObject ( _OwnContext_r.ownHDC, _OwnContext_r.ownHBITMAP );
	if (!tobj || tobj == HGDI_ERROR)
	{
		ERROR_MESSAGE_LE("could not select bitmap")
		DeleteObject(_OwnContext_r.ownHBITMAP);
		DeleteDC(_OwnContext_r.ownHDC);
		return NULL;
	}

	int iPixelFormat = GetPixelFormat( hGameWindowDC );
	PIXELFORMATDESCRIPTOR *ppfd = new (PIXELFORMATDESCRIPTOR);
	DescribePixelFormat( hGameWindowDC, iPixelFormat, sizeof(PIXELFORMATDESCRIPTOR), ppfd );

	ppfd->dwFlags = (ppfd->dwFlags & !( (DWORD)PFD_DRAW_TO_WINDOW | (DWORD)PFD_DOUBLEBUFFER  )) | PFD_DRAW_TO_BITMAP;
	//ppfd->dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED;

	iPixelFormat = ChoosePixelFormat(_OwnContext_r.ownHDC,ppfd);
	if (iPixelFormat == 0)
	{
		ERROR_MESSAGE_LE("could not choose PixelFormat")
		delete ppfd;
		DeleteObject(_OwnContext_r.ownHBITMAP);
		DeleteDC(_OwnContext_r.ownHDC);
		return NULL;
	}

	if (TRUE != SetPixelFormat(_OwnContext_r.ownHDC,iPixelFormat,ppfd))
	{
		ERROR_MESSAGE_LE("could not Set PixelFormat")
		delete ppfd;
		DeleteObject(_OwnContext_r.ownHBITMAP);
		DeleteDC(_OwnContext_r.ownHDC);
		return NULL;
	}

	delete ppfd;

	_ownHGLRC = wglCreateContext(_OwnContext_r.ownHDC);
	if (!_ownHGLRC)
	{
		ERROR_MESSAGE_LE("could not create own context")
		DeleteObject(_OwnContext_r.ownHBITMAP);
		DeleteDC(_OwnContext_r.ownHDC);
		return NULL;
	}

	_eRenderTarget=RT_OWNCONTEXT;

	return _ownHGLRC;
}

BOOL CHlaeSupportRender::_Delete_RT_OWNCONTEXT ()
{
	BOOL wbRet = wglDeleteContext(_ownHGLRC);
	
	if (TRUE != wbRet)
		return wbRet;

	_eRenderTarget=RT_NULL;
	_ownHGLRC=NULL;

	DeleteObject(_OwnContext_r.ownHBITMAP);
	DeleteDC(_OwnContext_r.ownHDC);

	return wbRet;
}

BOOL CHlaeSupportRender::_MakeCurrent_RT_OWNCONTEXT (HDC hGameWindowDC)
{
	return wglMakeCurrent( _OwnContext_r.ownHDC, _ownHGLRC );
}

BOOL CHlaeSupportRender::_SwapBuffers_RT_GAMEWINDOW (HDC hGameWindowDC)
{
	BOOL bwRet=FALSE;
	bwRet=BitBlt(hGameWindowDC,0,0,_iWidth,_iHeight,_OwnContext_r.ownHDC,0,0,SRCCOPY);
	//SwapBuffers(hGameWindowDC);
	return bwRet;
}

HGLRC CHlaeSupportRender::_Create_RT_FRAMEBUFFEROBJECT (HDC hGameWindowDC)
{
	return NULL;
}

BOOL CHlaeSupportRender::_Delete_RT_FRAMEBUFFEROBJECT ()
{
	return FALSE;
}

BOOL CHlaeSupportRender::_MakeCurrent_RT_FRAMEBUFFEROBJECT (HDC hGameWindowDC)
{
	return FALSE;
}


/* bool CHlaeSupportRender::_CreateFrameBuffer()
{
	if (!_bFBOsupported || _bHaveFramBufferObject)
	{
		if (!_bFBOsupported) ERROR_MESSAGE("FBO not supported OR already present")
		return false;
	}

	glGenFramebuffersEXT( 1, &_frameBuffer );

	glGenRenderbuffersEXT( 1, &_depthRenderBuffer );
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, _depthRenderBuffer );
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, _iWidth, _iHeight );
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, _depthRenderBuffer );

	glGenRenderbuffersEXT( 1, &_rgbaRenderBuffer );
	glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, _rgbaRenderBuffer );
	glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_RGBA, _iWidth, _iHeight );
	glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, _rgbaRenderBuffer );

	if (GL_FRAMEBUFFER_COMPLETE_EXT != glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT ))
	{
		ERROR_MESSAGE("FBO status not complete")
		return false;
	}

	return true;
}

bool CHlaeSupportRender::_ReleaseFrameBuffer()
{
	if (!_bHaveFramBufferObject) return false;

	_bHaveFramBufferObject = false;

	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

	glDeleteFramebuffersEXT( 1, &_frameBuffer );

	glDeleteRenderbuffersEXT( 1, &_rgbaRenderBuffer );
	glDeleteRenderbuffersEXT( 1, &_depthRenderBuffer );

	return true;
}
*/