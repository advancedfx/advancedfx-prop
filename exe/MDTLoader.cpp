/*
File        : exe/MDTLoader.cpp
Project     : Mirv Demo Tool
Authors     : Gavin Bramhill, Dominik Tugend
Description : The MDT loader's main file
*/

#define WIN32_LEAN_AND_MEAN


#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <tlhelp32.h>

#include <Commdlg.h> // we want the OpenFile Dialog

#include "Resource.h"

#pragma comment(lib, "version.lib")

#define DLL_NAME	"Mirv Demo Tool.dll"
#define MAX_PATHFILE_LENGTH 1024

HWND hWnd;
HFONT hFont;
HBRUSH hBrush;

OPENFILENAME	g_ofStruct; // we use a global struct for our browse dialog currently;

char*			g_psHLexe = new char[MAX_PATHFILE_LENGTH]; // path/file of hl.exe goes here
char*			g_psMDTpath = new char[MAX_PATHFILE_LENGTH]; // mdt path
char*			g_psLoaderCfg = new char[MAX_PATHFILE_LENGTH]; // mdt_loader.ini path
char*			g_psCfgAppName = "MDT Loader";

// cfg strings
#define MDTLCFGSTR_force_res "force_res"
#define MDTLCFGSTR_hl_exe_path "hl_exe_path"
#define MDTLCFGSTR_color_dropdown "color_dropdown"
#define MDTLCFGSTR_color_custom "color_custom"
#define MDTLCFGSTR_video_dropdown "video_dropdown"
#define MDTLCFGSTR_video_width "video_width"
#define MDTLCFGSTR_video_height "video_height"
#define MDTLCFGSTR_mod_dropdown "mod_dropdown"
#define MDTLCFGSTR_mod_custom "mod_custom"
#define MDTLCFGSTR_launchoptions "launchoptions"

// maximum length of HL launchoptions:
#define MAXLEN_HLRES 5
#define MAXLEN_HLCOLOR 2

// 6 for "-steam", xfor " -window -anyaspec -width 12345 -height 12345", 7 for " -game " + 100 max. for mod + 1000 useroption + 1 NewLine + 100 cause I can't calculate
#define MAXLEN_HLOPT 3000

char			*g_pHLOString=new char [MAXLEN_HLOPT];  // HL launchoptions

int				g_iOrgWindowHeight;
bool			g_bAdvancedMode = false; // by default we are in traditional mode
bool			g_bForceRes = false;

PROCESS_INFORMATION g_HLpi;		// global proccessinfo for HL, only valid when HL has been started by InitDebug and is still runing
STARTUPINFO g_HLsi;				// I don't know why I made this global, used in InitDebug

// syncing vars for critical sections (I am not sure if there might be rare cases on multiprocesspr CPUS where should have used locks / monitros / gates instead, we mainly relay on the threadsafety of the WinAPI alls to DlgProc and if that does not work probability :S) :
bool			g_bOldLoaderLive = false;   // it only indicates if the old loader could still cause an hook into half-life or not, it does not indicate wether the thread is still running or not
bool			g_bOldLoaderWanted = false; // this is checked by the old loader in order to see if it is still be ment to use live ammunition (= ment to hook into hl)
bool			g_bDebugLive = false; //see g_bOldLoader
bool			g_bDebugWanted = false; // see g_bOldLoader


//

typedef char * (*pfnGetMDTVersion_t)();

LRESULT CALLBACK DlgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DWORD GetProcessId(LPCTSTR lpName);
VOID InitLoader();
bool InitDebug(HWND hWndDlg);
HRESULT InjectDll(DWORD pID, LPCTSTR name);
BOOL DoesFileExist(LPCTSTR pszName);

//
// size_t myStrCpy (char *pDestString, char *pSrcString)
//
// Copys NULL terminated strings *Src to *Dest and returns length of *Src
// Note: this is not bufferoverun safe etc.!
//
/* !!!Don't Comment the x86 version out, it has erros for some reason:
//#ifdef _X86_
// I am sick of this shity compiler.
__declspec(naked) size_t myStrCpy(char *pDestString, char *pSrcString)
// http://msdn2.microsoft.com/en-us/library/4d12973a(VS.80).aspx
// Visual C++ Language Reference "Rules and Limitations for Naked Functions"
// http://msdn2.microsoft.com/en-us/library/6xy06s51(VS.80).aspx
// Visual C++ Language Reference "Considerations for Writing Prolog/Epilog Code"
{
	_asm{
		push ebp			; preserve stack pointer
		mov  ebp,esp		; .

		push ECX			; preserve registers we want to use
		push EDX			; .
		push BX				; .

		mov EAX,[ESP+10]	; remember old pDestSting
		mov ECX,[ESP+14]	; pSourceString
		mov EDX,EAX			; pDestString

		a_loop:				; copy characters
			mov BX,[EDX]	; .
			mov [ECX],BL	; .
			INC ECX			; .
			INC EDX			; .
			OR	BL,00		; .
			JNZ a_loop		; .

		sub ECX,EAX			; calculate strlen
		dec ECX				; .
		mov EAX,ECX			; and move it to the return_Value

		pop	BX				; restore registers we used
		pop EDX				; .
		pop ECX				; .

		mov esp,ebp			; restore stack pointer
		pop ebp				; .

		ret
	};
}
#else*/
size_t myStrCpy(char *pDestString, char *pSrcString)
{
	char *pCurr=pDestString;
	char cCurr;
	do *(pCurr++)=cCurr=*(pSrcString++); while (cCurr!=0);
	return pCurr-pDestString-1;
}
//#endif


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				   LPSTR lpCmdLine, int nCmdShow)
{
	// try to autodectect HL's current location from the registry:
	// not implemented yet.
	
	// set the default properties of our struct used for browsing the hl.exe:
	memset(&g_ofStruct,NULL,sizeof(OPENFILENAME));
	g_ofStruct.lStructSize			= sizeof(OPENFILENAME);
	g_ofStruct.hwndOwner			= NULL; // set later by Dialog in DlgProc
	g_ofStruct.hInstance			= hInstance;
	g_ofStruct.lpstrFilter			= "hl.exe of your game\0hl.exe\0\0";
	g_ofStruct.lpstrCustomFilter	= NULL;
	g_ofStruct.nMaxCustFilter		= 0;
	g_ofStruct.nFilterIndex			= 1;

	memcpy(g_psHLexe,"hl.exe",7);
	g_ofStruct.lpstrFile			= g_psHLexe; // this might throw an exception if there is not enough memory
	g_ofStruct.nMaxFile				= MAX_PATHFILE_LENGTH;
	g_ofStruct.nFileOffset			= 0;
	g_ofStruct.nFileExtension		= 3;
	g_ofStruct.lpstrDefExt			= "exe";

	g_ofStruct.lpstrInitialDir		= NULL;
	g_ofStruct.lpstrTitle			= "Select your game's hl.exe ...";
	g_ofStruct.Flags				= OFN_FILEMUSTEXIST | /*OFN_FORCESHOWHIDDEN |*/ OFN_HIDEREADONLY | OFN_LONGNAMES | OFN_PATHMUSTEXIST | OFN_SHAREAWARE;

	g_ofStruct.lCustData			= NULL;
	g_ofStruct.lpfnHook				= NULL;
	g_ofStruct.lpTemplateName		= NULL;
//	g_ofStruct.pvReserved			= NULL;
//	g_ofStruct.dwReserved			= 0;

	// detect the directory of our exe (cause we will search the other components there)
	if(!GetCurrentDirectory(MAX_PATHFILE_LENGTH,g_psMDTpath))
		*g_psMDTpath=0; // if it fails fall back to working directory (but this might be changed by the openfiledialog in case it is used)

	// make mdt_loader.ini path:
	char *pTString = g_psLoaderCfg;
	pTString+=myStrCpy(pTString,g_psMDTpath);
	pTString+=myStrCpy(pTString,"\\mdt_loader.ini");

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN),
	          hWnd, reinterpret_cast<DLGPROC> (DlgProc));

	hWnd = 0;

	DeleteObject(hBrush);
	DeleteObject(hFont);

	return FALSE;
}

