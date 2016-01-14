#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2016-01-02 dominik.matrixstorm.com
//
// First changes:
// 2010-09-27 dominik.matrixstorm.com

#include "addresses.h"

#include <shared/binutils.h>

using namespace Afx::BinUtils;

AFXADDR_DEF(csgo_C_BasePlayer_OFS_m_skybox3d_scale)
AFXADDR_DEF(csgo_CUnknown_GetPlayerName)
AFXADDR_DEF(csgo_CUnknown_GetPlayerName_DSZ)
AFXADDR_DEF(csgo_CHudDeathNotice_FireGameEvent)
AFXADDR_DEF(csgo_CHudDeathNotice_FireGameEvent_DSZ)
AFXADDR_DEF(csgo_CHudDeathNotice_UnkAddDeathNotice)
AFXADDR_DEF(csgo_CHudDeathNotice_UnkAddDeathNotice_DSZ)
//AFXADDR_DEF(csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf)
//AFXADDR_DEF(csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf_DSZ)
AFXADDR_DEF(csgo_CSkyboxView_Draw)
AFXADDR_DEF(csgo_CSkyboxView_Draw_DSZ)
AFXADDR_DEF(csgo_CViewRender_Render)
AFXADDR_DEF(csgo_CViewRender_Render_DSZ)
AFXADDR_DEF(csgo_Spritecard_CShader_vtable)
AFXADDR_DEF(csgo_UnlitGeneric_CShader_vtable)
AFXADDR_DEF(csgo_VertexLitGeneric_CShader_vtable)
AFXADDR_DEF(csgo_S_StartSound_StringConversion)
AFXADDR_DEF(csgo_Scaleformui_CUnkown_Loader)
AFXADDR_DEF(csgo_Scaleformui_CUnkown_Loader_DSZ)
AFXADDR_DEF(csgo_pLocalPlayer)
AFXADDR_DEF(csgo_gpGlobals_OFS_curtime)
AFXADDR_DEF(csgo_gpGlobals_OFS_interpolation_amount)
AFXADDR_DEF(csgo_gpGlobals_OFS_interval_per_tick)
AFXADDR_DEF(csgo_snd_mix_timescale_patch)
AFXADDR_DEF(csgo_snd_mix_timescale_patch_DSZ)
AFXADDR_DEF(csgo_view)
AFXADDR_DEF(cstrike_gpGlobals_OFS_absoluteframetime)
AFXADDR_DEF(cstrike_gpGlobals_OFS_curtime)
AFXADDR_DEF(cstrike_gpGlobals_OFS_interpolation_amount)
AFXADDR_DEF(cstrike_gpGlobals_OFS_interval_per_tick)

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
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
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

		// csgo_S_StartSound_StringConversion:
		{
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)engineDll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
					{
						MemRange result = FindCString(sections.GetMemRange(), "Starting sound '%s' while system disabled.\n");
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
					addr = result.Start -0x1d;

					// check for pattern to see if it is the right address:
					unsigned char pattern[14] = { 0x8B, 0x01, 0x8D, 0x54, 0x24, 0x78, 0x68, 0x04, 0x01, 0x00, 0x00, 0x52, 0xFF, 0x10 };

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
				AFXADDR_SET(csgo_S_StartSound_StringConversion, addr);
			}
			else
			{
				AFXADDR_SET(csgo_S_StartSound_StringConversion, 0x0);
			}
		}
	}
	else
	{
		AFXADDR_SET(csgo_snd_mix_timescale_patch, 0x0);
		AFXADDR_SET(csgo_S_StartSound_StringConversion, 0x0);
	}
	AFXADDR_SET(csgo_snd_mix_timescale_patch_DSZ, 0x09);
}

void Addresses_InitScaleformuiDll(AfxAddr scaleformuiDll, bool isCsgo)
{
	if(isCsgo)
	{
		// csgo_Scaleformui_CUnkown_Loader:
		{
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)scaleformuiDll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
					{
						MemRange result = FindCString(sections.GetMemRange(), "Loader failed to open '%s', FileOpener not installe");
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
				ImageSectionsReader sections((HMODULE)scaleformuiDll);
			
				MemRange baseRange = sections.GetMemRange();
				MemRange result = FindBytes(baseRange, (char const *)&strAddr, sizeof(strAddr));
				if(!result.IsEmpty())
				{
					addr = result.Start -0x26;

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
				AFXADDR_SET(csgo_Scaleformui_CUnkown_Loader, addr);
			}
			else
			{
				AFXADDR_SET(csgo_Scaleformui_CUnkown_Loader, 0x0);
			}
		}
	}
	else
	{
		AFXADDR_SET(csgo_Scaleformui_CUnkown_Loader, 0x0);
	}

	AFXADDR_SET(csgo_Scaleformui_CUnkown_Loader_DSZ, 0x9);
}

