#pragma once

#include <windows.h>
#include <vector>

typedef struct MdtMemBlockInfo_s {
	PVOID  BaseAddress;
	SIZE_T RegionSize;
	DWORD  Protect;
} MdtMemBlockInfo;

typedef std::vector<MdtMemBlockInfo> MdtMemBlockInfos;

void *DetourApply(BYTE *orig, BYTE *hook, int len);
void *DetourClassFunc(BYTE *src, const BYTE *dst, const int len);

void * InterceptDllCall(HMODULE hModule, char * szDllName, char * szFunctionName, DWORD pNewFunction);

void MdtMemAccessBegin(LPVOID lpAddress, size_t size, MdtMemBlockInfos *mdtMemBlockInfos);
void MdtMemAccessEnd(MdtMemBlockInfos *mdtMemBlockInfos);
LPVOID MdtAllocExecuteableMemory(size_t size);