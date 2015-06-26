#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-06-26 dominik.matrixstorm.com
//
// First changes:
// 2015-06-26 dominik.matrixstorm.com

#include "SourceInterfaces.h"
#include "AfxInterfaces.h"

#include <string>

class CAfxStreams
{
public:
	CAfxStreams();
	~CAfxStreams();

	void OnMaterialSystem(IMaterialSystem_csgo * value);

	void OnAfxVRenderView(IAfxVRenderView * value);

	void OnAfxBaseClientDll(IAfxBaseClientDll * value);

	void Console_RecordName_set(const char * value);
	const char * Console_RecordName_get();
	void Console_Record_Start();
	void Console_Record_End();
	void Console_AddMatteWorldStream(const char * streamName);
	void Console_AddMatteEntityStream(const char * streamName);
	void Console_PrintStreams();
	void Console_RemoveStream(int index);

private:
	class CFreeDelegate : public IAfxFreeable
	{
	public:
		CFreeDelegate(IAfxFreeMaster * freeMaster, CAfxStreams * classPtr, void (CAfxStreams::*classFn)(void))
		: m_FreeMaster(freeMaster)
		, m_ClassPtr(classPtr)
		, m_ClassFn(classFn)
		{
			m_FreeMaster->AfxFreeable_Register(this);
		}

		~CFreeDelegate()
		{
			AfxFree();
		}

		virtual void AfxFree(void)
		{
			if(m_ClassPtr) { (m_ClassPtr->*m_ClassFn)(); m_ClassPtr = 0; }
			if(m_FreeMaster) { m_FreeMaster->AfxFreeable_Unregister(this); m_FreeMaster = 0; }
		}

	private:
		IAfxFreeMaster * m_FreeMaster;
		CAfxStreams * m_ClassPtr;
		void (CAfxStreams::*m_ClassFn)(void);

	};

	std::string m_RecordName;
	CFreeDelegate * m_OnAfxBaseClientDll_Free;

	void OnAfxBaseClientDll_Free(void);
};

extern CAfxStreams g_AfxStreams;
