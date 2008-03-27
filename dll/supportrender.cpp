// Project :  Half-Life Advanced Effects
// File    :  dll/supportrender.h

// Authors : last change / first change / name
// 2008-03-27 / 2008-03-27 / Dominik Tugend

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include "../shared\ogl\glext.h"

#include "supportrender.h"

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

	_eCurrentTarget = RT_FAILED;

	_bHaveGameWindow = false;
	_bHaveHiddenWindow = false;
	_bHaveFramBufferObject = false;

	_bFBOsupported = InstallFrameBufferExtentsion(); // Install EXT_framebuffer_object
}

CHlaeSupportRender::~CHlaeSupportRender()
{
	CleanUp(true);
}

void CHlaeSupportRender::CleanUp(bool bFallBackToGame)
{
	if (bFallBackToGame)
		_eCurrentTarget = RT_GAMEWINDOW;

	if (_eCurrentTarget != RT_FRAMEBUFFEROBJECT)
		_ReleaseFrameBuffer();
}

void CHlaeSupportRender::OnRenderBegin(void)
{
	switch(_eCurrentTarget)
	{
	case RT_FRAMEBUFFEROBJECT:
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT,_frameBuffer );
		break;
	};
}

void CHlaeSupportRender::OnRenderEnd(void)
{
	switch(_eCurrentTarget)
	{
	case RT_FRAMEBUFFEROBJECT:
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
		break;
	};
}

bool CHlaeSupportRender::_CreateFrameBuffer()
{
	if (!_bFBOsupported || _bHaveFramBufferObject) return false;

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
		return false;

	return true;
}

bool CHlaeSupportRender::_ReleaseFrameBuffer()
{
	if (_bHaveFramBufferObject) return false;

	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

	glDeleteFramebuffersEXT( 1, &_frameBuffer );

	glDeleteRenderbuffersEXT( 1, &_rgbaRenderBuffer );
	glDeleteRenderbuffersEXT( 1, &_depthRenderBuffer );

	return true;
}