#include "stdafx.h"

#include "Host_Frame.h"

#include <hl_addresses.h>

#include "../../AfxGoldSrcComClient.h"

#include <hooks/shared/detours.h>

typedef void (*Host_Frame_t) (float time);
Host_Frame_t g_Old_HostFrame = 0;

void New_Host_Frame (float time)
{
	g_AfxGoldSrcComClient.OnHostFrame();

	g_Old_HostFrame(time);
}

void Hook_Host_Frame()
{
	if( !g_Old_HostFrame && 0 != HL_ADDR_GET(Host_Frame) )
	{
		g_Old_HostFrame = (Host_Frame_t) DetourApply((BYTE *)HL_ADDR_GET(Host_Frame), (BYTE *)New_Host_Frame, (int)HL_ADDR_GET(Host_Frame_DSZ));
	}
}
