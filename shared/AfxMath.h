#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-03-22 dominik.matrixstorm.com
//
// First changes:
// 2014-11-02 dominik.matrixstorm.com

#include "RefCounted.h"
#include <map>

namespace Afx {
namespace Math {


#define AFX_MATH_EPS 1.0e-6

/// <summary>Make vectors from angles in degrees in right-hand-grip rule.</summary>
void MakeVectors(
	double rForward, double rLeft, double rUp,
	double (& outForward)[3], double (& outRight)[3], double (& outUp)[3]
);

bool LUdecomposition(const double matrix[4][4], unsigned char (&outP)[4], unsigned char (&outQ)[4], double (& outL)[4][4], double (& outU)[4][4]);

void SolveWithLU(const double L[4][4], const double U[4][4], const unsigned char P[4], const unsigned char Q[4], const double b[4], double (& outX)[4]);

int round(double x);

////////////////////////////////////////////////////////////////////////////////

void spline(double x[], double y[], int n, bool y1Natural, double yp1, bool ynNatural, double ypn, double y2[]);

void splint(double xa[], double ya[], double y2a[], int n, double x, double *y);

void qspline_init(
	int n, int maxit, double tol, double wi[], double wf[],
	double x[], double y[][4],
	double h[], double dtheta[], double e[][3], double w[][3]
);

void qspline_interp(
	int n, double xi, double x[], double y[][4],
	double h[], double dtheta[], double e[][3], double w[][3], 
	double q[4], double omega[3], double alpha[3]
);

double getang(double qi[], double qf[], double e[]);

// Vector3 /////////////////////////////////////////////////////////////////////

struct Vector3;

Vector3 operator * (double value, Vector3 x);

struct Vector3
{
public:
	/// <remarks>Data is uninitalized for this constructor.</remarks>
	Vector3();

	Vector3(double x, double y, double z);
	Vector3(double value[3]);

	Vector3(const Vector3 & v);

	Vector3 operator + (const Vector3 & y) const;
	
	void operator += (const Vector3 & y);

	Vector3 operator - (const Vector3 & y) const;

	void operator -= (const Vector3 & y);

	Vector3 operator * (double value) const;

	void operator *= (double value);

	Vector3 operator / (double value) const;

	void operator /= (double value);

	void ToArray(double (& outValue)[3]) const;

	double Length() const;

	Vector3 Normalize();

public:
	double X;
	double Y;
	double Z;

};

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

/*
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
	double Fov;
	void * pUser;
};

typedef std::map<double,COSValue> COSPoints;

class CubicObjectSpline;

class ICosObjectSplineValueRemoved abstract
{
public:
	virtual void CosObjectSplineValueRemoved(CubicObjectSpline * cos, COSValue & value) abstract = 0;
};
*/

////////////////////////////////////////////////////////////////////////////////

template<class T>
class CInterpolationMap
: public std::map<double, T>
{
};

template<class TMap, class T>
class CInterpolationMapViewIterator
{
public:
	/// <remarks>
	/// Object state will be invalid!
	/// For temporary variable use only (meaning copy constructor will initalize it afterwards).
	/// </remarks>
	CInterpolationMapViewIterator()
	{
	}

	CInterpolationMapViewIterator(typename CInterpolationMap<TMap>::const_iterator & mapIterator, T (* selector)(TMap const & value))
	: m_MapIterator(mapIterator)
	, m_Selector(selector)
	{
	}

	double GetTime()
	{
		return m_MapIterator->first;
	}

	T const GetValue()
	{
		return m_Selector(m_MapIterator->second);
	}

	CInterpolationMapViewIterator<TMap, T> & operator ++ ()
	{
		++m_MapIterator;

		return *this;
	}

	CInterpolationMapViewIterator<TMap, T> & operator -- ()
	{
		--m_MapIterator;

		return *this;
	}

	bool operator == (CInterpolationMapViewIterator<TMap, T> const &it) const
	{
		return m_MapIterator == it.m_MapIterator;
	}

	bool operator != (CInterpolationMapViewIterator<TMap, T> const &it) const
	{
		return m_MapIterator != it.m_MapIterator;
	}

private:
	typename CInterpolationMap<TMap>::const_iterator m_MapIterator;
	T (* m_Selector)(TMap const & value);
};

template<class TMap, class T>
class CInterpolationMapView
{
public:
	CInterpolationMapView(CInterpolationMap<TMap> * map, T (* selector)(TMap const & value))
	: m_Map(map)
	, m_Selector(selector)
	{
	}

