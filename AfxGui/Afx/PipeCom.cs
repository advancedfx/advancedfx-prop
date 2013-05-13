// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-06-26 by dominik.matrixstorm.com
//
// First changes:
// 2010-01-12 by dominik.matrixstorm.com

// Description:
//
// Classes providing low-level (inter-process)
// in-order low-traffic client-to-server Win32-API-based
// communication.


using System;
using System.Runtime.InteropServices;

namespace Afx {

class PipeCom
{
    //
    // Public members:

    public PipeCom(IntPtr readPipe, IntPtr writePipe)
    {
        m_ReadPipe = readPipe;
        m_WritePipe = writePipe;
    }

    /// <summary>
    /// Checks for incoming bytes.
    /// </summary>
    /// <returns>Returns true if one or more bytes are waiting to be read.</returns>
    public bool Incoming()
    {
        UInt32 bytesAvail = 0;
        if (!PeekNamedPipe(m_ReadPipe, IntPtr.Zero, 0, IntPtr.Zero, out bytesAvail, IntPtr.Zero))
            throw new System.ApplicationException();

        return 0 < bytesAvail;
    }

    public void Read(Byte[] outBuffer)
    {
        Read(outBuffer, 0, outBuffer.Length);
    }

    public void Read(Byte[] outBuffer, int index, int count)
    {
        UInt32 bytesRead;

        while (0 < count)
        {
            ArrayWithOffset arr = new ArrayWithOffset(outBuffer, index);

            if (!ReadFile(m_ReadPipe, arr, (UInt32)count, out bytesRead, IntPtr.Zero))
                throw new System.ApplicationException();

            index += (int)bytesRead;
            count -= (int)bytesRead;
        }
    }

    public Boolean ReadBoolean()
    {
        return 0 != ReadByte();
    }

    public Byte ReadByte()
    {
        Byte[] bytes = new Byte[1];

        Read(bytes);

        return bytes[0];
    }

    public Double ReadDouble()
    {
        Byte[] bytes = new Byte[8];

        Read(bytes);

        return BitConverter.ToDouble(bytes, 0);
    }

    public Int32 ReadInt32()
    {
        Byte[] bytes = new Byte[4];

        Read(bytes);

        return BitConverter.ToInt32(bytes, 0);
    }

    public UInt32 ReadUInt32()
    {
        Byte[] bytes = new Byte[4];

        Read(bytes);

        return BitConverter.ToUInt32(bytes, 0);
    }

    public void Write(Byte[] buffer)
    {
        Write(buffer, 0, buffer.Length);
    }

    public void Write(Byte[] buffer, int index, int count)
    {
	    UInt32 bytesWritten = 0;

	    while(0 < count)
	    {
            ArrayWithOffset arr = new ArrayWithOffset(buffer, index);

		    if(!WriteFile(m_WritePipe, arr, (uint)count, out bytesWritten, IntPtr.Zero))
                throw new System.ApplicationException();

            index += (int)bytesWritten;
            count -= (int)bytesWritten;
	    }
    }

    public void Write(Boolean value)
    {
        Write(new Byte[] { (Byte)(value ? 1 : 0) }, 0, 1);
    }

    public void Write(Byte value)
    {
        Write(new Byte[]{value}, 0, 1);
    }

    public void Write(Double value)
    {
        Write(BitConverter.GetBytes(value));
    }

    public void Write(Int32 value)
    {
        Write(BitConverter.GetBytes(value));
    }

    public void Write(UInt32 value)
    {
        Write(BitConverter.GetBytes(value));
    }

    //
    // Protected members:

    protected PipeCom()
    {
    }

    protected IntPtr ReadPipe { get { return m_ReadPipe; } set { m_ReadPipe = value; } }
    protected IntPtr WritePipe { get { return m_WritePipe; } set { m_WritePipe = value; } }

    //
    // Private members:

    [DllImport("Kernel32.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    static extern Boolean PeekNamedPipe(
        IntPtr hNamedPipe,
        IntPtr lpBuffer,
        UInt32 nBufferSize,
        IntPtr lpBytesRead,
        out UInt32 lpTotalBytesAvail,
        IntPtr lpBytesLeftThisMessage
    );

    [DllImport("Kernel32.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    static extern Boolean ReadFile(
        IntPtr hFile,
        [In, Out] ArrayWithOffset lpBuffer,
        UInt32 nNumberOfBytesToRead,
        out UInt32 lpNumberOfBytesRead,
        IntPtr lpOverlapped
    );

    [DllImport("Kernel32.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    static extern Boolean WriteFile(
        IntPtr hFile,
        [In, Out] ArrayWithOffset lpBuffer,
        UInt32 nNumberOfBytesToWrite,
        out UInt32 lpNumberOfBytesWritten,
        IntPtr lpOverlapped
    );

    IntPtr m_ReadPipe;
    IntPtr m_WritePipe;
}

class PipeComServer : PipeCom, IDisposable
{
    //
    // Public members:

    public PipeComServer()
    {
        CreatePipes();
    }

    public void Dispose()
    {
        if (m_Disposed) return;

        FreePipes();

        m_Disposed = true;

        GC.SuppressFinalize(this);
    }

    public IntPtr ClientReadPipe
    {
        get
        {
            return m_ClientReadPipe;
        }
    }

    public IntPtr ClientWritePipe
    {
        get
        {
            return m_ClientWritePipe;
        }
    }

    //
    // Private members:

    [StructLayout(LayoutKind.Sequential)]
    struct SECURITY_ATTRIBUTES
    {
        public UInt32 nLength;
        public IntPtr lpSecurityDescriptor;
        [MarshalAs(UnmanagedType.Bool)]
        public Boolean bInheritHandle;
    }

    [DllImport("Kernel32.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    static extern Boolean CloseHandle(
        IntPtr hObject
    );

    [DllImport("Kernel32.dll")]
    [return: MarshalAs(UnmanagedType.Bool)]
    static extern Boolean CreatePipe(
        out IntPtr hReadPipe,
        out IntPtr hWritePipe,
        ref SECURITY_ATTRIBUTES lpPipeAttributes,
        UInt32 nSize
    );

    IntPtr m_ClientReadPipe;
    IntPtr m_ClientWritePipe;
    bool m_Disposed;

    ~PipeComServer()
    {
        Dispose();
    }

    void CreatePipes()
    {
        IntPtr serverReadPipe;
        IntPtr serverWritePipe;
        SECURITY_ATTRIBUTES secAttrib = new SECURITY_ATTRIBUTES();
        secAttrib.nLength = (UInt32)Marshal.SizeOf(secAttrib);
        secAttrib.lpSecurityDescriptor = IntPtr.Zero;
        secAttrib.bInheritHandle = true;

        if (!CreatePipe(out m_ClientReadPipe, out serverWritePipe, ref secAttrib, 0))
            throw new ApplicationException();

        if (!CreatePipe(out serverReadPipe, out m_ClientWritePipe, ref secAttrib, 0))
            throw new ApplicationException();

        this.ReadPipe = serverReadPipe;
        this.WritePipe = serverWritePipe;
    }

    void FreePipes()
    {
        CloseHandle(m_ClientReadPipe); m_ClientReadPipe = IntPtr.Zero;
        CloseHandle(m_ClientWritePipe); m_ClientWritePipe = IntPtr.Zero;
        CloseHandle(this.ReadPipe); this.ReadPipe = IntPtr.Zero;
        CloseHandle(this.WritePipe); this.WritePipe = IntPtr.Zero;
    }
}

} // namespace Afx {