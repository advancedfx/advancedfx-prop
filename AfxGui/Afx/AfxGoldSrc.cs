// Copyright (c) by advancedfx.org
//
// Last changes:
// 2013-05-06 by dominik.matrixstorm.com
//
// First changes:
// 2009-12-06 by dominik.matrixstorm.com

using System;
using System.Threading;
using System.Windows.Forms;

namespace Afx {

class AfxGoldSrc : IDisposable
{
    //
    // Public members:

    public enum RenderMode
    {
        Default = 0,
        FrameBufferObject,
        MemoryDC
    }

    public class CameraSplines
    {
        public AfxGui.Spline X = new AfxGui.Spline();
        public AfxGui.Spline Y = new AfxGui.Spline();
        public AfxGui.Spline Z = new AfxGui.Spline();
        public AfxGui.Spline Pitch = new AfxGui.Spline();
        public AfxGui.Spline Yaw = new AfxGui.Spline();
        public AfxGui.Spline Roll = new AfxGui.Spline();

        public bool Active = false;
    }

    public class StartSettings
    {
        public bool Alpha8;
        public byte Bpp;
        public String CustomLaunchOptions;
        public bool ForceRes;
        public bool FullScreen;
        public ScrollableControl GameWindowParent;
        public String HalfLifePath;
        public uint Height;
        public String Modification;
        public bool OptWindowVisOnRec;
        public RenderMode RenderMode;
        public uint Width;

        public StartSettings(
            ScrollableControl gameWindowParent,
            String halfLifePath,
            String modification
        )
        {
            Alpha8 = false;
            Bpp = 32;
            CustomLaunchOptions = "";
            ForceRes = true;
            FullScreen = false;
            GameWindowParent = gameWindowParent;
            HalfLifePath = halfLifePath;
            Height = 480;
            Modification = modification;
            OptWindowVisOnRec = true;
            RenderMode = RenderMode.Default;
            Width = 640;
        }
    }

    public AfxGoldSrc()
    {
        m_DoUpdateWindowSize = new DoUpdateWindowSizeDelegate(DoUpdateWindowSize);

        m_PipeComServer = new PipeComServer();
    }

    public void Dispose()
    {
        if (m_Disposed) return;

        Stop();

        m_PipeComServer.Dispose();

        m_Disposed = true;
    }

    public bool Start(StartSettings startSettings)
    {
        Stop();

	    if(0 < System.Diagnostics.Process.GetProcessesByName( "hl" ).Length)
            return false;

        String cmds, s1;

	    //
	    //	build parameters:

	    cmds = "-steam -gl";
	
	    cmds += " -game " + startSettings.Modification;

	    // gfx settings

        cmds += startSettings.FullScreen ? " -full -stretchaspect" : " -window";

	    s1 = startSettings.Bpp.ToString();
	    if( 0 < s1.Length) cmds += " -" + "bpp";

	    s1 = startSettings.Width.ToString();
	    if( 0 < s1.Length) cmds += " -w " + s1;

	    s1 = startSettings.Height.ToString();
	    if( 0 < s1.Length) cmds += " -h " + s1;

	    if(startSettings.ForceRes) cmds += " -forceres";

	    // pipe handles:
	    cmds += " -afxpipes "
            + m_PipeComServer.ClientReadPipe.ToString()
            + ","
		    + m_PipeComServer.ClientWritePipe.ToString()
	    ;

	    // advanced

	    // custom command line

	    s1 = startSettings.CustomLaunchOptions;
	    if( 0 < s1.Length)
		    cmds += " " + s1;

	    //
	    // Launch:

        m_GameWindowParentHandle = startSettings.GameWindowParent.Handle;
        m_StartSettings = startSettings;

        StartServer();

        if (!AfxCppCli.AfxHook.LauchAndHook(
            startSettings.HalfLifePath,
            cmds,
            System.Windows.Forms.Application.StartupPath + "\\AfxHookGoldSrc.dll"
        ))
        {
            //TODO:
            // this won't actually work properly yet, the sever cannot stop properly
            // once it's started atm.
            StopServer();
            return false;
        }


        return true;
    }

