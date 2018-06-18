#include "stdafx.h"

#include <shared/detours.h>

//TODO: MdtAllocExecuteableMemory needs probably FlushInstructionCache right after when used
// but we currently only obey that in DetourClassFunc.

//#define MDT_DEBUG

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

#define PtrFromRva( base, rva ) ( ( ( PBYTE ) base ) + rva )

// Code taken from
// http://jpassing.com/2008/01/06/using-import-address-table-hooking-for-testing/
// needs to be replaced by own code.
//
/*++
  Routine Description:
    Replace the function pointer in a module's IAT.

  Parameters:
    Module              - Module to use IAT from.
    ImportedModuleName  - Name of imported DLL from which 
                          function is imported.
    ImportedProcName    - Name of imported function.
    AlternateProc       - Function to be written to IAT.
    OldProc             - Original function.

  Return Value:
    S_OK on success.
    (any HRESULT) on failure.
--*/
HRESULT PatchIat(
  __in HMODULE Module,
  __in PCSTR ImportedModuleName,
  __in PCSTR ImportedProcName,
  __in PVOID AlternateProc,
  __out_opt PVOID *OldProc
  )
{
  PIMAGE_DOS_HEADER DosHeader = ( PIMAGE_DOS_HEADER ) Module;
  PIMAGE_NT_HEADERS NtHeader; 
  PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor;
  UINT Index;

  _ASSERTE( Module );
  _ASSERTE( ImportedModuleName );
  _ASSERTE( ImportedProcName );
  _ASSERTE( AlternateProc );

  NtHeader = ( PIMAGE_NT_HEADERS ) 
    PtrFromRva( DosHeader, DosHeader->e_lfanew );
  if( IMAGE_NT_SIGNATURE != NtHeader->Signature )
  {
    return HRESULT_FROM_WIN32( ERROR_BAD_EXE_FORMAT );
  }

  ImportDescriptor = ( PIMAGE_IMPORT_DESCRIPTOR ) 
    PtrFromRva( DosHeader, 
      NtHeader->OptionalHeader.DataDirectory
        [ IMAGE_DIRECTORY_ENTRY_IMPORT ].VirtualAddress );

  //
  // Iterate over import descriptors/DLLs.
  //
  for ( Index = 0; 
        ImportDescriptor[ Index ].Characteristics != 0; 
        Index++ )
  {
    PSTR dllName = ( PSTR ) 
      PtrFromRva( DosHeader, ImportDescriptor[ Index ].Name );

    if ( 0 == _strcmpi( dllName, ImportedModuleName ) )
    {
      //
      // This the DLL we are after.
      //
      PIMAGE_THUNK_DATA Thunk;
      PIMAGE_THUNK_DATA OrigThunk;

      if ( ! ImportDescriptor[ Index ].FirstThunk ||
         ! ImportDescriptor[ Index ].OriginalFirstThunk )
      {
        return E_INVALIDARG;
      }

      Thunk = ( PIMAGE_THUNK_DATA )
        PtrFromRva( DosHeader, 
          ImportDescriptor[ Index ].FirstThunk );
      OrigThunk = ( PIMAGE_THUNK_DATA )
        PtrFromRva( DosHeader, 
          ImportDescriptor[ Index ].OriginalFirstThunk );

      for ( ; OrigThunk->u1.Function != NULL; 
              OrigThunk++, Thunk++ )
      {
        if ( OrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG )
        {
          //
          // Ordinal import - we can handle named imports
          // ony, so skip it.
          //
          continue;
        }

        PIMAGE_IMPORT_BY_NAME import = ( PIMAGE_IMPORT_BY_NAME )
          PtrFromRva( DosHeader, OrigThunk->u1.AddressOfData );

        if ( 0 == strcmp( ImportedProcName, 
                              ( char* ) import->Name ) )
        {
          //
          // Proc found, patch it.
          //
          DWORD junk;
          MEMORY_BASIC_INFORMATION thunkMemInfo;

          //
          // Make page writable.
          //
          VirtualQuery(
            Thunk,
            &thunkMemInfo,
            sizeof( MEMORY_BASIC_INFORMATION ) );
          if ( ! VirtualProtect(
            thunkMemInfo.BaseAddress,
            thunkMemInfo.RegionSize,
            PAGE_EXECUTE_READWRITE,
            &thunkMemInfo.Protect ) )
          {
            return HRESULT_FROM_WIN32( GetLastError() );
          }

          //
          // Replace function pointers (non-atomically).
          //
          if ( OldProc )
          {
            *OldProc = ( PVOID ) ( DWORD_PTR ) 
                Thunk->u1.Function;
          }
#ifdef _WIN64
          Thunk->u1.Function = ( ULONGLONG ) ( DWORD_PTR ) 
              AlternateProc;
#else
          Thunk->u1.Function = ( DWORD ) ( DWORD_PTR ) 
              AlternateProc;
#endif
          //
          // Restore page protection.
          //
          if ( ! VirtualProtect(
            thunkMemInfo.BaseAddress,
            thunkMemInfo.RegionSize,
            thunkMemInfo.Protect,
            &junk ) )
          {
            return HRESULT_FROM_WIN32( GetLastError() );
          }

          return S_OK;
        }
      }
      
      //
      // Import not found.
      //
      return HRESULT_FROM_WIN32( ERROR_PROC_NOT_FOUND );    
    }
  }

  //
  // DLL not found.
  //
  return HRESULT_FROM_WIN32( ERROR_MOD_NOT_FOUND );
}

