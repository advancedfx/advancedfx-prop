#pragma once

#include <stdio.h>
#include <windows.h>


/// <remarks> not thread safe, due to ms_readbuff </remarks>
class BvhImport
{
public:
	BvhImport();
	~BvhImport();

	bool IsActive();

	bool LoadMotionFile(char const * pszFileName);
	void CloseMotionFile();

	// outformat: see BvhChannel_t
	// return: true on success, false otherwise
	bool GetCamPositon(float fTimeOfs, float outCamdata[6]);

private:
	enum BvhChannel_t { BC_Xposition=0, BC_Yposition, BC_Zposition, BC_Zrotation, BC_Xrotation, BC_Yrotation };

	int channelcode[6];
	bool m_Active;
	FILE * m_File;
	float m_Cache[6];
	int m_Frames;
	int m_LastFrame;
	float m_FrameTime;
	long m_MotionFPos;
	static char ms_readbuff[200];

	int DecodeBvhChannel(char * pszRemainder, char * & aoutNewRemainder);
};
