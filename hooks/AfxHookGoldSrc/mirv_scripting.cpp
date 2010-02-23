//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-16 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "mirv_scripting.h"

#include <modules/ModColor_S.h>
#include <modules/ModHide_S.h>
#include <modules/ModColorMask_S.h>
#include <modules/ModReplace_S.h>
#include <modules/ModInfo_S.h>

#include "hl_addresses.h"

#include <windows.h>
#include <gl\gl.h>

// BEGIN HLSDK includes
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <hlsdk/multiplayer/cl_dll/wrect.h>
#include <hlsdk/multiplayer/cl_dll/cl_dll.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include "cmdregister.h"

//#include <malloc.h>
//#include <string.h>

#include <string>

#include <script.h>


std::string g_ScriptFolder("");

JSRuntime * g_JsRt = NULL;
JSContext * g_JsCx = NULL;
JSObject  * g_JsGlobal = NULL;
bool g_JsRunning = false;

extern cl_enginefuncs_s *pEngfuncs;

bool g_Script_CanConsolePrint = false;

void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
	if(g_Script_CanConsolePrint)
		pEngfuncs->Con_Printf(
			"AFX ERROR: %s:%u:%s\n",
            report->filename ? report->filename : "<no filename>",
            (unsigned int) report->lineno,
            message
		);
}

// Ogl /////////////////////////////////////////////////////////////////////////

static JSBool
Ogl_glColorMask(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSBool jbR = JS_TRUE;
	JSBool jbG = JS_TRUE;
	JSBool jbB = JS_TRUE;
	JSBool jbA = JS_TRUE;

	JS_ValueToBoolean(g_JsCx, argv[0], &jbR);
	JS_ValueToBoolean(g_JsCx, argv[1], &jbG);
	JS_ValueToBoolean(g_JsCx, argv[2], &jbB);
	JS_ValueToBoolean(g_JsCx, argv[3], &jbA);

	glColorMask(jbR, jbG, jbB, jbA);

    return JS_TRUE;
}

static JSBool
Ogl_GlClear(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int32 jMode = 0;

	JS_ValueToInt32(g_JsCx, argv[0], &jMode);

	glClear(jMode);

    return JS_TRUE;
}


static JSFunctionSpec Ogl_functions[] = {
    {"glClear", Ogl_glColorMask, 1, JSMIRVSHAREDPROP|JSPROP_READONLY,0},
    {"glColorMask", Ogl_GlClear, 4, JSMIRVSHAREDPROP|JSPROP_READONLY,0},
	{NULL,NULL,0,0,0}
};

static JSClass Ogl_class = {
    "Ogl", 0,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};





// Events //////////////////////////////////////////////////////////////////////

JSObject  * g_JsEvents = NULL;

struct ScriptEvents_s {
	std::string On_glBegin;
	std::string On_glEnd;
	std::string On_RecordStarting;
	std::string On_RecordEnded;
	std::string On_RenderViewBegin;
	std::string On_RenderViewEnd;
} g_ScriptEvents;

enum Events_tinyid {
	TID_Events_On_glBegin,
	TID_Events_On_glEnd,
	TID_Events_On_RecordStarting,
	TID_Events_On_RecordEnded,
	TID_Events_On_RenderViewBegin,
	TID_Events_On_RenderViewEnd,
};

void SetEventFnString(std::string *pstr, JSContext *cx, jsval *vp) {
	JSString *jstr;
	if(
		JSTYPE_FUNCTION == JS_TypeOfValue(cx, *vp)
		&& NULL != (jstr = JS_GetFunctionId(JS_ValueToFunction(cx, *vp)))
	)
		pstr->assign(JS_GetStringBytes(jstr));
	else
		pstr->assign("");
}


JSBool Events_On_glBegin_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	SetEventFnString(&g_ScriptEvents.On_glBegin, cx, vp);

	return JS_TRUE;
}

JSBool Events_On_glEnd_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	SetEventFnString(&g_ScriptEvents.On_glEnd, cx, vp);

	return JS_TRUE;
}

JSBool Events_On_RenderViewBegin_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	SetEventFnString(&g_ScriptEvents.On_RenderViewBegin, cx, vp);

	return JS_TRUE;
}