void *InterceptDllCall(HMODULE hModule, const char *szDllName, const char *szFunctionName, DWORD pNewFunction)
{
/*
	For some setups this old code won't probably because GetProcAddress returns
	a different address from the one that the IAT uses.

	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	MdtMemBlockInfos mbis;
	void *pOldFunction;

#ifdef MDT_DEBUG
	MessageBox(0, szFunctionName, "InterceptDllCall", MB_OK|MB_ICONINFORMATION);
#endif


	if (!(pOldFunction = GetProcAddress(GetModuleHandle(szDllName), szFunctionName)))
	{
#ifdef MDT_DEBUG
		MessageBox(0, "GetProcAddress failed.", "InterceptDllCall", MB_OK|MB_ICONERROR);
#endif
		return NULL;
	}

	pDosHeader = (PIMAGE_DOS_HEADER) hModule;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
#ifdef MDT_DEBUG
		MessageBox(0, "No IMAGE_DOS_SIGNATURE", "InterceptDllCall", MB_OK|MB_ICONERROR);
#endif
		return NULL;
	}

	pNTHeader = MakePtr(PIMAGE_NT_HEADERS, pDosHeader, pDosHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE
	|| (pImportDesc = MakePtr(PIMAGE_IMPORT_DESCRIPTOR, pDosHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress)) == (PIMAGE_IMPORT_DESCRIPTOR) pNTHeader)
	{
#ifdef MDT_DEBUG
		MessageBox(0, "No IMAGE_NT_SINGATURE or ImportDesc is NtHeader", "InterceptDllCall", MB_OK|MB_ICONERROR);
#endif
		return NULL;
	}

	while (pImportDesc->Name)
	{
		char *szModuleName = MakePtr(char *, pDosHeader, pImportDesc->Name);
		if (!_stricmp(szModuleName, szDllName))
			break;
		pImportDesc++;
	}
	if (pImportDesc->Name == NULL)
	{
#ifdef MDT_DEBUG
		MessageBox(0, "DLLName does not match.", "InterceptDllCall", MB_OK|MB_ICONERROR);
#endif
		return NULL;
	}

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

#ifdef MDT_DEBUG
	MessageBox(0, "Function not found.", "InterceptDllCall", MB_OK|MB_ICONERROR);
#endif
*/
	void *pOldFunction;

	if(S_OK == PatchIat(
		hModule,
		szDllName,
		szFunctionName,
		(PVOID)pNewFunction,
		(PVOID *)&pOldFunction
	))
		return pOldFunction;

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
		VirtualProtect(mbi2.BaseAddress, mbi2.RegionSize, PAGE_EXECUTE_READWRITE, &dwDummy);
	}
}