void UpdateResControls(HWND hWndDlg,BOOL bEnable)
 // also updates g_bForceRes
{
	g_bForceRes = bEnable;
	EnableWindow(GetDlgItem(hWndDlg,IDC_STATIC_WIDTH),g_bForceRes);
	EnableWindow(GetDlgItem(hWndDlg,IDC_EDIT_WIDTH),g_bForceRes);
	EnableWindow(GetDlgItem(hWndDlg,IDC_STATIC_HEIGHT),g_bForceRes);
	EnableWindow(GetDlgItem(hWndDlg,IDC_EDIT_HEIGHT),g_bForceRes);
	EnableWindow(GetDlgItem(hWndDlg,IDC_STATIC_COLOR),g_bForceRes);
	EnableWindow(GetDlgItem(hWndDlg,IDC_COMBO_COLOR),g_bForceRes);
	EnableWindow(GetDlgItem(hWndDlg,IDC_EDIT_CCOLOR),g_bForceRes&&(SendDlgItemMessage(hWndDlg,IDC_COMBO_COLOR,CB_GETCURSEL,NULL,NULL)==0));
	EnableWindow(GetDlgItem(hWndDlg,IDC_STATIC_DMODE),g_bForceRes);
	EnableWindow(GetDlgItem(hWndDlg,IDC_COMBO_DMODE),g_bForceRes);
}

void UpdateCurrent(HWND hWndDlg)
// this updates the current lauchoptionsfield 
{
	char* pTString=g_pHLOString;

	// add "-steam -gl":
	pTString+=myStrCpy(pTString,"-steam -gl");

	// add forcing info for our dll (if any):
	if(g_bForceRes)
	{
		pTString+=myStrCpy(pTString," -mdtres "); // our DLL will examine for this
		pTString+=GetDlgItemText(hWndDlg,IDC_EDIT_WIDTH,pTString,MAXLEN_HLRES+1); // width
		pTString+=myStrCpy(pTString,"x");
		pTString+=GetDlgItemText(hWndDlg,IDC_EDIT_HEIGHT,pTString,MAXLEN_HLRES+1); // height
		pTString+=myStrCpy(pTString,"x");

		// Fullscreen or Windowed?:
		switch(SendDlgItemMessage(hWndDlg,IDC_COMBO_COLOR,CB_GETCURSEL,NULL,NULL))
		{
		case 0:
			// Custom
			DWORD	uTmp;
			CHAR	*pTOld;
			pTOld=pTString;
			uTmp=GetDlgItemText(hWndDlg,IDC_EDIT_CCOLOR,pTString,MAXLEN_HLCOLOR+1);
			pTString+=uTmp;
			pTString+=myStrCpy(pTString," -bpp");
			memcpy(pTString,pTOld,uTmp);
			pTString+=uTmp;
			*pTString=0;
			break;
		case 1:
			// 16 Bit
			pTString+=myStrCpy(pTString,"16 -bpp16");
			break;
		case 2:
			pTString+=myStrCpy(pTString,"24 -bpp24");
			break;
		default:
			// 32 bit
			pTString+=myStrCpy(pTString,"32 -bpp32");
		}

		if (SendDlgItemMessage(hWndDlg,IDC_COMBO_DMODE,CB_GETCURSEL,NULL,NULL)==1)
			pTString+=myStrCpy(pTString," -full");
		else
			pTString+=myStrCpy(pTString," -window");
	}

	// add sellected mod (if any):
	LRESULT lrModSel = SendDlgItemMessage(hWndDlg,IDC_COMBO_MOD,CB_GETCURSEL,NULL,NULL);
	switch (lrModSel)
	{
		case 0:
			// custom user mod
			pTString+=myStrCpy(pTString," -game ");
			pTString+=GetDlgItemText(hWndDlg,IDC_EDIT_CMOD,pTString,100);
			break;
		case 1:
			// Half-Life = no additional option
			break;
		case 2:
		default:
			pTString+=myStrCpy(pTString," -game cstrike");
			break;
		case 3:
			pTString+=myStrCpy(pTString," -game dod");
			break;
		case 4:
			pTString+=myStrCpy(pTString," -game tfc");
	}

	// add any user options:
	if (GetWindowTextLength(GetDlgItem(hWndDlg,IDC_EDIT_UCOPT))>0)
	{
		pTString+=myStrCpy(pTString," ");
		pTString+=GetDlgItemText(hWndDlg,IDC_EDIT_UCOPT,pTString,1000);
	}

	SetDlgItemText(hWndDlg,IDC_EDIT_CURR,g_pHLOString);
	return;
}

