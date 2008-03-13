// Project :  Half-Life Advanced Effects
// File    :  shared/com/basecom.h

// Authors : last change / first change / name

// 2008-03-11 / 2008-03-11 / Dominik Tugend


//
// Headers for basecom between the core (GUI) and the hook
//

// The Hlae Basecom system is based on the WM_COPYDATA Win32API service.

// It's designed for synchronous message processing,
// by using blocking SendMessage calls.

// It's a communication between only one server and one client.
// Other scenarios are not supported by design.


//
// coding conventions:
//

// for version checking:

// The HELLO message codes may not be changed.
// This also applies to the first 32 Bit of the belonging message structures.


// returns of the WindowProc:

// If the reciever processed the message it's WindowProc shall return TRUE==1
// otherwise it shall return FALSE==0.
// Please note that a FALSE return from SendMessage also happens when the
// message could not be delivered to the reciever.

// For queries (QRY) a return with TRUE also indicates that the RET message
// has been successfully delivered.

// Other return values are not allowed.
// If you need other return values you'll have to supply those in the RET
// message in response to a query (QRY).


// This is to avoid first order loops and deadlocks:

// The processing of a QRY may create one and only one RET message
// before the WindowProc returns.

// The processing of ANY OTHER MESSAGE must not create any messages
// before the WindowProc returned.

// Always look out to avoid ping-pong situations.


//
// Includes:
//

#include <windows.h>

//
// Window Identifiers
//

#define HLAE_BASECOM_ID L"Half-Life Advanced Effects BaseCom"
#define HLAE_BASECOM_SERVER_ID HLAE_BASECOM_ID L" Server"
#define HLAE_BASECOM_CLIENT_ID HLAE_BASECOM_ID L" Client"
#define HLAE_BASECOM_CLASSNAME L"HLAEBaseComWindow"


//
// Messages
//

// naming convention:

// HLAE_BASECOM_xxxSV_+ client -> server
// HLAE_BASECOM_xxxCL_+ server -> client

// where xxx is one of the following:
// MSG - one way message, no answer expected
// QRY - query message, answer expected
// RET - response  message, in reaction to a query message
// ___ - reserved, may not be used

// The QRY and it's bellonging RET message shall share the same trail (+).


// message code conventions:

// For one-may messages their return code shall be reserved.

// The query and it's response shall use the same message code.

// Appart from that no two (different) messages may share the same messge
// code.


// basecom server messages:

#define HLAE_BASECOM_QRYSV_HELLO				0x00000000
// the server may choose not to reply to HELLO.

#define HLAE_BASECOM_QRYSV_OnCreateWindow		0x00000001
#define HLAE_BASECOM_MSGSV_OnDestroyWindow		0x00000002
#define HLAE_BASECOM____SV_OnGameWindowFocus	0x00000007
#define HLAE_BASECOM____SV_OnServerClose		0x00000008
#define HLAE_BASECOM____SV_MouseEvent			0x00000009
#define HLAE_BASECOM____SV_KeyBoardEvent		0x0000000A
#define HLAE_BASECOM_MSGSV_UpdateWindow			0x0000000F

// basecom client messages:

#define HLAE_BASECOM_RETCL_HELLO				0x00000000

#define HLAE_BASECOM_RETCL_OnCreateWindow		0x00000001
#define HLAE_BASECOM____CL_OnDestroyWindow		0x00000002
#define HLAE_BASECOM_MSGCL_OnGameWindowFocus	0x00000007
#define HLAE_BASECOM_MSGCL_OnServerClose		0x00000008
#define HLAE_BASECOM_MSGCL_MouseEvent			0x00000009
#define HLAE_BASECOM_MSGCL_KeyBoardEvent		0x0000000A
#define HLAE_BASECOM____CL_UpdateWindow			0x0000000F

//
// Message Structures:
//

// Note: in some cases the delivered messages may contain piggy backed data
// after the regular structure (i.e. for transmitting strings etc.).
// The exact format of a message depends on the functions exchanging it and is
// not defined here.

struct HLAE_BASECOM_HELLO_s
{
	DWORD dwClientVersion;
};

struct HLAE_BASECOM_RET_HELLO_s
{
	DWORD	dwServerVersion;
	bool	bConnectionAccepted;
};

struct HLAE_BASECOM_OnCreateWindow_s
{
	int nWidth;
	int nHeight;
};

struct HLAE_BASECOM_RET_OnCreateWindow_s
{
	HWND parentWindow;
};

struct HLAE_BASECOM_OnDestroyWindow_s
{
	// empty
};

struct HLAE_BASECOM_OnGameWindowFocus_s
{
	// empty
};

struct HLAE_BASECOM_OnServerClose_s
{
	// empty
};

struct HLAE_BASECOM_MSGCL_MouseEvent_s
{
	unsigned int uMsg; // WM_* code
	unsigned int wParam; // 
	unsigned short iX; // Virtual coord
	unsigned short iY; // .
};
struct HLAE_BASECOM_MSGCL_KeyBoardEvent_s
{
	unsigned int uMsg; // WM_* code
	unsigned int uKeyCode;
	unsigned int uKeyFlags;
};

struct HLAE_BASECOM_UpdateWindows_s
{
	int nWidth;
	int nHeight;
};
