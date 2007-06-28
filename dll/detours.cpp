
#include <windows.h>
#include "detours.h"

#define JMP32_SZ	5	// the size of JMP <address>
#define POPREG_SZ	1	// the size of a POP <reg>
#define NOP			0x90 // opcode for NOP
#define JMP			0xE9 // opcode for JUMP
#define POP_EAX		0x58
#define POP_ECX		0x59
#define PUSH_EAX	0x50
#define PUSH_ECX	0x51

// Detour
void *DetourApply(BYTE *orig, BYTE *hook, int len)
{
	DWORD dwProt = 0;
	BYTE *jmp = (BYTE*)malloc(len+JMP32_SZ);
	VirtualProtect(orig, len, PAGE_READWRITE, &dwProt);
	memcpy(jmp, orig, len);

	jmp += len; // increment to the end of the copied bytes
	jmp[0] = JMP;
	*(DWORD*)(jmp+1) = (DWORD)(orig+len - jmp) - JMP32_SZ;

	memset(orig, NOP, len);
	orig[0] = JMP;
	*(DWORD*)(orig+1) = (DWORD)(hook - orig) - JMP32_SZ;

	VirtualProtect(orig, len, dwProt, 0);
	return (jmp-len);
}

void *DetourClassFunc(BYTE *src, const BYTE *dst, const int len)
{
	BYTE *jmp = (BYTE*)malloc(len+JMP32_SZ+POPREG_SZ+POPREG_SZ+POPREG_SZ);

	DWORD dwBack;
	VirtualProtect(src, len, PAGE_READWRITE, &dwBack);
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

	VirtualProtect(src, len, dwBack, &dwBack);

	return jmp;
}