LRESULT CALLBACK DlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	static char pcTmp[MAXLEN_HLOPT+1];

	LRESULT lrComboSel;

	switch(Msg)
	{
	case WM_INITDIALOG:
		int iTmp;
		pcTmp[0]=0;

		hWnd = hWndDlg;
		g_ofStruct.hwndOwner=hWndDlg; // set this dialog as owner for the Browse File Dialog

		// set the maximum length of text user can enter into width and heigth and bitsperpixel
		SendDlgItemMessage(hWndDlg,IDC_EDIT_WIDTH,EM_SETLIMITTEXT,MAXLEN_HLRES,NULL);
		SendDlgItemMessage(hWndDlg,IDC_EDIT_HEIGHT,EM_SETLIMITTEXT,MAXLEN_HLRES,NULL);
		SendDlgItemMessage(hWndDlg,IDC_EDIT_CCOLOR,EM_SETLIMITTEXT,MAXLEN_HLCOLOR,NULL);

		// Init HL EXE PATH:
		GetPrivateProfileString(g_psCfgAppName,MDTLCFGSTR_hl_exe_path,"hl.exe",g_ofStruct.lpstrFile,MAX_PATHFILE_LENGTH-1,g_psLoaderCfg);
		SetDlgItemText(hWndDlg,IDC_EDIT_GPATH,g_ofStruct.lpstrFile);
		
		// Init the Color Dropdown:
		iTmp = GetPrivateProfileInt(g_psCfgAppName,MDTLCFGSTR_color_dropdown,3,g_psLoaderCfg);
		if (iTmp<0||iTmp>3) iTmp=3;
		SendDlgItemMessage(hWndDlg,IDC_COMBO_COLOR,CB_ADDSTRING,NULL,(LPARAM)"Custom:");
		SendDlgItemMessage(hWndDlg,IDC_COMBO_COLOR,CB_ADDSTRING,NULL,(LPARAM)"16 Bit");
		SendDlgItemMessage(hWndDlg,IDC_COMBO_COLOR,CB_ADDSTRING,NULL,(LPARAM)"24 Bit");
		SendDlgItemMessage(hWndDlg,IDC_COMBO_COLOR,CB_ADDSTRING,NULL,(LPARAM)"32 Bit");
		SendDlgItemMessage(hWndDlg,IDC_COMBO_COLOR,CB_SETCURSEL,iTmp,NULL); // select

		// Intit the custom bits:
		GetPrivateProfileString(g_psCfgAppName,MDTLCFGSTR_color_custom,"32",pcTmp,MAXLEN_HLOPT,g_psLoaderCfg);
		SetDlgItemText(hWndDlg,IDC_EDIT_CCOLOR,pcTmp);
		

		// Init the VideoMode Dropdown:
		iTmp = GetPrivateProfileInt(g_psCfgAppName,MDTLCFGSTR_video_dropdown,0,g_psLoaderCfg);
		if (iTmp<0||iTmp>1) iTmp=0;
		SendDlgItemMessage(hWndDlg,IDC_COMBO_DMODE,CB_ADDSTRING,NULL,(LPARAM)"Windowed");
		SendDlgItemMessage(hWndDlg,IDC_COMBO_DMODE,CB_ADDSTRING,NULL,(LPARAM)"Fullscreen");
		SendDlgItemMessage(hWndDlg,IDC_COMBO_DMODE,CB_SETCURSEL,iTmp,NULL); // select

		// init video_width
		GetPrivateProfileString(g_psCfgAppName,MDTLCFGSTR_video_width,"800",pcTmp,MAXLEN_HLOPT,g_psLoaderCfg);
		SetDlgItemText(hWndDlg,IDC_EDIT_WIDTH,pcTmp);

		// init video_height
		GetPrivateProfileString(g_psCfgAppName,MDTLCFGSTR_video_height,"600",pcTmp,MAXLEN_HLOPT,g_psLoaderCfg);
		SetDlgItemText(hWndDlg,IDC_EDIT_HEIGHT,pcTmp);

		// Init the Mod's Dropdown menus (this doesn't seem to work with the data property Value, I guess because Either we are in a dialog or bc we aren't using the classes, whtever, who cares ...)
		iTmp = GetPrivateProfileInt(g_psCfgAppName,MDTLCFGSTR_mod_dropdown,2,g_psLoaderCfg);
		if (iTmp<0||iTmp>4) iTmp=2;
		SendDlgItemMessage(hWndDlg,IDC_COMBO_MOD,CB_ADDSTRING,NULL,(LPARAM)"Custom:");
		SendDlgItemMessage(hWndDlg,IDC_COMBO_MOD,CB_ADDSTRING,NULL,(LPARAM)"(Half-Life)");
		SendDlgItemMessage(hWndDlg,IDC_COMBO_MOD,CB_ADDSTRING,NULL,(LPARAM)"cstrike (Counter-Strike 1.6)");
		SendDlgItemMessage(hWndDlg,IDC_COMBO_MOD,CB_ADDSTRING,NULL,(LPARAM)"dod (Day Of Defeat)");
		SendDlgItemMessage(hWndDlg,IDC_COMBO_MOD,CB_ADDSTRING,NULL,(LPARAM)"tfc (Team Fortress Classic)");
		SendDlgItemMessage(hWndDlg,IDC_COMBO_MOD,CB_SETCURSEL,iTmp,NULL); // select

		// init the custom mod:
		GetPrivateProfileString(g_psCfgAppName,MDTLCFGSTR_mod_custom,"",pcTmp,MAXLEN_HLOPT,g_psLoaderCfg);
		SetDlgItemText(hWndDlg,IDC_EDIT_CMOD,pcTmp);

		// init the custom options:
		GetPrivateProfileString(g_psCfgAppName,MDTLCFGSTR_launchoptions,"-demoedit",pcTmp,MAXLEN_HLOPT,g_psLoaderCfg);
		SetDlgItemText(hWndDlg,IDC_EDIT_UCOPT,pcTmp);

		// select traditional by default:
		CheckRadioButton(hWndDlg,IDC_RADIO_MODET,IDC_RADIO_MODEX,IDC_RADIO_MODET);

		// disable start game button:
		EnableWindow(GetDlgItem(hWndDlg,IDC_BUTTON_START),FALSE);

		// adjust window size: by default we hide the advanced stuff
		RECT rectDlg;
		RECT rectMode;
		if (GetWindowRect(hWndDlg,&rectDlg)&&GetWindowRect(GetDlgItem(hWndDlg,IDC_STATIC_LMODE),&rectMode))
		{
			g_iOrgWindowHeight = rectDlg.bottom-rectDlg.top; // store original height
			MoveWindow(hWndDlg,rectDlg.left,rectDlg.top,rectDlg.right-rectDlg.left,rectDlg.bottom-rectDlg.top-(rectDlg.bottom-rectMode.bottom)+8,FALSE);
		} else g_iOrgWindowHeight=400; // just make s.th. up, I guess we could also get it from the resource directly, don't know


		// adjust the font of the DISCLAIMER
		hBrush = CreateSolidBrush(GetSysColor(CTLCOLOR_DLG));

		hFont = CreateFont(12,
							 0,
							 0,
							 0,
							 FW_NORMAL,
							 FALSE,
							 FALSE,
							 FALSE,
							 ANSI_CHARSET,
							 OUT_DEFAULT_PRECIS,
							 CLIP_DEFAULT_PRECIS,
							 DEFAULT_QUALITY,
							 DEFAULT_PITCH | FF_DONTCARE,
							 "Tahoma");

		SendMessage(GetDlgItem(hWnd, IDC_DISCLAIMER), WM_SETFONT, (WPARAM) hFont, (LPARAM)FALSE);

		// init res forcing:
		iTmp = GetPrivateProfileInt(g_psCfgAppName,MDTLCFGSTR_force_res,0,g_psLoaderCfg);
		CheckDlgButton(hWndDlg,IDC_CHECK_FRES,iTmp==1);
		UpdateResControls(hWndDlg,iTmp==1); // also updates g_bForceRes

		UpdateCurrent(hWndDlg); // update current launchoptions preview

		InitLoader();

		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam)) // Hey Gavin, I think there should be LOWORD here :)
		{
		case IDOK:
		case IDCANCEL:
			// we cannot simply quit here, because EndDialog might quit and destroy handles when the threads are already in their critical live sections and asume them to be valid!
			g_bDebugWanted = false; // indicate we don't want them anymore
			g_bOldLoaderWanted = false; // .
			while(g_bDebugLive || g_bOldLoaderLive)
			{
				if (g_bDebugLive) SetDlgItemText(hWndDlg,IDC_STATUS,"Waiting for threads (Please exit Half-Life!) ...");
				else SetDlgItemText(hWndDlg,IDC_STATUS,"Waiting for threads ...");
				Sleep(250);
			}
			EndDialog(hWndDlg, 0);
			return TRUE;

		case IDC_BUTTON_BROWSE:
			if (GetOpenFileName(&g_ofStruct))
				SetDlgItemText(hWndDlg,IDC_EDIT_GPATH,g_ofStruct.lpstrFile);
			else SetDlgItemText(hWndDlg,IDC_EDIT_GPATH,"Selection failed.");
			return TRUE;

		case IDC_BUTTON_START:
			// first we want to make some checks in order to prevent the user doing some stupid things:
			static char myerrorText[1000];
			bool bAllOk; bAllOk=true;
			char* pTmp; pTmp=myerrorText;

			pTmp+=myStrCpy(pTmp,"Please fix the following things first:\n");

			// Check Resolution:
			if(g_bForceRes)
			{
				// user wants to force a resolution, so check it:

				// Check Width:
				GetDlgItemText(hWndDlg,IDC_EDIT_WIDTH,pTmp,MAXLEN_HLRES+1);
				if (atoi(pTmp)<4)
				{
					pTmp+=myStrCpy(pTmp,"\n    * Width has to be at least 4 pixels");
					bAllOk=false;
				}

				// Check Height:
				GetDlgItemText(hWndDlg,IDC_EDIT_HEIGHT,pTmp,MAXLEN_HLRES+1);
				if (atoi(pTmp)<480)
				{
					pTmp+=myStrCpy(pTmp,"\n    * Height has to be at least 480 pixels");
					bAllOk=false;
				}
			}

			// Check Color Quality:
            if (SendDlgItemMessage(hWndDlg,IDC_COMBO_COLOR,CB_GETCURSEL,NULL,NULL)==0)
			{
				// user has set Custom Color Quality, so check it:
				GetDlgItemText(hWndDlg,IDC_EDIT_CCOLOR,pTmp,MAXLEN_HLRES+1);
				if (atoi(pTmp)<1)
				{
					pTmp+=myStrCpy(pTmp,"\n    * Color Quality has to be at least 1 Bit");
					bAllOk=false;
				}
			}

			// since we use pTmp also temporary there might be crap at the end, so set end to NULL:
			pTmp+=myStrCpy(pTmp,"");

			// update exe path from edit box:
			GetDlgItemText(hWndDlg,IDC_EDIT_GPATH,g_ofStruct.lpstrFile,MAX_PATHFILE_LENGTH);

			if (bAllOk)
			{
				if (InitDebug(hWndDlg))
				{
					EnableWindow(GetDlgItem(hWndDlg,IDC_BUTTON_START),FALSE);

					//
					// save options:
					//
					pTmp[0]=0;

					// HL Exe Path:
					WritePrivateProfileString(g_psCfgAppName,MDTLCFGSTR_hl_exe_path,g_ofStruct.lpstrFile,g_psLoaderCfg);

					// color_dropdown:
					WritePrivateProfileString(g_psCfgAppName,MDTLCFGSTR_color_dropdown,ltoa(SendDlgItemMessage(hWndDlg,IDC_COMBO_COLOR,CB_GETCURSEL,NULL,NULL),pcTmp,10),g_psLoaderCfg);

					// custom bits:
					GetDlgItemText(hWndDlg,IDC_EDIT_CCOLOR,pcTmp,MAXLEN_HLOPT);
					WritePrivateProfileString(g_psCfgAppName,MDTLCFGSTR_color_custom,pcTmp,g_psLoaderCfg);

					// video_dropdown:
					WritePrivateProfileString(g_psCfgAppName,MDTLCFGSTR_video_dropdown,ltoa(SendDlgItemMessage(hWndDlg,IDC_COMBO_DMODE,CB_GETCURSEL,NULL,NULL),pcTmp,10),g_psLoaderCfg);

					// video_width:
					GetDlgItemText(hWndDlg,IDC_EDIT_WIDTH,pcTmp,MAXLEN_HLOPT);
					WritePrivateProfileString(g_psCfgAppName,MDTLCFGSTR_video_width,pcTmp,g_psLoaderCfg);

					// video_height:
					GetDlgItemText(hWndDlg,IDC_EDIT_HEIGHT,pcTmp,MAXLEN_HLOPT);
					WritePrivateProfileString(g_psCfgAppName,MDTLCFGSTR_video_height,pcTmp,g_psLoaderCfg);

					// mod_dropdown:
					WritePrivateProfileString(g_psCfgAppName,MDTLCFGSTR_mod_dropdown,ltoa(SendDlgItemMessage(hWndDlg,IDC_COMBO_MOD,CB_GETCURSEL,NULL,NULL),pcTmp,10),g_psLoaderCfg);

					// custom mod:
					GetDlgItemText(hWndDlg,IDC_EDIT_CMOD,pcTmp,MAXLEN_HLOPT);
					WritePrivateProfileString(g_psCfgAppName,MDTLCFGSTR_mod_custom,pcTmp,g_psLoaderCfg);

					// custom options:
					GetDlgItemText(hWndDlg,IDC_EDIT_UCOPT,pcTmp,MAXLEN_HLOPT);
					WritePrivateProfileString(g_psCfgAppName,MDTLCFGSTR_launchoptions,pcTmp,g_psLoaderCfg);

					// force_res:
					WritePrivateProfileString(g_psCfgAppName,MDTLCFGSTR_force_res,itoa((int)g_bForceRes,pcTmp,10),g_psLoaderCfg);
				}
			} else
				MessageBox(hWndDlg,myerrorText,"MDT - You are not allowed to start yet",MB_OK|MB_ICONHAND);
			return TRUE;

		case IDC_EDIT_WIDTH:
		case IDC_EDIT_HEIGHT:
		case IDC_EDIT_CMOD:
		case IDC_EDIT_UCOPT:
		case IDC_EDIT_CCOLOR:
			if (HIWORD(wParam)==EN_CHANGE)
			{
				UpdateCurrent(hWndDlg); // update current launchoptions preview
				return TRUE;
			}

			break;

		case IDC_RADIO_MODET:
			if (HIWORD(wParam)==BN_CLICKED)
			// user wants to swtich to traditioal mode.
			{
				// disable start game button:
				EnableWindow(GetDlgItem(hWndDlg,IDC_BUTTON_START),FALSE);

				// first the debug thread shall leave the critical section pleasew:
				g_bDebugWanted = false;
				while (g_bDebugLive)
				{
					SetDlgItemText(hWndDlg,IDC_STATUS,"Waiting for threads (Please exit Half-Life!) ...");
					Sleep(300);
				}

				// switch to basic mode:
				// resize window:
				RECT rectDlg;
				RECT rectMode;
				if (GetWindowRect(hWndDlg,&rectDlg)&&GetWindowRect(GetDlgItem(hWndDlg,IDC_STATIC_LMODE),&rectMode))
					MoveWindow(hWndDlg,rectDlg.left,rectDlg.top,rectDlg.right-rectDlg.left,rectDlg.bottom-rectDlg.top-(rectDlg.bottom-rectMode.bottom)+8,TRUE);  // TRUE == repaint!

				// restart loader thread:
				SetDlgItemText(hWndDlg,IDC_STATUS,"(Re-)Starting old loader ...");
				Sleep(250);
				InitLoader();
				return TRUE;
			}
			break;

		case IDC_RADIO_MODEX:
			if (HIWORD(wParam)==BN_CLICKED && !g_bAdvancedMode)
			// user wants to swtich to extended mode.
			{
				// stop the loader thread:
				// Note: there might be a hang or crash if a tard manages to make the old loader hook into hl and switches to advanced mode during that, heh. Yeah yeah I know make it threadsafe blah, we need to get it done.
				g_bOldLoaderWanted = false;
				while (g_bOldLoaderLive)
				{
					SetDlgItemText(hWndDlg,IDC_STATUS,"Waiting for traditional Loader to leave live state ...");
					Sleep(300);
				}
				
				// switch to advanced mode:
				// resize window:
				RECT rectDlg;
				if (GetWindowRect(hWndDlg,&rectDlg))
					MoveWindow(hWndDlg,rectDlg.left,rectDlg.top,rectDlg.right-rectDlg.left,g_iOrgWindowHeight,TRUE); // TRUE == repaint!

				// enable start game button:
				EnableWindow(GetDlgItem(hWndDlg,IDC_BUTTON_START),TRUE);

				UpdateCurrent(hWndDlg); // update current launchoptions preview

				// user friendly message:
				SetDlgItemText(hWndDlg,IDC_STATUS,"Please make your selection:");

				return TRUE;
			}
			break;

		case IDC_COMBO_MOD:
			// s.th. happend top our comobo box, so lets ee what:
			if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				// selection might have changed, take actions:
				lrComboSel = SendDlgItemMessage(hWndDlg,IDC_COMBO_MOD,CB_GETCURSEL,NULL,NULL);

				switch (lrComboSel)
				{
					case 1:
					case 2:
					case 3:
					case 4:
						// we asume return values to be valid (see MS reference for reason):
						EnableWindow(GetDlgItem(hWndDlg,IDC_EDIT_CMOD),FALSE);
						break;
					default:
						// error or whatever or we selected 0=custom:
						EnableWindow(GetDlgItem(hWndDlg,IDC_EDIT_CMOD),TRUE);
				}
				
				UpdateCurrent(hWndDlg); // update current launchoptions preview
				return TRUE;
			}
			break;

		case IDC_CHECK_FRES:
			if (HIWORD(wParam)==BN_CLICKED)
			{
				UpdateResControls(hWndDlg,!g_bForceRes); // also updates g_bForceRes
				UpdateCurrent(hWndDlg); // update current launchoptions preview
				return TRUE;
			}
			break;

		case IDC_COMBO_COLOR:
			if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				lrComboSel = SendDlgItemMessage(hWndDlg,IDC_COMBO_COLOR,CB_GETCURSEL,NULL,NULL);

				if (lrComboSel==0)
				{
					//if(MessageBox(hWndDlg,"We don't recommend Custom Color Quality, are you sure?","MDT - Color Quality selection",MB_OK|MB_ICONQUESTION)==IDYES)
					MessageBox(hWndDlg,"Currently only 16,24 and 32 Bit Per Pixel are supported.\nBut you can try :-)","MDT - Color Quality selection",MB_OK|MB_ICONINFORMATION);
						EnableWindow(GetDlgItem(hWndDlg,IDC_EDIT_CCOLOR),true);
					//else
					//{
					//	EnableWindow(GetDlgItem(hWndDlg,IDC_EDIT_CCOLOR),false);
					//	SendDlgItemMessage(hWndDlg,IDC_COMBO_COLOR,CB_SETCURSEL,3,NULL); // select 32 Bit
					//}
				} else
					EnableWindow(GetDlgItem(hWndDlg,IDC_EDIT_CCOLOR),false);

				UpdateCurrent(hWndDlg); // update current launchoptions preview

				return TRUE;
			}
			break;

		case IDC_COMBO_DMODE:
			if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				lrComboSel = SendDlgItemMessage(hWndDlg,IDC_COMBO_DMODE,CB_GETCURSEL,NULL,NULL);

				if (lrComboSel==1)
				{
					if(MessageBox(hWndDlg,"Warning: You want to select Fullscreen Display Mode.\nThis might not work or even can damage your graphic card or monitor!\n\nAre you sure?","MDT - Display Mode selection",MB_YESNO|MB_ICONEXCLAMATION)!=IDYES)
						SendDlgItemMessage(hWndDlg,IDC_COMBO_DMODE,CB_SETCURSEL,0,NULL); // select windowed
				}

				UpdateCurrent(hWndDlg); // update current launchoptions preview

			}
			break;

		}		// case WM_COMMAND: switch(...
		break;	//

	case WM_DESTROY:
		// we cannot simply quit here, because EndDialog might quit and destroy handles when the threads are already in their critical live sections and asume them to be valid!
		g_bDebugWanted = false;
		g_bOldLoaderWanted = false;
		while(g_bDebugLive || g_bOldLoaderLive)
		{
			if (g_bDebugLive) SetDlgItemText(hWndDlg,IDC_STATUS,"Waiting for threads (Please exit Half-Life!) ...");
			else SetDlgItemText(hWndDlg,IDC_STATUS,"Waiting for threads ...");
			Sleep(250);
		}
		EndDialog(hWndDlg, 0);
		return TRUE;

	case WM_CTLCOLORDLG:
        return (LRESULT) hBrush;

	case WM_CTLCOLORSTATIC:
		HWND hWnd = (HWND) lParam;
		HDC hDC = (HDC) wParam;

		if (GetWindowLong(hWnd, GWL_ID) == IDC_DISCLAIMER)
		{
			SetTextColor(hDC, GetSysColor(COLOR_GRAYTEXT));
			SetBkMode(hDC, TRANSPARENT);
			return (LRESULT) hBrush;
		}
	}

	return FALSE;
}

