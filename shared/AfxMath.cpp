#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-11-02 dominik.matrixstorm.com
//
// First changes:
// 2014-11-02 dominik.matrixstorm.com

#include "AfxMath.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Afx {
namespace Math {


////////////////////////////////////////////////////////////////////////////////

// Copyright (c) by NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING (ISBN 0-521-43108-5)
void spline(double x[], double y[], int n, bool y1Natural, double yp1, bool ynNatural, double ypn, double y2[])
{
    int i, k;
    double p, qn, sig, un;
    double *u = new double[n - 1 - 1 + 1];

    if (y1Natural)
        y2[0] = u[0] = 0.0f;
    else
    {
        y2[0] = -0.5f;
        u[0] = (3.0f / (x[1] - x[0])) * ((y[1] - y[0]) / (x[1] - x[0]) - yp1);
    }

    for (i = 1; i <= n - 2; i++)
    {
        sig = (x[i] - x[i - 1]) / (x[i + 1] - x[i - 1]);
        p = sig * y2[i - 1] + 2.0f;
        y2[i] = (sig - 1.0f) / p;
        u[i] = (y[i + 1] - y[i]) / (x[i + 1] - x[i]) - (y[i] - y[i - 1]) / (x[i] - x[i - 1]);
        u[i] = (6.0f * u[i] / (x[i + 1] - x[i - 1]) - sig * u[i - 1]) / p;
    }

    if (ynNatural)
        qn = un = 0.0f;
    else
    {
        qn = 0.5f;
        un = (3.0f / (x[n - 1] - x[n - 2])) * (ypn - (y[n - 1] - y[n - 2]) / (x[n - 1] - x[n - 2]));
    }

    y2[n - 1] = (un - qn * u[n - 2]) / (qn * y2[n - 2] + 1.0f);

    for (k = n - 2; k >= 0; k--)
        y2[k] = y2[k] * y2[k + 1] + u[k];

	delete u;
}

// Copyright (c) by NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING (ISBN 0-521-43108-5)
void splint(double xa[], double ya[], double y2a[], int n, double x, double *y)
{
    int klo, khi, k;
    double h, b, a;

    klo = 0;
    khi = n - 1;
    while (khi - klo > 1)
    {
        k = (khi + klo) >> 1;
        if (xa[k] > x) khi = k;
        else klo = k;
    }
    h = xa[khi] - xa[klo];
    if (h == 0.0) throw "splint: Bad xa input.";
    a = (xa[khi] - x) / h;
    b = (x - xa[klo]) / h;
    *y = a * ya[klo] + b * ya[khi] + ((a * a * a - a) * y2a[klo] + (b * b * b - b) * y2a[khi]) * (h * h) / 6.0f;
}

////////////////////////////////////////////////////////////////////////////////

#define DZERO (double **)0
#define ZERO (double *)0

void freeall(
	int n, double *h, double *a, double *b, double *c, double *dtheta,
	double **e, double **w, double **wprev	
);

double getang(double qi[], double qf[], double e[]);

void rates(
	int n, int maxit, double tol, double wi[], double wf[], double h[],
	double a[], double b[], double c[], double dtheta[], double e[][3],
	double w[][3], double wprev[][3]
);

int bd(
	double e[], double dtheta, int flag, double xin[], double xout[]
);

void rf(
	double e[], double dtheta, double win[], double rhs[]
);

void slew3_init(
	double dt, double dtheta, double e[], double wi[], double ai[],
	double wf[], double af[]
);

void slew3(
	double t, double dt, double qi[], double q[],
	double omega[], double alpha[], double jerk[]
);

double unvec(
	double a[], double au[]
);

void crossp(
	double b[], double c[], double a[]
);

// Note: This function is based on the qspline CC0 project by James McEnnan:
// http://sourceforge.net/projects/qspline-cc0
//
/// <param name="n">number of input points (n >= 4).</param>
/// <param name="maxit">maximum number of iterations.</param>
/// <param name="tol">convergence tolerance (rad/sec) for iteration termination.</param>
/// <param name="wi">initial angular rate vector.</param>
/// <param name="wf">final angular rate vector.</param>
/// <param name="x">input vector of n time values.</param>
/// <param name="y">input vector of quaternion values.</param>
/// <param name="h">out: vector of n-1 x-interval values.</param>
/// <param name="dtheta">out: vector of n-1 rotation angles.</param>
/// <param name="e">out: rray of n-1 rotation axis vectors.</param>
/// <param name="w">out: n intermediate angular rates.</param>
void qspline_init(
	int n, int maxit, double tol, double wi[], double wf[],
	double x[], double y[][4],
	double h[], double dtheta[], double e[][3], double w[][3]
)
{
  int i, j;
  double *a, *b, *c, (*wprev)[3];

  if(n < 4) throw "qspline_init: insufficient input data.\n";

  wprev = new double[n][3];
  a = new double[n-1];
  b = new double[n-1];
  c = new double[n-1];

  for(i = 0;i < n;i++)
    for(j = 0;j < 3;j++)
      w[i][j] = 0.0;

  for(i = 0;i < n - 1;i++)
  {
    h[i] = x[i + 1] - x[i];

    if(h[i] <= 0.0) throw "qspline_init: x is not monotonic.\n";
  }

  /* compute spline coefficients. */

  for(i = 0;i < n - 1;i++)
    dtheta[i] = getang(y[i],y[i + 1],e[i]);

  rates(n,maxit,tol,wi,wf,h,a,b,c,dtheta,e,w,wprev);

  delete c;
  delete b;
  delete a;
  delete [] wprev;
}

// Note: This function is based on the qspline CC0 project by James McEnnan:
// http://sourceforge.net/projects/qspline-cc0
// Todo: if used in a class it could reduce calls to slew3_init.
//
/// <summary>Interpolates a quaternion value.</summary>
/// <param name="n">number of input points (n>=4)</param>
/// <param name="xi">input time</param>
/// <param name="x">input vector of n time values.</param>
/// <param name="y">input vector of n quaternion values.</param>
/// <param name="h">vector of n-1 x-interval values.</param>
/// <param name="dtheta">vector of n-1 rotation angles.</param>
/// <param name="e">array of n-1 rotation axis vectors.</param>
/// <param name="w">n intermediate angular rates.</param>
/// <param name="q">out: interpolated quaternion value.</param>
/// <param name="omega">out: interpolated angular rate value (rad/sec).</param>
/// <param name="alpha">out: interpolated angular acceleration value (rad/sec^2).</param>
void qspline_interp(
	int n, double xi, double x[], double y[][4],
	double h[], double dtheta[], double e[][3], double w[][3], 
	double q[4], double omega[3], double alpha[3]
)
{
	double dum1[3], dum2[3];
	
	int klo, khi, k;

	klo = 0;
	khi = n - 1;
	while (khi - klo > 1)
	{
		k = (khi + klo) >> 1;
		if (x[k] > xi) khi = k;
		else klo = k;
	}

	/* interpolate and output results. */

	slew3_init(h[klo],dtheta[klo],e[klo],w[klo],dum1,w[klo+1],dum2);

    slew3(xi - x[klo],h[klo],y[klo],q,omega,alpha,dum1);
}

// Note: This function has been slighlty modified from it's original (definition only).
double getang(double qi[], double qf[], double e[])
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

purpose

Subroutine getang computes the slew angle and axis between the input initial and
final states.

calling sequence

variable     i/o     description
--------     ---     -----------

qi            i      initial attitude quaternion.

qf            i      final attitude quaternion.

e             o      unit vector along slew eigen-axis.

return value

slew angle in radians

external references

unvec

programming

J. J. McEnnan, May, 2000.

COPYRIGHT (C) 2003 by James McEnnan

