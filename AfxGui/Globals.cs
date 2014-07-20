// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-06-27 by dominik.matrixstorm.com
//
// First changes:
// 2008-10-18 by dominik.matrixstorm.com


namespace AfxGui {

class Globals
{
    internal static bool AutoStartAfxHookGoldSrc { get { return m_AutoStartAfxHookGoldSrc; } set { m_AutoStartAfxHookGoldSrc = value; } }
    internal static bool EnableHlaeRemote { get { return m_EnableHlaeRemote; } set { m_EnableHlaeRemote = value; } }

    static bool m_AutoStartAfxHookGoldSrc;
    static bool m_EnableHlaeRemote;
}

class GlobalConfig
{
    internal static Config Instance { get { return m_Instance; } set { m_Instance = value; } }
    static Config m_Instance;
}


class GlobalUpdateCheck
{
    internal static UpdateCheck Instance { get { return m_Instance; } set { m_Instance = value; } }
    static UpdateCheck m_Instance;
}

} //namespace AfxGui {
