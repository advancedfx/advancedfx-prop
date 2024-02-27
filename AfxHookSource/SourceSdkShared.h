#pragma once

// Based on Source engine SDK(s):
// Copyright (c) 1996-2005, Valve Corporation, All rights reserved

namespace SOURCESDK {

#ifndef SOURCESDK_NULL
#define SOURCESDK_NULL 0
#endif

#define SOURCESDK_abstract_class class __declspec(novtable)

#define SOURCESDK_FORCEINLINE __forceinline
#define SOURCESDK_FORCEINLINE_CVAR SOURCESDK_FORCEINLINE

#define SOURCESDK_Assert(condition)
#define SOURCESDK_Warning(message)
#define SOURCESDK_CHECK_VALID( _v)

#define SOURCESDK_Q_memset memset
#define SOURCESDK_Q_memcpy strncpy

#define SOURCESDK_DECLARE_POINTER_HANDLE(name) struct name##__ { int unused; }; typedef struct name##__ *name
#define SOURCESDK_FORWARD_DECLARE_HANDLE(name) typedef struct name##__ *name

#define SOURCESDK_ALIGN16 __declspec( align( 16 ) )
#define SOURCESDK_ALIGN16_POST

typedef float vec_t;
typedef signed char int8;
typedef signed __int16 int16;
typedef signed __int32 int32;
typedef signed __int64 int64;
typedef unsigned char uint8;
typedef unsigned __int32 uint32;
typedef unsigned __int16 uint16;
typedef unsigned __int64 uint64;

// for when we don't care about how many bits we use
typedef unsigned int	uint;

typedef void * FileHandle_t;

typedef unsigned char byte;

typedef int	string_t;

inline vec_t BitsToFloat(uint32 i)
{
	union Convertor_t
	{
		vec_t f;
		unsigned long ul;
	}tmp;
	tmp.ul = i;
	return tmp.f;
}

#define SOURCESDK_FLOAT32_NAN_BITS     (uint32)0x7FC00000	// not a number!
#define SOURCESDK_FLOAT32_NAN          BitsToFloat( SOURCESDK_FLOAT32_NAN_BITS )

#define SOURCESDK_VEC_T_NAN SOURCESDK_FLOAT32_NAN

// Vector //////////////////////////////////////////////////////////////////////

class Vector				
{
public:
	Vector() : x(SOURCESDK_VEC_T_NAN), y(SOURCESDK_VEC_T_NAN), z(SOURCESDK_VEC_T_NAN) {}

	Vector(vec_t X, vec_t Y, vec_t Z) : x(X), y(Y), z(Z) {}

	Vector(const Vector& other)
	{
		x = other.x; y = other.y; z = other.z;
	}

	// Members
	vec_t x, y, z;

	// Initialization
	void Init(vec_t ix = 0.0f, vec_t iy = 0.0f, vec_t iz = 0.0f);
	// TODO (Ilya): Should there be an init that takes a single float for consistency?

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	// Base address...
	vec_t* Base();
	vec_t const* Base() const;

	// Get the vector's magnitude squared.
	SOURCESDK_FORCEINLINE vec_t LengthSqr(void) const
	{
		SOURCESDK_CHECK_VALID(*this);
		return (x*x + y * y + z * z);
	}

	SOURCESDK_FORCEINLINE Vector&	operator*=(float s);

	// shortened.

	Vector operator+(const Vector other) const {
		return Vector(this->x + other.x, this->y + other.y, this->z + other.z);
	}

	Vector operator-(const Vector other) const {
		return Vector(this->x - other.x, this->y - other.y, this->z - other.z);
	}
};

inline vec_t& Vector::operator[](int i)
{
	SOURCESDK_Assert((i >= 0) && (i < 3));
	return ((vec_t*)this)[i];
}

inline vec_t Vector::operator[](int i) const
{
	SOURCESDK_Assert((i >= 0) && (i < 3));
	return ((vec_t*)this)[i];
}

inline vec_t* Vector::Base()
{
	return (vec_t*)this;
}

inline vec_t const* Vector::Base() const
{
	return (vec_t const*)this;
}


SOURCESDK_FORCEINLINE  Vector& Vector::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;
	SOURCESDK_CHECK_VALID(*this);
	return *this;
}


// QAngle //////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// Degree Euler QAngle pitch, yaw, roll
//-----------------------------------------------------------------------------
class QAngleByValue;