    To the extent possible under law, James McEnnan ( jmcennan@mailaps.org )
    has waived all copyright and related or neighboring rights to qspline CC0
    ( http://sourceforge.net/projects/qspline-cc0 ) under the CC0 1.0
    license ( http://creativecommons.org/publicdomain/zero/1.0/ ).
    This work is published from: United States.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
{
  double dtheta, sa, ca, temp[3];

  temp[0] = qi[3]*qf[0] - qi[0]*qf[3] - qi[1]*qf[2] + qi[2]*qf[1];
  temp[1] = qi[3]*qf[1] - qi[1]*qf[3] - qi[2]*qf[0] + qi[0]*qf[2];
  temp[2] = qi[3]*qf[2] - qi[2]*qf[3] - qi[0]*qf[1] + qi[1]*qf[0];

  ca =  qi[0]*qf[0] + qi[1]*qf[1] + qi[2]*qf[2] + qi[3]*qf[3];

  sa = unvec(temp,e);

  dtheta = 2.0*atan2(sa,ca);

  return dtheta;
}

// Note: This function has been slighlty modified from it's original (definition only).
void rates(
	int n, int maxit, double tol, double wi[], double wf[], double h[],
	double a[], double b[], double c[], double dtheta[], double e[][3],
	double w[][3], double wprev[][3]
)
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

purpose

subroutine rates computes intermediate angular rates for interpolation.

calling sequence

variable     i/o     description
--------     ---     -----------

n             i      number of input data points.

maxit         i      maximum number of iterations.

tol           i      convergence tolerance (rad/sec) for iteration termination.

wi            i      initial angular rate vector.

wf            i      final angular rate vector.

h             i      pointer to vector of time interval values.

a             i      pointer to intermediate work space.

b             i      pointer to intermediate work space.

c             i      pointer to intermediate work space.

dtheta        i      pointer to vector of rotation angles.

e             i      pointer to array of rotation axis vectors.

w             o      pointer to output intermediate angular rate values.

wprev         o      pointer to previous intermediate angular rate values.

return value

none

external references

bd
rf

programming

J. J. McEnnan, April, 2003.

COPYRIGHT (C) 2003 by James McEnnan

    To the extent possible under law, James McEnnan ( jmcennan@mailaps.org )
    has waived all copyright and related or neighboring rights to qspline CC0
    ( http://sourceforge.net/projects/qspline-cc0 ) under the CC0 1.0
    license ( http://creativecommons.org/publicdomain/zero/1.0/ ).
    This work is published from: United States.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
{
  int i, j, iter;
  double dw, temp1[3], temp2[3];

  iter = 0;

  do                                                 /* start iteration loop. */
  {
    for(i = 1;i < n - 1;i++)
      for(j = 0;j < 3;j++)
        wprev[i][j] = w[i][j];

    /* set up the tridiagonal matrix. d initially holds the RHS vector array;
       it is then overlaid with the calculated angular rate vector array. */

    for(i = 1;i < n - 1;i++)
    {
      a[i] = 2.0/h[i - 1];
      b[i] = 4.0/h[i - 1] + 4.0/h[i];
      c[i] = 2.0/h[i];
  
      rf(e[i - 1],dtheta[i - 1],wprev[i],temp1);

      for(j = 0;j < 3;j++)
        w[i][j] = 6.0*(dtheta[i - 1]*e[i - 1][j]/(h[i - 1]*h[i - 1]) +
                       dtheta[i    ]*e[i    ][j]/(h[i    ]*h[i    ])) -
                  temp1[j];
    }
  
    bd(e[0    ],dtheta[0    ],1,wi,temp1);
    bd(e[n - 2],dtheta[n - 2],0,wf,temp2);
  
    for(j = 0;j < 3;j++)
    {
      w[1    ][j] -= a[1    ]*temp1[j];
      w[n - 2][j] -= c[n - 2]*temp2[j];
    }
  
    /* reduce the matrix to upper triangular form. */
  
    for(i = 1;i < n - 2;i++)
    {
      b[i + 1] -= c[i]*a[i + 1]/b[i];
  
      for(j = 0;j < 3;j++)
      {
        bd(e[i],dtheta[i],1,w[i],temp1);
        
        w[i + 1][j] -= temp1[j]*a[i + 1]/b[i];
      }
    }
  
    /* solve using back substitution. */
  
    for(j = 0;j < 3;j++)
      w[n - 2][j] /= b[n - 2];
  
    for(i = n - 3;i > 0;i--)
    {
      bd(e[i],dtheta[i],0,w[i + 1],temp1);
  
      for(j = 0;j < 3;j++)
        w[i][j] = (w[i][j] - c[i]*temp1[j])/b[i];
    }
  
    dw = 0.0;

    for(i = 1;i < n - 1;i++)
      for(j =  0;j < 3;j++)
        dw += (w[i][j] - wprev[i][j])*(w[i][j] - wprev[i][j]);

    dw = sqrt(dw);
  }
  while(iter++ < maxit && dw > tol);

  /* solve for end conditions. */
  
  for(j = 0;j < 3;j++)
  {
    w[0    ][j] = wi[j];
    w[n - 1][j] = wf[j];
  }
}

#define EPS 1.0e-6

// Note: This function has been slighlty modified from it's original (definition only).
int bd(
	double e[], double dtheta, int flag, double xin[], double xout[]
)
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

purpose

Subroutine bd performs the transformation between the coefficient vector and
the angular rate vector.

calling sequence

variable     i/o     description
--------     ---     -----------

e             i      unit vector along slew eigen-axis.

dtheta        i      slew angle (rad).

flag          i      flag determining direction of transformation.
                      = 0 -> compute coefficient vector from
                      angular rate vector
                      = 1 -> compute angular rate vector from
                      coefficient vector

xin           i      input vector.

xout          o      output vector.

return value

 0 -> no error
-1 -> transformation direction incorrectly specified.

external references

crossp

programming

J. J. McEnnan, April, 2003.

COPYRIGHT (C) 2003 by James McEnnan

    To the extent possible under law, James McEnnan ( jmcennan@mailaps.org )
    has waived all copyright and related or neighboring rights to qspline CC0
    ( http://sourceforge.net/projects/qspline-cc0 ) under the CC0 1.0
    license ( http://creativecommons.org/publicdomain/zero/1.0/ ).
    This work is published from: United States.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
{
  int i;
  double sa, ca, b0, b1, b2, temp1[3], temp2[3];

  if(dtheta > EPS)
  {
    ca = cos(dtheta);
    sa = sin(dtheta);

    if(flag == 0)
    {
      b1 = 0.5*dtheta*sa/(1.0 - ca);
      b2 = 0.5*dtheta;
    }
    else if(flag == 1)
    {
      b1 = sa/dtheta;
      b2 = (ca - 1.0)/dtheta;
    }
    else
      return -1;

    b0 = xin[0]*e[0] + xin[1]*e[1] + xin[2]*e[2];

    crossp(e,xin,temp2);

    crossp(temp2,e,temp1);

    for(i = 0;i < 3;i++)
      xout[i] = b0*e[i] + b1*temp1[i] + b2*temp2[i];
  }
  else
  {
    for(i = 0;i < 3;i++)
      xout[i] = xin[i];
  }

  return 0;
}

// Note: This function has been slighlty modified from it's original (definition only).
void rf(
	double e[], double dtheta, double win[], double rhs[]
)
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

purpose

Subroutine rf computes the non-linear rate contributions to the final
angular acceleration.

calling sequence

variable     i/o     description
--------     ---     -----------

e             i      unit vector along slew eigen-axis.

dtheta        i      slew angle (rad).

win           i      input final angular rate vector.

rhs           o      output vector containing non-linear rate contributions
                     to the final acceleration.

return value

none

external references

crossp

programming

J. J. McEnnan, May, 2003.

COPYRIGHT (C) 2003 by James McEnnan

    To the extent possible under law, James McEnnan ( jmcennan@mailaps.org )
    has waived all copyright and related or neighboring rights to qspline CC0
    ( http://sourceforge.net/projects/qspline-cc0 ) under the CC0 1.0
    license ( http://creativecommons.org/publicdomain/zero/1.0/ ).
    This work is published from: United States.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
{
  int i;
  double sa, ca, dot, mag, c1, r0, r1, temp1[3], temp2[3];

  if(dtheta > EPS)
  {
    ca = cos(dtheta);
    sa = sin(dtheta);

    crossp(e,win,temp2);

    crossp(temp2,e,temp1);

    dot = win[0]*e[0] + win[1]*e[1] + win[2]*e[2];

    mag = win[0]*win[0] + win[1]*win[1] + win[2]*win[2];

    c1 = (1.0 - ca);

    r0 = 0.5*(mag - dot*dot)*(dtheta - sa)/c1;

    r1 = dot*(dtheta*sa - 2.0*c1)/(dtheta*c1);

    for(i = 0;i < 3;i++)
      rhs[i] = r0*e[i] + r1*temp1[i];
  }
  else
  {
    for(i = 0;i < 3;i++)
      rhs[i] = 0.0;
  }
}

static double a[3][3], b[3][3], c[2][3], d[3];

// Note: This function has been slighlty modified from it's original (definition only).
void slew3_init(
	double dt, double dtheta, double e[], double wi[], double ai[],
	double wf[], double af[]
)
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

purpose

Subroutine slew3_init computes the coefficients for a third-order polynomial
interpolation function describing a slew between the input initial and
final states.

calling sequence

variable     i/o     description
--------     ---     -----------

dt            i      slew time (sec).

dtheta        i      slew angle (rad).

e             i      unit vector along slew eigen-axis.

wi            i      initial body angular rate (rad/sec).

ai            i      initial body angular acceleration (rad/sec^2)
                     (included for compatibility only).

wf            i      final body angular rate (rad/sec).

af            i      final body angular acceleration (rad/sec^2)
                     (included for compatibility only).

return value

none

external references

none

programming

J. J. McEnnan, March, 2003.

COPYRIGHT (C) 2003 by James McEnnan

    To the extent possible under law, James McEnnan ( jmcennan@mailaps.org )
    has waived all copyright and related or neighboring rights to qspline CC0
    ( http://sourceforge.net/projects/qspline-cc0 ) under the CC0 1.0
    license ( http://creativecommons.org/publicdomain/zero/1.0/ ).
    This work is published from: United States.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
{
  int i;
  double sa, ca, c1, c2;
  double b0, bvec1[3], bvec2[3], bvec[3];

  if(dt <= 0.0)
    return;

  sa = sin(dtheta);
  ca = cos(dtheta);

  /* final angular rate terms. */

  if(dtheta > EPS)
  {
    c1 = 0.5*sa*dtheta/(1.0 - ca);

    c2 = 0.5*dtheta;

    b0 = e[0]*wf[0] + e[1]*wf[1] + e[2]*wf[2];

    crossp(e,wf,bvec2);

    crossp(bvec2,e,bvec1);

    for(i = 0;i < 3;i++)
      bvec[i] = b0*e[i] + c1*bvec1[i] + c2*bvec2[i];
  }
  else
  {
    for(i = 0;i < 3;i++)
      bvec[i] = wf[i];
  }

  /* compute coefficients. */

  for(i = 0;i < 3;i++)
  {
    b[0][i] = wi[i];
    a[2][i] = e[i]*dtheta;
    b[2][i] = bvec[i];

    a[0][i] =  b[0][i]*dt;
    a[1][i] = (b[2][i]*dt - 3.0*a[2][i]);

    b[1][i] = (2.0*a[0][i] + 2.0*a[1][i])/dt;
    c[0][i] = (2.0*b[0][i] +     b[1][i])/dt;
    c[1][i] = (    b[1][i] + 2.0*b[2][i])/dt;

       d[i] = (    c[0][i] +     c[1][i])/dt;
  }
}

// Note: This function has been slighlty modified from it's original (definition only).
void slew3(
	double t, double dt, double qi[], double q[],
	double omega[], double alpha[], double jerk[]
)
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

purpose

Subroutine slew3 computes the quaternion, body angular rate, acceleration and
jerk as a function of time corresponding to a third-order polynomial
interpolation function describing a slew between initial and final states.

calling sequence

variable     i/o     description
--------     ---     -----------

t             i      current time (seconds from start).

dt            i      slew time (sec).

qi            i      initial attitude quaternion.

q             o      current attitude quaternion.

omega         o      current body angular rate (rad/sec).

alpha         o      current body angular acceleration (rad/sec^2).

jerk          o      current body angular jerk (rad/sec^3).

return value

none

external references

unvec

programming

J. J. McEnnan, March, 2003.

COPYRIGHT (C) 2003 by James McEnnan

    To the extent possible under law, James McEnnan ( jmcennan@mailaps.org )
    has waived all copyright and related or neighboring rights to qspline CC0
    ( http://sourceforge.net/projects/qspline-cc0 ) under the CC0 1.0
    license ( http://creativecommons.org/publicdomain/zero/1.0/ ).
    This work is published from: United States.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
{
  int i;
  double x, ang, sa, ca, u[3], x1[2];
  double th0[3], th1[3], th2[3], th3[3], temp0[3], temp1[3], temp2[3];
  double thd1, thd2, thd3, w2, td2, ut2, wwd;
  double w[3], udot[3], wd1[3], wd1xu[3], wd2[3], wd2xu[3];

  if(dt <= 0.0)
    return;

  x = t/dt;

  x1[0] = x - 1.0;

  for(i = 1;i < 2;i++)
    x1[i] = x1[i - 1]*x1[0];

  for(i = 0;i < 3;i++)
  {
    th0[i] = ((x*a[2][i] + x1[0]*a[1][i])*x + x1[1]*a[0][i])*x;

    th1[i] = (x*b[2][i] + x1[0]*b[1][i])*x + x1[1]*b[0][i];

    th2[i] = x*c[1][i] + x1[0]*c[0][i];

    th3[i] = d[i];
  }

  ang = unvec(th0,u);

  ca = cos(0.5*ang);
  sa = sin(0.5*ang);

  q[0] = ca*qi[0] + sa*( u[2]*qi[1] - u[1]*qi[2] + u[0]*qi[3]);
  q[1] = ca*qi[1] + sa*(-u[2]*qi[0] + u[0]*qi[2] + u[1]*qi[3]);
  q[2] = ca*qi[2] + sa*( u[1]*qi[0] - u[0]*qi[1] + u[2]*qi[3]);
  q[3] = ca*qi[3] + sa*(-u[0]*qi[0] - u[1]*qi[1] - u[2]*qi[2]);

  ca = cos(ang);
  sa = sin(ang);

  if(ang > EPS)
  {
    /* compute angular rate vector. */

    crossp(u,th1,temp1);

    for(i = 0;i < 3;i++)
      w[i] = temp1[i]/ang;

    crossp(w,u,udot);

    thd1 = u[0]*th1[0] + u[1]*th1[1] + u[2]*th1[2];

    for(i = 0;i < 3;i++)
      omega[i] = thd1*u[i] + sa*udot[i] - (1.0 - ca)*w[i];

    /* compute angular acceleration vector. */

    thd2 = udot[0]*th1[0] + udot[1]*th1[1] + udot[2]*th1[2] +
              u[0]*th2[0] +    u[1]*th2[1] +    u[2]*th2[2];

    crossp(u,th2,temp1);

    for(i = 0;i < 3;i++)
      wd1[i] = (temp1[i] - 2.0*thd1*w[i])/ang;

    crossp(wd1,u,wd1xu);

    for(i = 0;i < 3;i++)
      temp0[i] = thd1*u[i] - w[i];

    crossp(omega,temp0,temp1);

    for(i = 0;i < 3;i++)
      alpha[i] = thd2*u[i] + sa*wd1xu[i] - (1.0 - ca)*wd1[i] +
      thd1*udot[i] + temp1[i];

    /* compute angular jerk vector. */

    w2 = w[0]*w[0] + w[1]*w[1] + w[2]*w[2];

    thd3 = wd1xu[0]*th1[0] + wd1xu[1]*th1[1] + wd1xu[2]*th1[2] -
           w2*(u[0]*th1[0] + u[1]*th1[1] + u[2]*th1[2]) +
           2.0*(udot[0]*th2[0] + udot[1]*th2[1] + udot[2]*th2[2]) +
           u[0]*th3[0] + u[1]*th3[1] + u[2]*th3[2];

    crossp(th1,th2,temp1);

    for(i = 0;i < 3;i++)
      temp1[i] /= ang;

    crossp(u,th3,temp2);

    td2 = (th1[0]*th1[0] + th1[1]*th1[1] + th1[2]*th1[2])/ang;

    ut2 = u[0]*th2[0] + u[1]*th2[1] + u[2]*th2[2];

    wwd = w[0]*wd1[0] + w[1]*wd1[1] + w[2]*wd1[2];

    for(i = 0;i < 3;i++)
      wd2[i] = (temp1[i] + temp2[i] - 2.0*(td2 + ut2)*w[i] -
      4.0*thd1*wd1[i])/ang;

    crossp(wd2,u,wd2xu);

    for(i = 0;i < 3;i++)
      temp2[i] = thd2*u[i] + thd1*udot[i] - wd1[i];

    crossp(omega,temp2,temp1);

    crossp(alpha,temp0,temp2);

    for(i = 0;i < 3;i++)
      jerk[i] = thd3*u[i] + sa*wd2xu[i] - (1.0 - ca)*wd2[i] +
      2.0*thd2*udot[i] + thd1*((1.0 + ca)*wd1xu[i] - w2*u[i] - sa*wd1[i]) -
      wwd*sa*u[i] + temp1[i] + temp2[i];
  }
  else
  {
    crossp(th1,th2,temp1);

    for(i = 0;i < 3;i++)
    {
      omega[i] = th1[i];
      alpha[i] = th2[i];
       jerk[i] = th3[i] - 0.5*temp1[i];
    }
  }
}

// Note: This function has been slighlty modified from it's original (definition only).
double unvec(
	double a[], double au[]
)
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

purpose

subroutine unvec unitizes a vector and computes its magnitude.

calling sequence

variable     i/o     description
--------     ---     -----------

a             i      input vector.

au            o      output unit vector.

return value

magnitude of vector a.

external references

none

programming

J. J. McEnnan, December, 1987.

COPYRIGHT (C) 2003 by James McEnnan

    To the extent possible under law, James McEnnan ( jmcennan@mailaps.org )
    has waived all copyright and related or neighboring rights to qspline CC0
    ( http://sourceforge.net/projects/qspline-cc0 ) under the CC0 1.0
    license ( http://creativecommons.org/publicdomain/zero/1.0/ ).
    This work is published from: United States.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
{
  double amag;

  amag = sqrt(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);

  if(amag > 0.0)
  {
    au[0] = a[0]/amag;
    au[1] = a[1]/amag;
    au[2] = a[2]/amag;
  }
  else
  {
    au[0] = 0.0;
    au[1] = 0.0;
    au[2] = 0.0;
  }

  return amag;
}

// Note: This function has been slighlty modified from it's original (definition only).
void crossp(
	double b[], double c[], double a[]
)
/*
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

purpose

subroutine crossp computes the vector cross product b x c.

calling sequence

variable     i/o     description
--------     ---     -----------

b             i      input vector.

c             i      input vector.

a             o      output vector = b x c.

return value

none

external references

none

programming

J. J. McEnnan, February, 1988.

COPYRIGHT (C) 2003 by James McEnnan

    To the extent possible under law, James McEnnan ( jmcennan@mailaps.org )
    has waived all copyright and related or neighboring rights to qspline CC0
    ( http://sourceforge.net/projects/qspline-cc0 ) under the CC0 1.0
    license ( http://creativecommons.org/publicdomain/zero/1.0/ ).
    This work is published from: United States.

++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
{
  a[0] = b[1]*c[2] - b[2]*c[1];
  a[1] = b[2]*c[0] - b[0]*c[2];
  a[2] = b[0]*c[1] - b[1]*c[0];
}

// CubicObjectSpline ///////////////////////////////////////////////////////////

CubicObjectSpline::CubicObjectSpline()
{
	m_Rebuild = true;
}

CubicObjectSpline::~CubicObjectSpline()
{
	Free();
}

void CubicObjectSpline::Add(double t, COSValue y)
{
	m_Rebuild = true;
	m_Points[t] = y;
}

void CubicObjectSpline::Clear(void)
{
	m_Rebuild = true;
	m_Points.clear();
}

COSValue CubicObjectSpline::Eval(double t)
{
	COSValue result;

	if(m_Points.size() < 4) throw "CubicObjectSpline::Eval only allowed with at least 4 points";

	int n = m_Points.size();

	if(m_Rebuild)
	{
		m_Rebuild = false;

		Free();

		m_Build.T = new double[n];
		m_Build.X = new double[n];
		m_Build.X2 = new double[n];
		m_Build.Y = new double[n];
		m_Build.Y2 = new double[n];
		m_Build.Z = new double[n];
		m_Build.Z2 = new double[n];
		m_Build.Q_y = new double[n][4];
		m_Build.Q_h = new double[n-1];
		m_Build.Q_dtheta = new double[n-1];
		m_Build.Q_e = new double[n-1][3];
		m_Build.Q_w = new double[n][3];
		m_Build.Fov = new double[n];
		m_Build.Fov2 = new double[n];

		{
			Quaternion QLast;

			int i = 0;
			for(COSPoints::iterator it = m_Points.begin(); it!=m_Points.end(); it++)
			{
				m_Build.T[i] = it->first;
				m_Build.X[i] = it->second.T.X;
				m_Build.Y[i] = it->second.T.Y;
				m_Build.Z[i] = it->second.T.Z;
				m_Build.Fov[i] = it->second.Fov;

				Quaternion Q = it->second.R;
				
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

		spline(m_Build.T , m_Build.X, n, false, 0.0, false, 0.0, m_Build.X2);
		spline(m_Build.T , m_Build.Y, n, false, 0.0, false, 0.0, m_Build.Y2);
		spline(m_Build.T , m_Build.Z, n, false, 0.0, false, 0.0, m_Build.Z2);

		double wi[3] = {0.0,0.0,0.0};
		double wf[3] = {0.0,0.0,0.0};
		qspline_init(n, 2, EPS, wi, wf, m_Build.T, m_Build.Q_y, m_Build.Q_h, m_Build.Q_dtheta, m_Build.Q_e, m_Build.Q_w);

		spline(m_Build.T , m_Build.Fov, n, false, 0.0, false, 0.0, m_Build.Fov2);
	}

	double x,y,z;
	double Q[4],dum1[4],dum2[4];
	double fov;

	splint(m_Build.T, m_Build.X, m_Build.X2, n, t, &x);
	splint(m_Build.T, m_Build.Y, m_Build.Y2, n, t, &y);
	splint(m_Build.T, m_Build.Z, m_Build.Z2, n, t, &z);

	qspline_interp(n, t, m_Build.T, m_Build.Q_y, m_Build.Q_h, m_Build.Q_dtheta, m_Build.Q_e, m_Build.Q_w, Q, dum1, dum2);

	splint(m_Build.T, m_Build.Fov, m_Build.Fov2, n, t, &fov);

	result.T.X = x;
	result.T.Y = y;
	result.T.Z = z;
	result.R.W = Q[3];
	result.R.X = Q[0];
	result.R.Y = Q[1];
	result.R.Z = Q[2];
	result.Fov = fov;

	return result;
}

void CubicObjectSpline::Free()
{
	delete m_Build.Fov2;
	m_Build.Fov2 = 0;
	delete m_Build.Fov;
	m_Build.Fov = 0;
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
	delete m_Build.Z2;
	m_Build.Z2 = 0;
	delete m_Build.Z;
	m_Build.Z = 0;
	delete m_Build.Y2;
	m_Build.Y2 = 0;
	delete m_Build.Y;
	m_Build.Y = 0;
	delete m_Build.X2;
	m_Build.X2 = 0;
	delete m_Build.X;
	m_Build.X = 0;
	delete m_Build.T;
	m_Build.T = 0;
}

COSPoints::const_iterator CubicObjectSpline::GetBegin(void)
{
	return m_Points.begin();
}

COSPoints::const_iterator CubicObjectSpline::GetEnd(void)
{
	return m_Points.end();
}

double CubicObjectSpline::GetLowerBound()
{
	if(m_Points.size() < 1) throw "CubicObjectSpline::GetLowerBound: Size less than 1.";

	return m_Points.begin()->first;
}

double CubicObjectSpline::GetUpperBound()
{
	if(m_Points.size() < 1) throw "CubicObjectSpline::GetUpperBound: Size less than 1.";

	COSPoints::iterator it = m_Points.end();
	it--;

	return it->first;
}

size_t CubicObjectSpline::GetSize()
{
	return m_Points.size();
}

void CubicObjectSpline::Remove(double t)
{
	m_Rebuild = true;
	m_Points.erase(t);
}


// QEulerAngles ////////////////////////////////////////////////////////////////

QEulerAngles::QEulerAngles(double pitch, double yaw, double roll)
{
    Pitch = pitch;
    Yaw = yaw;
    Roll = roll;
}

// QREulerAngles ///////////////////////////////////////////////////////////////

QREulerAngles::QREulerAngles(double pitch, double yaw, double roll)
{
    Pitch = pitch;
    Yaw = yaw;
    Roll = roll;
}

QREulerAngles QREulerAngles::FromQEulerAngles(QEulerAngles a)
{
	return QREulerAngles(
		M_PI * a.Pitch / 180.0,
		M_PI * a.Yaw / 180.0,
		M_PI * a.Roll / 180.0
	);
}

QEulerAngles QREulerAngles::ToQEulerAngles(void)
{
	return QEulerAngles(
		180.0 * Pitch / M_PI,
		180.0 * Yaw / M_PI,
		180.0 * Roll / M_PI
	);
}

// Quaternion //////////////////////////////////////////////////////////////////

Quaternion operator +(Quaternion a, Quaternion b)
{
    return Quaternion(
        a.W + b.W,
        a.X + b.X,
        a.Y + b.Y,
        a.Z + b.Z
    );
}

Quaternion operator *(double a, Quaternion b)
{
    return Quaternion(
        a*b.W,
        a*b.X,
        a*b.Y,
        a*b.Z
    );
}

Quaternion operator *(Quaternion a, Quaternion b)
{
    return Quaternion(
        a.W*b.W - a.X*b.X - a.Y*b.Y - a.Z*b.Z,
        a.W*b.X + a.X*b.W + a.Y*b.Z - a.Z*b.Y,
        a.W*b.Y - a.X*b.Z + a.Y*b.W + a.Z*b.X,
        a.W*b.Z + a.X*b.Y - a.Y*b.X + a.Z*b.W
    );
}

double DotProduct(Quaternion a, Quaternion b)
{
	return a.W*b.W + a.X*b.X +a.Y*b.Y +a.Z*b.Z;
}

Quaternion Quaternion::FromQREulerAngles(QREulerAngles a)
{
	// double pitchH = 0.5 * a.Pitch;
	// Quaternion qPitchY(cos(pitchH), 0.0, sin(pitchH), 0.0);
	// 
	// double yawH = 0.5 * a.Yaw;
	// Quaternion qYawZ(cos(yawH), 0.0, 0.0, sin(yawH));
	// 
	// double rollH = 0.5 * a.Roll;
	// Quaternion qRollX(cos(rollH), sin(rollH), 0.0, 0.0);
	// 
	// return qYawZ * qPitchY * qRollX;
	// 
	// qPitch * qRollX =
	// (cos(pitchH)*cos(rollH),
	// cos(pitchH)*sin(rollH),
	// sin(pitchH)*cos(rollH),
	// sin(pitchH)*sin(rollH))
	// qYawZ * qPitch * qRollX =
	// (cos(yawH)*cos(pitchH)*cos(rollH) -sin(yawH)*sin(pitchH)*sin(rollH),
	// cos(yawH)*cos(pitchH)*sin(rollH) -sin(yawH)*sin(pitchH)*cos(rollH),
	// cos(yawH)*sin(pitchH)*cos(rollH) +sin(yawH)*cos(pitchH)*sin(rollH),
	// cos(yawH)*sin(pitchH)*sin(rollH) +sin(yawH)*cos(pitchH)*cos(rollH))

	double xRollH = 0.5 * a.Roll;
	double yPitchH = 0.5 * a.Pitch;
	double zYawH = 0.5 * a.Yaw;
	double sinXRollH = sin(xRollH);
	double cosXRollH = cos(xRollH);
	double sinYPitchH = sin(yPitchH);
	double cosYPitchH = cos(yPitchH);
	double sinZYawH = sin(zYawH);
	double cosZYawH = cos(zYawH);

	return Quaternion(
		cosZYawH*cosYPitchH*cosXRollH -sinZYawH*sinYPitchH*sinXRollH,
		cosZYawH*cosYPitchH*sinXRollH -sinZYawH*sinYPitchH*cosXRollH,
		cosZYawH*sinYPitchH*cosXRollH +sinZYawH*cosYPitchH*sinXRollH,
		cosZYawH*sinYPitchH*sinXRollH +sinZYawH*cosYPitchH*cosXRollH
	);
}

Quaternion::Quaternion()
{
	W = 1.0;
	X = 0.0;
	Y = 0.0;
	Z = 0.0;
}

Quaternion::Quaternion(double w, double x, double y, double z)
{
	W = w;
	X = x;
	Y = y;
	Z = z;
}

double Quaternion::Norm()
{
    return sqrt(W*W +X*X +Y*Y +Z*Z);
}

QREulerAngles Quaternion::ToQREulerAngles()
{
	// TODO: There might still be a problem with singualrities in here!

	double qNorm = Norm();
	double qNormInv = 0.0 != qNorm ? 1.0/qNorm : 0.0;
	Quaternion QUnit = qNormInv * (*this);	

	double angle = 2.0 * acos(QUnit.W);
    double norm = sqrt(QUnit.X * QUnit.X + QUnit.Y * QUnit.Y + QUnit.Z * QUnit.Z);
	double invNorm = 0.0 != norm ? 1.0 / norm : 0.0;
	double vector[3] = { invNorm*QUnit.X, invNorm*QUnit.Y, invNorm*QUnit.Z };

	// R = 
	// |c_a +p*p(1 -c_a)   , p*q(1 -c_a) -r*s_a , p*r(1 -c_a) +q*s_a|
	// |q*p(1 -c_a) +r*s_a , c_a +q*q(1 -c_a)   , q*r(1 -c_a) -p*s_a|
	// |r*p(1 -c_a) -q*s_a , r*q(1 -c_a) +p*s_a , c_a +r*r(1 -c_a)  |

	// X =            Y =            Z =
	// |1, 0  , 0   | |c_y , 0, s_y| |c_z, -s_z, 0|
	// |0, c_x, -s_x| |0   , 1, 0  | |s_z, c_z , 0|
	// |0, s_x, c_x | |-s_y, 0, c_y| |0  , 0   , 1|

	// Z*Y =
	// |c_z*c_y, -s_z, c_z*s_y|
	// |s_z*c_y, c_z , s_z*s_y|
	// |-s_y   , 0   , c_y    |

	// Z*Y*X =
	// |c_z*c_y, -s_z*c_x+c_z*s_y*s_x, s_z*s_y +c_z*s_y*c_x |
	// |s_z*c_y, c_z*c_x +s_z*s_y*s_x, -c_z*s_y +s_z*s_y*c_x|
	// |-s_y   , c_y*s_x             , c_y*c_x              |

	// For comparison: Quake rotation matrix (derived rom Quake 1 Source\QW\client\mathlib.c\AngleVectors).
	// |c_y*c_z, s_x*s_y*c_z -c_x*s_z, c_x*s_y*c_z +s_x*s_z|
	// |c_y*s_z, s_x*s_y*s_z +c_x*c_z, c_x*s_y*s_z -s_x*c_z|
	// |-s_y   , s_x*c_y             , c_x*c_y   
	// where x=roll, y=pitch, z=yaw

	// Z*Y*X = R
	// 1) c_z*c_y = c_a +p*p(1 -c_a)
	// 2) -s_z*c_x+c_z*s_y*s_x = p*q(1 -c_a) -r*s_a
	// 3) s_z*s_y +c_z*s_y*c_x = p*r(1 -c_a) +q*s_a
	// 4) s_z*c_y = q*p(1 -c_a) +r*s_a
	// 5) c_z*c_x +s_z*s_y*s_x = c_a +q*q(1 -c_a)
	// 6) -c_z*s_y +s_z*s_y*c_x = q*r(1 -c_a) -p*s_a
	// 7) -s_y = r*p(1 -c_a) -q*s_a
	// 8) c_y*s_x = r*q(1 -c_a) +p*s_a
	// 9) c_y*c_x = c_a +r*r(1 -c_a)
	//
	// 7=> y = arcsin( -r*p(1 -c_a) +q*s_a)
	//
	// For c_z*c_y != 0:
	// 4/1=> z = arctan2( q*p(1 -c_a) +r*s_a, c_a +p*p(1 -c_a) )
	//
	// For c_y*c_x != 0:
	// 8/9=> x = arctan2( r*q(1 -c_a) +p*s_a, c_a +r*r(1 -c_a) )
	//
	// Case c_y=0,s_y=1:
	// 1) 0 = c_a +p*p(1 -c_a)
	// 2) -s_z*c_x+c_z*s_x = p*q(1 -c_a) -r*s_a
	// 3) s_z +c_z*c_x = p*r(1 -c_a) +q*s_a
	// 4) 0 = q*p(1 -c_a) +r*s_a
	// 5) c_z*c_x +s_z*s_x = c_a +q*q(1 -c_a)
	// 6) -c_z +s_z*c_x = q*r(1 -c_a) -p*s_a
	// 7) -1 = r*p(1 -c_a) -q*s_a
	// 8) 0 = r*q(1 -c_a) +p*s_a
	// 9) 0 = c_a +r*r(1 -c_a)
	// =>
	// 2) s(z-x) = -p*r(1_a) +r*s_a
	// 5) c(z-x) = c_a +q*q(1 -c_a)
	// =>
	// For c(z-x) != 0:
	// 2/5=> z-x = arctan2(-p*r(1 -c_a) +r*s_a, c_a +q*q(1 -c_a))
	//
	// Case c_y=0,s_y=-1:
	// 1) 0 = c_a +p*p(1 -c_a)
	// 2) -s_z*c_x-c_z*s_x = p*q(1 -c_a) -r*s_a
	// 3) -s_z -c_z*c_x = p*r(1 -c_a) +q*s_a
	// 4) 0 = q*p(1 -c_a) +r*s_a
	// 5) c_z*c_x -s_z*s_x = c_a +q*q(1 -c_a)
	// 6) c_z -s_z*c_x = q*r(1 -c_a) -p*s_a
	// 7) 1 = r*p(1 -c_a) -q*s_a
	// 8) 0 = r*q(1 -c_a) +p*s_a
	// 9) 0 = c_a +r*r(1 -c_a)
	// =>
	// 2) s(z+x) = -p*q(1 -c_a) +r*s_a
	// 5) c(z+x) = c_a +q*q(1 -c_a)
	// For c(z+x) != 0:
	// 2/5=> z+x = arctan2(-p*r(1 -c_a) +r*s_a, c_a +q*q(1 -c_a))
	
	double cosA = cos(angle);
	double sinA = sin(angle);
	
	double sinYPitch = -vector[2]*vector[0]*(1.0 -cosA) +vector[1]*sinA;
	double yPitch;
	double zYaw;
	double xRoll;

	if(sinYPitch > 1.0 -EPS)
	{
		// south pole singularity:

		yPitch = M_PI / 2.0;

		// => sinYPitchH = cosYPitchH = about 0.7071
		//
		// W = 0.7071*(cosZYawH*cosXRollH -sinZYawH*sinXRollH)
		// X = 0.7071*(cosZYawH*sinXRollH -sinZYawH*cosXRollH)
		// Y = 0.7071*(cosZYawH*cosXRollH +sinZYawH*sinXRollH)
		// Z = 0.7071*(cosZYawH*sinXRollH +sinZYawH*cosXRollH)
		//
		// W = 0.7071*cos(zYawH +xRollH)
		// X = 0.7071*sin(xRollH -zYawH)
		// Y = 0.7071*cos(zYawH -xRollH)
		// Z = 0.7071*sin(xRollH +zYawH)
		//
		// Z/W = tan(xRollH +zYawH)
		// xRollH +zYawH = atan2(Z,W)

		xRoll = -2.0*atan2(QUnit.Z,QUnit.W);
		zYaw = 0;
	}
	else
	if(sinYPitch < -1.0 +EPS)
	{
		// north pole singularity:

		yPitch = -M_PI / 2.0;
		xRoll = 2.0*atan2(QUnit.Z,QUnit.W);
		zYaw = 0;
	}
	else
	{
		// hopefully no singularity:

		yPitch = asin( sinYPitch );
		zYaw = atan2( vector[1]*vector[0]*(1.0 -cosA) +vector[2]*sinA, cosA +vector[0]*vector[0]*(1.0 -cosA) );
		xRoll = atan2( vector[2]*vector[1]*(1.0 -cosA) +vector[0]*sinA, cosA +vector[2]*vector[2]*(1.0 -cosA) );
	}

    return QREulerAngles(
        yPitch,
        zYaw,
        xRoll
    );
}

} // namespace Afx {
} // namespace Math
