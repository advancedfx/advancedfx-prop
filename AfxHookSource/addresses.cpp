#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2014-10-21 by dominik.matrixstorm.com
//
// First changes:
// 2010-09-27 dominik.matrixstorm.com

#include "addresses.h"

#include <shared/binutils.h>

using namespace Afx::BinUtils;

AFXADDR_DEF(csgo_CUnknown_GetPlayerName)
AFXADDR_DEF(csgo_CUnknown_GetPlayerName_DSZ)
AFXADDR_DEF(csgo_CHudDeathNotice_FireGameEvent)
AFXADDR_DEF(csgo_CHudDeathNotice_FireGameEvent_DSZ)
AFXADDR_DEF(csgo_CViewRender_Render)
AFXADDR_DEF(csgo_CViewRender_Render_DSZ)
AFXADDR_DEF(csgo_gpGlobals_OFS_curtime)
AFXADDR_DEF(csgo_snd_mix_timescale_patch)
AFXADDR_DEF(csgo_snd_mix_timescale_patch_DSZ)
AFXADDR_DEF(cstrike_gpGlobals_OFS_curtime)

void ErrorBox(char const * messageText);

#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define MkErrStr(file,line) "Problem in " file ":" STRINGIZE(line)

void Addresses_InitEngineDll(AfxAddr engineDll, bool isCsgo)
{
	if(isCsgo)
	{
		// csgo_snd_mix_timescale_patch:
		{
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)engineDll);
				if(!sections.EOF())
				{
					sections.Next(); // skip .text
					if(!sections.EOF())
					{
						MemRange result = FindCString(sections.GetMemRange(), "Start profiling MIX_PaintChannels\n");
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
			if(strAddr)
			{
				ImageSectionsReader sections((HMODULE)engineDll);
			
				MemRange baseRange = sections.GetMemRange();
				MemRange result = FindBytes(baseRange, (char const *)&strAddr, sizeof(strAddr));
				if(!result.IsEmpty())
				{
					addr = result.Start +0xB9;

					addr = addr +4 + *(DWORD *)addr;
					// in MIX_PaintChannels now.

					addr = addr +0x237;

					addr = addr +4 + *(DWORD *)addr;
					// In SoundMixFunction2 now.

					addr = addr +0xED;

					addr = addr +4 + *(DWORD *)addr;
					// In SoundMixFunction now.

					addr = addr +0x66;

					// After call VEnglineClient013::GetTimeScale now.
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			if(addr)
			{
				AFXADDR_SET(csgo_snd_mix_timescale_patch, addr);
			}
			else
			{
				AFXADDR_SET(csgo_snd_mix_timescale_patch, 0x0);
			}
		}
	}
	else
	{
		AFXADDR_SET(csgo_snd_mix_timescale_patch, 0x0);
	}
	AFXADDR_SET(csgo_snd_mix_timescale_patch_DSZ, 0x09);
}

void Addresses_InitClientDll(AfxAddr clientDll, bool isCsgo)
{
	AFXADDR_SET(csgo_gpGlobals_OFS_curtime, 4*4);

	if(isCsgo)
	{
		// csgo_CUnknown_GetPlayerName:
		{
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)clientDll);
				if(!sections.EOF())
				{
					sections.Next(); // skip .text
					if(!sections.EOF())
					{
						MemRange result = FindCString(sections.GetMemRange(), "#SFUI_bot_controlled_by");
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
			if(strAddr)
			{
				ImageSectionsReader sections((HMODULE)clientDll);
			
				MemRange baseRange = sections.GetMemRange();
				MemRange result = FindBytes(baseRange, (char const *)&strAddr, sizeof(strAddr));
				if(!result.IsEmpty())
				{
					addr = result.Start -0x35F;

					// check for pattern to see if it is the right address:
					unsigned char pattern[3] = { 0x55, 0x8B, 0xEC };

					DWORD patternSize = sizeof(pattern)/sizeof(pattern[0]);
					MemRange patternRange(addr, addr+patternSize);
					MemRange result = FindBytes(patternRange, (char *)pattern, patternSize);
					if(result.Start != patternRange.Start || result.End != patternRange.End)
					{
						addr = 0;
						ErrorBox(MkErrStr(__FILE__,__LINE__));
					}
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			if(addr)
			{
				AFXADDR_SET(csgo_CUnknown_GetPlayerName, addr);
			}
			else
			{
				AFXADDR_SET(csgo_CUnknown_GetPlayerName, 0x0);
			}
		}


		// csgo_CHudDeathNotice_FireGameEvent:
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

		// csgo_CViewRender_Render:
		AFXADDR_SET(csgo_CViewRender_Render, 0x0);
		/*
		{
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)clientDll);
				if(!sections.EOF())
				{
					sections.Next(); // skip .text
					if(!sections.EOF())
					{
						MemRange result = FindCString(sections.GetMemRange(), "CViewRender::Render");
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
			if(strAddr)
			{
				ImageSectionsReader sections((HMODULE)clientDll);
			
				MemRange baseRange = sections.GetMemRange();
				MemRange result = FindBytes(baseRange, (char const *)&strAddr, sizeof(strAddr));
				if(!result.IsEmpty())
				{
					addr = result.Start -0x31;
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			if(addr)
			{
				AFXADDR_SET(csgo_CViewRender_Render, addr);
			}
			else
			{
				AFXADDR_SET(csgo_CViewRender_Render, 0x0);
			}
		}
		*/

	}
	else
	{
		AFXADDR_SET(csgo_CUnknown_GetPlayerName, 0x0);
		AFXADDR_SET(csgo_CHudDeathNotice_FireGameEvent, 0x0);
		AFXADDR_SET(csgo_CViewRender_Render, 0x0);
	}
	AFXADDR_SET(csgo_CUnknown_GetPlayerName_DSZ, 0x0b);
	AFXADDR_SET(csgo_CHudDeathNotice_FireGameEvent_DSZ, 0x0b);
	//AFXADDR_SET(csgo_CViewRender_Render_DSZ, 0x0c);
	AFXADDR_SET(cstrike_gpGlobals_OFS_curtime, 3*4);
}