    public void Stop()
    {
        StopServer();

        m_GameWindowParentHandle = IntPtr.Zero;
        m_StartSettings = null;
    }

    //
    // Public properties:

    //
    // Private members:

    enum ClientMessage
    {
	    EOT = 0,
	    Closed,
	    OnHostFrame,
	    OnRecordStarting,
	    OnRecordEnded,
	    UpdateWindowSize,
        CameraAdd,
        CameraRemove,
        CameraPrint,
        CameraClear,
        CameraActive,
        CameraGet
    };

    enum ServerMessage
    {
	    EOT = 0,
	    Close
    };

    enum ComRenderMode
    {
	    Standard = 0,
	    FrameBufferObject,
	    MemoryDc
    };

    delegate void DoUpdateWindowSizeDelegate(int width, int height);

    const int COM_VERSION = 0;

    CameraSplines m_CameraSplines = new CameraSplines();
    bool m_Disposed;
    IntPtr m_GameWindowParentHandle;
    PipeComServer m_PipeComServer;
    DoUpdateWindowSizeDelegate m_DoUpdateWindowSize;
    StartSettings m_StartSettings;
    bool m_ServerShutdown;
    Thread m_ServerThread;

    void ClientMessage_CameraAdd()
    {
        double time = m_PipeComServer.ReadDouble();

        m_CameraSplines.X.AddPoint(time, m_PipeComServer.ReadDouble());
        m_CameraSplines.Y.AddPoint(time, m_PipeComServer.ReadDouble());
        m_CameraSplines.Z.AddPoint(time, m_PipeComServer.ReadDouble());
        m_CameraSplines.Pitch.AddPoint(time, m_PipeComServer.ReadDouble());
        m_CameraSplines.Yaw.AddPoint(time, m_PipeComServer.ReadDouble());
        m_CameraSplines.Roll.AddPoint(time, m_PipeComServer.ReadDouble());
    }

    void ClientMessage_CameraRemove()
    {
        int index = m_PipeComServer.ReadInt32();

        if (0 <= index && index < m_CameraSplines.X.Keys.Count)
        {
            double x = m_CameraSplines.X.Keys[index];

            m_CameraSplines.X.RemovePoint(x);
            m_CameraSplines.Y.RemovePoint(x);
            m_CameraSplines.Z.RemovePoint(x);
            m_CameraSplines.Pitch.RemovePoint(x);
            m_CameraSplines.Yaw.RemovePoint(x);
            m_CameraSplines.Roll.RemovePoint(x);
        }
    }


    void ClientMessage_CameraPrint()
    {
        int count = m_CameraSplines.X.Keys.Count;

        m_PipeComServer.Write((Int32)count);

        for(int i=0;i<count;i++)
        {
            m_PipeComServer.Write((Double)m_CameraSplines.X.Keys[i]);
            m_PipeComServer.Write((Double)m_CameraSplines.X.Values[i]);
            m_PipeComServer.Write((Double)m_CameraSplines.Y.Values[i]);
            m_PipeComServer.Write((Double)m_CameraSplines.Z.Values[i]);
            m_PipeComServer.Write((Double)m_CameraSplines.Pitch.Values[i]);
            m_PipeComServer.Write((Double)m_CameraSplines.Yaw.Values[i]);
            m_PipeComServer.Write((Double)m_CameraSplines.Roll.Values[i]);
        }
    }

    void ClientMessage_CameraClear()
    {
        while (0 < m_CameraSplines.X.Keys.Count)
        {
            double x = m_CameraSplines.X.Keys[0];

            m_CameraSplines.X.RemovePoint(x);
            m_CameraSplines.Y.RemovePoint(x);
            m_CameraSplines.Z.RemovePoint(x);
            m_CameraSplines.Pitch.RemovePoint(x);
            m_CameraSplines.Yaw.RemovePoint(x);
            m_CameraSplines.Roll.RemovePoint(x);
        }
    }

    void ClientMessage_CameraActive()
    {
        m_CameraSplines.Active = m_PipeComServer.ReadBoolean();
    }