void Addresses_InitClientDll(AfxAddr clientDll, bool isCsgo)
{
	if(isCsgo)
	{
		// csgo_CUnknown_GetPlayerName:
		{
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)clientDll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
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
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)clientDll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
					{
						MemRange result = FindCString(sections.GetMemRange(), "realtime_passthrough");
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
					addr = result.Start +0x0D;

					// check for pattern to see if it is the right address:
					unsigned char pattern[4] = { 0x56, 0x8B, 0xCF, 0xE8 };

					DWORD patternSize = sizeof(pattern)/sizeof(pattern[0]);
					MemRange patternRange(addr, addr+patternSize);
					MemRange result = FindBytes(patternRange, (char *)pattern, patternSize);
					if(result.Start != patternRange.Start || result.End != patternRange.End)
					{
						addr = 0;
						ErrorBox(MkErrStr(__FILE__,__LINE__));
					}
					else
					{
						addr += patternSize;
						addr = addr +4 + *(DWORD *)addr; // get CALL address
					}
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			AFXADDR_SET(csgo_CHudDeathNotice_FireGameEvent, addr);
		}

		// csgo_CHudDeathNotice_UnkAddDeathNotice:
		//
		// This function is called at the end of csgo_CHudDeathNotice_FireGameEvent,
		// however we search for the string near the call instead to be more stable
		// against updates.
		{
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)clientDll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
					{
						MemRange result = FindWCString(sections.GetMemRange(), L"%s%s%s%s%s%s%s%s");
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
					addr = (DWORD)((char const *)result.Start +0x2c);
					addr = addr +4 + *(DWORD *)addr;
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			AFXADDR_SET(csgo_CHudDeathNotice_UnkAddDeathNotice, addr);
		}

		// csgo_CViewRender_Render:
		AFXADDR_SET(csgo_CViewRender_Render, 0x0);
		/*
		{
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)clientDll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
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

		// csgo_pLocalPlayer:
		{
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)clientDll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
					{
						MemRange result = FindCString(sections.GetMemRange(), "time to initial render = %f\n");
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
					addr = result.Start;
					addr += 0xe;
					addr = *(DWORD *)addr;
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			if(addr)
			{
				AFXADDR_SET(csgo_pLocalPlayer, addr);
			}
			else
			{
				AFXADDR_SET(csgo_pLocalPlayer, 0x0);
			}
		}

		// csgo_CSkyboxView_Draw:
		{
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)clientDll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
					{
						MemRange result = FindCString(sections.GetMemRange(), "CViewRender::Draw3dSkyboxworld");
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
					addr = result.Start;
					addr -= 0x2a;
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			if(addr)
			{
				AFXADDR_SET(csgo_CSkyboxView_Draw, addr);
			}
			else
			{
				AFXADDR_SET(csgo_CSkyboxView_Draw, 0x0);
			}
		}

		// csgo_view
		{
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)clientDll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
					{
						MemRange result = FindCString(sections.GetMemRange(), "CViewRender::SetUpView->OnRenderEnd");
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
					addr = result.Start;
					addr += 0xb;
					addr = *(DWORD *)addr;
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			if(addr)
			{
				AFXADDR_SET(csgo_view, addr);
			}
			else
			{
				AFXADDR_SET(csgo_view, 0x0);
			}
		}

		/*
		// csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf
		{
			DWORD addr = 0;
			DWORD strAddr = 0;
			{
				ImageSectionsReader sections((HMODULE)clientDll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
					{
						MemRange result = FindCString(sections.GetMemRange(), "resource/flash/");
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
					addr = result.Start;
					addr -= 0x10;
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			if(addr)
			{
				AFXADDR_SET(csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf, addr);
			}
			else
			{
				AFXADDR_SET(csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf, 0x0);
			}
		}
		*/

		// csgo_C_BasePlayer_OFS_m_skybox3d_scale
		{
			// this basically uses the RecvProp* functions in c_baseplayer.cpp to get the offsets of the fields.

			DWORD strAddr_m_Local = 0;
			DWORD strAddr_m_skybox3d_scale = 0;
			DWORD ofs_m_Local_m_skybox3d_scale = 0;
			{
				ImageSectionsReader sections((HMODULE)clientDll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
					{
						{
							MemRange result = FindCString(sections.GetMemRange(), "m_Local");
							if(!result.IsEmpty())
							{
								strAddr_m_Local = result.Start;
							}
							else ErrorBox(MkErrStr(__FILE__,__LINE__));
						}
						{
							MemRange result = FindCString(sections.GetMemRange(), "m_skybox3d.scale");
							if(!result.IsEmpty())
							{
								strAddr_m_skybox3d_scale = result.Start;
							}
							else ErrorBox(MkErrStr(__FILE__,__LINE__));
						}
					}
					else ErrorBox(MkErrStr(__FILE__,__LINE__));
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}

			if(strAddr_m_Local && strAddr_m_skybox3d_scale)
			{
				bool has_m_Local = false;
				bool has_m_skybox3d_scale = false;

				DWORD ofs_m_Local = 0;
				DWORD ofs_m_skybox3d_scale = 0;
				DWORD addr = 0;

				{
					ImageSectionsReader sections((HMODULE)clientDll);
			
					MemRange baseRange = sections.GetMemRange();
					MemRange result = FindBytes(baseRange, (char const *)&strAddr_m_Local, sizeof(strAddr_m_Local));
					if(!result.IsEmpty())
					{
						addr = result.Start;
						addr += 4+2+4;
						addr = *(DWORD *)addr;
						ofs_m_Local = addr;
						has_m_Local = true;
					}
					else ErrorBox(MkErrStr(__FILE__,__LINE__));				
				}
				{
					ImageSectionsReader sections((HMODULE)clientDll);
			
					MemRange baseRange = sections.GetMemRange();
					MemRange result = FindBytes(baseRange, (char const *)&strAddr_m_skybox3d_scale, sizeof(strAddr_m_skybox3d_scale));
					if(!result.IsEmpty())
					{
						addr = result.Start;
						addr += 4+2+4;
						addr = *(DWORD *)addr;
						ofs_m_skybox3d_scale = addr;
						has_m_skybox3d_scale = true;
					}
					else ErrorBox(MkErrStr(__FILE__,__LINE__));				
				}

				if(has_m_Local && has_m_skybox3d_scale)
					ofs_m_Local_m_skybox3d_scale = ofs_m_Local + ofs_m_skybox3d_scale;
			}

			AFXADDR_SET(csgo_C_BasePlayer_OFS_m_skybox3d_scale, ofs_m_Local_m_skybox3d_scale);
		}
	}
	else
	{
		AFXADDR_SET(csgo_C_BasePlayer_OFS_m_skybox3d_scale, (AfxAddr)-1);
		AFXADDR_SET(csgo_CUnknown_GetPlayerName, 0x0);
		AFXADDR_SET(csgo_CHudDeathNotice_FireGameEvent, 0x0);
		AFXADDR_SET(csgo_CHudDeathNotice_UnkAddDeathNotice, 0x0);
		//AFXADDR_SET(csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf, 0x0);
		AFXADDR_SET(csgo_CSkyboxView_Draw, 0x0);
		AFXADDR_SET(csgo_CViewRender_Render, 0x0);
		AFXADDR_SET(csgo_pLocalPlayer, 0x0);
		AFXADDR_SET(csgo_view, 0x0);
	}

	AFXADDR_SET(csgo_CUnknown_GetPlayerName_DSZ, 0x0b);
	AFXADDR_SET(csgo_CHudDeathNotice_FireGameEvent_DSZ, 0x0b);
	AFXADDR_SET(csgo_CHudDeathNotice_UnkAddDeathNotice_DSZ, 0x09);
	//AFXADDR_SET(csgo_CScaleformSlotInitControllerClientImpl_UnkCheckSwf_DSZ,0x0c);
	AFXADDR_SET(csgo_CSkyboxView_Draw_DSZ, 0x0d);
	AFXADDR_SET(csgo_gpGlobals_OFS_curtime, 4*4);
	AFXADDR_SET(csgo_gpGlobals_OFS_interpolation_amount, 9*4);
	AFXADDR_SET(csgo_gpGlobals_OFS_interval_per_tick, 8*4);
	//AFXADDR_SET(csgo_CViewRender_Render_DSZ, 0x0c);
	AFXADDR_SET(cstrike_gpGlobals_OFS_curtime, 3*4);
	AFXADDR_SET(cstrike_gpGlobals_OFS_absoluteframetime, 2*4);
	AFXADDR_SET(cstrike_gpGlobals_OFS_interpolation_amount, 8*4);
	AFXADDR_SET(cstrike_gpGlobals_OFS_interval_per_tick, 7*4);
}

