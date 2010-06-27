// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-06-27 by dominik.matrixstorm.com
//
// First changes:
// 2008-10-18 by dominik.matrixstorm.com


namespace AfxGui {


class GlobalConfig
{
    public static Config Instance { get { return m_Instance; } set { m_Instance = value; } }
    static Config m_Instance;
}


class GlobalUpdateCheck
{
    public static UpdateCheck Instance { get { return m_Instance; } set { m_Instance = value; } }
    static UpdateCheck m_Instance;
}


class GlobalAfxGoldSrc
{
    public static Afx.AfxGoldSrc GetInstanceOrCreate()
    {
        if (null == m_Instance) m_Instance = new Afx.AfxGoldSrc();
        return m_Instance;
    }

    public static Afx.AfxGoldSrc Instance { get { return m_Instance; } set { m_Instance = value; } }

    static Afx.AfxGoldSrc m_Instance;
}


} //namespace AfxGui {