    void ClientMessage_CameraGet()
    {
        m_PipeComServer.Write((Boolean)m_CameraSplines.Active);
        if (m_CameraSplines.Active)
        {
            double x = m_PipeComServer.ReadDouble();

            m_PipeComServer.Write((Double)m_CameraSplines.X.Eval(x));
            m_PipeComServer.Write((Double)m_CameraSplines.Y.Eval(x));
            m_PipeComServer.Write((Double)m_CameraSplines.Z.Eval(x));
            m_PipeComServer.Write((Double)m_CameraSplines.Pitch.Eval(x));
            m_PipeComServer.Write((Double)m_CameraSplines.Yaw.Eval(x));
            m_PipeComServer.Write((Double)m_CameraSplines.Roll.Eval(x));
        }
    }

    void ClientMessage_OnHostFrame()
    {
	    if(m_ServerShutdown)
		    SendMessage(ServerMessage.Close);
        SendMessage(ServerMessage.EOT);
    }


    void ClientMessage_UpdateWindowSize()
    {
	    int width, height;

        width = m_PipeComServer.ReadInt32();
        height = m_PipeComServer.ReadInt32();

        m_StartSettings.GameWindowParent.Invoke(
            m_DoUpdateWindowSize,
            new object[] { width, height }
        );
    }

    IntPtr DoGetHandle(IWin32Window window)
    {
        return window.Handle;
    }

    void DoUpdateWindowSize(int width, int height)
    {
	    m_StartSettings.GameWindowParent.AutoScrollMinSize = new System.Drawing.Size(width, height);
    }

    ClientMessage RecvMessage()
    {
        return (ClientMessage)m_PipeComServer.ReadInt32();
    }

    void SendMessage(ServerMessage msg)
    {
        m_PipeComServer.Write((Int32)msg);
    }


    bool ServerInit()
    {
        m_PipeComServer.Write((Int32)COM_VERSION); // version

        if (!m_PipeComServer.ReadBoolean()) // ok?
            return false; // abort

        m_PipeComServer.Write((Int32)m_StartSettings.Width);
        m_PipeComServer.Write((Int32)m_StartSettings.Height);
        m_PipeComServer.Write((Boolean)m_StartSettings.Alpha8);
        m_PipeComServer.Write((Boolean)m_StartSettings.FullScreen);
        m_PipeComServer.Write((Boolean)m_StartSettings.OptWindowVisOnRec);
        m_PipeComServer.Write((Int32)m_GameWindowParentHandle.ToInt32());
        m_PipeComServer.Write((Int32)m_StartSettings.RenderMode);

	    return true;
    }


    void ServerWorker()
    {
        if(!ServerInit())
            return;

	    ClientMessage clientMessage;

	    while(ClientMessage.Closed != (clientMessage = RecvMessage()))
	    {
		    switch(clientMessage)
		    {
            case ClientMessage.OnHostFrame: 
			    ClientMessage_OnHostFrame();
			    break;
            case ClientMessage.UpdateWindowSize:
                ClientMessage_UpdateWindowSize();
			    break;
            case ClientMessage.CameraAdd:
                ClientMessage_CameraAdd();
                break;
            case ClientMessage.CameraRemove:
                ClientMessage_CameraRemove();
                break;
            case ClientMessage.CameraPrint:
                ClientMessage_CameraPrint();
                break;
            case ClientMessage.CameraClear:
                ClientMessage_CameraClear();
                break;
            case ClientMessage.CameraActive:
                ClientMessage_CameraActive();
                break;
            case ClientMessage.CameraGet:
                ClientMessage_CameraGet();
                break;
		    }
	    }
    }

    void StartServer()
    {
        StopServer();

        m_ServerThread = new Thread(new ThreadStart(ServerWorker));
        m_ServerThread.Start();
    }

    void StopServer()
    {
        if (null != m_ServerThread && m_ServerThread.IsAlive)
        {
            m_ServerShutdown = true;
            m_ServerThread.Join();
            m_ServerShutdown = false;
        }
        m_ServerThread = null;
    }
}

} // namespace Afx {

