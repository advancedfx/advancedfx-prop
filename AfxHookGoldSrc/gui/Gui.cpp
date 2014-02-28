#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2014-02-21 dominik.matrixstorm.com
//
// First changes
// 2014-02-13 dominik.matrixstorm.com

#ifdef AFX_GUI

#include "Gui.h"

#include <Rocket/Core.h>
#include <GL/GL.h>
#include "GuiFileInterface.h"
#include "GuiRenderInterfaceOpenGL.h"
#include "GuiSystemInterface.h"


class Gui
{
public:
	Gui();
	~Gui();

	bool HandleSdlEvent(SDL_Event * event);
	void Render();
	void SetActive(bool value);
    void SetViewport(int width, int height);

private:
	bool m_Active;
	Rocket::Core::Context* m_Context;
	int m_Height;
	int m_KeyModifierState;
	int m_Width;
	struct {
		int x;
		int y;
	} mouse, old_mouse;
	bool activity;
	bool activityRead;

	void LoadFonts();
};

Gui * g_Gui;

void glSetEnabled(GLenum cap, GLboolean mode)
{
	(mode ? glEnable(cap) : glDisable(cap));
}


bool AfxGui_HandleSdlEvent(SDL_Event * event)
{
	if(g_Gui) return g_Gui->HandleSdlEvent(event);

	return false;
}

void AfxGui_Init()
{
	if(!g_Gui) g_Gui = new Gui();
}

void AfxGui_Render()
{
	if(g_Gui) g_Gui->Render();
}

void AfxGui_SetActive(bool value)
{
	if(g_Gui) g_Gui->SetActive(value);
}

void AfxGui_SetViewport(int width, int height)
{
	if(g_Gui) g_Gui->SetViewport(width, height);
}

void AfxGui_ShutDown()
{
	if(g_Gui) delete g_Gui;
	g_Gui = 0;
}

int SdlMouseButtonToRocketButtonIndex(Uint8 button)
{
	switch(button)
	{
	case SDL_BUTTON_LEFT:
		return 0;
	case SDL_BUTTON_MIDDLE:
		return 2;
	case SDL_BUTTON_RIGHT:
		return 1;
	case SDL_BUTTON_X1:
		return 3;
	case SDL_BUTTON_X2:
		return 4;
	}

	return 5;
}

