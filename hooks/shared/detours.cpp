#include "stdafx.h"

#include <hooks/shared/detours.h>

#define JMP32_SZ	5	// the size of JMP <address>
#define POPREG_SZ	1	// the size of a POP <reg>
#define NOP			0x90 // opcode for NOP
#define JMP			0xE9 // opcode for JUMP
#define POP_EAX		0x58
#define POP_ECX		0x59
#define PUSH_EAX	0x50
#define PUSH_ECX	0x51


//#pragma warning(disable: 4312)
//#pragma warning(disable: 4311)
#define MakePtr(cast, ptr, addValue) (cast)((DWORD)(ptr) + (DWORD)(addValue))

void *InterceptDllCall(HMODULE hModule, char *szDllName, char *szFunctionName, DWORD pNewFunction)
{
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	MdtMemBlockInfos mbis;
	void *pOldFunction;


	if (!(pOldFunction = GetProcAddress(GetModuleHandle(szDllName), szFunctionName)))
		return NULL;

	pDosHeader = (PIMAGE_DOS_HEADER) hModule;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	pNTHeader = MakePtr(PIMAGE_NT_HEADERS, pDosHeader, pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE
	|| (pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, pDosHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)) == (PIMAGE_IMPORT_DESCRIPTOR) pNTHeader)
		return NULL;

	while (pImportDesc->Name)
	{
		char *szModuleName = MakePtr(char *, pDosHeader, pImportDesc->Name);
		if (!_stricmp(szModuleName, szDllName))
			break;
		pImportDesc++;
	}
	if (pImportDesc->Name == NULL)
		return NULL;

	pThunk = MakePtr(PIMAGE_THUNK_DATA, pDosHeader,	pImportDesc->FirstThunk);
	while (pThunk->u1.Function)
	{
		if (pThunk->u1.Function == (DWORD)pOldFunction)
		{
			MdtMemAccessBegin((void *) &pThunk->u1.Function, sizeof(DWORD), &mbis);
			pThunk->u1.Function = (DWORD) pNewFunction;
			MdtMemAccessEnd(&mbis);

			return pOldFunction;
		}
		pThunk++;
	}

	return NULL;
}


LPVOID MdtAllocExecuteableMemory(size_t size)
{
	DWORD dwDummy;
	LPVOID mem = malloc(size);

	VirtualProtect(mem, size, PAGE_EXECUTE_READWRITE, &dwDummy);

	return mem;
}


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

	for(size_t i=0; i < mdtMemBlockInfos->size(); i++) {
		mbi2 = mdtMemBlockInfos->at(i);
		VirtualProtect(mbi2.BaseAddress, mbi2.RegionSize, PAGE_READWRITE, &dwDummy);
	}
}

void MdtMemAccessEnd(MdtMemBlockInfos *mdtMemBlockInfos)
{
	MdtMemBlockInfo mbi2;
	DWORD dwDummy;

	for(size_t i=0; i < mdtMemBlockInfos->size(); i++) {
		mbi2 = mdtMemBlockInfos->at(i);
		VirtualProtect(mbi2.BaseAddress, mbi2.RegionSize, mbi2.Protect, &dwDummy);
	}
}

// Detour
void *DetourApply(BYTE *orig, BYTE *hook, int len)
{
	MdtMemBlockInfos mbis;
	BYTE *jmp = (BYTE*)MdtAllocExecuteableMemory(len+JMP32_SZ);

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
	BYTE *jmp = (BYTE*)MdtAllocExecuteableMemory(len+JMP32_SZ+POPREG_SZ+POPREG_SZ+POPREG_SZ);
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


void Asm32ReplaceWithJmp(void * replaceAt, size_t countBytes, void * jmpTo)
{
	MdtMemBlockInfos mbis;

	MdtMemAccessBegin(replaceAt, countBytes, &mbis);

	memset(replaceAt, NOP, countBytes);
	((BYTE *)replaceAt)[0] = JMP;
	*(DWORD*)((BYTE *)replaceAt+1) = (DWORD)((BYTE *)jmpTo - (BYTE *)replaceAt) - JMP32_SZ;

	MdtMemAccessEnd(&mbis);
}
