// Copyright (c) by advancedfx.org
//
// Last changes:
// 2013-05-30 by dominik.matrixstorm.com
//
// First changes:
// 2013-05-27 by dominik.matrixstorm.com

using System;
using System.Collections.Generic;

namespace AfxGui {

/// <summary>
/// Euler angles, applied in order of pitch(Y/left), yaw(Z/up), roll(X/forward), right hand grip rule.
/// </summary>
struct EulerAngles
{
    public EulerAngles(double pitch, double yaw, double roll)
    {
        m_Pitch = pitch;
        m_Yaw = yaw;
        m_Roll = roll;
    }

    public double Pitch { get { return m_Pitch; } }
    public double Yaw { get { return m_Yaw; } }
    public double Roll { get { return m_Roll; } }

    double m_Pitch;
    double m_Yaw;
    double m_Roll;
}

struct Quaternion
{
    public static Quaternion operator +(Quaternion a, Quaternion b)
    {
        return new Quaternion(
            a.Q0 + b.Q0,
            a.Q1 + b.Q1,
            a.Q2 + b.Q2,
            a.Q3 + b.Q3
        );
    }

    public static Quaternion operator *(double a, Quaternion b)
    {
        return new Quaternion(
            a*b.Q0,
            a*b.Q1,
            a*b.Q2,
            a*b.Q3
        );
    }

    public static Quaternion operator *(Quaternion a, Quaternion b)
    {
        return new Quaternion(
            a.Q0*b.Q0 - a.Q1*b.Q1 - a.Q2*b.Q2 - a.Q3*b.Q3,
            a.Q0*b.Q1 + a.Q1*b.Q0 + a.Q2*b.Q3 - a.Q3*b.Q2,
            a.Q0*b.Q2 - a.Q1*b.Q3 + a.Q2*b.Q0 + a.Q3*b.Q1,
            a.Q0*b.Q3 + a.Q1*b.Q2 - a.Q2*b.Q1 + a.Q3*b.Q0
        );
    }

    public static Quaternion FromEulerAngles(EulerAngles a)
    {
        double pitchH = 0.5d * a.Pitch;
        Quaternion qPitch = new Quaternion(Math.Cos(pitchH), 0.0d, Math.Sin(pitchH), 0.0d);
        
        double yawH = 0.5d * a.Yaw;
        Quaternion qYaw = new Quaternion(Math.Cos(yawH), 0.0d, 0.0d, Math.Sin(yawH));

        double rollH = 0.5d * a.Roll;
        Quaternion qRoll = new Quaternion(Math.Cos(rollH), Math.Sin(rollH), 0.0d, 0.0d);

        return qRoll * qYaw * qPitch;
    }

    public Quaternion(double q0, double q1, double q2, double q3)
    {
        m_Q0 = q0;
        m_Q1 = q1;
        m_Q2 = q2;
        m_Q3 = q3;
    }

    public double Norm()
    {
        return Math.Sqrt(m_Q0 * m_Q0 + m_Q1 * m_Q1 + m_Q2 * m_Q2 + m_Q3 * m_Q3);
    }

