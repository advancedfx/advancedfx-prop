/*
File        : mdt_media.cpp
Started     : 2007-08-07 12:05:11
Project     : Mirv Demo Tool
Authors     : Dominik Tugend
Description : see mdt_media.h
*/

#include "mdt_media.h"

////////////////////////////////////////////////////////////////////////////////
// Implementation of class CMdt_Media_BASE

// _CMdt_Media_BASE:
void CMdt_Media_BASE::_CMdt_Media_BASE()
{
	pOnErrorHandler = NULL; // by default the class user didn't set an own error handler
	_has_unhandled_error = false;
}

// CMdt_Media_BASE:
CMdt_Media_BASE::CMdt_Media_BASE()
{
	_CMdt_Media_BASE(); // call stuff all constructors should do

	// store media diretion and type:
	_media_direction = MD_UNKOWN;
	_media_type      = MT_UNKOWN;
}

// CMdt_Media_BASE:
CMdt_Media_BASE::CMdt_Media_BASE(MEDIA_TYPE eMediaType, MEDIA_DIRECTION eMediaDirection)
{
	_CMdt_Media_BASE(); // call stuff all constructors should do

	// store media type and direction
	_media_type      = eMediaType;
	_media_direction = eMediaDirection;
}

// GetMediaDirection:
CMdt_Media_BASE::MEDIA_DIRECTION CMdt_Media_BASE::GetMediaDirection()
{
	return _media_direction;
}

// GetMediaType:
CMdt_Media_BASE::MEDIA_TYPE CMdt_Media_BASE::GetMediaType()
{
	return _media_type;
}

// SetMediaDirection:
void CMdt_Media_BASE::SetMediaDirection (MEDIA_DIRECTION eMediaDirection)
{
	_media_direction = eMediaDirection;
}

// SetMediaType:
void CMdt_Media_BASE::SetMediaType(MEDIA_TYPE eMediaType)
{
	_media_type = eMediaType;
}

// _OnErrorHandler:
MDT_MEDIA_ERROR_ACTION	 CMdt_Media_BASE::_OnErrorHandler(unsigned int iErrorCode)
{
	if (pOnErrorHandler) return pOnErrorHandler(iErrorCode); // call the error handler if present
	else
	{
		// there is no error handler present, so we handle it internally:
		_last_unhandled_error = iErrorCode;
		_has_unhandled_error  = true;
		return MMOEA_DEFAULT; // let class decide the action to take
	}
}

////////////////////////////////////////////////////////////////////////////////
// Implementation of class CMdt_Media_RAWGLPIC

// _GetSizeComponent:
bool CMdt_Media_RAWGLPIC::_CalcSizeComponent (GLenum eGLtype, unsigned char* outSizeComponent, bool* outIsSigned)
{
	// well this is not too nice, replace it if u want (may be there is a native GL or GLU header file that already has functions for that:
	// see GL reference for glDrawPixels
	switch(eGLtype)
	{
	case GL_UNSIGNED_BYTE:
		*outIsSigned      = false;
		*outSizeComponent = 1;
		return true;
	case GL_BYTE:
		*outIsSigned      = true;
		*outSizeComponent = 1;
		return true;
	case GL_BITMAP:
		// I am to lazy to code that now, this would require special treatment determining the bitdepth etc.
		return false; // not treated yet
	case GL_UNSIGNED_SHORT:
		*outIsSigned      = false;
		*outSizeComponent = 2;
		return true;
	case GL_SHORT:
		*outIsSigned      = true;
		*outSizeComponent = 2;
		return true;
	case GL_UNSIGNED_INT:
		*outIsSigned      = false;
		*outSizeComponent = 4;
		return true;
	case GL_INT:
		*outIsSigned      = true;
		*outSizeComponent = 4;
		return true;
	case GL_FLOAT:
		*outIsSigned      = true;
		*outSizeComponent = 4;
		return true;
	default:
		return false;
	}
}

