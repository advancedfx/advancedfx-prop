#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-23 by dominik.matrixstorm.com
//
// First changes:
// 2010-03-23 by dominik.matrixstorm.com

#include <shared/com/AfxGoldSrcComDefs.h>

using namespace System;
using namespace System::Threading;

using namespace AfxGoldSrcCom;

class PipeComServer;

enum class AfxGoldSrcRenderMode {
	Default,
	FrameBufferObject,
	MemoryDC
};

ref class AfxGoldSrcComServer
{
public:
	AfxGoldSrcComServer();
	
	void Start(
		bool fullScreen,
		bool forceAlpha8,
		System::Windows::Forms::Panel ^gameWindowParent,
		AfxGoldSrcRenderMode renderMode,
		bool optimizeCaptureVis
	);

	property IntPtr ClientRecvPipeHandle { IntPtr get(); }
	property IntPtr ClientSendPipeHandle { IntPtr get(); }


protected:
	~AfxGoldSrcComServer();

private:
	delegate void WindowSizeDelegate(int width, int height);

	ComBool m_ForceAlpha8;
	ComBool m_FullScreen;
	System::Windows::Forms::Panel ^ m_GameWindowParent;
	ComBool m_OptimizeCaptureVis;
	ComHandle m_ParentWindow;
	PipeComServer * m_PipeComServer;
	ComRenderMode m_RenderMode;
	bool m_ServerShutdown;
	Thread^ m_ServerThread;
	bool m_Started;

	void CLM_OnHostFrame();
	void CLM_UpdateWindowSize();

	void DoUpdateWindowSize(int width, int height);

	bool InitServer();

	bool ProcessMessages();

	ClientMessage RecvMessage(void);
	void SendMessage(ServerMessage message);

	/// <summary> m_ServerThread procedure </summary>
	void ServerWorker(void);
};
