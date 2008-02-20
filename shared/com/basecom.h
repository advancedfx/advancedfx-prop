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

#define HLAE_BASECOM_MSG_TESTDUMMY			0x00000000

#define HLAE_BASECOM_MSGSV_CreateWindowExA	0x00000001
#define HLAE_BASECOM_MSGSV_RegisterClassA	0x00000002
#define HLAE_BASECOM_MSGSV_DestroyWindow	0x00000003

#define HLAE_BASECOM_MSGCL_RET_CreateWindowExA	0x00000001
#define HLAE_BASECOM_MSGCL_RET_RegisterClassA	0x00000002
#define HLAE_BASECOM_MSGCL_RET_DestroyWindow	0x00000003

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