void Addresses_InitStdshader_dx9Dll(AfxAddr stdshader_dx9Dll, bool isCsgo)
{
	if(isCsgo)
	{
		// csgo_Spritecard_CShader_vtable:
		{
			DWORD addr = 0;
			{
				ImageSectionsReader sections((HMODULE)stdshader_dx9Dll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
					{
						MemRange firstDataRange = sections.GetMemRange();

						sections.Next(); // skip first .data
						if(!sections.Eof())
						{
							MemRange result = FindCString(sections.GetMemRange(), ".?AVCShader@Spritecard@@");
							if(!result.IsEmpty())
							{
								DWORD tmpAddr = result.Start;
								tmpAddr -= 0x8;

								result = FindBytes(firstDataRange, (char const *)&tmpAddr, sizeof(tmpAddr));
								if(!result.IsEmpty())
								{
									DWORD tmpAddr = result.Start;
									tmpAddr -= 0xC;

									result = FindBytes(firstDataRange, (char const *)&tmpAddr, sizeof(tmpAddr));
									if(!result.IsEmpty())
									{
										DWORD tmpAddr = result.Start;
										tmpAddr += (1)*4;

										addr = tmpAddr;
									}
									else ErrorBox(MkErrStr(__FILE__,__LINE__));
								}
								else ErrorBox(MkErrStr(__FILE__,__LINE__));
							}
							else ErrorBox(MkErrStr(__FILE__,__LINE__));
						}
						else ErrorBox(MkErrStr(__FILE__,__LINE__));
					}
					else ErrorBox(MkErrStr(__FILE__,__LINE__));
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			AFXADDR_SET(csgo_Spritecard_CShader_vtable, addr);
		}

		// csgo_UnlitGeneric_CShader_vtable:
		{
			DWORD addr = 0;
			{
				ImageSectionsReader sections((HMODULE)stdshader_dx9Dll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
					{
						MemRange firstDataRange = sections.GetMemRange();

						sections.Next(); // skip first .data
						if(!sections.Eof())
						{
							MemRange result = FindCString(sections.GetMemRange(), ".?AVCShader@UnlitGeneric@@");
							if(!result.IsEmpty())
							{
								DWORD tmpAddr = result.Start;
								tmpAddr -= 0x8;

								result = FindBytes(firstDataRange, (char const *)&tmpAddr, sizeof(tmpAddr));
								if(!result.IsEmpty())
								{
									DWORD tmpAddr = result.Start;
									tmpAddr -= 0xC;

									result = FindBytes(firstDataRange, (char const *)&tmpAddr, sizeof(tmpAddr));
									if(!result.IsEmpty())
									{
										DWORD tmpAddr = result.Start;
										tmpAddr += (1)*4;

										addr = tmpAddr;
									}
									else ErrorBox(MkErrStr(__FILE__,__LINE__));
								}
								else ErrorBox(MkErrStr(__FILE__,__LINE__));
							}
							else ErrorBox(MkErrStr(__FILE__,__LINE__));
						}
						else ErrorBox(MkErrStr(__FILE__,__LINE__));
					}
					else ErrorBox(MkErrStr(__FILE__,__LINE__));
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			AFXADDR_SET(csgo_UnlitGeneric_CShader_vtable, addr);
		}

		// csgo_VertexLitGeneric_CShader_vtable:
		{
			DWORD addr = 0;
			{
				ImageSectionsReader sections((HMODULE)stdshader_dx9Dll);
				if(!sections.Eof())
				{
					sections.Next(); // skip .text
					if(!sections.Eof())
					{
						MemRange firstDataRange = sections.GetMemRange();

						sections.Next(); // skip first .data
						if(!sections.Eof())
						{
							MemRange result = FindCString(sections.GetMemRange(), ".?AVCShader@VertexLitGeneric@@");
							if(!result.IsEmpty())
							{
								DWORD tmpAddr = result.Start;
								tmpAddr -= 0x8;

								result = FindBytes(firstDataRange, (char const *)&tmpAddr, sizeof(tmpAddr));
								if(!result.IsEmpty())
								{
									result = FindBytes(MemRange(result.End,firstDataRange.End), (char const *)&tmpAddr, sizeof(tmpAddr));
									if(!result.IsEmpty())
									{
										DWORD tmpAddr = result.Start;
										tmpAddr -= 0xC;

										result = FindBytes(firstDataRange, (char const *)&tmpAddr, sizeof(tmpAddr));
										if(!result.IsEmpty())
										{
											DWORD tmpAddr = result.Start;
											tmpAddr += (1)*4;

											addr = tmpAddr;
										}
										else ErrorBox(MkErrStr(__FILE__,__LINE__));
									}
									else ErrorBox(MkErrStr(__FILE__,__LINE__));
								}
								else ErrorBox(MkErrStr(__FILE__,__LINE__));
							}
							else ErrorBox(MkErrStr(__FILE__,__LINE__));
						}
						else ErrorBox(MkErrStr(__FILE__,__LINE__));
					}
					else ErrorBox(MkErrStr(__FILE__,__LINE__));
				}
				else ErrorBox(MkErrStr(__FILE__,__LINE__));
			}
			AFXADDR_SET(csgo_VertexLitGeneric_CShader_vtable, addr);
		}
	}
	else
	{
		AFXADDR_SET(csgo_Spritecard_CShader_vtable, 0x0);
		AFXADDR_SET(csgo_UnlitGeneric_CShader_vtable, 0x0);
		AFXADDR_SET(csgo_VertexLitGeneric_CShader_vtable, 0x0);
	}
}