	CInterpolationMapViewIterator<TMap, T> GetBegin(void)
	{
		return CInterpolationMapViewIterator<TMap, T>(m_Map->begin(), m_Selector);
	}

	CInterpolationMapViewIterator<TMap, T> GetEnd(void)
	{
		return CInterpolationMapViewIterator<TMap, T>(m_Map->end(), m_Selector);
	}

	size_t GetSize()
	{
		return m_Map->size();
	}

	void GetNearestInterval(double time, CInterpolationMapViewIterator<TMap, T> & outLower, CInterpolationMapViewIterator<TMap, T> & outUpper)
	{
		CInterpolationMapViewIterator<TMap, T> end = CInterpolationMapViewIterator<TMap, T>(m_Map->end(), m_Selector);
		outUpper = CInterpolationMapViewIterator<TMap, T>(m_Map->upper_bound(time), m_Selector);

		if(end == outUpper)
		{
			--outUpper;
		}

		outLower = outUpper;
		--outLower;

		if(end == outLower)
		{
			outLower = outUpper;
		}
	}

private:
	CInterpolationMap<TMap> * m_Map;
	T (* m_Selector)(TMap const & value);
};

template<class T>
class CInterpolation abstract
{
public:
	CInterpolation()
	{
	}

	virtual ~CInterpolation()
	{
	}

	virtual void InterpolationMapChanged(void) = 0;

	virtual bool CanEval(void) = 0;

