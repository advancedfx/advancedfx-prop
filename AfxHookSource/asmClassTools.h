#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-06-12 by dominik.matrixstorm.com
//
// First changes:
// 2015-06-26 by dominik.matrixstorm.com

// The first 3 asm instructions unroll the compiler epiloge code,
// this might need to be updated. I couldn't find a better way yet,
// since __declspec(naked) won't work on member functions.
// TODO:
// A better way might be filling a struct with function pointers
// and returning a pointer to that maybe.
#define JMP_IFACE_FN(iface,index) \
	__asm pop ecx \
	__asm mov esp, ebp \
	__asm pop ebp \
	__asm mov ecx, iface \
	__asm mov eax, [ecx] \
	__asm mov eax, [eax +4*index] \
	__asm jmp eax

#define JMP_CLASSMEMBERIFACE_FN(classType,classMemberIface,index) \
	__asm mov eax, this \
	__asm mov eax, [eax]classType.classMemberIface \
	__asm pop ecx \
	__asm mov esp, ebp \
	__asm pop ebp \
	__asm mov ecx, eax \
	__asm mov eax, [ecx] \
	__asm mov eax, [eax +4*index] \
	__asm jmp eax

#define JMP_CLASSMEMBERIFACE_FN_DBG(classType,classMemberIface,index) \
	__asm mov ecx, this \
	__asm mov eax, index \
	__asm push eax \
	__asm call Debug \
	__asm mov eax, this \
	__asm mov eax, [eax]classType.classMemberIface \
	__asm pop ecx \
	__asm mov esp, ebp \
	__asm pop ebp \
	__asm mov ecx, eax \
	__asm mov eax, [ecx] \
	__asm mov eax, [eax +4*index] \
	__asm jmp eax

#define JMP_CLASSMEMBERIFACE_OFS_FN(classType,classMemberIface,ofs,index) \
	__asm mov eax, this \
	__asm sub eax, 4*ofs \
	__asm mov eax, [eax]classType.classMemberIface \
	__asm pop ecx \
	__asm mov esp, ebp \
	__asm pop ebp \
	__asm mov ecx, eax \
	__asm mov eax, [ecx +4*ofs] \
	__asm mov eax, [eax +4*index] \
	__asm jmp eax

#define JMP_CLASSMEMBERIFACE_OFS_FN_DBG(classType,classMemberIface,ofs,index) \
	__asm mov ecx, this \
	__asm sub ecx, 4*ofs \
	__asm mov eax, index \
	__asm push eax \
	__asm mov eax, ofs \
	__asm push eax \
	__asm call Debug \
	__asm mov eax, this \
	__asm sub eax, 4*ofs \
	__asm mov eax, [eax]classType.classMemberIface \
	__asm pop ecx \
	__asm mov esp, ebp \
	__asm pop ebp \
	__asm mov ecx, eax \
	__asm mov eax, [ecx +4*ofs] \
	__asm mov eax, [eax +4*index] \
	__asm jmp eax
