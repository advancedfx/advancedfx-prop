// Copyright (c) by advancedfx.org
//
// Last changes:
// 2013-05-06 by dominik.matrixstorm.com
//
// First changes:
// 2013-05-05 by dominik.matrixstorm.com

using System;
using System.Collections.Generic;

namespace AfxGui {

class Spline
{
    //
    // Public members:

    public Spline()
    {
        m_PointsKeysRo = new System.Collections.ObjectModel.ReadOnlyCollection<double>(m_Points.Keys);
        m_PointsValuesRo = new System.Collections.ObjectModel.ReadOnlyCollection<double>(m_Points.Values);
    }

    /// <summary>
    /// Adds a point.
    /// </summary>
    /// <param name="x">Key</param>
    /// <param name="y">Value</param>
    /// <remarks>
    /// If the point already exists it's value will be overriden.
    /// </remarks>
    public void AddPoint(double x, double y)
    {
        m_Points[x] = y;

        m_Rebuild = true;
    }

    /// <summary>
    /// Evaluates the spline function for the given argument.
    /// </summary>
    /// <param name="x">Key</param>
    /// <returns>Value for Key</returns>
    public double Eval(double x)
    {
        double y = 0;

        if (2 <= m_Points.Count)
        {
            if (m_Rebuild)
            {
                m_Rebuild = false;

                m_Points2 = new double[m_Points.Count];

                spline(m_Points.Keys, m_Points.Values, m_Points.Count, null, null, m_Points2);
            }

            splint(m_Points.Keys, m_Points.Values, m_Points2, m_Points.Count, x, out y);
        }
        else if (1 == m_Points.Count)
        {
            y = m_Points.Values[0];
        }

        return y;
    }

    /// <summary>
    /// Removes a point.
    /// </summary>
    /// <param name="x">Key of point to remove.</param>
    public void RemovePoint(double x)
    {
        m_Points.Remove(x);

        m_Rebuild = true;
    }

    /// <summary>
    /// Readonly access to the Keys of the Spline points,
    /// the Keys are sorted and will contain no dublicates.
    /// </summary>
    public IList<double> Keys {
        get {
            return m_PointsKeysRo;
        }
    }

    /// <summary>
    /// Readonly access to the Values of the Spline points.
    /// </summary>
    public IList<double> Values
    {
        get
        {
            return m_PointsValuesRo;
        }
    }


    //
    // Private members:

    SortedList<double, double> m_Points = new SortedList<double, double>();
    IList<double> m_Points2;
    IList<double> m_PointsKeysRo;
    IList<double> m_PointsValuesRo;
    bool m_Rebuild;

    // Copyright (c) by NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING (ISBN 0-521-43108-5)
    void spline(IList<double> x, IList<double> y, int n, double? yp1, double? ypn, IList<double> y2)
    {
        int i, k;
        double p, qn, sig, un;
        double[] u = new double[n - 1 - 1 + 1];

        if (!yp1.HasValue)
            y2[0] = u[0] = 0.0f;
        else
        {
            y2[0] = -0.5f;
            u[0] = (3.0f / (x[1] - x[0])) * ((y[1] - y[0]) / (x[1] - x[0]) - yp1.Value);
        }

        for (i = 1; i <= n - 2; i++)
        {
            sig = (x[i] - x[i - 1]) / (x[i + 1] - x[i - 1]);
            p = sig * y2[i - 1] + 2.0f;
            y2[i] = (sig - 1.0f) / p;
            u[i] = (y[i + 1] - y[i]) / (x[i + 1] - x[i]) - (y[i] - y[i - 1]) / (x[i] - x[i - 1]);
            u[i] = (6.0f * u[i] / (x[i + 1] - x[i - 1]) - sig * u[i - 1]) / p;
        }

        if (!ypn.HasValue)
            qn = un = 0.0f;
        else
        {
            qn = 0.5f;
            un = (3.0f / (x[n - 1] - x[n - 2])) * (ypn.Value - (y[n - 1] - y[n - 2]) / (x[n - 1] - x[n - 2]));
        }

        y2[n - 1] = (un - qn * u[n - 2]) / (qn * y2[n - 2] + 1.0f);

        for (k = n - 2; k >= 0; k--)
            y2[k] = y2[k] * y2[k + 1] + u[k];
    }

    // Copyright (c) by NUMERICAL RECIPES IN C: THE ART OF SCIENTIFIC COMPUTING (ISBN 0-521-43108-5)
    void splint(IList<double> xa, IList<double> ya, IList<double> y2a, int n, double x, out double y)
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
        if (h == 0.0) throw new System.ArgumentException("Bad xa input to routine splint");
        a = (xa[khi] - x) / h;
        b = (x - xa[klo]) / h;
        y = a * ya[klo] + b * ya[khi] + ((a * a * a - a) * y2a[klo] + (b * b * b - b) * y2a[khi]) * (h * h) / 6.0f;
    }
}

} // namespace AfxGui {