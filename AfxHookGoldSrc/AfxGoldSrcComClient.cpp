#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-20 dominik.matrixstorm.com
//
// First changes
// 2010-03-20 dominik.matrixstorm.com

#include "AfxGoldSrcComClient.h"

#include "hooks/user32Hooks.h"


AfxGoldSrcComClient g_AfxGoldSrcComClient;

HANDLE g_ReadPipe = NULL;
HANDLE g_WritePipe = NULL;

void GetAfxPipes()
{
	static bool firstRun = true;
	if(!firstRun)
		return;
	firstRun = false;

	char tmp[33];
	tmp[32] = 0;

	LPSTR cmdLine = GetCommandLine();

	char const * strRead = strstr(cmdLine, "-afxpipes ");
	if(!strRead)
		return;
	strRead += strlen("-afxpipes ");

	char const * strWrite = strstr(strRead, ",");
	if(!strWrite)
		return;
	strWrite += strlen(",");

	char const * strEnd = strstr(strWrite, " ");
	if(!strEnd)
		strEnd = cmdLine +strlen(cmdLine);

	memset(tmp, 0, 33);
	strncpy(tmp, strRead, min(32, strWrite -strRead -1));
	g_ReadPipe = (HANDLE)atoi(tmp);

	memset(tmp, 0, 33);
	strncpy(tmp, strWrite, min(32, strEnd -strWrite));
	g_WritePipe = (HANDLE)atoi(tmp);
}

HANDLE GetClientReadPipe()
{
	GetAfxPipes();

	return g_ReadPipe;
}

HANDLE GetClientWritePipe()
{
	GetAfxPipes();

	return g_WritePipe;
}


// AfxGoldSrcComClient /////////////////////////////////////////////////////////

AfxGoldSrcComClient::AfxGoldSrcComClient()
: PipeCom(GetClientReadPipe(), GetClientWritePipe())
, m_Connected(false)
{
	ComVersion version;
	ComBool ok = 0;

	m_Connected = false;

	this->ReadBytes(&version, sizeof(version));

	if(COM_VERSION != version)
	{
		this->WriteBytes(&ok, sizeof(ok));
		throw "AfxGoldSrcComVersion mismatch";
		return;
	}

	ok = 1;
	this->WriteBytes(&ok, sizeof(ok));

	this->ReadBytes(&m_ForceAlpha8, sizeof(m_ForceAlpha8));
	this->ReadBytes(&m_FullScreen, sizeof(m_FullScreen));
	this->ReadBytes(&m_OptimizeCaptureVis, sizeof(m_OptimizeCaptureVis));
	this->ReadBytes(&m_ParentWindow, sizeof(m_ParentWindow));
	this->ReadBytes(&m_RenderMode, sizeof(m_RenderMode));

	m_Connected = ok;
}

AfxGoldSrcComClient::~AfxGoldSrcComClient()
{
	Close();
}


void AfxGoldSrcComClient::Close()
{
	if(!m_Connected)
		return;

	SendMessage(CLM_Closed);
}

bool AfxGoldSrcComClient::GetFullScreen()
{
	return m_FullScreen;
}


bool AfxGoldSrcComClient::GetForceAlpha8()
{
	return m_ForceAlpha8;
}


HWND AfxGoldSrcComClient::GetParentWindow()
{
	return (HWND)m_ParentWindow;
}


ComRenderMode AfxGoldSrcComClient::GetRenderMode()
{
	return m_RenderMode;
}

bool AfxGoldSrcComClient::GetOptimizeCaptureVis()
{
	return m_OptimizeCaptureVis;
}

ServerMessage AfxGoldSrcComClient::RecvMessage(void)
{
	ComInt msg;

	ReadBytes(&msg, sizeof(msg));

	return (ServerMessage)msg;
}

void AfxGoldSrcComClient::SendMessage(ClientMessage message)
{
	ComInt msg = (ComInt)message;
	WriteBytes(&msg, sizeof(msg));
}


void AfxGoldSrcComClient::OnRecordStarting()
{
	SendMessage(CLM_OnRecordStarting);
}

void AfxGoldSrcComClient::OnRecordEnded()
{
	SendMessage(CLM_OnRecordEnded);
}

void AfxGoldSrcComClient::OnHostFrame()
{
	SendMessage(CLM_OnHostFrame);

	//
	// Pull messages from server:

	ServerMessage serverMessage;

	while(SVM_EOT != (serverMessage = RecvMessage()))
	{
		switch(serverMessage)
		{
		case SVM_Close:
			CloseGameWindow();
			break;
		}
	}
}


void AfxGoldSrcComClient::UpdateWindowSize(ComInt x, ComInt y)
{
	SendMessage(CLM_UpdateWindowSize);

	WriteBytes(&x, sizeof(x));
	WriteBytes(&y, sizeof(y));
}