JSBool Events_On_RenderViewEnd_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	SetEventFnString(&g_ScriptEvents.On_RenderViewEnd, cx, vp);

	return JS_TRUE;
}

JSBool Events_On_RecordStarting_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	SetEventFnString(&g_ScriptEvents.On_RecordStarting, cx, vp);

	return JS_TRUE;
}

JSBool Events_On_RecordEnded_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	SetEventFnString(&g_ScriptEvents.On_RecordEnded, cx, vp);

	return JS_TRUE;
}

static JSPropertySpec Events_props[] = {
  {"on_glBegin", TID_Events_On_glBegin, JSMIRVPROP, NULL, Events_On_glBegin_set},
  {"on_glEnd"  , TID_Events_On_glEnd  , JSMIRVPROP, NULL, Events_On_glEnd_set},
  {"on_RecordStarting"  , TID_Events_On_RecordStarting, JSMIRVPROP, NULL, Events_On_RecordStarting_set},
  {"on_RecordEnded"  , TID_Events_On_RecordEnded, JSMIRVPROP, NULL, Events_On_RecordEnded_set},
  {"on_RenderViewBegin", TID_Events_On_RenderViewBegin, JSMIRVPROP, NULL, Events_On_RenderViewBegin_set},
  {"on_RenderViewEnd"  , TID_Events_On_RenderViewEnd  , JSMIRVPROP, NULL, Events_On_RenderViewEnd_set},
  {NULL,0,0,NULL,NULL}
};

static JSClass Events_class = {
    "Events", 0,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

void ScriptEvent_OnGlBegin(unsigned int mode) {
	if(!g_ScriptEvents.On_glBegin.empty()) {
		jsval r;
		jsval args[1];
		args[0] = INT_TO_JSVAL(mode);
		JS_CallFunctionName(g_JsCx, g_JsGlobal, g_ScriptEvents.On_glBegin.c_str(), 1, args, &r);
	}
}

void ScriptEvent_OnGlEnd() {
	if(!g_ScriptEvents.On_glEnd.empty()) {
		jsval r;
		JS_CallFunctionName(g_JsCx, g_JsGlobal, g_ScriptEvents.On_glEnd.c_str(), 0, NULL, &r);
	}
}

void ScriptEvent_OnRecordStarting() {
	if(!g_ScriptEvents.On_RecordStarting.empty()) {
		jsval r;
		JS_CallFunctionName(g_JsCx, g_JsGlobal, g_ScriptEvents.On_RecordStarting.c_str(), 0, NULL, &r);
	}
}
void ScriptEvent_OnRecordEnded() {
	if(!g_ScriptEvents.On_RecordEnded.empty()) {
		jsval r;
		JS_CallFunctionName(g_JsCx, g_JsGlobal, g_ScriptEvents.On_RecordEnded.c_str(), 0, NULL, &r);
	}
}

bool ScriptEvent_OnRenderViewBegin() {
	if(g_ScriptEvents.On_RenderViewBegin.empty())
		return true;

	jsval r;
	JSBool jB;

	JS_CallFunctionName(g_JsCx, g_JsGlobal, g_ScriptEvents.On_RenderViewBegin.c_str(), 0, NULL, &r);

	return JS_ValueToBoolean(g_JsCx, r, &jB) ? (bool)jB : true;
}

bool ScriptEvent_OnRenderViewEnd() {
	if(g_ScriptEvents.On_RenderViewEnd.empty())
		return false;

	jsval r;
	JSBool jB;

	JS_CallFunctionName(g_JsCx, g_JsGlobal, g_ScriptEvents.On_RenderViewEnd.c_str(), 0, NULL, &r);

	return JS_ValueToBoolean(g_JsCx, r, &jB) ? (bool)jB : false;
}


// Fx //////////////////////////////////////////////////////////////////////////

static JSClass Fx_class = {
    "Fx", 0,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};


// Addr ////////////////////////////////////////////////////////////////////////


static JSBool
Addr_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    JSString * str = JS_ValueToString(cx, id);

	if(str) {
		HlAddress_t * padr = HlAddr_GetByName(JS_GetStringBytes(str));
		if(padr) *vp = INT_TO_JSVAL((*padr));
	}

    return JS_TRUE;
}


