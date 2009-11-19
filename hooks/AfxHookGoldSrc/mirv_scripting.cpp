//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2009-11-16 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "mirv_scripting.h"

#include "FxColor.h"
#include "FxHide.h"
#include "FxRgbMask.h"
#include "MirvInfo.h"

#include "hl_addresses.h"

#include <windows.h>
#include <gl\gl.h>

// BEGIN HLSDK includes
#pragma push_macro("HSPRITE")
#define HSPRITE MDTHACKED_HSPRITE
//
#include <wrect.h>
#include <cl_dll.h>
//
#undef HSPRITE
#pragma pop_macro("HSPRITE")
// END HLSDK includes

#include "cmdregister.h"

//#include <malloc.h>
//#include <string.h>

#include <string>


#define XP_WIN
#include <jsapi.h>
#include <jsstr.h>

#define JSMIRVPROP JSPROP_ENUMERATE|JSPROP_PERMANENT
#define JSMIRVSHAREDPROP JSMIRVPROP|JSPROP_SHARED

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



// FxColor /////////////////////////////////////////////////////////////////////

enum FxColor_tinyid {
	FxColor_ENABLED, FxColor_RED, FxColor_GREEN, FxColor_BLUE, FxColor_ALPHA
};

static JSPropertySpec FxColor_props[] = {
    {"enabled", FxColor_ENABLED, JSMIRVSHAREDPROP, NULL, NULL},
    {"red"    , FxColor_RED    , JSMIRVSHAREDPROP, NULL, NULL},
    {"green"  , FxColor_GREEN  , JSMIRVSHAREDPROP, NULL, NULL},
    {"blue"   , FxColor_BLUE   , JSMIRVSHAREDPROP, NULL, NULL},
    {"alpha"  , FxColor_ALPHA  , JSMIRVSHAREDPROP, NULL, NULL},
    {NULL,0,0,NULL,NULL}
};

static JSBool
FxColor_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    if (!JSVAL_IS_INT(id)) return JS_TRUE;

	switch (JSVAL_TO_INT(id)) {
	case FxColor_ENABLED:
		*vp = BOOLEAN_TO_JSVAL(g_FxColor.Enabled_get());
		break;
	case FxColor_RED:
		*vp = DOUBLE_TO_JSVAL(g_FxColor.Red_get());
		break;
	case FxColor_GREEN:
		*vp = DOUBLE_TO_JSVAL(g_FxColor.Green_get());
		break;
	case FxColor_BLUE:
		*vp = DOUBLE_TO_JSVAL(g_FxColor.Blue_get());
		break;
	case FxColor_ALPHA:
		*vp = DOUBLE_TO_JSVAL(g_FxColor.Alpha_get());
		break;
	}

    return JS_TRUE;
}


JS_STATIC_DLL_CALLBACK(JSBool)
FxColor_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    if (!JSVAL_IS_INT(id)) return JS_TRUE;

	jsdouble jD;
	JSBool jB;

	switch (JSVAL_TO_INT(id)) {
	case FxColor_ENABLED:
		if(JS_ValueToBoolean(cx, *vp, &jB)) g_FxColor.Enabled_set(jB);
		break;
	case FxColor_RED:
		if(JS_ValueToNumber(cx, *vp, &jD)) g_FxColor.Red_set(jD);
		break;
	case FxColor_GREEN:
		if(JS_ValueToNumber(cx, *vp, &jD)) g_FxColor.Green_set(jD);
		break;
	case FxColor_BLUE:
		if(JS_ValueToNumber(cx, *vp, &jD)) g_FxColor.Blue_set(jD);
		break;
	case FxColor_ALPHA:
		if(JS_ValueToNumber(cx, *vp, &jD)) g_FxColor.Alpha_set(jD);
		break;
	}

	return JS_TRUE;
}

static JSClass FxColor_class = {
    "FxColor", 0,
    JS_PropertyStub, JS_PropertyStub, FxColor_getProperty, FxColor_setProperty,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};


// FxRgbMask /////////////////////////////////////////////////////////////////////

enum FxRgbMask_tinyid {
	TID_FxRgbMask_Enabled,
	TID_FxRgbMask_OpBlue,
	TID_FxRgbMask_OpGreen,
	TID_FxRgbMask_OpRed,
	TID_FxRgbMask_Supported
};


