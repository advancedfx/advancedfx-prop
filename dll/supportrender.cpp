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

#define ERROR_MESSAGE(errmsg) pEngfuncs->Con_Printf("SupportRender: " errmsg "\n");
#define ERROR_MESSAGE_LE(errmsg) pEngfuncs->Con_Printf("SupportRender: " errmsg " (Last Error: %i)\n",::GetLastError());


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

bool InstallFrameBufferExtentsion(void)
{
	if (g_bEXT_framebuffer_object) return true;

	bool bTemp;

	bTemp = NULL != strstr( (char *)glGetString( GL_EXTENSIONS ), "EXT_framebuffer_object" );

	if (bTemp)
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

		bTemp =
			glIsRenderbufferEXT && glBindRenderbufferEXT && glDeleteRenderbuffersEXT && glGenRenderbuffersEXT
			&& glRenderbufferStorageEXT && glGetRenderbufferParameterivEXT && glIsFramebufferEXT && glBindFramebufferEXT
			&& glDeleteFramebuffersEXT && glGenFramebuffersEXT && glCheckFramebufferStatusEXT && glFramebufferTexture1DEXT
			&& glFramebufferTexture2DEXT && glFramebufferTexture3DEXT && glFramebufferRenderbufferEXT && glGetFramebufferAttachmentParameterivEXT
			&& glGenerateMipmapEXT
		;

	}

	g_bEXT_framebuffer_object = bTemp;

	return bTemp;
}

//
//  CHlaeSupportRender
//

CHlaeSupportRender::CHlaeSupportRender(void *hGameWindow, int iWidth, int iHeight)
{
	_hGameWindow = hGameWindow;
	_iWidth = iWidth;
	_iHeight = iHeight;

	_eRenderTarget = RT_GAMEWINDOW;

	//_bHaveGameWindow = false;
	_bHaveOwnContext = false;
	_bHaveFramBufferObject = false;

	_bFBOsupported = InstallFrameBufferExtentsion(); // Install EXT_framebuffer_object
}

CHlaeSupportRender::~CHlaeSupportRender()
{
	_eRenderTarget = RT_GAMEWINDOW;
	_CleanUp();
}

bool CHlaeSupportRender::Has_EXT_FrameBufferObject()
{
	return _bFBOsupported;
}

CHlaeSupportRender::ERenderTarget CHlaeSupportRender::SetRenderTarget (ERenderTarget newRenderTarget, bool bSoftFallBack)
{
	// clean up current render target:
	SwitchToDisplayTarget();
	_eRenderTarget = RT_GAMEWINDOW;
	_CleanUp();

	// switch to new render target:
	
	ERenderTarget myRenderTarget=_eRenderTarget; // on problems we don't change

	switch (newRenderTarget)
	{
	case RT_FRAMEBUFFEROBJECT:
		if (!_CreateFrameBuffer())
		{
			if (!bSoftFallBack)
				break; // no softfallback
		}
		else
		{
			myRenderTarget = RT_FRAMEBUFFEROBJECT;
			break;
		}
		// else do softfallback:

	case RT_OWNCONTEXT:
		if (_CreateOwnContext())
			myRenderTarget = RT_OWNCONTEXT;
		break;

	}
	
	_eRenderTarget = myRenderTarget; // remember new target we set

	return myRenderTarget;
}

void CHlaeSupportRender::SwitchToRenderTarget(void)
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
}

bool CHlaeSupportRender::_CreateFrameBuffer()
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

bool CHlaeSupportRender::_CreateOwnContext()
{
	if (_bHaveOwnContext)
		return false;

	_hGameContextDC = wglGetCurrentDC();
	if ( !_hGameContextDC  )
	{
		ERROR_MESSAGE_LE("could not retrive GameHDC")
		return false;
	}

	_hGameContextGLRC = (void*)(wglGetCurrentContext());
	if ( !_hGameContextGLRC )
	{
		ERROR_MESSAGE_LE("could not retrive GameHGLRC")
		return false;
	}

	_hOwnContextDC = (void *)(CreateCompatibleDC((HDC)_hGameContextDC));
	if (!_hOwnContextDC)
	{
		ERROR_MESSAGE_LE("could not create compatible context")
		return false;
	}

    _hOwnContextBITMAP = (void *)(CreateCompatibleBitmap ( (HDC)_hGameContextDC, _iWidth, _iHeight ));
	if (!_hOwnContextBITMAP)
	{
		ERROR_MESSAGE_LE("could not create compatible bitmap")
		DeleteDC((HDC)_hOwnContextDC);
		return false;
	}

    HGDIOBJ tobj = SelectObject ( (HDC)_hOwnContextDC, (HBITMAP)_hOwnContextBITMAP );
	if (!tobj || tobj == HGDI_ERROR)
	{
		ERROR_MESSAGE_LE("could not select bitmap")
		DeleteObject(_hOwnContextBITMAP);
		DeleteDC((HDC)_hOwnContextDC);
		return false;
	}

	int iPixelFormat = GetPixelFormat((HDC)_hGameContextDC);
	PIXELFORMATDESCRIPTOR *ppfd = new (PIXELFORMATDESCRIPTOR);
	DescribePixelFormat((HDC)_hGameContextDC,iPixelFormat,sizeof(PIXELFORMATDESCRIPTOR),ppfd);

	ppfd->dwFlags = (ppfd->dwFlags & (!(DWORD)PFD_DRAW_TO_WINDOW)) | PFD_DRAW_TO_BITMAP;
	//ppfd->dwFlags = PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED;

	iPixelFormat = ChoosePixelFormat((HDC)_hOwnContextDC,ppfd);
	if (iPixelFormat == 0)
	{
		ERROR_MESSAGE_LE("could not choose PixelFormat")
		delete ppfd;
		DeleteObject(_hOwnContextBITMAP);
		DeleteDC((HDC)_hOwnContextDC);
		return false;
	}

	if (TRUE != SetPixelFormat((HDC)_hOwnContextDC,iPixelFormat,ppfd))
	{
		ERROR_MESSAGE_LE("could not Set PixelFormat")
		delete ppfd;
		DeleteObject(_hOwnContextBITMAP);
		DeleteDC((HDC)_hOwnContextDC);
		return false;
	}

	delete ppfd;

	_hOwnContextGLRC = (void *)(wglCreateContext((HDC)_hOwnContextDC));
	if (!_hOwnContextGLRC)
	{
		ERROR_MESSAGE_LE("could not create own context")
		DeleteObject(_hOwnContextBITMAP);
		DeleteDC((HDC)_hOwnContextDC);
		return false;
	}

	/*if (!wglCopyContext((HGLRC)_hGameContextGLRC,(HGLRC)_hOwnContextGLRC,GL_ALL_ATTRIB_BITS))
	{
		ERROR_MESSAGE_LE("could not copy game context")
		DeleteObject(_hOwnContextBITMAP);
		DeleteDC((HDC)_hOwnContextDC);
		return false;
	}*/

	_bHaveOwnContext=true;

	return true;
}

bool CHlaeSupportRender::_ReleaseOwnContext()
{
	if (!_bHaveOwnContext) return false;

	_bHaveOwnContext = false;

	wglDeleteContext((HGLRC)_hOwnContextGLRC);
	DeleteObject(_hOwnContextBITMAP);
	DeleteDC((HDC)_hOwnContextDC);

	return true;
}

void CHlaeSupportRender::_CleanUp(void)
{
	if (_eRenderTarget != RT_FRAMEBUFFEROBJECT)
		_ReleaseFrameBuffer();

	if (_eRenderTarget != RT_OWNCONTEXT)
		_ReleaseOwnContext();
}
