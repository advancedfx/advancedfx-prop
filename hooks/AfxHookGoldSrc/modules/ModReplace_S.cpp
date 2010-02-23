//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-02-23 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "ModReplace_S.h"

#include "ModReplace.h"



enum ModReplace_tinyid {
	TID_ModReplace_Enabled,
	TID_ModReplace_Blue,
	TID_ModReplace_Green,
	TID_ModReplace_Red,
	TID_ModReplace_Supported
};

JSBool ModReplace_Blue_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModReplace.Blue_get());
	return JS_TRUE;
}

JSBool ModReplace_Blue_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	JSBool jB;
	if(JS_ValueToBoolean(cx, *vp, &jB)) g_ModReplace.Blue_set(jB);
	return JS_TRUE;
}

JSBool ModReplace_Enabled_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModReplace.Enabled_get());
	return JS_TRUE;
}

JSBool ModReplace_Enabled_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	JSBool jB;
	if(JS_ValueToBoolean(cx, *vp, &jB)) g_ModReplace.Enabled_set(jB);
	return JS_TRUE;
}

JSBool ModReplace_Green_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModReplace.Green_get());
	return JS_TRUE;
}

JSBool ModReplace_Green_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	JSBool jB;
	if(JS_ValueToBoolean(cx, *vp, &jB)) g_ModReplace.Green_set(jB);
	return JS_TRUE;
}

JSBool ModReplace_Red_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModReplace.Red_get());
	return JS_TRUE;
}

JSBool ModReplace_Red_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	JSBool jB;
	if(JS_ValueToBoolean(cx, *vp, &jB)) g_ModReplace.Red_set(jB);
	return JS_TRUE;
}

JSBool ModReplace_Supported_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModReplace.Supported_get());
	return JS_TRUE;
}

static JSPropertySpec ModReplace_props[] = {
  {"blue"     , TID_ModReplace_Blue     , JSMIRVSHAREDPROP, ModReplace_Blue_get, ModReplace_Blue_set},
  {"enabled"  , TID_ModReplace_Enabled  , JSMIRVSHAREDPROP, ModReplace_Enabled_get, ModReplace_Enabled_set},
  {"green"    , TID_ModReplace_Green    , JSMIRVSHAREDPROP, ModReplace_Green_get, ModReplace_Green_set},
  {"red"      , TID_ModReplace_Red      , JSMIRVSHAREDPROP, ModReplace_Red_get, ModReplace_Red_set},
  {"supported", TID_ModReplace_Supported, JSMIRVSHAREDPROP|JSPROP_READONLY, ModReplace_Supported_get, NULL              },
  {NULL,0,0,NULL,NULL}
};

static JSClass ModReplace_class = {
    "ModReplace", 0,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};


JSObject * ModReplace_S_Register(JSContext * cx, JSObject * obj, char const * name)
{
	JSObject * jo = JS_DefineObject(cx, obj, name, &ModReplace_class, NULL, JSMIRVPROP|JSPROP_READONLY);

	if(jo && JS_DefineProperties(cx, jo, ModReplace_props))
		return jo;

	return 0;
}