char* CreateDllPath()
// Desc.  : This function creates the complete path/file to the mdt dll
// Returns: Pointer onto the NULL terminated (ANSI) string or NULL on error.
// Remarks: You have to free the memory allocated with the pointer afterwards!
{
	size_t iLmdt = strlen(g_psMDTpath);
	size_t iLdll = strlen(DLL_NAME);
	char* pcDLLpfile = (char *)malloc(iLmdt+iLdll+2);
	if (!pcDLLpfile)
		return NULL;

	// construct PATH/DLL\0:
	char* pTmp=pcDLLpfile;
	memcpy(pTmp,g_psMDTpath,iLmdt); pTmp += iLmdt;
	memcpy(pTmp,"\\",1); pTmp++;
	memcpy(pTmp,DLL_NAME,iLdll); pTmp += iLdll;
	memcpy(pTmp,"",1); pTmp++;

	return pcDLLpfile;
}

DWORD WINAPI LoaderThread(void *p)
{
	static char szBuffer[1024];
	static char *pszAnim[] = { ".", "..", "...", "...." };
	static int nAnim = sizeof(pszAnim) / sizeof(pszAnim[0]);
	static int iAnim = 0;

	for(; hWnd && g_bOldLoaderWanted; Sleep(200)) // only as long as our window is open and we are ment to be running
	{
		g_bOldLoaderLive = true; // keep it true in case some tard breaks the code a bit
		
		DWORD dwProcessId = GetProcessId("hl.exe");

		if (dwProcessId != 0)
		{
			if (hWnd)
				SetDlgItemText(hWnd, IDC_STATUS, "Loading MDT into Half-Life");

			// Injecting too quickly will crash HL
			Sleep(100);

			// Get the full path to the dll
			//DWORD nLen = GetCurrentDirectory(1024, szBuffer);
			//sprintf(szBuffer + nLen, "\\%s", DLL_NAME);
			char* tDLLpath=CreateDllPath();

			if (InjectDll(dwProcessId, tDLLpath))
			{
				Sleep(3000);
				g_bOldLoaderLive=false;
				SendMessage(hWnd, WM_DESTROY, 0, 0);

				free(tDLLpath);

				return 1;
			}

				free(tDLLpath);
		}

		if (hWnd)
		{
			sprintf(szBuffer, "Waiting for Half-Life%s", pszAnim[iAnim++ % nAnim]);
			SetDlgItemText(hWnd, IDC_STATUS, szBuffer);
		}
	}

	g_bOldLoaderLive=false;

	return 0;
}