// _GetNumComponents:
bool CMdt_Media_RAWGLPIC::_CalcNumComponents (GLenum eGLformat, unsigned char* outNumComponents)
{
	// well this is not too nice, replace it if u want:
	// see http://www.glprogramming.com/blue/ch05.html#id5527285 (OGL ref. for glReadPixels)
	switch(eGLformat)
	{
	case GL_COLOR_INDEX:
	case GL_STENCIL_INDEX:
	case GL_DEPTH_COMPONENT:
	case GL_RED:
	case GL_GREEN:
	case GL_BLUE:
	case GL_ALPHA:
	case GL_LUMINANCE:
		*outNumComponents = 1;
		return true;
	case GL_RGB:
		*outNumComponents = 3;
		return true;
	case GL_RGBA:
		*outNumComponents = 4;
		return true;
	case GL_LUMINANCE_ALPHA:
		*outNumComponents = 2; // WARNING POSSIBLE ERROR - I hope I understood that right heh
		return true;
	default:
		return false;
	}
}

// CMdt_MediaDesc_RAWGLPIC:
CMdt_Media_RAWGLPIC::CMdt_Media_RAWGLPIC(int iWidth, int iHeight, GLenum eGLDataFormat, GLenum eGLDataType,  unsigned char* outPointer)
                    :CMdt_Media_BASE(MT_RAWGLPIC,MD_IN_AND_OUT)
{
	// set pointers to NULL:
	*outPointer = NULL;	// always make sure that friggin pointer is realy NULL (in case s.th. went's wrong)
	_pBuffer    = NULL;	// .

	_iWidth  = iWidth;
	_iHeight = iHeight;

	_eGLformat = eGLDataFormat;
	_eGLtype   = eGLDataType;

	// set some other stuff (cause we might return bellow before it is set):
	_ucNumComponents = 0;
	_ucSizeComponent = 0;
	_bComponentIsSigned = false;
	_uiPixelSize = 0;
	_uiSize = 0;

	// calcualte date required to know the size of a pixel (if possible / known):
	if (!_CalcNumComponents(eGLDataFormat,&_ucNumComponents)) return;
	if (!_CalcSizeComponent(eGLDataType,&_ucSizeComponent,&_bComponentIsSigned)) return;

	// calcualte the size:
	_uiPixelSize = ((unsigned short)_ucNumComponents) * ((unsigned short)_ucSizeComponent);
	_uiSize      = _iWidth * _iHeight * ((unsigned int)_uiPixelSize);

	// try to allocate the memory:
	// fucking VC++ doesn't support new (nothrow) for array types, wtf1111^, that is why we have to fetch the exception:
	try {
		_pBuffer = new unsigned char[_uiSize]; 
	}
	catch(...) {
		// we don't want an exception here, just give NULL ffs
		#ifdef _DEBUG // (Microsoft specific)
			throw; // in _DEBUG mode we want the exception to be thrown
		#endif
		_pBuffer = NULL;
	}
}

// ~CMdt_MediaDesc_RAWGLPIC:
CMdt_Media_RAWGLPIC::~CMdt_Media_RAWGLPIC()
{
	delete[] _pBuffer; // according to C++ documentation this will automatically take care off NULL pointers heh ....
	_pBuffer = NULL; // Just to be sure biatches
}

// IsAlive:
bool CMdt_Media_RAWGLPIC::IsAlive()
{
	return _pBuffer != 0;
}

// GetWidth:
int CMdt_Media_RAWGLPIC::GetWidth()
{
	return _iWidth;
}

// GetHeight:
int CMdt_Media_RAWGLPIC::GetHeight()
{
	return _iHeight;
}

// GetSize:
unsigned int CMdt_Media_RAWGLPIC::GetSize()
{
	return _uiSize;
}

// GetPointer:
unsigned char* CMdt_Media_RAWGLPIC::GetPointer()
{
	return _pBuffer;
}

// GetPixelSize:
unsigned short CMdt_Media_RAWGLPIC::GetPixelSize()
{
	return _uiPixelSize;
}

// GetNumComponents:
unsigned char CMdt_Media_RAWGLPIC::GetNumComponents()
{
	return _ucNumComponents;
}

// GetSizeComponent:
unsigned char CMdt_Media_RAWGLPIC::GetSizeComponent()
{
	return _ucSizeComponent;
}

// GetGLFormat:
GLenum CMdt_Media_RAWGLPIC::GetGLFormat()
{
	return _eGLformat;
}

// GetGLType:
GLenum CMdt_Media_RAWGLPIC::GetGLType()
{
	return _eGLtype;
}