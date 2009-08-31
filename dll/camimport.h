#pragma once

#include <stdio.h>
#include <windows.h>

// temporary hack

class CCamImport
{
public:
	CCamImport();
	~CCamImport();

	bool IsActive();

	bool LoadMotionFile(char const * pszFileName);
	void CloseMotionFile();

	// if import is not active or fTimeOfs is out of range
	// then, outfPosition remains unchanged
	// outformat: see BvhChannel_t
	// return: if outCamdata was modified or not
	bool GetCamPositon(float fTimeOfs, float outCamdata[6]);

	float GetBaseTime();

	void SetBaseTime(float fBaseTime);

private:
	enum BvhChannel_t { BC_Xposition=0, BC_Yposition, BC_Zposition, BC_Zrotation, BC_Xrotation, BC_Yrotation };

	int channelcode[6];
	bool m_Active;
	float m_BaseTime;
	FILE * m_File;
	float m_Cache[6];
	int m_Frames;
	int m_LastFrame;
	float m_FrameTime;
	long m_MotionFPos;
	static char ms_readbuff[200];

	int DecodeBvhChannel(char * pszRemainder, char * & aoutNewRemainder);
};

extern CCamImport g_CamImport;