const char *GetPluginVersion(const char *pszName)
{
	static char szVersion[128] = { 0 };

	VS_FIXEDFILEINFO *lpFFI;
	DWORD dwBufSize;
	char *lpVI;

	DWORD dwLen = GetFileVersionInfoSize(pszName, &dwBufSize);

	if (dwLen == 0)
		return NULL;

	lpVI = (LPTSTR) GlobalAlloc(GPTR, dwLen);

	GetFileVersionInfo(pszName, NULL, dwLen, lpVI);

	if (VerQueryValue(lpVI, _T("\\"), (LPVOID *) &lpFFI, (UINT *) &dwBufSize))
	{
		sprintf(szVersion, "%d.%d.%d.%d", HIWORD(lpFFI->dwFileVersionMS),
											LOWORD(lpFFI->dwFileVersionMS),
											HIWORD(lpFFI->dwFileVersionLS),
											LOWORD(lpFFI->dwFileVersionLS));
	}
	else
		return NULL;

	GlobalFree((HGLOBAL) lpVI);

	return szVersion;
}

/*/
///
//// by Micorsoft Software Developer Network Library, doesn't work in Win98 etc.
//// http://msdn2.microsoft.com/en-us/library/aa366789.aspx

#include <psapi.h>
//#include <strsafe.h>

#define BUFSIZE 512

BOOL GetFileNameFromHandle(HANDLE hFile) 
{
  BOOL bSuccess = FALSE;
  TCHAR pszFilename[MAX_PATH+1];
  HANDLE hFileMap;

  // Get the file size.
  DWORD dwFileSizeHi = 0;
  DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi); 

  if( dwFileSizeLo == 0 && dwFileSizeHi == 0 )
  {
     printf("Cannot map a file with a length of zero.\n");
     return FALSE;
  }

  // Create a file mapping object.
  hFileMap = CreateFileMapping(hFile, 
                    NULL, 
                    PAGE_READONLY,
                    0, 
                    MAX_PATH,
                    NULL);

  if (hFileMap) 
  {
    // Create a file mapping to get the file name.
    void* pMem = MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);

    if (pMem) 
    {
      if (GetMappedFileName (GetCurrentProcess(), 
                             pMem, 
                             pszFilename,
                             MAX_PATH)) 
      {

        // Translate path with device name to drive letters.
        TCHAR szTemp[BUFSIZE];
        szTemp[0] = '\0';

        if (GetLogicalDriveStrings(BUFSIZE-1, szTemp)) 
        {
          TCHAR szName[MAX_PATH];
          TCHAR szDrive[3] = TEXT(" :");
          BOOL bFound = FALSE;
          TCHAR* p = szTemp;

          do 
          {
            // Copy the drive letter to the template string
            *szDrive = *p;

            // Look up each device name
            if (QueryDosDevice(szDrive, szName, BUFSIZE))
            {
              UINT uNameLen = _tcslen(szName);

              if (uNameLen < MAX_PATH) 
              {
                bFound = _tcsnicmp(pszFilename, szName, 
                    uNameLen) == 0;

                if (bFound) 
                {
                  // Reconstruct pszFilename using szTempFile
                  // Replace device path with DOS path
                  TCHAR szTempFile[MAX_PATH];
                  StringCchPrintf(szTempFile,
                            MAX_PATH,
                            TEXT("%s%s"),
                            szDrive,
                            pszFilename+uNameLen);
                  StringCchCopyN(pszFilename, MAX_PATH+1, szTempFile, _tcslen(szTempFile));
                }
              }
            }

            // Go to the next NULL character.
            while (*p++);
          } while (!bFound && *p); // end of string
        }
      }
      bSuccess = TRUE;
      UnmapViewOfFile(pMem);
    } 

    CloseHandle(hFileMap);
  }
  _tprintf(TEXT("File name is %s\n"), pszFilename);
  return(bSuccess);
}

////
/*/


