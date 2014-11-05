#pragma once

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2014-02-12 dominik.matrixstorm.com
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

enum ComRenderMode
{
	RM_Standard = 0,
	RM_FrameBufferObject,
	RM_MemoryDc
};


} // namespace AfxGoldSrcCom {