#include "stdafx.h"

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-10-04 dominik.matrixstorm.com
//
// First changes:
// 2014-10-28 dominik.matrixstorm.com

#include "binutils.h"

#define PtrFromRva( base, rva ) ( ( ( PBYTE ) base ) + rva )

namespace Afx {
namespace BinUtils {

////////////////////////////////////////////////////////////////////////////////

// TODO: optimize this.
MemRange FindBytes(MemRange memRange, char const * pattern, DWORD patternSize)
{
	DWORD matchDepth = 0;
	DWORD oldMemRangeStart = memRange.Start;

	if(1 > patternSize)
		return MemRange(oldMemRangeStart, oldMemRangeStart);

	for(;memRange.Start < memRange.End;memRange.Start++)
	{
		char cur = *(char const *)memRange.Start;

		if(cur == pattern[matchDepth])
			matchDepth++;
		else
		{
			memRange.Start -= matchDepth;
			matchDepth = 0;
			continue;
		}

		if(matchDepth == patternSize)
			return MemRange(memRange.Start-matchDepth+1, memRange.Start+1);
	}

	return MemRange(oldMemRangeStart, oldMemRangeStart);
}

MemRange FindCString(MemRange memRange, char const * pattern)
{
	return FindBytes(memRange, pattern, strlen(pattern)+1);
}

MemRange FindWCString(MemRange memRange, wchar_t const * pattern)
{
	return FindBytes(memRange, (char const *)pattern, sizeof(wchar_t)*(wcslen(pattern)+1));
}

// ImageSectionsReader /////////////////////////////////////////////////////////

ImageSectionsReader::ImageSectionsReader(HMODULE hModule)
{
	PIMAGE_DOS_HEADER DosHeader = ( PIMAGE_DOS_HEADER ) hModule;
	PIMAGE_NT_HEADERS NtHeader; 

	m_hModule = hModule;
	m_SectionsLeft = 0;

	NtHeader = (PIMAGE_NT_HEADERS) PtrFromRva( DosHeader, DosHeader->e_lfanew );
	if( IMAGE_NT_SIGNATURE != NtHeader->Signature )
	{
		return;		
	}

	m_SectionsLeft = NtHeader->FileHeader.NumberOfSections;
	m_Section = (PIMAGE_SECTION_HEADER) PtrFromRva( NtHeader, sizeof(IMAGE_NT_HEADERS) );
}

bool ImageSectionsReader::Eof(void)
{
	return !(0 < m_SectionsLeft);
}

void ImageSectionsReader::Next(void)
{
	if(Eof()) return;

	m_SectionsLeft--;
	m_Section = (PIMAGE_SECTION_HEADER) PtrFromRva( m_Section, sizeof(IMAGE_SECTION_HEADER) );
}

void ImageSectionsReader::Next(DWORD characteristicsFilter)
{
	for(;!Eof();Next())
	{
		if(characteristicsFilter == (m_Section->Characteristics & characteristicsFilter))
			break;
	}
}

PIMAGE_SECTION_HEADER ImageSectionsReader::Get(void)
{
	return m_Section;
}

MemRange ImageSectionsReader::GetMemRange(void)
{
	DWORD startAddress = GetStartAddress();
	return MemRange(startAddress, startAddress+GetSize());
}

DWORD ImageSectionsReader::GetStartAddress(void)
{
	return (DWORD)PtrFromRva(m_hModule, m_Section->VirtualAddress);
}

DWORD ImageSectionsReader::GetSize(void)
{
	return m_Section->Misc.VirtualSize;
}

// MemRange ////////////////////////////////////////////////////////////////////

MemRange::MemRange()
{
	Start = End = 0;
}

MemRange::MemRange(DWORD start, DWORD end)
{
	Start = start;
	End = end;
}

bool MemRange::IsEmpty(void)
{
	return End <= Start;
}


} // namespace Afx {
} // namespace BinUtils {