#define DEBUGTHREAD_DLLS_BEFORE_HOOK 4

DWORD WINAPI DebugThread(void *p)
{
	static char szBuffer[1024];
	static char *pszAnim[] = { ".", "..", "...", "...." };
	static int nAnim = sizeof(pszAnim) / sizeof(pszAnim[0]);
	static int iAnim = 0;

	int iDllsPassed=0;

	g_bDebugLive = true; // keep it true in case some tard breaks the code a bit

	//
	// Phase 1: Launch Half-Life
	//
	//	EnableDebugPrivilege(true);
	// Note: The CreateProcess call has to be made by the thread that want's to Debug!

	// dynamically (why - I don't know -> lol?) create the DLL path/file:
	char* pcDLLpfile=CreateDllPath();
	if(!pcDLLpfile)
	{
		SetDlgItemText(hWnd, IDC_STATUS,"ERROR: MEMORY ALLOC. FAILED!");
		g_bDebugLive = false;
		return FALSE;
	}

	// check if the DLL realy exists:
	if (DoesFileExist(pcDLLpfile) == FALSE)
	{
		SetDlgItemText(hWnd, IDC_STATUS, DLL_NAME " not found!");
		// don't forget to free:
		free(pcDLLpfile);
		g_bDebugLive = false;
		return FALSE;
	}

	const char *pszVersion = GetPluginVersion(pcDLLpfile);

	// don't forget to free:
	free(pcDLLpfile);

	if (pszVersion)
	{
		char buf[512];
		sprintf(buf, "Mirv Demo Tool (DLL v%s)", pszVersion);
		SetWindowText(hWnd, buf);
	}

	// calculate path component (so we can call HL in the correct directory):
	static char psHLpath[MAX_PATHFILE_LENGTH];
	char* pcTmp=psHLpath;

	//cpy:
	memcpy(psHLpath,g_psHLexe,MAX_PATHFILE_LENGTH);

	// search for delimiter and 'cut' application file off:
	if (pcTmp=strrchr(psHLpath,'\\')) *pcTmp=0; // cut string with \0
	else if(pcTmp=strrchr(psHLpath,'/')) *pcTmp=0; // cut string \0
	else *psHLpath=0; // cut sring with \0, means empty


	// cunstruct the final launchoptions: "\"PATHWITHEXE\" LAUNCHOPTIONS"
	static char	psHLfinalStr [MAX_PATHFILE_LENGTH + MAXLEN_HLOPT + 10]; // "PATHSTRING" LAUNCHOPTIONSSTRING
	pcTmp = psHLfinalStr;
	pcTmp+=myStrCpy(pcTmp,"\"");
	pcTmp+=myStrCpy(pcTmp,g_psHLexe);
	pcTmp+=myStrCpy(pcTmp,"\" ");
	pcTmp+=myStrCpy(pcTmp,g_pHLOString);
	pcTmp+=myStrCpy(pcTmp,""); // not needed here, but won't hurt
	

	// there should be some detecton if HL is already running here:
	// and if it can be launched (file exists):
	
	//
	// not implemented yet.
	//

	// Launch Half-Life:
    ZeroMemory( &g_HLsi, sizeof(g_HLsi) );
	//GetStartupInfo(&g_HLsi);
    g_HLsi.cb = sizeof(g_HLsi);
    ZeroMemory( &g_HLpi, sizeof(g_HLpi) );

	SetDlgItemText(hWnd, IDC_STATUS, "Launching Half-Life ...");

	if(!CreateProcess(
		g_psHLexe,
		psHLfinalStr,
		NULL,
		NULL,
		FALSE,
			//CREATE_DEFAULT_ERROR_MODE|
			CREATE_NEW_PROCESS_GROUP|
			DETACHED_PROCESS|
			CREATE_SUSPENDED
			//DEBUG_ONLY_THIS_PROCESS|
			//DEBUG_PROCESS				// we want to catch debug event's (sadly also of childs)
			,
		NULL,
		psHLpath,
		&g_HLsi,
		&g_HLpi)  )
	{
		SetDlgItemText(hWnd, IDC_STATUS, "ERROR: Failed to launch Half-Life.");
		g_bDebugLive = false;
		return false;
	}

	char* tDLLpath=CreateDllPath();
	if (!InjectDll(g_HLpi.dwProcessId, tDLLpath))
		SetDlgItemText(hWnd, IDC_STATUS,"ERROR: Starting injection failed!");

	Sleep(6000);

	ResumeThread(g_HLpi.hThread);

	g_bDebugLive = false;
	SendMessage(hWnd, WM_DESTROY, 0, 0); // close launcher

	return false;




	//
	//
	// note: bellow here is not processed i just left it in for now
	//
	//


	//
	// Phase 2: Wait for an event where we can do our fast injection
	//

	SetDlgItemText(hWnd, IDC_STATUS,"Preparing injection");

	// we wait until ddraw.dll is loaded:

	bool bOpenFire=false;
	DEBUG_EVENT dbgEvent;
	HANDLE hProcessImageFile=NULL;

	BOOL bEvent=TRUE; // I'm such a tard lol, I had FALSE here lololl

	DWORD dwProcessid=g_HLpi.dwProcessId; // by default current

	bool bIntial=true;

	DWORD dwDebugContinueStatus;

	while (!bOpenFire && bEvent)
	{
		bEvent=WaitForDebugEvent(&dbgEvent,INFINITE);
		dwDebugContinueStatus=DBG_EXCEPTION_NOT_HANDLED;

		if (bEvent)
		{
			switch(dbgEvent.dwDebugEventCode)
			{
			case CREATE_PROCESS_DEBUG_EVENT:
				dwProcessid = dbgEvent.dwProcessId; bIntial = false;
				hProcessImageFile = dbgEvent.u.CreateProcessInfo.hFile;

				//sprintf(szBuffer, "Sartaddr: 0x%08x %s",dbgEvent.u.CreateProcessInfo.lpStartAddress, pszAnim[iAnim++ % nAnim]);
				sprintf(szBuffer, "PROCESSID: %i %s",dwProcessid, pszAnim[iAnim++ % nAnim]);
				SetDlgItemText(hWnd,IDC_STATUS,szBuffer);
				Sleep(4000);

				CloseHandle(hProcessImageFile); // similar to as was advised by Microsoft in the WaitForDebugEvent WIN32 doc
				break;
			case EXIT_PROCESS_DEBUG_EVENT:
				//even that seems to be faked by HL, totally crzy, or they do kill the mainthread, don't know.
				if(dbgEvent.dwProcessId==g_HLpi.dwProcessId) bEvent=false; // HL messes again around with us ...
				break;
			case LOAD_DLL_DEBUG_EVENT:
				//if(dbgEvent.u.LoadDll.hFile!=NULL)
				//{
				//	// we have an DLL handle
				//	if(dbgEvent.u.LoadDll.hFile!=NULL)
				//	{
				//		PIMAGE_DOS_HEADER pDosHeader= (PIMAGE_DOS_HEADER)dbgEvent.u.LoadDll.hFile;
				//
				//		SetDlgItemText(hWnd, IDC_STATUS, szBuffer);
				//		//if(!lstrcmp((LPCSTR)dbgEvent.u.LoadDll.lpImageName,"DDRAW.DLL")) bOpenFire=true;
				//		Sleep(2000);
				//	} else {
				//		GetLastError(); // we caused it, so get it out of the way
				//		sprintf(szBuffer, "Preparing injection%s", pszAnim[iAnim++ % nAnim]);
				//		SetDlgItemText(hWnd, IDC_STATUS, szBuffer);
				//		Sleep(200);
				//	}
				//}

				//sprintf(szBuffer, "DLLID: %i %s",dwProcessid, pszAnim[iAnim++ % nAnim]);
				//SetDlgItemText(hWnd,IDC_STATUS,szBuffer);
				//Sleep(4000);

				iDllsPassed++; // count loaded dll's             
				if (iDllsPassed>= DEBUGTHREAD_DLLS_BEFORE_HOOK ) bOpenFire=true; // we want to hook

				CloseHandle(dbgEvent.u.LoadDll.hFile); // as advised by Microsoft
				break;
			case EXCEPTION_DEBUG_EVENT:
				dwDebugContinueStatus=DBG_CONTINUE; // fetch debug errors created by HL on intention
				break;
			}
			if (!bOpenFire) ContinueDebugEvent(dbgEvent.dwProcessId,dbgEvent.dwThreadId,dwDebugContinueStatus);
		}
	}

	//InitLoader();

	//Sleep(10000);

	//dwProcessid = GetProcessId("hl.exe");

	if (!bEvent)
	{
		// we came out of the loop due to an error with Debugging or didn't find our dll
		DWORD iLast = GetLastError();
		sprintf(szBuffer, "ERROR(%i): Preparing injection failed!", iLast);
		SetDlgItemText(hWnd, IDC_STATUS,szBuffer);
		g_bDebugLive=false;
		return 0;
	}

	//bOpenFire=false;

	if (bOpenFire)
	{

		//
		// Phase 3: Inject DLL
		//

		// WARNING: this phase is not properly worked out yet, we could end up in deadlocks if our dll has to wait for the mainprocess and causes debugevents for it
		SetDlgItemText(hWnd, IDC_STATUS,"Starting injection ...");

		char* tDLLpath=CreateDllPath();
		if (!InjectDll(dwProcessid, tDLLpath))
			SetDlgItemText(hWnd, IDC_STATUS,"ERROR: Starting injection failed!");
		else SetDlgItemText(hWnd, IDC_STATUS,"Injecting ...");
		free(tDLLpath);

		////InitLoader();
	} else SetDlgItemText(hWnd, IDC_STATUS,"ERROR: Injection point not catched!");

	// WARNING THERE HAD TO BE SOME SYNCING HERE I GUESS!
	// we hope that mdt injection will finish during that time:
	//Sleep(10000);

	// we try to resume the main hl thread:
	ContinueDebugEvent(dbgEvent.dwProcessId,dbgEvent.dwThreadId,DBG_EXCEPTION_NOT_HANDLED);

	//
	// Phase 4: waiting for HL to finish
	//

	// Our debugger stays attached until HL finishes, I am not sure if we could deattach it during runtime.

	SetDlgItemText(hWnd, IDC_STATUS,"Half-Life is running ...");

	// catch debug event's cuz wee need to handle some : /

	bEvent=TRUE;
    
	while (bEvent)
	{
		bEvent=WaitForDebugEvent(&dbgEvent,INFINITE);

		dwDebugContinueStatus=DBG_EXCEPTION_NOT_HANDLED;

		if (bEvent)
		{
			switch(dbgEvent.dwDebugEventCode)
			{
			case CREATE_PROCESS_DEBUG_EVENT:
				// this event shouldn't occour anymore
				CloseHandle(hProcessImageFile); // as advised by Microsoft in the WaitForDebugEvent WIN32 doc
				break;
			case EXIT_PROCESS_DEBUG_EVENT:
				//HL fakes this ... if(dbgEvent.dwProcessId==g_HLpi.dwProcessId) bEvent=false; // HL messes again around with us ...
				break;
			case LOAD_DLL_DEBUG_EVENT:
				CloseHandle(dbgEvent.u.LoadDll.hFile); // as advised by Microsoft
				break;
			case EXCEPTION_DEBUG_EVENT:
				dwDebugContinueStatus=DBG_CONTINUE; // fetch debug errors created by HL on intention
				break;
			}
			
			ContinueDebugEvent(dbgEvent.dwProcessId,dbgEvent.dwThreadId,dwDebugContinueStatus);// DBG_CONTINUE);
		}
	}


/*    // Wait until child process exits. // that is what MSDN says heh
    WaitForSingleObject( g_HLpi.hProcess, INFINITE );*/

    // Close process and thread handles. 
    CloseHandle( g_HLpi.hProcess );
    CloseHandle( g_HLpi.hThread );

	g_bDebugLive=false;
	SetDlgItemText(hWnd, IDC_STATUS,"Half-Life has quit.");

	return 1;

	//
	//
	// note: above here until preceding note is not processed i just left it in for now
	//
	//

}

