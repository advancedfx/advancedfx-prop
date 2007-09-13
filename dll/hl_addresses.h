/*
File        : dll/hl_addresses.h
Started     : 2007-09-13 13:37:00
Project     : Mirv Demo Tool
Authors     : Gavin Bramhill, Dominik Tugend
Description : Address defintions inside Half-Life until we have an better system.
*/

#ifndef HL_ADDRESSES_H
#define HL_ADDRESSES_H

// main, ...
#define HL_ADDR_CL_ENGINEFUNCS_S 0x01ECCB00
// old: 0x01EA0A08
#define HL_ADDR_ENGINE_SUTDIO_API_S 0x01EE8FC0
// old: 0x01EBC978
#define HL_ADDR_PLAYERMOVE_S 0x02D82500
// old: 0x02D590A0;

// demoedit:
#define HL_ADDR_HUDSPECTATOR_FUNC_TFC 0x01909A00
#define HL_ADDR_HUDSPECTATOR_CMPA0_TFC 0x01909A23

// Render View and RefDef for information how to find these values reffer to cameraofs.cpp:
#define HL_ADDR_R_RenderView 0x01d51d90
// old: 0x01d50550
#define HL_ADDR_r_refdef 0x02c42740
// old: 0x02c192e0

#endif // #ifndef HL_ADDRESSES_H