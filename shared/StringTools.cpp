#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-05-19 by dominik.matrixstorm.com
//
// Firstchanges:
// 2010-03-17 by dominik.matrixstorm.com

#include "StringTools.h"

#include <windows.h>

bool AnsiStringToWideString(char const * ansiChars, std::wstring & outWideString)
{
	LPWSTR wideChars;
	int length;
	bool bOk;

	if(0 == (length = MultiByteToWideChar(
		CP_ACP,
		0,
		ansiChars,
		-1,
		NULL,
		0
	)))
		return false;
	
	if(!(wideChars = (LPWSTR)malloc(sizeof(WCHAR) * length)))
		return false;

	bOk = length == MultiByteToWideChar(
			CP_ACP,
			0,
			ansiChars,
			-1,
			wideChars,
			length
	);

	if(bOk)
		outWideString.assign(wideChars);

	free(wideChars);

	return bOk;
}

bool WideStringToAnsiString(wchar_t const * wideChars, std::string & outAnsiString)
{
	LPSTR ansiChars;
	int length;
	bool bOk;

	if(0 == (length = WideCharToMultiByte(
		CP_ACP,
		0,
		wideChars,
		-1,
		NULL,
		0,
		NULL,
		NULL
	)))
		return false;

	if(!(ansiChars = (LPSTR)malloc(sizeof(CHAR) * length)))
		return false;

	bOk = length == WideCharToMultiByte(
		CP_ACP,
		0,
		wideChars,
		-1,
		ansiChars,
		sizeof(CHAR) * length,
		NULL,
		NULL
	);

	if(bOk)
		outAnsiString.assign(ansiChars);

	free(ansiChars);

	return bOk;
}


bool StringEndsWith(char const * target, char const * ending) {
	size_t lenTarget = strlen(target);
	size_t lenEnding = strlen(ending);

	if(lenTarget < lenEnding) return false;

	return !strcmp(target +(lenTarget-lenEnding), ending);
}

bool StringBeginsWith(char const * target, char const * beginning) {
	while(*target && *beginning) {
		if(*beginning != *target)
			return false;
		target++;
		beginning++;
	}

	if(*beginning && !*target)
		return false;

	return true;
}

bool StringIsAlNum(char const * value)
{
	if(StringIsEmpty(value)) return true;

	while(*value)
	{
		if(!isalnum(*value))
			return false;

		++value;
	}

	return true;
}

bool StringIsEmpty(char const * value)
{
	return !StringIsNull(value) && *value == '\0';
}

bool StringIsNull(char const * value)
{
	return 0 == value;
}