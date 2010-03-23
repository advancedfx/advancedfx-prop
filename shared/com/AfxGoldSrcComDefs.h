#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-21 dominik.matrixstorm.com
//
// First changes
// 2010-03-21 dominik.matrixstorm.com

namespace AfxGoldSrcCom
{

#define COM_VERSION 0

enum ClientMessage
{
	CLM_EOT = 0,
	CLM_Closed,
	CLM_OnHostFrame,
	CLM_OnRecordStarting,
	CLM_OnRecordEnded,
	CLM_UpdateWindowSize
};

enum ServerMessage
{
	SVM_EOT = 0,
	SVM_Close
};

typedef unsigned __int32 ComVersion;

enum ComRenderMode
{
	RM_Standard = 0,
	RM_FrameBufferObject,
	RM_MemoryDc
};

typedef bool ComBool;
typedef __int32 ComHandle;
typedef __int32 ComInt;

} // namespace AfxGoldSrcCom {