VOID InitLoader()
{
	char* pcDLLpfile=CreateDllPath();
	if(pcDLLpfile==NULL)
	{
		SetDlgItemText(hWnd, IDC_STATUS,"ERROR: MEMORY ALLOC. FAILED!");
		return;
	}

	if (DoesFileExist(pcDLLpfile) == FALSE)
	{
		SetDlgItemText(hWnd, IDC_STATUS, DLL_NAME " not found!");
		// don't forget to free:
		free(pcDLLpfile);
		return;
	}

	const char *pszVersion = GetPluginVersion(pcDLLpfile);

	// don't forget to free:
	free(pcDLLpfile);

	if (pszVersion)
	{
		char buf[512];
		sprintf(buf, "Mirv Demo Tool (DLL v%s)", pszVersion);
		SetWindowText(hWnd, buf);
	}

	g_bOldLoaderWanted = true; // since we called InitLoader we probably also want it running
	g_bOldLoaderLive = true; // indicate loader is live
	HANDLE hThread = CreateThread(0, 0, LoaderThread, 0, 0, 0);
}

bool InitDebug(HWND hWndDlg)
{
	g_bDebugWanted = true;
	g_bDebugLive = true; // indicate debugging thread is live
	HANDLE hThread = CreateThread(0, 0, DebugThread, 0, 0, 0);

	return true;
}

