
#include "detours.h"

#define JMP32_SZ	5	// the size of JMP <address>
#define POPREG_SZ	1	// the size of a POP <reg>
#define NOP			0x90 // opcode for NOP
#define JMP			0xE9 // opcode for JUMP
#define POP_EAX		0x58
#define POP_ECX		0x59
#define PUSH_EAX	0x50
#define PUSH_ECX	0x51


void MdtMemAccessBegin(LPVOID lpAddress, size_t size, MdtMemBlockInfos *mdtMemBlockInfos)
{
	MEMORY_BASIC_INFORMATION mbi;
	MdtMemBlockInfo mbi2;
	DWORD dwDummy;

	LPVOID lpEnd = (BYTE *)lpAddress + size;

	while(lpAddress < lpEnd)
	{
		if(VirtualQuery(lpAddress, &mbi, sizeof(mbi)))
		{
			mbi2.BaseAddress = mbi.BaseAddress;
			mbi2.Protect = mbi.Protect;
			mbi2.RegionSize = mbi.RegionSize;

			mdtMemBlockInfos->push_back(mbi2);
		} else
			break;

		lpAddress = (BYTE *)mbi.BaseAddress + mbi.RegionSize;
	}

	for(int i=0; i < mdtMemBlockInfos->size(); i++) {
		mbi2 = mdtMemBlockInfos->at(i);
		VirtualProtect(mbi2.BaseAddress, mbi2.RegionSize, PAGE_READWRITE, &dwDummy);
	}
}

void MdtMemAccessEnd(MdtMemBlockInfos *mdtMemBlockInfos)
{
	MdtMemBlockInfo mbi2;
	DWORD dwDummy;

	for(int i=0; i < mdtMemBlockInfos->size(); i++) {
		mbi2 = mdtMemBlockInfos->at(i);
		VirtualProtect(mbi2.BaseAddress, mbi2.RegionSize, mbi2.Protect, &dwDummy);
	}
}

// Detour
void *DetourApply(BYTE *orig, BYTE *hook, int len)
{
	MdtMemBlockInfos mbis;
	BYTE *jmp = (BYTE*)malloc(len+JMP32_SZ);

	MdtMemAccessBegin(orig, len, &mbis);

	memcpy(jmp, orig, len);

	jmp += len; // increment to the end of the copied bytes
	jmp[0] = JMP;
	*(DWORD*)(jmp+1) = (DWORD)(orig+len - jmp) - JMP32_SZ;

	memset(orig, NOP, len);
	orig[0] = JMP;
	*(DWORD*)(orig+1) = (DWORD)(hook - orig) - JMP32_SZ;

	MdtMemAccessEnd(&mbis);

	return (jmp-len);
}

void *DetourClassFunc(BYTE *src, const BYTE *dst, const int len)
{
	BYTE *jmp = (BYTE*)malloc(len+JMP32_SZ+POPREG_SZ+POPREG_SZ+POPREG_SZ);
	MdtMemBlockInfos mbis;

	MdtMemAccessBegin(src, len, &mbis);

	memcpy(jmp+3, src, len);

	// calculate callback function call
	jmp[0] = POP_EAX;						// pop eax
	jmp[1] = POP_ECX;						// pop ecx
	jmp[2] = PUSH_EAX;						// push eax
	jmp[len+3] = JMP;						// jmp
	*(DWORD*)(jmp+len+4) = (DWORD)((src+len) - (jmp+len+3)) - JMP32_SZ;

	// detour source function call
	src[0] = POP_EAX;						// pop eax;
	src[1] = PUSH_ECX;						// push ecx
	src[2] = PUSH_EAX;						// push eax
	src[3] = JMP;							// jmp
	*(DWORD*)(src+4) = (DWORD)(dst - (src+3)) - JMP32_SZ;

	memset(src+8, NOP, len - 8);

	MdtMemAccessEnd(&mbis);

	return jmp;
}