#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-23 by dominik.matrixstorm.com
//
// First changes:
// 2010-03-23 by dominik.matrixstorm.com

#pragma unmanaged
#include <shared/com/PipeCom.h>
#pragma managed

#include "AfxGoldSrcComServer.h"


AfxGoldSrcComServer::AfxGoldSrcComServer()
{
	m_PipeComServer = new PipeComServer();
}

AfxGoldSrcComServer::~AfxGoldSrcComServer()
{
	m_ServerShutdown = true;
	if(m_ServerThread->IsAlive)
		m_ServerThread->Join();
	m_ServerThread = nullptr;

	delete m_PipeComServer;
	m_PipeComServer = 0;
}


System::IntPtr AfxGoldSrcComServer::ClientRecvPipeHandle::get()
{
	return IntPtr((void *)m_PipeComServer->GetClientReadPipe());
}


System::IntPtr AfxGoldSrcComServer::ClientSendPipeHandle::get()
{
	return IntPtr((void *)m_PipeComServer->GetClientWritePipe());
}


void AfxGoldSrcComServer::CLM_OnHostFrame()
{
	if(m_ServerShutdown)
		SendMessage(SVM_Close);
	SendMessage(SVM_EOT);
}


void AfxGoldSrcComServer::CLM_UpdateWindowSize()
{
	ComInt width, height;

	m_PipeComServer->ReadBytes(&width, sizeof(width));
	m_PipeComServer->WriteBytes(&height, sizeof(height));

	m_GameWindowParent->AutoScrollMinSize = System::Drawing::Size( width, height );
}


bool AfxGoldSrcComServer::InitServer()
{
	ComVersion version = COM_VERSION;
	ComBool ok = false;

	m_PipeComServer->WriteBytes(&version, sizeof(version));
	m_PipeComServer->ReadBytes(&ok, sizeof(ok));

	if(ok)
	{
		ComBool cb;
		ComHandle ch;
		ComRenderMode crm;

		cb = m_ForceAlpha8;
		m_PipeComServer->WriteBytes(&cb, sizeof(cb));
		cb = m_FullScreen;
		m_PipeComServer->WriteBytes(&cb, sizeof(cb));
		cb = m_OptimizeCaptureVis;
		m_PipeComServer->WriteBytes(&cb, sizeof(cb));
		ch = (ComHandle)m_ParentWindow;
		m_PipeComServer->WriteBytes(&ch, sizeof(ch));
		crm = m_RenderMode;
		m_PipeComServer->WriteBytes(&crm, sizeof(crm));
	}

	return ok;
}


ClientMessage AfxGoldSrcComServer::RecvMessage(void)
{
	ClientMessage clientMessage;

	m_PipeComServer->ReadBytes(&clientMessage, sizeof(clientMessage));

	return clientMessage;
}


void AfxGoldSrcComServer::SendMessage(ServerMessage message)
{
	m_PipeComServer->WriteBytes(&message, sizeof(message));
}

void AfxGoldSrcComServer::Start(
	bool fullScreen,
	bool forceAlpha8,
	System::Windows::Forms::Panel ^gameWindowParent,
	AfxGoldSrcRenderMode renderMode,
	bool optimizeCaptureVis)
{
	if(m_Started)
		throw "already started";

	m_ForceAlpha8 = forceAlpha8;
	m_FullScreen = fullScreen;
	m_GameWindowParent = gameWindowParent;
	m_OptimizeCaptureVis = optimizeCaptureVis;
	m_ParentWindow = (ComHandle)gameWindowParent->Handle.ToPointer();
	m_Started = true;

	switch(renderMode)
	{
	case AfxGoldSrcRenderMode::FrameBufferObject:
		m_RenderMode = RM_FrameBufferObject;
		break;
	case AfxGoldSrcRenderMode::MemoryDC:
		m_RenderMode = RM_MemoryDc;
		break;
	default:
		m_RenderMode = RM_Standard;
	}

	m_ServerShutdown = false;
	m_ServerThread = gcnew Thread(gcnew ThreadStart( this, &AfxGoldSrcComServer::ServerWorker ));

	if(InitServer())
		m_ServerThread->Start();
}




bool AfxGoldSrcComServer::ProcessMessages()
{
	ClientMessage clientMessage;

	while(CLM_Closed != (clientMessage = RecvMessage()))
	{
		switch(clientMessage)
		{
		case AfxGoldSrcCom::CLM_OnHostFrame:
			CLM_OnHostFrame();
			break;
		case AfxGoldSrcCom::CLM_UpdateWindowSize:
			CLM_UpdateWindowSize();
			break;
		}
	}

	return false;
}


void AfxGoldSrcComServer::ServerWorker(void)
{
	while(ProcessMessages());
}



