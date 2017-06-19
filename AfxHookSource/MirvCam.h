#pragma once

#include "SourceInterfaces.h"

#include <string>

class MirvCam
{
public:
	enum Origin_e {
		O_Net,
		O_View
	};

	enum Angles_e {
		A_Net,
		A_View
	};

	void ApplySource(float & x, float & y, float & z, float & xRotation, float & yRotation, float & zRotation);

	void ApplyOffset(float & x, float & y, float & z, float & xRotation, float & yRotation, float & zRotation);

public:
	SOURCESDK::CSGO::CBaseHandle m_SourceHandle;
	Origin_e m_SourceOrigin = O_View;
	Angles_e m_SourceAngles = A_View;
	std::string m_SourceAttachment;
	bool m_SourceUseX = true;
	bool m_SourceUseY = true;
	bool m_SourceUseZ = true;
	bool m_SourceUseXRotation = true;
	bool m_SourceUseYRotation = true;
	bool m_SourceUseZRotation = true;
	float m_OffsetForwad = 0;
	float m_OffsetLeft = 0;
	float m_OffsetUp = 0;
	float m_OffsetForwardRot = 0;
	float m_OffseLeftRot = 0;
	float m_OffsetUpRot = 0;
};

extern MirvCam g_MirvCam;
