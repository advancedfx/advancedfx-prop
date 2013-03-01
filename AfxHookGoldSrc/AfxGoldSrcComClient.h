#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-20 dominik.matrixstorm.com
//
// First changes
// 2010-03-20 dominik.matrixstorm.com

#include <shared/com/PipeCom.h>
#include <shared/com/AfxGoldSrcComDefs.h>

using namespace AfxGoldSrcCom;

class AfxGoldSrcComClient;

typedef void (* AfxGoldSrcComClientEvent)(AfxGoldSrcComClient * com);

class AfxGoldSrcComClient : public PipeCom
{
public:
	/// <summary> Creates and starts the com client. </summary>
	AfxGoldSrcComClient();

	~AfxGoldSrcComClient();

	/// <summary> Closes the communication client. </summary>
	void Close();

	/// <summary> full screen mode? </summary>
	bool GetFullScreen();
	
	/// <summary> force alpha bit planes request? </summary>
	bool GetForceAlpha8();

	int GetHeight();

	/// <summary> GameWindow parent (for docked mode) </summary>
	HWND GetParentWindow();

	int GetWidth();

	/// <summary> Desired render mode. </summary>
	ComRenderMode GetRenderMode();

	/// <summary> Whether to optimize window visibility for capturing (to 
	/// make as much pixels visible as possible to prevent undefined pixel
	/// content. </summary>
	bool GetOptimizeCaptureVis();

	void OnRecordEnded();

	void OnRecordStarting();

	void OnHostFrame();

	/// <summary> Update Parent Window with new GameWindow size. </summary>
	void UpdateWindowSize(ComInt x, ComInt y);

	//
	// Events:

private:
	bool m_Connected;
	ComBool m_ForceAlpha8;
	ComBool m_FullScreen;
	ComInt m_Height;
	ComBool m_OptimizeCaptureVis;
	ComHandle m_ParentWindow;
	ComRenderMode m_RenderMode;
	ComInt m_Width;

	ServerMessage RecvMessage(void);
	void SendMessage(ClientMessage message);

};

/// <summary> global static singleton instance. </summary>
extern AfxGoldSrcComClient g_AfxGoldSrcComClient;