    // this needs to be checked if it follows the correct rotation order
    public EulerAngles ToEulerAngles()
    {
        double angle = 2.0d * Math.Acos(m_Q0);
        double norm = Math.Sqrt(m_Q1 * m_Q1 + m_Q2 * m_Q2 + m_Q3 * m_Q3);
        double invNorm = 0.0d != norm ? 1.0d / norm : 0.0d;
        double[] vector = new double[3]{ invNorm * m_Q1, invNorm * m_Q2, invNorm * m_Q3 };

        // R = 
        // |cos(a) + p*p(1-cos(a))  , p*q(1-cos(a)) -r*sin(a), p*r(1-cos(a)) + q*sin(a)|
        // |q*p(1-cos(a)) + r*sin(a), cos(a) +q*q(1-cos(a))  , q*r(1-cos(a)) - p*sin(a)|
        // |r*p(1-cos(a)) - q*sin(a), r*q(1-cos(a))+p*sin(a) , cos(a) + r*r(1-cos(a))  |

        // X =                  Y =                  Z =
        // |1, 0      , 0     | |cos(y), 0, -sin(y)| |cos(z) , sin(z), 0|
        // |0, cos(x) , sin(x)| |0     , 1, 0      | |-sin(z), cos(z), 0|
        // |0, -sin(x), cos(x)| |sin(y), 0, cos(y) | |0      , 0     , 1|

        // ZY = Z*Y =
        // |cos(z)*cos(y) , sin(z), -cos(z)*sin(y)|
        // |-sin(z)*cos(y), cos(z), sin(z)*sin(y) |
        // |sin(y)        , 0     , cos(y)        |

        // XZY = X*ZY =
        // |cos(z)*cos(y)                        , sin(z)        , -cos(z)*sin(y)                       |
        // |-cos(x)*sin(z)*cos(y) + sin(x)*sin(y), cos(x)*cos(z) , cos(x)*sin(z)*sin(y) + sin(x)*cos(y) |
        // |sin(x)*sin(z)*cos(y) + cos(x)*sin(y) , -sin(x)*cos(z), -sin(x)*sin(z)*sin(y) + cos(x)*cos(y)|

        // XZY = R:
        // 1) cos(z)*cos(y)                         = cos(a) + p*p(1-cos(a))
        // 2) sin(z)                                = p*q(1-cos(a)) -r*sin(a)
        // 3) -cos(z)*sin(y)                        = p*r(1-cos(a)) + q*sin(a)
        // 4) -cos(x)*sin(z)*cos(y) + sin(x)*sin(y) = q*p(1-cos(a)) + r*sin(a)
        // 5) cos(x)*cos(z)                         = cos(a) +q*q(1-cos(a))
        // 6) cos(x)*sin(z)*sin(y) + sin(x)*cos(y)  = q*r(1-cos(a)) - p*sin(a)
        // 7) sin(x)*sin(z)*cos(y) + cos(x)*sin(y)  = r*p(1-cos(a)) - q*sin(a)
        // 8) -sin(x)*cos(z)                        = r*q(1-cos(a))+p*sin(a)
        // 9) -sin(x)*sin(z)*sin(y) + cos(x)*cos(y) = cos(a) + r*r(1-cos(a))
        // =>
        // 2=> z =  arcsin( p*q(1-cos(a)) -r*sin(a) )
        // 5=> x = arccos((cos(a) +q*q(1-cos(a))) / cos(z))
        // 1=> y = arccos((cos(a) + p*p(1-cos(a))) / cos(z))
        // shit I suck at math

        // Test of z-Rotation EulerAngles-To-Quat-To-Eulerangles:
        //
        // yawH = 0.5d * a.Yaw
        // qYaw = Quaternion(Math.Cos(yawH), 0.0d, 0.0d, Math.Sin(yawH));
        //
        // angle = a.Yaw
        // v = |0,0,Math.Sin(yawH)|
        // zYaw = arcsin( p*q(1-cos(a)) -r*sin(a) )
        //      = arcsin( -1 * sin(a.Yaw) )
        //
        // obviously there is a sign error in beween, fuck.

        double cosA = Math.Cos(angle);
        double zYaw = Math.Asin(vector[0] * vector[1] * (1 - Math.Cos(angle)) - vector[2] * Math.Sin(angle));
        double cosZ = Math.Cos(zYaw);
        double xRoll = 0.0d; // Math.Acos(0.0d != cosZ ? (cosA + vector[1] * vector[1] * (1 - cosA)) / cosZ : 0.0d);
        double yPitch = 0.0d; // Math.Acos(0.0d != cosZ ? (cosA + vector[0] * vector[0] * (1 - cosA)) / cosZ : 0.0d);

        return new EulerAngles(
            yPitch,
            zYaw,
            xRoll
        );
    }

    public double Q0 { get { return m_Q0; } }
    public double Q1 { get { return m_Q1; } }
    public double Q2 { get { return m_Q2; } }
    public double Q3 { get { return m_Q3; } }

    double m_Q0;
    double m_Q1;
    double m_Q2;
    double m_Q3;
}

class QuaternionTest
{
    public QuaternionTest()
    {
        m_PointsKeysRo = new System.Collections.ObjectModel.ReadOnlyCollection<double>(m_Points.Keys);
        m_PointsValuesRo = new System.Collections.ObjectModel.ReadOnlyCollection<Quaternion>(m_Points.Values);
    }

    public void AddPoint(double x, Quaternion y)
    {
        m_Points[x] = y;

        m_Rebuild = true;
    }

    public Quaternion Eval(double x)
    {
        Quaternion y = new Quaternion(0,0,0,0);

        if (2 <= m_Points.Count)
        {
            if (m_Rebuild)
            {
                m_Rebuild = false;
            }
            
            int klo, khi, k;

            klo = 0;
            khi = m_Points.Keys.Count - 1;
            while (khi - klo > 1)
            {
                k = (khi + klo) >> 1;
                if (m_Points.Keys[k] > x) khi = k;
                else klo = k;
            }

            double t = (x - m_Points.Keys[klo])/(m_Points.Keys[khi] - m_Points.Keys[klo]);

            y = t * m_Points.Values[klo] + (1.0d - t) * m_Points.Values[khi];
        }
        else if (1 == m_Points.Count)
        {
            y = m_Points.Values[0];
        }

        return y;
    }

    public void RemovePoint(double x)
    {
        m_Points.Remove(x);

        m_Rebuild = true;
    }

    public IList<double> Keys
    {
        get
        {
            return m_PointsKeysRo;
        }
    }

    public IList<Quaternion> Values
    {
        get
        {
            return m_PointsValuesRo;
        }
    }

    SortedList<double, Quaternion> m_Points = new SortedList<double, Quaternion>();
    IList<double> m_PointsKeysRo;
    IList<Quaternion> m_PointsValuesRo;
    bool m_Rebuild;
}

} // namespace AfxGui {