class QAngle
{
public:
	// Members
	vec_t x, y, z;

	// Construction/destruction
	QAngle(void);
	QAngle(vec_t X, vec_t Y, vec_t Z);
	//	QAngle(RadianEuler const &angles);	// evil auto type promotion!!!

	// Allow pass-by-value
	operator QAngleByValue &() { return *((QAngleByValue *)(this)); }
	operator const QAngleByValue &() const { return *((const QAngleByValue *)(this)); }

	// Initialization
	void Init(vec_t ix = 0.0f, vec_t iy = 0.0f, vec_t iz = 0.0f);
	void Random(vec_t minVal, vec_t maxVal);

	// Got any nasty NAN's?
	bool IsValid() const;
	void Invalidate();

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	// Base address...
	vec_t* Base();
	vec_t const* Base() const;

	// equality
	bool operator==(const QAngle& v) const;
	bool operator!=(const QAngle& v) const;

	// arithmetic operations
	QAngle&	operator+=(const QAngle &v);
	QAngle&	operator-=(const QAngle &v);
	QAngle&	operator*=(float s);
	QAngle&	operator/=(float s);

	// Get the vector's magnitude.
	vec_t	Length() const;
	vec_t	LengthSqr() const;

	// negate the QAngle components
	//void	Negate(); 

	// No assignment operators either...
	QAngle& operator=(const QAngle& src);

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// copy constructors

	// arithmetic operations
	QAngle	operator-(void) const;

	QAngle	operator+(const QAngle& v) const;
	QAngle	operator-(const QAngle& v) const;
	QAngle	operator*(float fl) const;
	QAngle	operator/(float fl) const;
#else

private:
	// No copy constructors allowed if we're in optimal mode
	QAngle(const QAngle& vOther);

#endif
};

//-----------------------------------------------------------------------------
// Allows us to specifically pass the vector by value when we need to
//-----------------------------------------------------------------------------
class QAngleByValue : public QAngle
{
public:
	// Construction/destruction:
	QAngleByValue(void) : QAngle() {}
	QAngleByValue(vec_t X, vec_t Y, vec_t Z) : QAngle(X, Y, Z) {}
	QAngleByValue(const QAngleByValue& vOther) { *this = vOther; }
};


inline void VectorAdd(const QAngle& a, const QAngle& b, QAngle& result)
{
	SOURCESDK_CHECK_VALID(a);
	SOURCESDK_CHECK_VALID(b);
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	result.z = a.z + b.z;
}

inline void VectorMA(const QAngle &start, float scale, const QAngle &direction, QAngle &dest)
{
	SOURCESDK_CHECK_VALID(start);
	SOURCESDK_CHECK_VALID(direction);
	dest.x = start.x + scale * direction.x;
	dest.y = start.y + scale * direction.y;
	dest.z = start.z + scale * direction.z;
}


//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------
inline QAngle::QAngle(void)
{
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
	// Initialize to NAN to catch errors
	x = y = z = VEC_T_NAN;
#endif
#endif
}

inline QAngle::QAngle(vec_t X, vec_t Y, vec_t Z)
{
	x = X; y = Y; z = Z;
	SOURCESDK_CHECK_VALID(*this);
}


