//
// Headers for basecom between the core (GUI) and the hook
//

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

// HLAE_BASECOM_MSG_+ is understood by both, Server and Client
// HLAE_BASECOM_MSGSV_+ client -> server
// HLAE_BASECOM_MSGCL_+ server -> client

// If the reciever proccessed the message it shall return TRUE to SendMessage
// otherwise it shall return FALSE to SendMessage

// Some messages are only one-way, however their repsonse code is reserved (commented out)

#define HLAE_BASECOM_MSG_TESTDUMMY				0x00000000

#define HLAE_BASECOM_MSGSV_CreateWindowExA		0x00000001
#define HLAE_BASECOM_MSGSV_RegisterClassA		0x00000002
#define HLAE_BASECOM_MSGSV_DestroyWindow		0x00000003
//n/a	#define HLAE_BASECOM_MSGSV_RET_CallWndProc_s	0x00000004
#define HLAE_BASECOM_MSGSV_GameWndPrepare		0x00000005
#define HLAE_BASECOM_MSGSV_GameWndGetDC			0x00000006
#define HLAE_BASECOM_MSGSV_GameWndRelease		0x00000007
//n/a	#define HLAE_BASECOM_MSGSV_RET_WndRectUpdate	0x00000008
//n/a	#define HLAE_BASECOM_MSGSV_RET_MouseEvent		0x00000009
//n/a	#define HLAE_BASECOM_MSGCL_RET_KeyBoardEvent	0x0000000A

#define HLAE_BASECOM_MSGCL_RET_CreateWindowExA	0x00000001
#define HLAE_BASECOM_MSGCL_RET_RegisterClassA	0x00000002
#define HLAE_BASECOM_MSGCL_RET_DestroyWindow	0x00000003
#define HLAE_BASECOM_MSGCL_CallWndProc_s		0x00000004
//n/a	#define HLAE_BASECOM_MSGCL_RET_GameWndPrepare	0x00000005
#define HLAE_BASECOM_MSGCL_RET_GameWndGetDC		0x00000006
//n/a	#define HLAE_BASECOM_MSGCL_RET_GameWndRelease	0x00000007
#define HLAE_BASECOM_MSGCL_WndRectUpdate		0x00000008
#define HLAE_BASECOM_MSGCL_MouseEvent			0x00000009
#define HLAE_BASECOM_MSGCL_KeyBoardEvent		0x0000000A

//
// Message Structures:
//

struct HLAE_BASECOM_CreateWindowExA_s
{
    DWORD dwExStyle;
    LPCTSTR lpClassName; // relative (piggy backed)
    LPCTSTR lpWindowName; // relative (piggy backed)
    DWORD dwStyle;
    int x;
    int y;
    int nWidth;
    int nHeight;
    HWND hWndParent;
    HMENU hMenu;
    HINSTANCE hInstance;
    LPVOID lpParam; // only valid in client process space
};

struct HLAE_BASECOM_RET_CreateWindowExA_s
{
	HWND retResult;
};

struct HLAE_BASECOM_DestroyWindow_s
{
    HWND hWnd;
};

struct HLAE_BASECOM_RET_DestroyWindow_s
{
	BOOL retResult;
};

typedef WNDCLASSA HLAE_BASECOM_RegisterClassA_s;
// lpszMenuName // relative (piggy backed)
// lpszClassName // relative (piggy backed)
// lpfnWndProc // only valid in client process space

struct HLAE_BASECOM_RET_RegisterClassA_s
{
	ATOM retResult;
};

struct HLAE_BASECOM_CallWndProc_s
{
    HWND hwnd;
    UINT uMsg;
    WPARAM wParam;
    LPARAM lParam;
};

struct HLAE_BASECOM_GameWndPrepare_s
{
	int nWidth;
	int nHeight;
};

struct HLAE_BASECOM_GameWndGetDC_s
{
    // empty
};

struct HLAE_BASECOM_RET_GameWndGetDC_s
{
	HWND retResult; // MSDN2: Note that the handle to the DC can only be used by a single thread at any one time. This is why we return the Window instead and carry out GetDC and ReleaseDC locally.
};

struct HLAE_BASECOM_GameWndRelease_s
{
	// empty
};

struct HLAE_BASECOM_WndRectUpdate_s
{
	int iLeft;
	int iTop;
	int iWidthVisible;
	int iHeightVisible;
	int iWidthTotal;
	int iHeightTotal;
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