Rocket::Core::Input::KeyIdentifier
SdlKeyCodeToRocketKeyIdentifier(SDL_Keycode keyCode)
{
	switch(keyCode)
	{
	case SDLK_UNKNOWN: return Rocket::Core::Input::KI_UNKNOWN;

	case SDLK_RETURN: return Rocket::Core::Input::KI_RETURN;
	case SDLK_ESCAPE: return Rocket::Core::Input::KI_ESCAPE;
	case SDLK_BACKSPACE: return Rocket::Core::Input::KI_BACK;
	case SDLK_TAB: return Rocket::Core::Input::KI_TAB;
	case SDLK_SPACE: return Rocket::Core::Input::KI_SPACE;
	case SDLK_EXCLAIM: return Rocket::Core::Input::KI_1; //! translated to lower case US layout
	case SDLK_QUOTEDBL: return Rocket::Core::Input::KI_2; //! translated to lower case US layout
    case SDLK_HASH: return Rocket::Core::Input::KI_3; //! translated to lower case US layout
    case SDLK_PERCENT: return Rocket::Core::Input::KI_5; //! translated to lower case US layout
    case SDLK_DOLLAR: return Rocket::Core::Input::KI_4; //! translated to lower case US layout
	case SDLK_AMPERSAND: return Rocket::Core::Input::KI_7; //! translated to lower case US layout
	case SDLK_QUOTE: return Rocket::Core::Input::KI_OEM_7;
    case SDLK_LEFTPAREN: return Rocket::Core::Input::KI_9; //! translated to lower case US layout
	case SDLK_RIGHTPAREN: return Rocket::Core::Input::KI_0; //! translated to lower case US layout
	case SDLK_ASTERISK: return Rocket::Core::Input::KI_8; //! translated to lower case US layout
	case SDLK_PLUS: return Rocket::Core::Input::KI_OEM_PLUS; //! translated to lower case US layout
	case SDLK_COMMA: return Rocket::Core::Input::KI_OEM_COMMA;
	case SDLK_MINUS: return Rocket::Core::Input::KI_OEM_MINUS;
	case SDLK_PERIOD: return Rocket::Core::Input::KI_OEM_PERIOD;
	case SDLK_SLASH: return Rocket::Core::Input::KI_OEM_2;
	case SDLK_0: return Rocket::Core::Input::KI_0;
    case SDLK_1: return Rocket::Core::Input::KI_1;
    case SDLK_2: return Rocket::Core::Input::KI_2;
    case SDLK_3: return Rocket::Core::Input::KI_3;
    case SDLK_4: return Rocket::Core::Input::KI_4;
    case SDLK_5: return Rocket::Core::Input::KI_5;
    case SDLK_6: return Rocket::Core::Input::KI_6;
    case SDLK_7: return Rocket::Core::Input::KI_7;
    case SDLK_8: return Rocket::Core::Input::KI_8;
    case SDLK_9: return Rocket::Core::Input::KI_9;
	case SDLK_COLON: return Rocket::Core::Input::KI_OEM_1; //! translated to lower case US layout
	case SDLK_SEMICOLON: return Rocket::Core::Input::KI_OEM_1;
	case SDLK_LESS: return Rocket::Core::Input::KI_OEM_COMMA; //! translated to lower case US layout
	case SDLK_EQUALS: return Rocket::Core::Input::KI_OEM_PLUS;
	case SDLK_GREATER: return Rocket::Core::Input::KI_OEM_PERIOD; //! translated to lower case US layout
	case SDLK_QUESTION: return Rocket::Core::Input::KI_OEM_2; //! translated to lower case US layout
	case SDLK_AT: return Rocket::Core::Input::KI_2; //! translated to lower case US layout

	case SDLK_LEFTBRACKET: return Rocket::Core::Input::KI_OEM_4;
	case SDLK_BACKSLASH: return Rocket::Core::Input::KI_OEM_5;
	case SDLK_RIGHTBRACKET: return Rocket::Core::Input::KI_OEM_6;
	case SDLK_CARET: return Rocket::Core::Input::KI_6; //! translated to lower case US layout
	case SDLK_UNDERSCORE: return Rocket::Core::Input::KI_OEM_MINUS; //! translated to lower case US layout
	case SDLK_BACKQUOTE: return Rocket::Core::Input::KI_OEM_3;
	case SDLK_a: return Rocket::Core::Input::KI_A;
    case SDLK_b: return Rocket::Core::Input::KI_B;
    case SDLK_c: return Rocket::Core::Input::KI_C;
    case SDLK_d: return Rocket::Core::Input::KI_D;
    case SDLK_e: return Rocket::Core::Input::KI_E;
    case SDLK_f: return Rocket::Core::Input::KI_F;
    case SDLK_g: return Rocket::Core::Input::KI_G;
    case SDLK_h: return Rocket::Core::Input::KI_H;
    case SDLK_i: return Rocket::Core::Input::KI_I;
    case SDLK_j: return Rocket::Core::Input::KI_J;
    case SDLK_k: return Rocket::Core::Input::KI_K;
    case SDLK_l: return Rocket::Core::Input::KI_L;
    case SDLK_m: return Rocket::Core::Input::KI_M;
    case SDLK_n: return Rocket::Core::Input::KI_N;
    case SDLK_o: return Rocket::Core::Input::KI_O;
    case SDLK_p: return Rocket::Core::Input::KI_P;
    case SDLK_q: return Rocket::Core::Input::KI_Q;
    case SDLK_r: return Rocket::Core::Input::KI_R;
    case SDLK_s: return Rocket::Core::Input::KI_S;
    case SDLK_t: return Rocket::Core::Input::KI_T;
    case SDLK_u: return Rocket::Core::Input::KI_U;
    case SDLK_v: return Rocket::Core::Input::KI_V;
    case SDLK_w: return Rocket::Core::Input::KI_W;
    case SDLK_x: return Rocket::Core::Input::KI_X;
    case SDLK_y: return Rocket::Core::Input::KI_Y;
    case SDLK_z: return Rocket::Core::Input::KI_Z;

	case SDLK_CAPSLOCK: return Rocket::Core::Input::KI_CAPITAL;

	case SDLK_F1: return Rocket::Core::Input::KI_F1;
    case SDLK_F2: return Rocket::Core::Input::KI_F2;
    case SDLK_F3: return Rocket::Core::Input::KI_F3;
    case SDLK_F4: return Rocket::Core::Input::KI_F4;
    case SDLK_F5: return Rocket::Core::Input::KI_F5;
    case SDLK_F6: return Rocket::Core::Input::KI_F6;
    case SDLK_F7: return Rocket::Core::Input::KI_F7;
    case SDLK_F8: return Rocket::Core::Input::KI_F8;
    case SDLK_F9: return Rocket::Core::Input::KI_F9;
    case SDLK_F10: return Rocket::Core::Input::KI_F10;
    case SDLK_F11: return Rocket::Core::Input::KI_F11;
    case SDLK_F12: return Rocket::Core::Input::KI_F12;

    case SDLK_PRINTSCREEN: return Rocket::Core::Input::KI_SNAPSHOT;
    case SDLK_SCROLLLOCK: return Rocket::Core::Input::KI_SCROLL;
    case SDLK_PAUSE: return Rocket::Core::Input::KI_PAUSE;
    case SDLK_INSERT: return Rocket::Core::Input::KI_INSERT;
    case SDLK_HOME: return Rocket::Core::Input::KI_HOME;
    case SDLK_PAGEUP: return Rocket::Core::Input::KI_PRIOR;
    case SDLK_DELETE: return Rocket::Core::Input::KI_DELETE;
    case SDLK_END: return Rocket::Core::Input::KI_END;
    case SDLK_PAGEDOWN: return Rocket::Core::Input::KI_NEXT;
    case SDLK_RIGHT: return Rocket::Core::Input::KI_RIGHT;
    case SDLK_LEFT: return Rocket::Core::Input::KI_LEFT;
    case SDLK_DOWN: return Rocket::Core::Input::KI_DOWN;
    case SDLK_UP: return Rocket::Core::Input::KI_UP;

    case SDLK_NUMLOCKCLEAR: return Rocket::Core::Input::KI_NUMLOCK;
    case SDLK_KP_DIVIDE: return Rocket::Core::Input::KI_DIVIDE;
    case SDLK_KP_MULTIPLY: return Rocket::Core::Input::KI_MULTIPLY;
    case SDLK_KP_MINUS: return Rocket::Core::Input::KI_SUBTRACT;
    case SDLK_KP_PLUS: return Rocket::Core::Input::KI_ADD;
    case SDLK_KP_ENTER: return Rocket::Core::Input::KI_NUMPADENTER;
    case SDLK_KP_1: return Rocket::Core::Input::KI_NUMPAD1;
    case SDLK_KP_2: return Rocket::Core::Input::KI_NUMPAD2;
    case SDLK_KP_3: return Rocket::Core::Input::KI_NUMPAD3;
    case SDLK_KP_4: return Rocket::Core::Input::KI_NUMPAD4;
    case SDLK_KP_5: return Rocket::Core::Input::KI_NUMPAD5;
    case SDLK_KP_6: return Rocket::Core::Input::KI_NUMPAD6;
    case SDLK_KP_7: return Rocket::Core::Input::KI_NUMPAD7;
    case SDLK_KP_8: return Rocket::Core::Input::KI_NUMPAD8;
    case SDLK_KP_9: return Rocket::Core::Input::KI_NUMPAD9;
    case SDLK_KP_0: return Rocket::Core::Input::KI_NUMPAD0;
    case SDLK_KP_PERIOD: return Rocket::Core::Input::KI_DECIMAL;

    case SDLK_APPLICATION: return Rocket::Core::Input::KI_APPS;
    case SDLK_POWER: return Rocket::Core::Input::KI_POWER;
    case SDLK_KP_EQUALS: return Rocket::Core::Input::KI_OEM_NEC_EQUAL;
    case SDLK_F13: return Rocket::Core::Input::KI_F13;
    case SDLK_F14: return Rocket::Core::Input::KI_F14;
    case SDLK_F15: return Rocket::Core::Input::KI_F15;
    case SDLK_F16: return Rocket::Core::Input::KI_F16;
    case SDLK_F17: return Rocket::Core::Input::KI_F17;
    case SDLK_F18: return Rocket::Core::Input::KI_F18;
    case SDLK_F19: return Rocket::Core::Input::KI_F19;
    case SDLK_F20: return Rocket::Core::Input::KI_F20;
    case SDLK_F21: return Rocket::Core::Input::KI_F21;
    case SDLK_F22: return Rocket::Core::Input::KI_F22;
    case SDLK_F23: return Rocket::Core::Input::KI_F23;
    case SDLK_F24: return Rocket::Core::Input::KI_F24;
    case SDLK_EXECUTE: return Rocket::Core::Input::KI_EXECUTE;
    case SDLK_HELP: return Rocket::Core::Input::KI_HELP;
    case SDLK_MENU: return Rocket::Core::Input::KI_LMENU;
    case SDLK_SELECT: return Rocket::Core::Input::KI_SELECT;
    case SDLK_STOP: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_AGAIN: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_UNDO: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_CUT: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_COPY: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_PASTE: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_FIND: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_MUTE: return Rocket::Core::Input::KI_VOLUME_MUTE;
    case SDLK_VOLUMEUP: return Rocket::Core::Input::KI_VOLUME_UP;
    case SDLK_VOLUMEDOWN: return Rocket::Core::Input::KI_VOLUME_DOWN;
    case SDLK_KP_COMMA: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_EQUALSAS400: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped

    case SDLK_ALTERASE: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_SYSREQ: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_CANCEL: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_CLEAR: return Rocket::Core::Input::KI_OEM_CLEAR;
    case SDLK_PRIOR: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_RETURN2: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_SEPARATOR: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_OUT: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_OPER: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_CLEARAGAIN: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_CRSEL: return Rocket::Core::Input::KI_CRSEL;
    case SDLK_EXSEL: return Rocket::Core::Input::KI_EXSEL;

    case SDLK_KP_00: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_000: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_THOUSANDSSEPARATOR: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_DECIMALSEPARATOR: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped, could be KI_SEPARATOR
    case SDLK_CURRENCYUNIT: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_CURRENCYSUBUNIT: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_LEFTPAREN: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_RIGHTPAREN: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_LEFTBRACE: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_RIGHTBRACE: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_TAB: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_BACKSPACE: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_A: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_B: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_C: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_D: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_E: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_F: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_XOR: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_POWER: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_PERCENT: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_LESS: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_GREATER: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_AMPERSAND: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_DBLAMPERSAND: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_VERTICALBAR: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_DBLVERTICALBAR: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_COLON: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_HASH: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_SPACE: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_AT: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_EXCLAM: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_MEMSTORE: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_MEMRECALL: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_MEMCLEAR: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_MEMADD: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_MEMSUBTRACT: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_MEMMULTIPLY: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_MEMDIVIDE: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_PLUSMINUS: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_CLEAR: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_CLEARENTRY: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_BINARY: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_OCTAL: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_DECIMAL: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KP_HEXADECIMAL: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped

    case SDLK_LCTRL: return Rocket::Core::Input::KI_LCONTROL;
    case SDLK_LSHIFT: return Rocket::Core::Input::KI_LSHIFT;
    case SDLK_LALT: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_LGUI: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_RCTRL: return Rocket::Core::Input::KI_RCONTROL;
    case SDLK_RSHIFT: return Rocket::Core::Input::KI_RSHIFT;
    case SDLK_RALT: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_RGUI: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped

    case SDLK_MODE: return Rocket::Core::Input::KI_MODECHANGE; //! is this correct?

    case SDLK_AUDIONEXT: return Rocket::Core::Input::KI_MEDIA_NEXT_TRACK;
    case SDLK_AUDIOPREV: return Rocket::Core::Input::KI_MEDIA_PREV_TRACK;
    case SDLK_AUDIOSTOP: return Rocket::Core::Input::KI_MEDIA_STOP;
    case SDLK_AUDIOPLAY: return Rocket::Core::Input::KI_MEDIA_PLAY_PAUSE;
    case SDLK_AUDIOMUTE: return Rocket::Core::Input::KI_VOLUME_MUTE; //! is this correct? used twice.
    case SDLK_MEDIASELECT: return Rocket::Core::Input::KI_LAUNCH_MEDIA_SELECT; //! is this correct?
    case SDLK_WWW: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_MAIL: return Rocket::Core::Input::KI_LAUNCH_MAIL;
    case SDLK_CALCULATOR: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_COMPUTER: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_AC_SEARCH: return Rocket::Core::Input::KI_BROWSER_SEARCH;
    case SDLK_AC_HOME: return Rocket::Core::Input::KI_BROWSER_HOME;
    case SDLK_AC_BACK: return Rocket::Core::Input::KI_BROWSER_BACK;
    case SDLK_AC_FORWARD: return Rocket::Core::Input::KI_BROWSER_FORWARD;
    case SDLK_AC_STOP: return Rocket::Core::Input::KI_BROWSER_STOP;
    case SDLK_AC_REFRESH: return Rocket::Core::Input::KI_BROWSER_REFRESH;
    case SDLK_AC_BOOKMARKS: return Rocket::Core::Input::KI_BROWSER_FAVORITES;

    case SDLK_BRIGHTNESSDOWN: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_BRIGHTNESSUP: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_DISPLAYSWITCH: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KBDILLUMTOGGLE: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KBDILLUMDOWN: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_KBDILLUMUP: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_EJECT: return Rocket::Core::Input::KI_UNKNOWN; //! not mapped
    case SDLK_SLEEP: return Rocket::Core::Input::KI_SLEEP;
	};

	return Rocket::Core::Input::KI_UNKNOWN;
}

