//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-02-23 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "ModColor_S.h"

#include "ModColor.h"

enum ModColor_tinyid {
	TID_ModColor_Alpha,
	TID_ModColor_Blue,
	TID_ModColor_Enabled,
	TID_ModColor_Green,
	TID_ModColor_Red
};


JSBool ModColor_Alpha_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = DOUBLE_TO_JSVAL(g_ModColor.Alpha_get());
	return JS_TRUE;
}

JSBool ModColor_Alpha_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	jsdouble jD;
	if(JS_ValueToNumber(cx, *vp, &jD)) g_ModColor.Alpha_set(jD);
	return JS_TRUE;
}

JSBool ModColor_Blue_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = DOUBLE_TO_JSVAL(g_ModColor.Blue_get());
	return JS_TRUE;
}

JSBool ModColor_Blue_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	jsdouble jD;
	if(JS_ValueToNumber(cx, *vp, &jD)) g_ModColor.Blue_set(jD);
	return JS_TRUE;
}


JSBool ModColor_Enabled_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModColor.Enabled_get());
	return JS_TRUE;
}

JSBool ModColor_Enabled_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	JSBool jB;
	if(JS_ValueToBoolean(cx, *vp, &jB)) g_ModColor.Enabled_set(jB);
	return JS_TRUE;
}

JSBool ModColor_Green_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = DOUBLE_TO_JSVAL(g_ModColor.Green_get());
	return JS_TRUE;
}

JSBool ModColor_Green_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	jsdouble jD;
	if(JS_ValueToNumber(cx, *vp, &jD)) g_ModColor.Green_set(jD);
	return JS_TRUE;
}

JSBool ModColor_Red_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = DOUBLE_TO_JSVAL(g_ModColor.Red_get());
	return JS_TRUE;
}

JSBool ModColor_Red_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	jsdouble jD;
	if(JS_ValueToNumber(cx, *vp, &jD)) g_ModColor.Red_set(jD);
	return JS_TRUE;
}

static JSPropertySpec ModColor_props[] = {
  {"alpha"  , TID_ModColor_Alpha  , JSMIRVSHAREDPROP, ModColor_Alpha_get  , ModColor_Alpha_set  },
  {"blue"   , TID_ModColor_Blue   , JSMIRVSHAREDPROP, ModColor_Blue_get   , ModColor_Blue_set   },
  {"enabled", TID_ModColor_Enabled, JSMIRVSHAREDPROP, ModColor_Enabled_get, ModColor_Enabled_set},
  {"green"  , TID_ModColor_Green  , JSMIRVSHAREDPROP, ModColor_Green_get  , ModColor_Green_set  },
  {"red"    , TID_ModColor_Red    , JSMIRVSHAREDPROP, ModColor_Red_get    , ModColor_Red_set    },
  {NULL,0,0,NULL,NULL}
};

static JSClass ModColor_class = {
  "ModColor", 0,
  JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
  JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
  JSCLASS_NO_OPTIONAL_MEMBERS
};

JSObject * ModColor_S_Register(JSContext * cx, JSObject * obj, char const * name)
{
	JSObject * jo = JS_DefineObject(cx, obj, name, &ModColor_class, NULL, JSMIRVPROP|JSPROP_READONLY);	

	if(jo && JS_DefineProperties(cx, jo, ModColor_props))
		return jo;

	return 0;
}