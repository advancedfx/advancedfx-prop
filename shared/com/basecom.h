//
// Headers for basecom between the core (GUI) and the hook
//

#define HLAE_BASECOM_ID L"Half-Life Advanced Effects BaseCom"
#define HLAE_BASECOM_SERVER_ID HLAE_BASECOM_ID L" Server"
#define HLAE_BASECOM_CLIENT_ID HLAE_BASECOM_ID L" Client"
#define HLAE_BASECOM_CLASSNAME L"HLAEBaseComWindow"

#define HLAE_BASECOM_MSG_EMPTY		0x00000000
// empty message for testing

#define HLAE_BASECOM_MSGSV_GETGWND	0x00000001
// client is requesting game window

#define HLAE_BASECOM_MSGCL_SENDWND	0x00000001
// server answers with HWND

//#define HLAE_BASECOM_MAKEMSG(cps,msgid,msgs) ##cps##.dwData = ##msgid##; ##cps##.cbData = sizeof(##msgs##); ##cps##.lpData = &##msgs##;

//
// Message Structures:
//

struct HLAE_BASECOM_GAMEWND_s
{
	HWND hwGameWindow;
};