JS_STATIC_DLL_CALLBACK(JSBool)
Addr_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    JSString * str = JS_ValueToString(cx, id);
	if(str) {
		int32 i32;
		HlAddress_t * padr = HlAddr_GetByName(JS_GetStringBytes(str));

		if(padr && JS_TRUE == JS_ValueToInt32(cx, *vp, &i32))
			*padr = (HlAddress_t)i32;
	}

    return JS_TRUE;
}

static JSClass Addr_class = {
    "Addr", 0,
    JS_PropertyStub, JS_PropertyStub, Addr_getProperty, Addr_setProperty,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};



// Global //////////////////////////////////////////////////////////////////////

static JSClass Global_class = {
    "Global", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool
Global_Load(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uintN i;
    JSString *str;
    const char *filename;
    JSScript *script;
    JSBool ok;
    jsval result;
    uint32 oldopts;

    for (i = 0; i < argc; i++) {
        str = JS_ValueToString(cx, argv[i]);

		
        if (!str)
            return JS_FALSE; // <-- original mistake from the SpiderMonkey js.c code, but oh well.

        argv[i] = STRING_TO_JSVAL(str);
        filename = JS_GetStringBytes(str);
        errno = 0;
        oldopts = JS_GetOptions(cx);

		std::string strFile(g_ScriptFolder);
		strFile += filename;

//		MessageBox(0, strFile.c_str(), "Ok", MB_OK);

        JS_SetOptions(cx, oldopts | JSOPTION_COMPILE_N_GO);
		script = JS_CompileFile(cx, obj, strFile.c_str());
        if (!script) {
            ok = JS_FALSE;
        } else {
            ok = JS_ExecuteScript(cx, obj, script, &result);
			JS_DestroyScript(cx, script);
        }
        JS_SetOptions(cx, oldopts);
        if (!ok)
            return JS_FALSE;
    }

    return JS_TRUE;
}

static JSBool
Global_Print(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    uintN i, n;
    JSString *str;

	if(!g_Script_CanConsolePrint)
		return JS_TRUE;

    for (i = n = 0; i < argc; i++) {
        str = JS_ValueToString(cx, argv[i]);
        if (!str)
            return JS_FALSE;
		pEngfuncs->Con_Printf("%s%s", i ? " " : "", JS_GetStringBytes(str));
    }
    n++;
    if (n)
        pEngfuncs->Con_Printf("\n");
    return JS_TRUE;
}

static JSFunctionSpec Global_functions[] = {
	{"load" , Global_Load , 1,JSMIRVSHAREDPROP|JSPROP_READONLY,0},
    {"print", Global_Print, 0,JSMIRVSHAREDPROP|JSPROP_READONLY,0},
	{NULL,NULL,0,0,0}
};



////////////////////////////////////////////////////////////////////////////////


bool JsExecute(char const * script) {
	if(!g_JsRunning) return false;

	jsval rval;
	return JS_TRUE == JS_EvaluateScript(g_JsCx, g_JsGlobal, script, strlen(script), NULL, 0, &rval);
}

bool JsIsRunning() {
	return g_JsRunning;
}

void JsShutDown() {
	g_JsRunning = false;
	if(g_JsCx) JS_DestroyContext(g_JsCx);
	if(g_JsRt) JS_DestroyRuntime(g_JsRt);
	JS_ShutDown();
}

bool JsStartUp() {
	bool bOk = true;

	JSObject * joFx;
	JSObject * jo;

	bOk =
		NULL != (g_JsRt = JS_NewRuntime(1L * 1024L * 1024L))
		&& NULL != (g_JsCx = JS_NewContext(g_JsRt, 8192))
		&& (
			JS_SetOptions(g_JsCx, JSOPTION_VAROBJFIX),
			JS_SetVersion(g_JsCx, JSVERSION_DEFAULT),
			JS_SetErrorReporter(g_JsCx, reportError),
			true
		)

		// . (global):
		&& NULL != (g_JsGlobal = JS_NewObject(g_JsCx, &Global_class, NULL, NULL)) // .
		&& JS_InitStandardClasses(g_JsCx, g_JsGlobal)
		&& JS_DefineFunctions(g_JsCx, g_JsGlobal, Global_functions)

		// .addr:
		&& NULL != (jo = JS_DefineObject(g_JsCx, g_JsGlobal, "addr", &Addr_class, NULL, JSMIRVPROP|JSPROP_READONLY))

		// .info:
		&& NULL != (jo = ModInfo_S_Register(g_JsCx, g_JsGlobal, "info"))

		// .ogl:
		&& NULL != (jo = JS_DefineObject(g_JsCx, g_JsGlobal, "ogl", &Ogl_class, NULL, JSMIRVPROP|JSPROP_READONLY))
		&& JS_DefineFunctions(g_JsCx, jo, Ogl_functions)

		// .events:
		&& NULL != (jo = JS_DefineObject(g_JsCx, g_JsGlobal, "events", &Events_class, NULL, JSMIRVPROP|JSPROP_READONLY))
		&& JS_DefineProperties(g_JsCx, jo, Events_props)

		// .fx:
		&& NULL != (joFx = JS_DefineObject(g_JsCx, g_JsGlobal, "fx", &Fx_class, NULL, JSMIRVPROP|JSPROP_READONLY))

		// .fx.color:
		&& NULL != (jo = ModColor_S_Register(g_JsCx, joFx, "color"))

		// .fx.colorMask
		&& NULL != (jo = ModColorMask_S_Register(g_JsCx, joFx, "colorMask"))

		// .fx.hide:
		&& NULL != (jo = ModHide_S_Register(g_JsCx, joFx, "hide"))

		// .fx.replace:
		&& NULL != (jo = ModReplace_S_Register(g_JsCx, joFx, "replace"))
	;

	if(!bOk)
		JsShutDown();

	g_JsRunning = bOk;

	return bOk;
}

void JsSetScriptFolder(char const * scriptfolder) {
	g_ScriptFolder  = scriptfolder;
}

// Client console access ///////////////////////////////////////////////////////


_REGISTER_CMD("afx", afx_cmd)
void afx_cmd() {
	if(!g_JsRunning) {
		pEngfuncs->Con_Printf("Error: script engine not running.\n");
		return;
	}

	int argc = pEngfuncs->Cmd_Argc();

	if(argc < 2) {
		// Print help and return:
		pEngfuncs->Con_Printf(
			"Usage: afx <script command(s)>\n"
		);
		return;
	}

	//
	// concat arguments to full string:

	char *ttt, *ct;
	unsigned int len=0;

	// calculate required space:
	for(int i=0; i<argc; i++) len += strlen(pEngfuncs->Cmd_Argv(i))+1;
	if(len<1) len=1;

	ct = ttt = (char *)malloc(sizeof(char)*len);

	if(!ct) {
		pEngfuncs->Con_Printf("ERROR: malloc failed.\n");
		return;
	}

	for(int i=1; i<argc; i++) {
		char const * cur = pEngfuncs->Cmd_Argv(i);
		unsigned int lcur = strlen(cur);
		
		if(1<i) {
			strcpy(ct, " ");
			ct++;
		}

		strcpy(ct, cur);
		ct += lcur;
	}
	*ct = 0; // Term

	jsval rval;
	JSBool ok;

	JS_AddRoot(g_JsCx,&rval);

	pEngfuncs->Con_DPrintf("%s\n", ttt);
	ok = JS_EvaluateScript(g_JsCx, g_JsGlobal, ttt, strlen(ttt), NULL, 0, &rval);

	if(!ok)
		pEngfuncs->Con_Printf("Error.\n");
	else if (!JSVAL_IS_VOID(rval)) {
		JSString *jstr = JS_ValueToString(g_JsCx, rval);
		if(jstr)
			pEngfuncs->Con_Printf("Result: %s\n", JS_GetStringBytes(jstr));
		else
			pEngfuncs->Con_Printf("Result error.\n");
	}

	JS_RemoveRoot(g_JsCx, &rval);

	free(ttt);
}
