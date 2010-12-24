#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-02-23 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "ModColorMask_S.h"

#include "ModColorMask.h"


// ModColorMask //////////////////////////////////////////////////////////////////////

enum ModColorMask_tinyid {
  TID_ModColorMask_Alpha,
  TID_ModColorMask_Blue,
  TID_ModColorMask_Enabled,
  TID_ModColorMask_Green,
  TID_ModColorMask_Red
};


JSBool ModColorMask_Alpha_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = INT_TO_JSVAL(g_ModColorMask.Alpha_get());
	return JS_TRUE;
}

JSBool ModColorMask_Alpha_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	int32 i32;
	if(JS_ValueToInt32(cx, *vp, &i32)) g_ModColorMask.Alpha_set(i32);
	return JS_TRUE;
}

JSBool ModColorMask_Blue_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = INT_TO_JSVAL(g_ModColorMask.Blue_get());
	return JS_TRUE;
}

JSBool ModColorMask_Blue_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	int32 i32;
	if(JS_ValueToInt32(cx, *vp, &i32)) g_ModColorMask.Blue_set(i32);
	return JS_TRUE;
}



JSBool ModColorMask_Enabled_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModColorMask.Enabled_get());
	return JS_TRUE;
}

JSBool ModColorMask_Enabled_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	JSBool jB;
	if(JS_ValueToBoolean(cx, *vp, &jB)) g_ModColorMask.Enabled_set(jB);
	return JS_TRUE;
}


JSBool ModColorMask_Green_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = INT_TO_JSVAL(g_ModColorMask.Green_get());
	return JS_TRUE;
}

JSBool ModColorMask_Green_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	int32 i32;
	if(JS_ValueToInt32(cx, *vp, &i32)) g_ModColorMask.Green_set(i32);
	return JS_TRUE;
}


JSBool ModColorMask_Red_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = INT_TO_JSVAL(g_ModColorMask.Red_get());
	return JS_TRUE;
}

JSBool ModColorMask_Red_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	int32 i32;
	if(JS_ValueToInt32(cx, *vp, &i32)) g_ModColorMask.Red_set(i32);
	return JS_TRUE;
}

static JSPropertySpec ModColorMask_props[] = {
  {"Alpha"  , TID_ModColorMask_Alpha  , JSMIRVSHAREDPROP, ModColorMask_Alpha_get  , ModColorMask_Alpha_set  },
  {"Blue"   , TID_ModColorMask_Blue   , JSMIRVSHAREDPROP, ModColorMask_Blue_get   , ModColorMask_Blue_set   },
  {"enabled", TID_ModColorMask_Enabled, JSMIRVSHAREDPROP, ModColorMask_Enabled_get, ModColorMask_Enabled_set},
  {"Green"  , TID_ModColorMask_Green  , JSMIRVSHAREDPROP, ModColorMask_Green_get  , ModColorMask_Green_set  },
  {"Red"    , TID_ModColorMask_Red    , JSMIRVSHAREDPROP, ModColorMask_Red_get    , ModColorMask_Red_set    },
  {NULL,0,0,NULL,NULL}
};

static JSClass ModColorMask_class = {
    "ModColorMask", 0,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};


JSObject * ModColorMask_S_Register(JSContext * cx, JSObject * obj, char const * name)
{
	JSObject * jo = JS_DefineObject(cx, obj, name, &ModColorMask_class, NULL, JSMIRVPROP|JSPROP_READONLY);

	if(jo && JS_DefineProperties(cx, jo, ModColorMask_props))
		return jo;

	return 0;
}