JSBool FxRgbMask_Enabled_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_FxRgbMask.Enabled_get());
	return JS_TRUE;
}

JSBool FxRgbMask_Enabled_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	JSBool jB;
	if(JS_ValueToBoolean(cx, *vp, &jB)) g_FxRgbMask.Enabled_set(jB);
	return JS_TRUE;
}

JSBool FxRgbMask_OpGreen_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = INT_TO_JSVAL(g_FxRgbMask.OpGreen_get());
	return JS_TRUE;
}

JSBool FxRgbMask_OpGreen_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	int32 i32;
	if(JS_TRUE == JS_ValueToInt32(cx, *vp, &i32)) g_FxRgbMask.OpGreen_set(i32);
	return JS_TRUE;
}

JSBool FxRgbMask_OpBlue_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = INT_TO_JSVAL(g_FxRgbMask.OpBlue_get());
	return JS_TRUE;
}

JSBool FxRgbMask_OpBlue_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	int32 i32;
	if(JS_TRUE == JS_ValueToInt32(cx, *vp, &i32)) g_FxRgbMask.OpBlue_set(i32);
	return JS_TRUE;
}

JSBool FxRgbMask_OpRed_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = INT_TO_JSVAL(g_FxRgbMask.OpRed_get());
	return JS_TRUE;
}

JSBool FxRgbMask_OpRed_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	int32 i32;
	if(JS_TRUE == JS_ValueToInt32(cx, *vp, &i32)) g_FxRgbMask.OpRed_set(i32);
	return JS_TRUE;
}

JSBool FxRgbMask_Supported_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_FxRgbMask.Supported_get());
	return JS_TRUE;
}


static JSPropertySpec FxRgbMask_props[] = {
  {"enabled"  , TID_FxRgbMask_Enabled  , JSMIRVSHAREDPROP, FxRgbMask_Enabled_get, FxRgbMask_Enabled_set},
  {"opBlue"     , TID_FxRgbMask_OpBlue     , JSMIRVSHAREDPROP, FxRgbMask_OpBlue_get, FxRgbMask_OpBlue_set},
  {"opGreen"    , TID_FxRgbMask_OpGreen    , JSMIRVSHAREDPROP, FxRgbMask_OpGreen_get, FxRgbMask_OpGreen_set},
  {"opRed"      , TID_FxRgbMask_OpRed      , JSMIRVSHAREDPROP, FxRgbMask_OpRed_get, FxRgbMask_OpRed_set},
  {"supported", TID_FxRgbMask_Supported, JSMIRVSHAREDPROP|JSPROP_READONLY, FxRgbMask_Supported_get, NULL              },
  {NULL,0,0,NULL,NULL}
};

static JSClass FxRgbMask_class = {
    "FxRgbMask", 0,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

// FxHide //////////////////////////////////////////////////////////////////////

enum FxHide_tinyid {
	TID_FxHide_Enabled
};


JSBool FxHide_Enabled_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_FxHide.Enabled_get());
	return JS_TRUE;
}

JSBool FxHide_Enabled_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	JSBool jB;
	if(JS_ValueToBoolean(cx, *vp, &jB)) g_FxHide.Enabled_set(jB);
	return JS_TRUE;
}



static JSPropertySpec FxHide_props[] = {
  {"enabled"  , TID_FxHide_Enabled  , JSMIRVSHAREDPROP, FxHide_Enabled_get, FxHide_Enabled_set},
  {NULL,0,0,NULL,NULL}
};

static JSClass FxHide_class = {
    "FxHide", 0,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

// Info /////////////////////////////////////////////////////////////////////

enum Info_tinyid {
	TID_Info_In_R_DrawEntitiesOnList,
	TID_Info_In_R_DrawParticles,
	TID_Info_In_R_DrawViewModel,
	TID_Info_In_R_Renderview,
	TID_Info_Recording
};

JSBool Info_In_R_DrawEntitiesOnList_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_MirvInfo.In_R_DrawEntitiesOnList_get());
	return JS_TRUE;
}

JSBool Info_In_R_DrawParticles_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_MirvInfo.In_R_DrawParticles_get());
	return JS_TRUE;
}

JSBool Info_In_R_DrawViewModel_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_MirvInfo.In_R_DrawViewModel_get());
	return JS_TRUE;
}

