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
#include "FxRgbMask.h"
#include "MirvInfo.h"

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

#include <malloc.h>
#include <string.h>

#define XP_WIN
#include <jsapi.h>
#include <jsstr.h>

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
    {"enabled", FxColor_ENABLED, JSPROP_ENUMERATE, NULL, NULL},
    {"red"    , FxColor_RED    , JSPROP_ENUMERATE, NULL, NULL},
    {"green"  , FxColor_GREEN  , JSPROP_ENUMERATE, NULL, NULL},
    {"blue"   , FxColor_BLUE   , JSPROP_ENUMERATE, NULL, NULL},
    {"alpha"  , FxColor_ALPHA  , JSPROP_ENUMERATE, NULL, NULL},
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
	FxRgbMask_ENABLED, FxRgbMask_OPRED, FxRgbMask_OPGREEN, FxRgbMask_OPBLUE,
	FxRgbMask_SUPPORTED
};

static JSPropertySpec FxRgbMask_props[] = {
	{"enabled"  , FxRgbMask_ENABLED  , JSPROP_ENUMERATE, NULL, NULL},
    {"opRed"    , FxRgbMask_OPRED    , JSPROP_ENUMERATE, NULL, NULL},
    {"opGreen"  , FxRgbMask_OPGREEN  , JSPROP_ENUMERATE, NULL, NULL},
    {"opBlue"   , FxRgbMask_OPBLUE   , JSPROP_ENUMERATE, NULL, NULL},
	{"supported", FxRgbMask_SUPPORTED, JSPROP_READONLY , NULL, NULL},
    {NULL,0,0,NULL,NULL}
};

static JSBool
FxRgbMask_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    if (!JSVAL_IS_INT(id)) return JS_TRUE;

	switch (JSVAL_TO_INT(id)) {
	case FxRgbMask_ENABLED:
		*vp = BOOLEAN_TO_JSVAL(g_FxRgbMask.Enabled_get());
		break;
	case FxRgbMask_OPRED:
		*vp = INT_TO_JSVAL(g_FxRgbMask.OpRed_get());
		break;
	case FxRgbMask_OPGREEN:
		*vp = INT_TO_JSVAL(g_FxRgbMask.OpGreen_get());
		break;
	case FxRgbMask_OPBLUE:
		*vp = INT_TO_JSVAL(g_FxRgbMask.OpBlue_get());
		break;
	case FxRgbMask_SUPPORTED:
		*vp = BOOLEAN_TO_JSVAL(g_FxRgbMask.Supported_get());
		break;
	}

    return JS_TRUE;
}


JS_STATIC_DLL_CALLBACK(JSBool)
FxRgbMask_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    if (!JSVAL_IS_INT(id)) return JS_TRUE;

	int32 i32;
	JSBool jB;

	switch (JSVAL_TO_INT(id)) {
	case FxRgbMask_ENABLED:
		if(JS_ValueToBoolean(cx, *vp, &jB)) g_FxRgbMask.Enabled_set(jB);
		break;
	case FxRgbMask_OPRED:
		if(JS_ValueToInt32(cx, *vp, &i32)) g_FxRgbMask.OpRed_set(i32);
		break;
	case FxRgbMask_OPGREEN:
		if(JS_ValueToInt32(cx, *vp, &i32)) g_FxRgbMask.OpGreen_set(i32);
		break;
	case FxRgbMask_OPBLUE:
		if(JS_ValueToInt32(cx, *vp, &i32)) g_FxRgbMask.OpBlue_set(i32);
		break;
	}

	return JS_TRUE;
}

static JSClass FxRgbMask_class = {
    "FxRgbMask", 0,
    JS_PropertyStub, JS_PropertyStub, FxRgbMask_getProperty, FxRgbMask_setProperty,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};


// Info /////////////////////////////////////////////////////////////////////

enum Info_tinyid {
	Info_RECORDING
};

JSBool Info_Recording_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_MirvInfo.Recording_get());
	return JS_TRUE;
}


static JSPropertySpec Info_props[] = {
	{"recording"  , Info_RECORDING  , JSPROP_READONLY, Info_Recording_get, NULL},
    {NULL,0,0,NULL,NULL}
};

static JSClass Info_class = {
    "Info", 0,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};


// Fx //////////////////////////////////////////////////////////////////////////

static JSClass Fx_class = {
    "Fx", 0,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
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
        JS_SetOptions(cx, oldopts | JSOPTION_COMPILE_N_GO);
        script = JS_CompileFile(cx, obj, filename);
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
	{"load" , Global_Load , 1,0,0},
    {"print", Global_Print, 0,0,0},
	{NULL,NULL,0,0,0}
};



////////////////////////////////////////////////////////////////////////////////

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

		// .info:
		&& NULL != (jo = JS_DefineObject(g_JsCx, g_JsGlobal, "info", &Info_class, NULL, JSPROP_READONLY))
		&& JS_DefineProperties(g_JsCx, jo, Info_props)


		// .fx:
		&& NULL != (joFx = JS_DefineObject(g_JsCx, g_JsGlobal, "fx", &Fx_class, NULL, JSPROP_READONLY))

		// .fx.color:
		&& NULL != (jo = JS_DefineObject(g_JsCx, joFx, "color", &FxColor_class, NULL, JSPROP_READONLY))
		&& JS_DefineProperties(g_JsCx, jo, FxColor_props)

		// .fx.rgbMask:
		&& NULL != (jo = JS_DefineObject(g_JsCx, joFx, "rgbMask", &FxRgbMask_class, NULL, JSPROP_READONLY))
		&& JS_DefineProperties(g_JsCx, jo, FxRgbMask_props)
	;

	if(!bOk)
		JsShutDown();

	g_JsRunning = bOk;

	return bOk;
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
