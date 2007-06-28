
#ifndef DETOURS_H
#define DETOURS_H

#include <windows.h>

void *DetourApply(BYTE *orig, BYTE *hook, int len);
void *DetourClassFunc(BYTE *src, const BYTE *dst, const int len);

#endif