JSBool Info_In_R_Renderview_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_MirvInfo.In_R_Renderview_get());
	return JS_TRUE;
}

JSBool Info_Recording_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_MirvInfo.Recording_get());
	return JS_TRUE;
}

static JSPropertySpec Info_props[] = {
  {"in_R_DrawEntitiesOnList", TID_Info_In_R_DrawEntitiesOnList, JSMIRVSHAREDPROP|JSPROP_READONLY, Info_In_R_DrawEntitiesOnList_get, NULL},
  {"in_R_DrawParticles"     , TID_Info_In_R_DrawParticles     , JSMIRVSHAREDPROP|JSPROP_READONLY, Info_In_R_DrawParticles_get     , NULL},
  {"in_R_DrawViewModel"     , TID_Info_In_R_DrawViewModel     , JSMIRVSHAREDPROP|JSPROP_READONLY, Info_In_R_DrawViewModel_get     , NULL},
  {"in_R_Renderview"        , TID_Info_In_R_Renderview        , JSMIRVSHAREDPROP|JSPROP_READONLY, Info_In_R_Renderview_get        , NULL},
  {"recording"              , TID_Info_Recording              , JSMIRVSHAREDPROP|JSPROP_READONLY, Info_Recording_get              , NULL},
  {NULL,0,0,NULL,NULL}
};

static JSBool
Info_GetCurrentEntityIndex(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = INT_TO_JSVAL(g_MirvInfo.GetCurrentEntityIndex());	

    return JS_TRUE;
}

static JSFunctionSpec Info_functions[] = {
    {"getCurrentEntityIndex", Info_GetCurrentEntityIndex, 0, JSMIRVSHAREDPROP|JSPROP_READONLY,0},
	{NULL,NULL,0,0,0}
};

static JSClass Info_class = {
    "Info", 0,
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
} g_ScriptEvents;

enum Events_tinyid {
	TID_Events_On_glBegin,
	TID_Events_On_glEnd,
	TID_Events_On_RecordStarting,
	TID_Events_On_RecordEnded
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
    "Global", 0,
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

		// .:
		&& NULL != (g_JsGlobal = JS_NewObject(g_JsCx, &Global_class, NULL, NULL)) // .
		&& JS_InitStandardClasses(g_JsCx, g_JsGlobal)
		&& JS_DefineFunctions(g_JsCx, g_JsGlobal, Global_functions)

		// .addr:
		&& NULL != (jo = JS_DefineObject(g_JsCx, g_JsGlobal, "addr", &Addr_class, NULL, JSMIRVPROP|JSPROP_READONLY))

		// .info:
		&& NULL != (jo = JS_DefineObject(g_JsCx, g_JsGlobal, "info", &Info_class, NULL, JSMIRVPROP|JSPROP_READONLY))
		&& JS_DefineProperties(g_JsCx, jo, Info_props)
		&& JS_DefineFunctions(g_JsCx, jo, Info_functions)

		// .events:
		&& NULL != (jo = JS_DefineObject(g_JsCx, g_JsGlobal, "events", &Events_class, NULL, JSMIRVPROP|JSPROP_READONLY))
		&& JS_DefineProperties(g_JsCx, jo, Events_props)

		// .fx:
		&& NULL != (joFx = JS_DefineObject(g_JsCx, g_JsGlobal, "fx", &Fx_class, NULL, JSMIRVPROP|JSPROP_READONLY))

		// .fx.color:
		&& NULL != (jo = JS_DefineObject(g_JsCx, joFx, "color", &FxColor_class, NULL, JSMIRVPROP|JSPROP_READONLY))
		&& JS_DefineProperties(g_JsCx, jo, FxColor_props)

		// .fx.hide:
		&& NULL != (jo = JS_DefineObject(g_JsCx, joFx, "hide", &FxHide_class, NULL, JSMIRVPROP|JSPROP_READONLY))
		&& JS_DefineProperties(g_JsCx, jo, FxHide_props)

		// .fx.replace:
		&& NULL != (jo = JS_DefineObject(g_JsCx, joFx, "rgbMask", &FxRgbMask_class, NULL, JSMIRVPROP|JSPROP_READONLY))
		&& JS_DefineProperties(g_JsCx, jo, FxRgbMask_props)
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