//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------
inline void QAngle::Init(vec_t ix, vec_t iy, vec_t iz)
{
	x = ix; y = iy; z = iz;
	SOURCESDK_CHECK_VALID(*this);
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------
inline QAngle& QAngle::operator=(const QAngle &vOther)
{
	SOURCESDK_CHECK_VALID(vOther);
	x = vOther.x; y = vOther.y; z = vOther.z;
	return *this;
}


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline vec_t& QAngle::operator[](int i)
{
	SOURCESDK_Assert((i >= 0) && (i < 3));
	return ((vec_t*)this)[i];
}

inline vec_t QAngle::operator[](int i) const
{
	SOURCESDK_Assert((i >= 0) && (i < 3));
	return ((vec_t*)this)[i];
}


//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline vec_t* QAngle::Base()
{
	return (vec_t*)this;
}

inline vec_t const* QAngle::Base() const
{
	return (vec_t const*)this;
}


//-----------------------------------------------------------------------------
// Invalidate
//-----------------------------------------------------------------------------

inline void QAngle::Invalidate()
{
	//#ifdef _DEBUG
	//#ifdef VECTOR_PARANOIA
	x = y = z = SOURCESDK_VEC_T_NAN;
	//#endif
	//#endif
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------
inline bool QAngle::operator==(const QAngle& src) const
{
	SOURCESDK_CHECK_VALID(src);
	SOURCESDK_CHECK_VALID(*this);
	return (src.x == x) && (src.y == y) && (src.z == z);
}

inline bool QAngle::operator!=(const QAngle& src) const
{
	SOURCESDK_CHECK_VALID(src);
	SOURCESDK_CHECK_VALID(*this);
	return (src.x != x) || (src.y != y) || (src.z != z);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------
inline void VectorCopy(const QAngle& src, QAngle& dst)
{
	SOURCESDK_CHECK_VALID(src);
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}


//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------
inline QAngle& QAngle::operator+=(const QAngle& v)
{
	SOURCESDK_CHECK_VALID(*this);
	SOURCESDK_CHECK_VALID(v);
	x += v.x; y += v.y; z += v.z;
	return *this;
}

inline QAngle& QAngle::operator-=(const QAngle& v)
{
	SOURCESDK_CHECK_VALID(*this);
	SOURCESDK_CHECK_VALID(v);
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}

inline QAngle& QAngle::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;
	SOURCESDK_CHECK_VALID(*this);
	return *this;
}

inline QAngle& QAngle::operator/=(float fl)
{
	SOURCESDK_Assert(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;
	SOURCESDK_CHECK_VALID(*this);
	return *this;
}


inline vec_t QAngle::LengthSqr() const
{
	SOURCESDK_CHECK_VALID(*this);
	return x * x + y * y + z * z;
}


//-----------------------------------------------------------------------------
// arithmetic operations (SLOW!!)
//-----------------------------------------------------------------------------
#ifndef SOURCESDK_VECTOR_NO_SLOW_OPERATIONS

inline QAngle QAngle::operator-(void) const
{
	QAngle ret(-x, -y, -z);
	return ret;
}

inline QAngle QAngle::operator+(const QAngle& v) const
{
	QAngle res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

inline QAngle QAngle::operator-(const QAngle& v) const
{
	QAngle res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}

inline QAngle QAngle::operator*(float fl) const
{
	QAngle res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;
}

inline QAngle QAngle::operator/(float fl) const
{
	QAngle res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;
}

inline QAngle operator*(float fl, const QAngle& v)
{
	QAngle ret(v * fl);
	return ret;
}

#endif // VECTOR_NO_SLOW_OPERATIONS


struct matrix3x4_t
{
	matrix3x4_t() {}

	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	matrix3x4_t(const matrix3x4_t& x) {
		m_flMatVal[0][0] = x.m_flMatVal[0][0];	m_flMatVal[0][1] = x.m_flMatVal[0][1]; m_flMatVal[0][2] = x.m_flMatVal[0][2]; m_flMatVal[0][3] = x.m_flMatVal[0][3];
		m_flMatVal[1][0] = x.m_flMatVal[1][0];	m_flMatVal[1][1] = x.m_flMatVal[1][1]; m_flMatVal[1][2] = x.m_flMatVal[1][2]; m_flMatVal[1][3] = x.m_flMatVal[1][3];
		m_flMatVal[2][0] = x.m_flMatVal[2][0];	m_flMatVal[2][1] = x.m_flMatVal[2][1]; m_flMatVal[2][2] = x.m_flMatVal[2][2]; m_flMatVal[2][3] = x.m_flMatVal[2][3];
	}	

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin)
	{
		m_flMatVal[0][0] = xAxis.x; m_flMatVal[0][1] = yAxis.x; m_flMatVal[0][2] = zAxis.x; m_flMatVal[0][3] = vecOrigin.x;
		m_flMatVal[1][0] = xAxis.y; m_flMatVal[1][1] = yAxis.y; m_flMatVal[1][2] = zAxis.y; m_flMatVal[1][3] = vecOrigin.y;
		m_flMatVal[2][0] = xAxis.z; m_flMatVal[2][1] = yAxis.z; m_flMatVal[2][2] = zAxis.z; m_flMatVal[2][3] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	matrix3x4_t(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector &vecOrigin)
	{
		Init(xAxis, yAxis, zAxis, vecOrigin);
	}

	inline void SetOrigin(Vector const & p)
	{
		m_flMatVal[0][3] = p.x;
		m_flMatVal[1][3] = p.y;
		m_flMatVal[2][3] = p.z;
	}

	inline void Invalidate(void)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m_flMatVal[i][j] = SOURCESDK_VEC_T_NAN;
			}
		}
	}

	float *operator[](int i) { SOURCESDK_Assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
	const float *operator[](int i) const { SOURCESDK_Assert((i >= 0) && (i < 3)); return m_flMatVal[i]; }
	float *Base() { return &m_flMatVal[0][0]; }
	const float *Base() const { return &m_flMatVal[0][0]; }

	matrix3x4_t& operator=(const matrix3x4_t& x) {
		m_flMatVal[0][0] = x.m_flMatVal[0][0];	m_flMatVal[0][1] = x.m_flMatVal[0][1]; m_flMatVal[0][2] = x.m_flMatVal[0][2]; m_flMatVal[0][3] = x.m_flMatVal[0][3];
		m_flMatVal[1][0] = x.m_flMatVal[1][0];	m_flMatVal[1][1] = x.m_flMatVal[1][1]; m_flMatVal[1][2] = x.m_flMatVal[1][2]; m_flMatVal[1][3] = x.m_flMatVal[1][3];
		m_flMatVal[2][0] = x.m_flMatVal[2][0];	m_flMatVal[2][1] = x.m_flMatVal[2][1]; m_flMatVal[2][2] = x.m_flMatVal[2][2]; m_flMatVal[2][3] = x.m_flMatVal[2][3];

		return *this;
	}

	float m_flMatVal[3][4];
};

class SOURCESDK_ALIGN16 matrix3x4a_t : public matrix3x4_t {

} SOURCESDK_ALIGN16_POST;

//-----------------------------------------------------------------------------
// Quaternion
//-----------------------------------------------------------------------------

class RadianEuler;

class Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a vec_t[4]
public:
	inline Quaternion(void) {

		// Initialize to NAN to catch errors
#ifdef _DEBUG
#ifdef SOURCESDK_VECTOR_PARANOIA
		x = y = z = w = VEC_T_NAN;
#endif
#endif
	}
	inline Quaternion(vec_t ix, vec_t iy, vec_t iz, vec_t iw) : x(ix), y(iy), z(iz), w(iw) { }
	inline Quaternion(RadianEuler const &angle);	// evil auto type promotion!!!

	inline void Init(vec_t ix = 0.0f, vec_t iy = 0.0f, vec_t iz = 0.0f, vec_t iw = 0.0f) { x = ix; y = iy; z = iz; w = iw; }

	bool IsValid() const;
	void Invalidate();

	bool operator==(const Quaternion &src) const;
	bool operator!=(const Quaternion &src) const;

	vec_t* Base() { return (vec_t*)this; }
	const vec_t* Base() const { return (vec_t*)this; }

	// array access...
	vec_t operator[](int i) const;
	vec_t& operator[](int i);

	vec_t x, y, z, w;
};


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
inline vec_t& Quaternion::operator[](int i)
{
	SOURCESDK_Assert((i >= 0) && (i < 4));
	return ((vec_t*)this)[i];
}

inline vec_t Quaternion::operator[](int i) const
{
	SOURCESDK_Assert((i >= 0) && (i < 4));
	return ((vec_t*)this)[i];
}


//-----------------------------------------------------------------------------
// Equality test
//-----------------------------------------------------------------------------
inline bool Quaternion::operator==(const Quaternion &src) const
{
	return (x == src.x) && (y == src.y) && (z == src.z) && (w == src.w);
}

inline bool Quaternion::operator!=(const Quaternion &src) const
{
	return !operator==(src);
}


// VMatrix /////////////////////////////////////////////////////////////////////

class MdtMatrix;
typedef MdtMatrix VMatrix;

class MdtMatrix
{
public:
	MdtMatrix();
	MdtMatrix(const MdtMatrix & mdtMatrix);

	// The matrix.
	vec_t		m[4][4];
};


typedef struct color32_s
{
	bool operator!=(const struct color32_s &other) const;

	byte r, g, b, a;

	// assign and copy by using the whole register rather
	// than byte-by-byte copy. (No, the compiler is not
	// smart enough to do this for you. /FAcs if you 
	// don't believe me.)
	inline unsigned *asInt(void) { return reinterpret_cast<unsigned*>(this); }
	inline const unsigned *asInt(void) const { return reinterpret_cast<const unsigned*>(this); }
	// This thing is in a union elsewhere, and union members can't have assignment
	// operators, so you have to explicitly assign using this, or be slow. SUCK.
	inline void Copy(const color32_s &rhs)
	{
		*asInt() = *rhs.asInt();
	}

} color32;

inline bool color32::operator!=(const color32 &other) const
{
	return r != other.r || g != other.g || b != other.b || a != other.a;
}

//-----------------------------------------------------------------------------
// Purpose: Basic handler for an rgb set of colors
//			This class is fully inline
//-----------------------------------------------------------------------------
class Color
{
public:
	// constructors
	Color()
	{
		*((int *)this) = 0;
	}
	Color(int _r, int _g, int _b)
	{
		SetColor(_r, _g, _b, 0);
	}
	Color(int _r, int _g, int _b, int _a)
	{
		SetColor(_r, _g, _b, _a);
	}

	// set the color
	// r - red component (0-255)
	// g - green component (0-255)
	// b - blue component (0-255)
	// a - alpha component, controls transparency (0 - transparent, 255 - opaque);
	void SetColor(int _r, int _g, int _b, int _a = 0)
	{
		_color[0] = (unsigned char)_r;
		_color[1] = (unsigned char)_g;
		_color[2] = (unsigned char)_b;
		_color[3] = (unsigned char)_a;
	}

	void GetColor(int &_r, int &_g, int &_b, int &_a) const
	{
		_r = _color[0];
		_g = _color[1];
		_b = _color[2];
		_a = _color[3];
	}

	void SetRawColor(int color32)
	{
		*((int *)this) = color32;
	}

	int GetRawColor() const
	{
		return *((int *)this);
	}

	inline int r() const { return _color[0]; }
	inline int g() const { return _color[1]; }
	inline int b() const { return _color[2]; }
	inline int a() const { return _color[3]; }

	unsigned char &operator[](int index)
	{
		return _color[index];
	}

	const unsigned char &operator[](int index) const
	{
		return _color[index];
	}

	bool operator == (const Color &rhs) const
	{
		return (*((int *)this) == *((int *)&rhs));
	}

	bool operator != (const Color &rhs) const
	{
		return !(operator==(rhs));
	}

	Color &operator=(const Color &rhs)
	{
		SetRawColor(rhs.GetRawColor());
		return *this;
	}

	Color &operator=(const color32 &rhs)
	{
		_color[0] = rhs.r;
		_color[1] = rhs.g;
		_color[2] = rhs.b;
		_color[3] = rhs.a;
		return *this;
	}

	color32 ToColor32() const
	{
		color32 newColor;
		newColor.r = _color[0];
		newColor.g = _color[1];
		newColor.b = _color[2];
		newColor.a = _color[3];
		return newColor;
	}

private:
	unsigned char _color[4];
};


// IBaseInterface //////////////////////////////////////////////////////////////

class IBaseInterface
{
public:
	virtual	~IBaseInterface() {}
};


/////////////////////////////////////////////////////////////////////////////////


namespace TF2 {

//-----------------------------------------------------------------------------
// Purpose: The engine reports to the client DLL what stage it's entering so the DLL can latch events
//  and make sure that certain operations only happen during the right stages.
// The value for each stage goes up as you move through the frame so you can check ranges of values
//  and if new stages get added in-between, the range is still valid.
//-----------------------------------------------------------------------------
enum ClientFrameStage_t
{
	FRAME_UNDEFINED=-1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
		// Data has been received and we're going to start calling PostDataUpdate
		FRAME_NET_UPDATE_POSTDATAUPDATE_START,
		// Data has been received and we've called PostDataUpdate on all data recipients
		FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,		

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};

} // namespace TF2 {

namespace CSS {

//-----------------------------------------------------------------------------
// Purpose: The engine reports to the client DLL what stage it's entering so the DLL can latch events
//  and make sure that certain operations only happen during the right stages.
// The value for each stage goes up as you move through the frame so you can check ranges of values
//  and if new stages get added in-between, the range is still valid.
//-----------------------------------------------------------------------------
enum ClientFrameStage_t
{
	FRAME_UNDEFINED=-1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
		// Data has been received and we're going to start calling PostDataUpdate
		FRAME_NET_UPDATE_POSTDATAUPDATE_START,
		// Data has been received and we've called PostDataUpdate on all data recipients
		FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,		

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};

} // namespace CSS {

namespace CSSV34 {

//-----------------------------------------------------------------------------
// Purpose: The engine reports to the client DLL what stage it's entering so the DLL can latch events
//  and make sure that certain operations only happen during the right stages.
// The value for each stage goes up as you move through the frame so you can check ranges of values
//  and if new stages get added in-between, the range is still valid.
//-----------------------------------------------------------------------------
enum ClientFrameStage_t
{
	FRAME_UNDEFINED=-1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
		// Data has been received and we're going to start calling PostDataUpdate
		FRAME_NET_UPDATE_POSTDATAUPDATE_START,
		// Data has been received and we've called PostDataUpdate on all data recipients
		FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,		

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};

} // namespace CSSV34 {


namespace CSGO {

template< class T, class I = int >
/// <remarks>Warning, only required elements declared and defined!</remarks>
class CUtlMemory
{
public:
	//
	// We don't need this
};

template< class T, class A = CUtlMemory<T> >
/// <remarks>Warning, only required elements declared and defined!</remarks>
class CUtlVector
{
	typedef A CAllocator;
public:
	typedef T ElemType_t;

protected:
	CAllocator m_Memory;
	int m_Size;

#ifndef _X360
	// For easier access to the elements through the debugger
	// it's in release builds so this can be used in libraries correctly
	T *m_pElements;

#else
#endif
};

} // namespace CSGO {

namespace SWARM {

class CUtlString;

template< class T, class I = int >
/// <remarks>Warning, only required elements declared and defined!</remarks>
class CUtlMemory
{
public:
	//
	// We don't need this
};

template< class T, class A = CUtlMemory<T> >
/// <remarks>Warning, only required elements declared and defined!</remarks>
class CUtlVector
{
	typedef A CAllocator;
public:
	typedef T ElemType_t;

protected:
	CAllocator m_Memory;
	int m_Size;

#ifndef _X360
	// For easier access to the elements through the debugger
	// it's in release builds so this can be used in libraries correctly
	T *m_pElements;

#else
#endif
};

} // namespace SWARM {

namespace L4D2 {

	class CUtlString;

	template< class T, class I = int >
	/// <remarks>Warning, only required elements declared and defined!</remarks>
	class CUtlMemory
	{
	public:
		//
		// We don't need this
	};

	template< class T, class A = CUtlMemory<T> >
	/// <remarks>Warning, only required elements declared and defined!</remarks>
	class CUtlVector
	{
		typedef A CAllocator;
	public:
		typedef T ElemType_t;

	protected:
		CAllocator m_Memory;
		int m_Size;

#ifndef _X360
		// For easier access to the elements through the debugger
		// it's in release builds so this can be used in libraries correctly
		T *m_pElements;

#else
#endif
	};

} // namespace L4D2 {

namespace BM {

	class CUtlString;

	template< class T, class I = int >
	/// <remarks>Warning, only required elements declared and defined!</remarks>
	class CUtlMemory
	{
	public:
		//
		// We don't need this
	};

	template< class T, class A = CUtlMemory<T> >
	/// <remarks>Warning, only required elements declared and defined!</remarks>
	class CUtlVector
	{
		typedef A CAllocator;
	public:
		typedef T ElemType_t;

	protected:
		CAllocator m_Memory;
		int m_Size;

#ifndef _X360
		// For easier access to the elements through the debugger
		// it's in release builds so this can be used in libraries correctly
		T *m_pElements;

#else
#endif
	};

} // namespace BM {


namespace CS2 {

typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

class CGlobalVarsBase;

} // namespace CS2 {


////////////////////////////////////////////////////////////////////////////////

SOURCESDK::vec_t DotProduct(const SOURCESDK::vec_t * v1, const SOURCESDK::vec_t * v2);

void VectorAngles( const SOURCESDK::Vector& forward, SOURCESDK::QAngle &angles );

void VectorTransform(const SOURCESDK::Vector & in1, const SOURCESDK::matrix3x4_t & in2, SOURCESDK::Vector & out);

void MatrixAngles(const SOURCESDK::matrix3x4_t & matrix, SOURCESDK::QAngle & angles);

void AngleMatrix( const SOURCESDK::QAngle &angles,  SOURCESDK::matrix3x4_t& matrix );

void AngleMatrix( const QAngle &angles, const Vector &position, matrix3x4_t& matrix );

// This is actually from Half-Life 1 sort of, but will do haha.
void R_ConcatTransforms (const SOURCESDK::matrix3x4_t & in1, const SOURCESDK::matrix3x4_t & in2, SOURCESDK::matrix3x4_t & out);

} // namespace SOURCESDK {
