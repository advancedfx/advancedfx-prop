/*
File        : mdt_media.h
Started     : 2007-08-07 12:05:11
Project     : Mirv Demo Tool
Authors     : Dominik Tugend
Description : Header File containing classes media files and handling them
*/
#ifndef MDT_MEDIA_H
#define MDT_MEDIA_H

#include <windows.h>
#include <gl\gl.h>

////////////////////////////////////////////////////////////////////////////////
// some defs:

// error codes:
#define MDT_MEDIA_ERROR_NONE	0 // no error happend
#define	MDT_MEDIA_ERROR_UNKOWN	1 // an unknown error happend

// error handle return codes:
#define MDT_MEDIA_EHANDLER_RET_NONE		0

////////////////////////////////////////////////////////////////////////////////
// Stuff that has to do with on the fly runtime error handling:
enum MDT_MEDIA_ERROR_ACTION // the classes will probably currently ignore this, it's there for upward compatibility of the code
{
	MMOEA_DEFAULT,			// let class decide the best action
	MMOEA_CONTINUE,			// try to continue with best effort (the class will try to solve or at least ignore the error with existing code if possible)
	MMOEA_EXIT				// try to break the current operations
};

typedef MDT_MEDIA_ERROR_ACTION (*MDT_MEDIA_ERROR_HANDLER_FN)(unsigned int iErrorCode);

////////////////////////////////////////////////////////////////////////////////
class CMdt_Media_BASE
////////////////////////////////////////////////////////////////////////////////
//
// This is our base class from which all Media Classes shall derive.
{
private:
public:
	enum MEDIA_DIRECTION	{ MD_UNKOWN, MD_IN, MD_OUT, MD_IN_AND_OUT };
	enum MEDIA_TYPE			{ MT_UNKOWN, MT_RAWGLPIC, MT_IMGSEQUENCE, MT_AVI };

	CMdt_Media_BASE();															// constructor, this one will set media direction and type to unknown
	CMdt_Media_BASE(MEDIA_TYPE eMediaType, MEDIA_DIRECTION eMediaDirection);	// constructor, will init type and direction

	MEDIA_DIRECTION	GetMediaDirection();
	MEDIA_TYPE		GetMediaType();

	unsigned int				GetLastUnhandledError(); // calling this will return the last unhandled error (or 0 / no error if there was none) and will set it to 0 (no error) again, so only if a new error occurs it will be different from 0
	MDT_MEDIA_ERROR_HANDLER_FN	pOnErrorHandler; // you can use this to hook an error handler (in this case the class won't collect Errors anymore)
private:
	MEDIA_DIRECTION	_media_direction;
	MEDIA_TYPE		_media_type;

	unsigned int	_last_unhandled_error;
	bool			_has_unhandled_error;

	void			_CMdt_Media_BASE(); // this is to share some stuff between different constructors

	void			SetMediaDirection (MEDIA_DIRECTION eMediaDirection);
	void			SetMediaType(MEDIA_TYPE eMediaType);

	MDT_MEDIA_ERROR_ACTION		_OnErrorHandler(unsigned int iErrorCode);
};

////////////////////////////////////////////////////////////////////////////////
class CMdt_Media_RAWGLPIC : CMdt_Media_BASE
////////////////////////////////////////////////////////////////////////////////
//
// This is a very simple class in order to organize a buffer for GL data.
// To use it prepare it using the constructor.
{
public:
	CMdt_Media_RAWGLPIC(int iWidth, int iHeight, GLenum eGLDataFormat, GLenum eGLDataType, unsigned char* outPointer);
	// Constructor
	// Width     : Width of the buffer in Pixels.
	// Height    : Height of the buffer in Pixels.
	// GLDataType: The data type you want to use, this is needed because it determines the size and interpretation of a bufferpixel.
	// outPointer: returns a pointer if the buffer could be created successfully otherwise NULL (calling functions of the class in this case should be avoided)

	~CMdt_Media_RAWGLPIC();	// destructor

	bool			IsAlive();		// u can use this to check if the class is alive and everything still okay (usually is not alive when constructor fails) :)

	// The following functions may return random data if the object is not Alive (happens when the Constructor failed)

	int				GetWidth();		// returns the width in pixels
	int				GetHeight();	// teturns the height in pixels

	unsigned int	GetSize();		// returns the total size in bytes of the buffer structure
	unsigned char*	GetPointer();	// returns the pointer to the buffer structure

	unsigned short	GetPixelSize();		// returns the size of the data structure in bytes for one pixel
	unsigned char	GetNumComponents();	// returns the number of components per pixel
	unsigned char	GetSizeComponent();	// returns the size of one component in bytes


	GLenum			GetGLFormat();	// returns the GLformat
	GLenum			GetGLType();	// returns the GLtype
private:
	int				_iWidth;	// width in pixels
	int				_iHeight;	// height in pixels

	unsigned int	_uiSize;			// total structure size in bytes
	unsigned short	_uiPixelSize;		// size of datatype for one pixel in bytes
	unsigned char	_ucSizeComponent;	// size of one component of a pixel in bytes
	unsigned char	_ucNumComponents;	// number of components a pixel has, also see OGL refrence for glReadPixels
	bool			_bComponentIsSigned;	//

	unsigned char*	_pBuffer;	// pointer to the buffer in memory

	GLenum			_eGLformat; // stores the GLformat
	GLenum			_eGLtype;	// stores the GLtype

	bool			_CalcSizeComponent (GLenum eGLtype, unsigned char* outSizeComponent, bool* outIsSigned);	// returns the size of one component in bytes if result is true, false if unknown, also used by constructor
	bool			_CalcNumComponents (GLenum eGLformat, unsigned char* outNumComponents);	// returns false if unknown otherwise true and the number of components per pixel

};


#endif