	/// <remarks>
	/// Must not be called if CanEval() returns false!<br />
	/// </remarks>
	virtual T Eval(double t) = 0;
};

template<class TMap>
class CBoolAndInterpolation
: public CInterpolation<bool>
{
public:
	/// <remarks>No extrapolation (will clamp)!</remarks>
	CBoolAndInterpolation(CInterpolationMapView<TMap, bool> * view)
	: CInterpolation<bool>()
	, m_View(view)
	{
		InterpolationMapChanged();
	}

	virtual void InterpolationMapChanged(void)
	{
		// nothing to do here.
	}

	virtual bool CanEval(void)
	{
		return 2 <= m_View->GetSize();
	}

	/// <remarks>
	/// Must not be called if CanEval() returns false!<br />
	/// </remarks>
	virtual bool Eval(double t)
	{
		CInterpolationMapViewIterator<TMap, bool> itLower;
		CInterpolationMapViewIterator<TMap, bool> itUpper;
		m_View->GetNearestInterval(t, itLower, itUpper);

		double lowerT = itLower.GetTime();
		bool lowerV = itLower.GetValue();

		if(t <= lowerT)
		{
			return lowerV;
		}

		double upperT = itUpper.GetTime();
		bool upperV = itUpper.GetValue();

		if(upperT <= t)
		{
			return upperV;
		}

		return lowerV && upperV;
	}

private:
	CInterpolationMapView<TMap, bool> * m_View;
};

template<class TMap>
class CLinearDoubleInterpolation
: public CInterpolation<double>
{
public:
	/// <remarks>No extrapolation (will clamp)!</remarks>
	CLinearDoubleInterpolation(CInterpolationMapView<TMap, double> * view)
	: CInterpolation<double>()
	, m_View(view)
	{
		InterpolationMapChanged();
	}

	virtual void InterpolationMapChanged(void)
	{
		// nothing to do here.
	}

	virtual bool CanEval(void)
	{
		return 2 <= m_View->GetSize();
	}

	/// <remarks>
	/// Must not be called if CanEval() returns false!<br />
	/// </remarks>
	virtual double Eval(double t)
	{
		CInterpolationMapViewIterator<TMap, double> itLower;
		CInterpolationMapViewIterator<TMap, double> itUpper;
		m_View->GetNearestInterval(t, itLower, itUpper);

		double lowerT = itLower.GetTime();
		double lowerV = itLower.GetValue();

		if(t <= lowerT)
		{
			return lowerV;
		}

		double upperT = itUpper.GetTime();
		double upperV = itUpper.GetValue();

		if(upperT <= t)
		{
			return upperV;
		}

		double deltaT = upperT -lowerT;

		return (1-(t-lowerT)/deltaT)*lowerV +((t-lowerT)/deltaT)*upperV;
	}

private:
	CInterpolationMapView<TMap, double> * m_View;
};

template<class TMap>
class CCubicDoubleInterpolation
: public CInterpolation<double>
{
public:
	CCubicDoubleInterpolation(CInterpolationMapView<TMap, double> * view)
	: CInterpolation<double>()
	, m_View(view)
	{
		InterpolationMapChanged();
	}

	virtual ~CCubicDoubleInterpolation()
	{
		Free();
	}

	virtual void InterpolationMapChanged(void)
	{
		m_Rebuild = true;
	}

	virtual bool CanEval(void)
	{
		return 4 <= m_View->GetSize();
	}

	/// <remarks>
	/// Must not be called if CanEval() returns false!<br />
	/// </remarks>
	virtual double Eval(double t)
	{
		int n = m_View->GetSize();

		if(n < 4) throw "CCubicDoubleInterpolation::Eval only allowed with at least 4 points.";

		if(m_Rebuild)
		{
			m_Rebuild = false;

			Free();

			m_Build.T = new double[n];
			m_Build.X = new double[n];
			m_Build.X2 = new double[n];

			{
				int i = 0;
				CInterpolationMapViewIterator<TMap, double> itEnd = m_View->GetEnd();
				for(CInterpolationMapViewIterator<TMap, double> it = m_View->GetBegin(); it != itEnd; ++it)
				{
					m_Build.T[i] = it.GetTime();
					m_Build.X[i] = it.GetValue();
					++i;
				}
			}

			spline(m_Build.T , m_Build.X, n, false, 0.0, false, 0.0, m_Build.X2);
		}

		double result;

		splint(m_Build.T, m_Build.X, m_Build.X2, n, t, &result);

		return result;
	}

private:
	CInterpolationMapView<TMap, double> * m_View;

	struct Build_s
	{
		double * T;
		double * X;
		double * X2;

		Build_s()
		: T(0)
		, X(0)
		, X2(0)
		{
		}
	} m_Build;

	bool m_Rebuild;

	void Free()
	{
		delete m_Build.X2;
		m_Build.X2 = 0;
		delete m_Build.X;
		m_Build.X = 0;
		delete m_Build.T;
		m_Build.T = 0;
	}
};

template<class TMap>
class CSLinearQuaternionInterpolation
: public CInterpolation<Quaternion>
{
public:
	/// <remarks>No extrapolation (will clamp)!</remarks>
	CSLinearQuaternionInterpolation(CInterpolationMapView<TMap, Quaternion> * view)
	: CInterpolation<Quaternion>()
	, m_View(view)
	{
		InterpolationMapChanged();
	}

	virtual void InterpolationMapChanged(void)
	{
		// nothing to do here.
	}

	virtual bool CanEval(void)
	{
		return 2 <= m_View->GetSize();
	}

	/// <remarks>
	/// Must not be called if CanEval() returns false!<br />
	/// </remarks>
	virtual Quaternion Eval(double t)
	{
		CInterpolationMapViewIterator<TMap, Quaternion> itLower;
		CInterpolationMapViewIterator<TMap, Quaternion> itUpper;
		m_View->GetNearestInterval(t, itLower, itUpper);

		double lowerT = itLower.GetTime();
		Quaternion lowerV = itLower.GetValue();

		if(t <= lowerT)
		{
			return lowerV;
		}

		double upperT = itUpper.GetTime();
		Quaternion upperV = itUpper.GetValue();

		if(upperT <= t)
		{
			return upperV;
		}

		// Make sure we will travel the short way:
		double dotProduct = DotProduct(upperV,lowerV);
		if(dotProduct<0.0)
		{
			upperV = -1.0 * upperV;
		}

		double deltaT = upperT -lowerT;
		
		double qi[4] = { lowerV.X, lowerV.Y, lowerV.Z, lowerV.W };
		double qf[4] = { upperV.X, upperV.Y, upperV.Z, upperV.W };

		double e[3];
		double dtheta = getang(qi,qf,e);

		t = (t-lowerT)/deltaT;

		double tDthetaDiv2 = t * dtheta / 2;
		
		double cosTDheta = cos(tDthetaDiv2);
		double sinTDHeta = sin(tDthetaDiv2);

		Quaternion dq_pow_t = Quaternion(cosTDheta,e[0]*sinTDHeta,e[1]*sinTDHeta,e[2]*sinTDHeta);

		return lowerV * dq_pow_t;
	}

private:
	CInterpolationMapView<TMap, Quaternion> * m_View;
};

template<class TMap>
class CSCubicQuaternionInterpolation
: public CInterpolation<Quaternion>
{
public:
	// TODO: make thread safe.
	/// <remarks>Currently NOT threadsafe (TODO), because of use of temporary static global variables (slew3_*).</remarks>
	CSCubicQuaternionInterpolation(CInterpolationMapView<TMap, Quaternion> * view)
	: CInterpolation<Quaternion>()
	, m_View(view)
	{
		InterpolationMapChanged();
	}

	virtual ~CSCubicQuaternionInterpolation()
	{
		Free();
	}

	virtual void InterpolationMapChanged(void)
	{
		m_Rebuild = true;
	}

	virtual bool CanEval(void)
	{
		return 4 <= m_View->GetSize();
	}

	/// <remarks>
	/// Must not be called if CanEval() returns false!<br />
	/// </remarks>
	virtual Quaternion Eval(double t)
	{
		int n = m_View->GetSize();

		if(n < 4) throw "CSCubicQuaternionInterpolation::Eval only allowed with at least 4 points.";

		if(m_Rebuild)
		{
			m_Rebuild = false;

			Free();

			m_Build.T = new double[n];
			m_Build.Q_y = new double[n][4];
			m_Build.Q_h = new double[n-1];
			m_Build.Q_dtheta = new double[n-1];
			m_Build.Q_e = new double[n-1][3];
			m_Build.Q_w = new double[n][3];

			{
				Quaternion QLast;

				int i = 0;
				CInterpolationMapViewIterator<TMap, Quaternion> itEnd = m_View->GetEnd();
				for(CInterpolationMapViewIterator<TMap, Quaternion> it = m_View->GetBegin(); it != itEnd; ++it)
				{
					m_Build.T[i] = it.GetTime();

					Quaternion Q = it.GetValue();
				
					// Make sure we will travel the short way:
					if(0<i)
					{
						// hasLast.
						double dotProduct = DotProduct(Q,QLast);
						if(dotProduct<0.0)
						{
							Q = -1.0 * Q;
						}
					}

					m_Build.Q_y[i][0] = Q.X;
					m_Build.Q_y[i][1] = Q.Y;
					m_Build.Q_y[i][2] = Q.Z;
					m_Build.Q_y[i][3] = Q.W;

					QLast = Q;
					i++;
				}
			}

			double wi[3] = {0.0,0.0,0.0};
			double wf[3] = {0.0,0.0,0.0};
			qspline_init(n, 2, AFX_MATH_EPS, wi, wf, m_Build.T, m_Build.Q_y, m_Build.Q_h, m_Build.Q_dtheta, m_Build.Q_e, m_Build.Q_w);
		}

		double Q[4],dum1[4],dum2[4];

		qspline_interp(n, t, m_Build.T, m_Build.Q_y, m_Build.Q_h, m_Build.Q_dtheta, m_Build.Q_e, m_Build.Q_w, Q, dum1, dum2);

		return Quaternion(Q[3], Q[0], Q[1], Q[2]);
	}

private:
	CInterpolationMapView<TMap, Quaternion> * m_View;

	struct Build_s
	{
		double * T;
		double (*Q_y)[4];
		double * Q_h;
		double * Q_dtheta;
		double (*Q_e)[3];
		double (*Q_w)[3];

		Build_s()
		: T(0)
		, Q_y(0)
		, Q_h(0)
		, Q_dtheta(0)
		, Q_e(0)
		, Q_w(0)
		{
		}
	} m_Build;

	bool m_Rebuild;

	void Free()
	{
		delete [] m_Build.Q_w;
		m_Build.Q_w = 0;
		delete [] m_Build.Q_e;
		m_Build.Q_e = 0;
		delete m_Build.Q_dtheta;
		m_Build.Q_dtheta = 0;
		delete m_Build.Q_h;
		m_Build.Q_h = 0;
		delete [] m_Build.Q_y;
		m_Build.Q_y = 0;
		delete m_Build.T;
		m_Build.T = 0;
	}
};

/*

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

	/// <remarks>
	/// Must not be called if GetSize is less than 4!<br />
	/// The returned pUser member is meaningless and should not be used.
	/// </remarks>
	COSValue Eval(double t);

	bool SetUser(double t, void * value);

	void OnValueRemoved_set(ICosObjectSplineValueRemoved * value);

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
		double * Fov;
		double * Fov2;

		Build_s() : T(0), X(0), X2(0), Y(0), Y2(0), Z(0), Z2(0), Q_y(0), Q_h(0),
			Q_dtheta(0), Q_e(0), Q_w(0), Fov(0), Fov2(0) {}
	} m_Build;
	ICosObjectSplineValueRemoved * m_OnValueRemoved;
	COSPoints m_Points;
	bool m_Rebuild;

	void Free();
	void ValueRemoved(COSValue & value);
};

*/

} // namespace Afx {
} // namespace Math {
