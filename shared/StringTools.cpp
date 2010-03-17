#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-17 by dominik.matrixstorm.com
///
// Firstchanges:
// 2010-03-17 by dominik.matrixstorm.com

#include "StringTools.h"

#include <string.h>


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