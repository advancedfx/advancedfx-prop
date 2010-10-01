// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-09-30 by dominik.matrixstorm.com
//
// First changes:
// 2010-09-30 by dominik.matrixstorm.com

using System;
using System.Xml.Serialization;

namespace AfxData
{

[XmlRootAttribute("AfxProject")]
public class Project
{
    public Object Data;
    public Guid FactoryId;
}

}

namespace AfxData.GoldSrc
{

public class Launcher
{
    public String CustomCmdLine;
    public Boolean ForceAlpha;
    public Boolean FullScreen;
    public String GamePath;
    public Boolean GfxForce;
    public UInt16 GfxWidth;
    public UInt16 GfxHeight;
    public Byte GfxBpp;
    public String Modification;
    public Boolean OptimizeDesktopRes;
    public Boolean OptimizeVisibilty;
    public Boolean RememberChanges;
    public Byte RenderMode;
}

public class Project
{
    public Launcher Launcher;
}

}
