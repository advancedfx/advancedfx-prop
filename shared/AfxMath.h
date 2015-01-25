#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-11-02 dominik.matrixstorm.com
//
// First changes:
// 2014-11-02 dominik.matrixstorm.com

#include <map>

namespace Afx {
namespace Math {

// QEulerAngles ////////////////////////////////////////////////////////////////

/// <summary>
/// Quake Euler angles in degrees, applied in order of pitch(Y/left), yaw(Z/up), roll(X/forward), right hand grip rule.
/// </summary>
struct QEulerAngles
{
	double Pitch;
	double Yaw;
	double Roll;

	QEulerAngles(double pitch, double yaw, double roll);
};

// QREulerAngles ///////////////////////////////////////////////////////////////

/// <summary>
/// Quake Euler angles in radians, applied in order of pitch(Y/left), yaw(Z/up), roll(X/forward), right hand grip rule.
/// </summary>
struct QREulerAngles
{
	static QREulerAngles FromQEulerAngles(QEulerAngles a);

	double Pitch;
	double Yaw;
	double Roll;

	QREulerAngles(double pitch, double yaw, double roll);

	QEulerAngles ToQEulerAngles(void);
};

// Quaternion //////////////////////////////////////////////////////////////////

struct Quaternion;

Quaternion operator +(Quaternion a, Quaternion b);

Quaternion operator *(double a, Quaternion b);

Quaternion operator *(Quaternion a, Quaternion b);

double DotProduct(Quaternion a, Quaternion b);

struct Quaternion
{
    static Quaternion FromQREulerAngles(QREulerAngles a);

	double W;
	double X;
	double Y;
	double Z;

	Quaternion();
    Quaternion(double w, double x, double y, double z);

    double Norm();

	/// <remarks>Don't use this function for security critical things, it may still have some bugs.</remarks>
    QREulerAngles ToQREulerAngles();
};

// CubicObjectSpline ///////////////////////////////////////////////////////////

struct Vec3
{
	double X;
	double Y;
	double Z;
};

struct COSValue
{
	Vec3 T;
	Quaternion R;
};

typedef double TripArray[3];
typedef double QuatArray[4];
typedef std::map<double,COSValue> COSPoints;

// TODO: make thread safe.
/// <remarks>Currently NOT threadsafe (TODO), because of use of temporary static global variables (slew3_*).</remarks>
class CubicObjectSpline
{
public:
	CubicObjectSpline();
	~CubicObjectSpline();

	void Add(double t, COSValue y);
	void Remove(double t);
	void Clear(void);

	COSPoints::const_iterator GetBegin(void);
	COSPoints::const_iterator GetEnd(void);
	size_t GetSize();

	/// <remarks>Must not be called if GetSize is less than 1!</remarks>
	double GetLowerBound();

	/// <remarks>Must not be called if GetSize is less than 1!</remarks>
	double GetUpperBound();

	/// <remarks>Must not be called if GetSize is less than 4!</remarks>
	COSValue Eval(double t);

private:
	struct Build_s
	{
		double * T;
		double * X;
		double * X2;
		double * Y;
		double * Y2;
		double * Z;
		double * Z2;
		double (*Q_y)[4];
		double * Q_h;
		double * Q_dtheta;
		double (*Q_e)[3];
		double (*Q_w)[3];

		Build_s() : T(0), X(0), X2(0), Y(0), Y2(0), Z(0), Z2(0), Q_y(0), Q_h(0),
			Q_dtheta(0), Q_e(0), Q_w(0) {}
	} m_Build;
	COSPoints m_Points;
	bool m_Rebuild;

	void Free();
};

} // namespace Afx {
} // namespace Math {