int
SdlKeyModToRocketKeyModifierState(Uint16 mod)
{
	int result = 0;

	// TODO: ScrollLock(KM_SCROLLLOCK) is not in SDL's keyModifier state and
	// would need to be parsed from keypresses / releases.

	if(KMOD_NUM & mod) result |= Rocket::Core::Input::KM_NUMLOCK;
	if(KMOD_CAPS & mod) result |= Rocket::Core::Input::KM_CAPSLOCK;
	if(KMOD_CTRL & mod) result |= Rocket::Core::Input::KM_CTRL;
	if(KMOD_SHIFT & mod) result |= Rocket::Core::Input::KM_SHIFT;
	if(KMOD_ALT & mod) result |= Rocket::Core::Input::KM_ALT;
	if(KMOD_GUI & mod) result |= Rocket::Core::Input::KM_META;

	return result;
}

// Convert a UTF-8 encoded character in "input" into a number. This
// function returns the unicode value of the UTF-8 character if
// successful, and -1 if not successful. "end_ptr" is set to the next
// character after the read character on success. "end_ptr" is set to
// the start of input on failure. "end_ptr" may not be null.
//
// Copyright (c) Ben Bullock 2009 - 2014:
// http://www.lemoda.net/c/utf8-to-ucs2/index.html
int utf8_to_ucs2 (const unsigned char * input, const unsigned char ** end_ptr)
{
    *end_ptr = input;
    if (input[0] == 0)
        return -1;
    if (input[0] < 0x80) {
        * end_ptr = input + 1;
        return input[0];
    }
    if ((input[0] & 0xE0) == 0xE0) {
        if (input[1] == 0 || input[2] == 0)
            return -1;
        * end_ptr = input + 3;
        return
            (input[0] & 0x0F)<<12 |
            (input[1] & 0x3F)<<6  |
            (input[2] & 0x3F);
    }
    if ((input[0] & 0xC0) == 0xC0) {
        if (input[1] == 0)
            return -1;
        * end_ptr = input + 2;
        return
            (input[0] & 0x1F)<<6  |
            (input[1] & 0x3F);
    }
    return -1;
}