void MdtMemAccessEnd(MdtMemBlockInfos *mdtMemBlockInfos)
{
	MdtMemBlockInfo mbi2;
	DWORD dwDummy;

	HANDLE currentProcess = GetCurrentProcess();

	for(size_t i=0; i < mdtMemBlockInfos->size(); i++) {
		mbi2 = mdtMemBlockInfos->at(i);
		FlushInstructionCache(currentProcess, mbi2.BaseAddress, mbi2.RegionSize);
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


void *DetourVoidClassFunc(BYTE *src, const BYTE *dst, const int len)
{
	BYTE *jmp = (BYTE*)MdtAllocExecuteableMemory(len+JMP32_SZ+POPREG_SZ);
	MdtMemBlockInfos mbis;

	MdtMemAccessBegin(src, len, &mbis);

	memcpy(jmp+1, src, len);

	// calculate callback function call
	jmp[0] = POP_ECX;						// pop ecx
	jmp[len+1] = JMP;						// jmp
	*(DWORD*)(jmp+len+2) = (DWORD)((src+len) - (jmp+len+1)) - JMP32_SZ;

	// detour source function call
	src[0] = 0x87; // XCHG ecx, [esp]
	src[1] = 0x0c; // .
	src[2] = 0x24; // .
	src[3] = PUSH_ECX; // push ecx
	src[4] = JMP;							// jmp
	*(DWORD*)(src+5) = (DWORD)(dst - (src+4)) - JMP32_SZ;

	memset(src+9, NOP, len - 9);

	MdtMemAccessEnd(&mbis);

	return jmp;
}


void DetourIfacePtr(DWORD * ptr, void const * hook, DetourIfacePtr_fn & outTarget)
{
	MdtMemBlockInfos mbis;
	DWORD orgAddr;
	HANDLE hCurrentProcss = GetCurrentProcess();

	MdtMemAccessBegin(ptr, sizeof(DWORD), &mbis);

	orgAddr = *ptr;

	BYTE *jmpTarget = (BYTE*)MdtAllocExecuteableMemory(JMP32_SZ+POPREG_SZ+POPREG_SZ+POPREG_SZ);

	// padding code that jumps to target:
	jmpTarget[0] = POP_EAX;						// pop eax
	jmpTarget[1] = POP_ECX;						// pop ecx
	jmpTarget[2] = PUSH_EAX;						// push eax
	jmpTarget[3] = JMP;						// jmp
	*(DWORD*)(jmpTarget+4) = (orgAddr - (DWORD)(jmpTarget+3)) - JMP32_SZ;

	FlushInstructionCache(hCurrentProcss, jmpTarget, JMP32_SZ + POPREG_SZ + POPREG_SZ + POPREG_SZ);


	outTarget = (void(*)(void))jmpTarget;


	BYTE * jmpHook = (BYTE*)MdtAllocExecuteableMemory(JMP32_SZ+POPREG_SZ+POPREG_SZ+POPREG_SZ);

	// padding code that jumps to our hook:
	jmpHook[0] = POP_EAX;						// pop eax;
	jmpHook[1] = PUSH_ECX;						// push ecx
	jmpHook[2] = PUSH_EAX;						// push eax
	jmpHook[3] = JMP;							// jmp
	*(DWORD*)(jmpHook+4) = (DWORD)((BYTE *)hook - (jmpHook+3)) - JMP32_SZ;

	FlushInstructionCache(hCurrentProcss, jmpHook, JMP32_SZ + POPREG_SZ + POPREG_SZ + POPREG_SZ);

	// update iface ptr:
	*ptr = (DWORD)jmpHook; // this needs to be an atomic operation!!! (currently is)

	MdtMemAccessEnd(&mbis);
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