DWORD GetProcessId(LPCTSTR lpName)
{
	HANDLE hAllProc = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 hProcEntry;
	hProcEntry.dwSize = sizeof(PROCESSENTRY32);
	hAllProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hAllProc == INVALID_HANDLE_VALUE)
		return 0;

	if (Process32First(hAllProc, &hProcEntry))
	{
		while (1)
		{
			if (hProcEntry.th32ProcessID != 0)
			if (!stricmp(hProcEntry.szExeFile, lpName))
			{
				CloseHandle(hAllProc);
				return hProcEntry.th32ProcessID;
			}

            if (!Process32Next(hAllProc, &hProcEntry))
				break;
		}
	}

	CloseHandle(hAllProc);
	return 0;
}

#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)

HRESULT InjectDll(DWORD pID, LPCTSTR dllName)
{
	HANDLE hProc = OpenProcess(CREATE_THREAD_ACCESS, FALSE, pID);

	if (!hProc)
		return false;

	LPVOID lLoadLibraryAddr = (LPVOID) GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");	

	if (!lLoadLibraryAddr)
	{
		CloseHandle(hProc);
		return false;
	}

	LPVOID lArgAddress = VirtualAllocEx(hProc, NULL, strlen(dllName) + 1, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE); 

	if (!lArgAddress)
	{
		CloseHandle(hProc);
		return false;
	}

	BOOL bResult = WriteProcessMemory(hProc, lArgAddress, dllName, strlen(dllName) + 1, NULL);

	if (!bResult)
	{
		VirtualFreeEx(hProc, lArgAddress, strlen(dllName) + 1, MEM_RELEASE|MEM_DECOMMIT);
		CloseHandle(hProc);
		return false;
	}
	
	HANDLE hThread = CreateRemoteThread(hProc, NULL, NULL, (LPTHREAD_START_ROUTINE) lLoadLibraryAddr, lArgAddress, NULL, NULL);

	if (!hThread)
	{
		VirtualFreeEx(hProc, lArgAddress, strlen(dllName) + 1, MEM_RELEASE|MEM_DECOMMIT);
		CloseHandle(hProc);
		return false;
	}

	VirtualFreeEx(hProc, lArgAddress, strlen(dllName) + 1, MEM_RELEASE|MEM_DECOMMIT);
	CloseHandle(hProc);

	return true;
}

BOOL DoesFileExist(LPCTSTR pszName)
{
	FILE *pFile = fopen(pszName, "r"); // was DLL_NAME before
	if (!pFile)
		return FALSE;
	fclose(pFile);
	return TRUE;
}
