#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-10-21 by dominik.matrixstorm.com
//
// First changes:
// 2010-09-27 dominik.matrixstorm.com

#include "addresses.h"

#include "SourceInterfaces.h"
#include <shared/binutils.h>

using namespace Afx::BinUtils;

AFXADDR_DEF(csgo_CHudDeathNotice_FireGameEvent)
AFXADDR_DEF(csgo_CHudDeathNotice_FireGameEvent_DSZ)
AFXADDR_DEF(csgo_gpGlobals_OFS_curtime)
AFXADDR_DEF(cstrike_gpGlobals_OFS_curtime)
AFXADDR_DEF(cstrike_OFS_CvarFloatValue)

void ErrorBox(char const * messageText);

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define MkErrStr(file,line) "Problem in " file ":" STRINGIZE(line)

void Addresses_InitClientDll(AfxAddr clientDll, bool isCsgo)
{
	AFXADDR_SET(csgo_gpGlobals_OFS_curtime, 4*4);

	if(isCsgo)
	{
		DWORD vat = 0;
		DWORD strAddr = 0;
		{
			ImageSectionsReader sections((HMODULE)clientDll);
			if(!sections.EOF())
			{
				sections.Next(); // skip .text
				if(!sections.EOF())
				{
					//sections.Next(); // skip .idata
					if(!sections.EOF())
					{
						MemRange result = FindCString(sections.GetMemRange(), "SFHudDeathNoticeAndBotStatus");
						if(!result.IsEmpty())
						{
							strAddr = result.Start;
						}
						else ErrorBox(MkErrStr(__FILE__,__LINE__));
					}
					else ErrorBox(MkErrStr(__FILE__,__LINE__));
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			else ErrorBox(MkErrStr(__FILE__,__LINE__));
		}
		if(strAddr)
		{
			ImageSectionsReader sections((HMODULE)clientDll);
			
			MemRange baseRange = sections.GetMemRange();
			MemRange result = FindBytes(baseRange, (char const *)&strAddr, sizeof(strAddr));
			if(!result.IsEmpty())
			{
				result = FindBytes(MemRange(result.End, baseRange.End), (char const *)&strAddr, sizeof(strAddr));
				if(!result.IsEmpty())
				{
					vat = *(DWORD *)((char const *)result.Start +0x13);
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			else ErrorBox(MkErrStr(__FILE__,__LINE__));
		}
		if(vat)
		{
			DWORD adr = *((DWORD *)vat +1);
			AFXADDR_SET(csgo_CHudDeathNotice_FireGameEvent, adr);
		}
		else
		{
			AFXADDR_SET(csgo_CHudDeathNotice_FireGameEvent, 0x0);
		}
	}
	else
	{
		AFXADDR_SET(csgo_CHudDeathNotice_FireGameEvent, 0x0);
	}
	AFXADDR_SET(csgo_CHudDeathNotice_FireGameEvent_DSZ, 0x0b);

	AFXADDR_SET(cstrike_gpGlobals_OFS_curtime, 3*4);
	AFXADDR_SET(cstrike_OFS_CvarFloatValue, 0x2c);
}