// Gui /////////////////////////////////////////////////////////////////////////

Gui::Gui()
{
	m_Active = true;
	m_Height = -1;
	m_KeyModifierState = 0;
	m_Width = -1;

	Rocket::Core::SetSystemInterface(&g_GuiSystemInterface);
	Rocket::Core::SetRenderInterface(&g_GuiRenderInterfaceOpenGL);
	Rocket::Core::SetFileInterface(&g_GuiFileInterface);

	Rocket::Core::Initialise();

	m_Context = Rocket::Core::CreateContext("default", Rocket::Core::Vector2i(640, 480));

	if(!m_Context) return;

	LoadFonts();

//	Rocket::Core::ElementDocument* cursor = m_Context->LoadMouseCursor("cursor.rml");
//	if (cursor)
//		cursor->RemoveReference();

	Rocket::Core::ElementDocument* document = m_Context->LoadDocument("main.rml");
	if (document != NULL)
	{
		document->Show();
		document->RemoveReference();
	}
}

Gui::~Gui()
{
	if(m_Context) m_Context->RemoveReference();
}

bool Gui::HandleSdlEvent(SDL_Event * event)
{
	if(activityRead)
	{
		activityRead = false;
		activity = !activity;
	}

	if(!(event && m_Active && m_Context)) return false;

	switch(event->type)
	{
	case SDL_KEYDOWN:
		{
			m_KeyModifierState = SdlKeyModToRocketKeyModifierState(event->key.keysym.mod);
			m_Context->ProcessKeyDown(
				SdlKeyCodeToRocketKeyIdentifier(event->key.keysym.sym),
				m_KeyModifierState
			);
			return true;
		}
		break;
	case SDL_KEYUP:
		{
			m_KeyModifierState = SdlKeyModToRocketKeyModifierState(event->key.keysym.mod);
			m_Context->ProcessKeyUp(
				SdlKeyCodeToRocketKeyIdentifier(event->key.keysym.sym),
				m_KeyModifierState
			);
			return true;
		}
		break;
	case SDL_TEXTINPUT:
		{
			const unsigned char * endPtrUnused;
			int ucs2Char = utf8_to_ucs2((const unsigned char *)event->text.text, &endPtrUnused);
			if(-1 != ucs2Char) m_Context->ProcessTextInput((Rocket::Core::word)ucs2Char);
			return true;
		}
		break;
	case SDL_MOUSEMOTION:
		{
			m_Context->ProcessMouseMove(
				event->motion.x, event->motion.y,
				m_KeyModifierState
			);
			old_mouse.x = mouse.x;
			old_mouse.y = mouse.y;
			mouse.x = event->motion.x;
			mouse.y = event->motion.y;
			return true;
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		{
			m_Context->ProcessMouseButtonDown(
				SdlMouseButtonToRocketButtonIndex(event->button.button),
				m_KeyModifierState
			);
			return true;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		{
			m_Context->ProcessMouseButtonUp(
				SdlMouseButtonToRocketButtonIndex(event->button.button),
				m_KeyModifierState
			);
			return true;
		}
		break;
	case SDL_MOUSEWHEEL:
		{
			m_Context->ProcessMouseWheel(
				(int)-event->wheel.y,
				m_KeyModifierState
			);

			return true;
		}
		break;
	default:
		return false;
	};

	return true;
}

void Gui::LoadFonts()
{
	Rocket::Core::String font_names[4];
	font_names[0] = "Delicious-Roman.otf";
	font_names[1] = "Delicious-Italic.otf";
	font_names[2] = "Delicious-Bold.otf";
	font_names[3] = "Delicious-BoldItalic.otf";

	for (int i = 0; i < sizeof(font_names) / sizeof(Rocket::Core::String); i++)
	{
		Rocket::Core::FontDatabase::LoadFontFace(font_names[i]);
	}
}


void Gui::Render()
{
	if(!m_Active) return;

	GLint mode, src, dst;
	GLfloat colours[4];

	// back-up state:
	glGetIntegerv(GL_MATRIX_MODE, &mode);
	glGetIntegerv(GL_BLEND_SRC, &src);
	glGetIntegerv(GL_BLEND_DST, &dst);
	glGetFloatv(GL_CURRENT_COLOR, colours);
	GLboolean alpha = glIsEnabled(GL_ALPHA_TEST);
	GLboolean lighting = glIsEnabled(GL_LIGHTING);
	GLboolean texture = glIsEnabled(GL_TEXTURE_2D);
	GLboolean blend = glIsEnabled(GL_BLEND);
	GLboolean cull = glIsEnabled(GL_CULL_FACE);
	GLboolean depth = glIsEnabled(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Change projection matrix to orth
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, m_Width, m_Height, 0, -1, 1);

	// Back to model for rendering
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	glCullFace(GL_BACK);

//	glEnableClientState(GL_VERTEX_ARRAY);
//	glEnableClientState(GL_COLOR_ARRAY);

	if(m_Context)
	{
		m_Context->Update();
		m_Context->Render();
	}

	glDisable(GL_TEXTURE_2D);

	glColor4f(0.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex2f(static_cast<GLfloat>(old_mouse.x), static_cast<GLfloat>(old_mouse.y + 5));	// Bottom Left
	glVertex2f(static_cast<GLfloat>(old_mouse.x + 5),static_cast<GLfloat>(old_mouse.y + 5));	// Bottom Right
	glVertex2f(static_cast<GLfloat>(old_mouse.x + 5), static_cast<GLfloat>(old_mouse.y));		// Top Right
	glVertex2f(static_cast<GLfloat>(old_mouse.x), static_cast<GLfloat>(old_mouse.y));		// Top Left
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glVertex2f(static_cast<GLfloat>(mouse.x), static_cast<GLfloat>(mouse.y + 5));	// Bottom Left
	glVertex2f(static_cast<GLfloat>(mouse.x + 5),static_cast<GLfloat>(mouse.y + 5));	// Bottom Right
	glVertex2f(static_cast<GLfloat>(mouse.x + 5), static_cast<GLfloat>(mouse.y));		// Top Right
	glVertex2f(static_cast<GLfloat>(mouse.x), static_cast<GLfloat>(mouse.y));		// Top Left
	glEnd();

	if(activity)
	{
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glBegin(GL_QUADS);
		glVertex2f(0.0f, 50.0f);	// Bottom Left
		glVertex2f(50.0f, 50.0f);	// Bottom Right
		glVertex2f(50.0f, 0.0f);		// Top Right
		glVertex2f(0.0f, 0.0f);		// Top Left
		glEnd();
	}
	activityRead = true;

	glPopMatrix();

	// Reset the projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Restore state:
	glMatrixMode(mode);
	glBlendFunc(src, dst);

	glColor4fv(colours);

	glSetEnabled(GL_ALPHA_TEST, alpha);
	glSetEnabled(GL_LIGHTING, lighting);
	glSetEnabled(GL_TEXTURE_2D, texture);
	glSetEnabled(GL_CULL_FACE, cull);
	glSetEnabled(GL_DEPTH_TEST, depth);
	glSetEnabled(GL_BLEND, blend);
}

void Gui::SetActive(bool value)
{
	m_Active = value;
}

void Gui::SetViewport(int width, int height)
{
	if(m_Context && (width != m_Width || height != m_Height))
		m_Context->SetDimensions(Rocket::Core::Vector2i(width, height));

	m_Width = width;
	m_Height = height;

	g_GuiRenderInterfaceOpenGL.SetViewport(width, height);
}

#endif // AFX_GUI
