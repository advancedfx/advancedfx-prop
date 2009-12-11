//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-30 dominik.matrixstorm.com
//
// First changes
// 2009-11-30 dominik.matrixstorm.com
/*
#include "RawMedia.h"

#include <gl/gl.h>
#include <malloc.h>



size_t CalcImageSize(unsigned int width, unsigned int height, unsigned int pixelByteCount) {
	GLint g_GlPackAlignment;
	
	glGetIntegerv(GL_PACK_ALIGNMENT, &g_GlPackAlignment);

	size_t row_data_bytes = (size_t)width *(size_t)pixelByteCount;

	return
		(size_t)height *(size_t)g_GlPackAlignment 
		*(
			row_data_bytes /(size_t)g_GlPackAlignment
			+(row_data_bytes % (size_t)g_GlPackAlignment ? 1 : 0)
		)
	;	
}


// RawBgrImage /////////////////////////////////////////////////////////////////

RawByteImage::RawByteImage(unsigned int width, unsigned int height)
: RawImage(width, height, CalcImageSize(width, height, 3*sizeof(GLbyte)) {
}



// RawByteImage ////////////////////////////////////////////////////////////////

RawByteImage::RawByteImage(unsigned int width, unsigned int height)
: RawImage(width, height, CalcImageSize(width, height, 1*sizeof(GLbyte)) {
}


// RawFloatImage //////////////////////////////////////////////////////////////////

RawFloatImage::RawFloatImage(unsigned int width, unsigned int height)
: RawImage(width, height, CalcImageSize(width, height, 1*sizeof(GLfloat)) {
}


// RawImage ////////////////////////////////////////////////////////////////////

RawImage::RawImage(unsigned int width, unsigned int height, unsigned int rowAlignment)
: RawMemory(
	(size_t)height *(size_t)rowAlignment *((size_t)width / (size_t)rowAlignment
		+(width % rowAlignment ? 1 : 0)
	)
) {
	m_Height = height;
	m_RowAlignment = rowAlignment;
	m_Width = width;
}

unsigned int RawImage::getHeight() {
	return m_Height;
}

unsigned int RawImage::getRowAlignment() {
	return m_RowAlignment;
}

unsigned int RawImage::getWidth() {
	return m_Width
}


// RawMemory ///////////////////////////////////////////////////////////////////

RawMemory::RawMemory(size_t size) {
	m_Memory = malloc(size);
}

RawMemory::~RawMemory(size_t size) {
	::free(m_Memory);
}

void * RawMemory::get() {
	return m_Memory;
}

void * RawMemory::getSize() {
	return _msize(m_Memory);
}

void * RawMemory::realloc(size_t size) {
	return
		m_Memory = ::realloc(m_Memory, size)
	;
}
*/

