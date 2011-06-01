#include "stdafx.h"

#include "SCR_UpdateScreen.h"

#include <shared/detours.h>
#include <hl_addresses.h>

#include "../../filming.h"


// asm related definitons we will use:
#define asmNOP 0x90 // opcode for NOP
#define asmJMP	0xE9 // opcode for JUMP
#define JMP32_SZ 5	// the size of JMP <address>

LPVOID g_lpCode_TourIn_Continue = 0;
LPVOID g_lpCode_TourOut_Continue = 0;
LPVOID g_lpCode_HudTours_Loop = 0;

__declspec(naked) void tour_HudBegin()
{
	__asm
	{
		PUSH ECX ; used by c++ to get g_Filming addr
	}
	g_Filming.OnHudBeginEvent();
	__asm
	{
		POP ECX
		JMP [g_lpCode_TourIn_Continue]
	}
}

__declspec(naked) void tour_HudEnd()
{
	__asm
	{
		PUSH EAX ; c++ ret value
		PUSH ECX ; used by c++ to get g_Filming addr
	}
	if(g_Filming.OnHudEndEvnet())
	{
		__asm {
			POP ECX
			POP EAX
			JMP [g_lpCode_HudTours_Loop]
		}
	} else {
		__asm {
			POP ECX
			POP EAX
			JMP [g_lpCode_TourOut_Continue]
		}
	}
}

void Hook_SCR_UpdateScreen()
{
	static bool firstRun = true;
	if(!firstRun) return;
	firstRun = false;

	unsigned char ucTemp;
	DWORD dwTemp;

	//
	//	Get access to code where detours will be applied:
	//

	MdtMemBlockInfos mbisIn, mbisOut;
	LPVOID pCodeTourIn = (LPVOID)HL_ADDR_GET(HUD_TOURIN);
	LPVOID pCodeTourOut = (LPVOID)HL_ADDR_GET(HUD_TOUROUT);
	size_t dwCodeSizeIn = 0x05;
	size_t dwCodeSizeOut = 0x05;

	if(!pCodeTourIn || !pCodeTourOut) return;

	MdtMemAccessBegin( pCodeTourIn, dwCodeSizeIn, &mbisIn );
	MdtMemAccessBegin( pCodeTourOut, dwCodeSizeOut, &mbisOut );

	//

	g_lpCode_HudTours_Loop = pCodeTourIn; // FILL IN ADDRESS

	//
	//	detour In:
	//

	// create continue code:

	// get mem that is never freed:
	LPVOID pDetouredCodeIn = (LPVOID)MdtAllocExecuteableMemory(dwCodeSizeIn + JMP32_SZ);

	g_lpCode_TourIn_Continue = pDetouredCodeIn; // FILL IN ADDRESS

	// copy the original mov instruction:
	memcpy(pDetouredCodeIn,pCodeTourIn,dwCodeSizeIn);

	// create jump back to continue in original code:
	ucTemp = asmJMP;
	memcpy((unsigned char *)pDetouredCodeIn + dwCodeSizeIn,&ucTemp,1);
	dwTemp = (DWORD)pCodeTourIn - (DWORD)pDetouredCodeIn  - JMP32_SZ;
	memcpy((unsigned char *)pDetouredCodeIn + dwCodeSizeIn + 1,&dwTemp,4);

	// detour original code to jump to the tour_HudBegin func:
	ucTemp = asmJMP;
	memcpy(pCodeTourIn,&ucTemp,1);
	dwTemp=(DWORD)&tour_HudBegin - (DWORD)pCodeTourIn - JMP32_SZ;
	memcpy((unsigned char *)pCodeTourIn+1,&dwTemp,4);

	//
	//	detour Out:
	//

	// create continue code:

	// get mem that is never freed:
	LPVOID pDetouredCodeOut = (LPVOID)MdtAllocExecuteableMemory(dwCodeSizeOut + JMP32_SZ);

	g_lpCode_TourOut_Continue = pDetouredCodeOut; // FILL IN ADDRESS

	// copy the original function call:
	memcpy(pDetouredCodeOut,pCodeTourOut,dwCodeSizeOut);

	// patch the call address:
	memcpy(&dwTemp,(unsigned char*)pDetouredCodeOut+1,4),
	dwTemp -= (DWORD)pDetouredCodeOut - (DWORD)pCodeTourOut;
	memcpy((unsigned char*)pDetouredCodeOut+1,&dwTemp,4);

	// create jump back to continue in original code:
	ucTemp = asmJMP;
	memcpy((unsigned char *)pDetouredCodeOut + dwCodeSizeOut,&ucTemp,1);
	dwTemp = (DWORD)pCodeTourOut - (DWORD)pDetouredCodeOut  - JMP32_SZ;
	memcpy((unsigned char *)pDetouredCodeOut + dwCodeSizeOut + 1,&dwTemp,4);

	// detour original code to jump to the tour_HudBegin func:
	ucTemp = asmJMP;
	memcpy(pCodeTourOut,&ucTemp,1);
	dwTemp=(DWORD)&tour_HudEnd - (DWORD)pCodeTourOut - JMP32_SZ;
	memcpy((unsigned char *)pCodeTourOut+1,&dwTemp,4);

	//
	//	Restore code access:
	//

	MdtMemAccessEnd(&mbisOut);
	MdtMemAccessEnd(&mbisIn);
}
