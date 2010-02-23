//#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-02-23 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "ModHide_S.h"

#include "ModHide.h"


enum ModHide_tinyid {
	TID_ModHide_Enabled
};


JSBool ModHide_Enabled_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModHide.Enabled_get());
	return JS_TRUE;
}

JSBool ModHide_Enabled_set(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	JSBool jB;
	if(JS_ValueToBoolean(cx, *vp, &jB)) g_ModHide.Enabled_set(jB);
	return JS_TRUE;
}


static JSPropertySpec ModHide_props[] = {
  {"enabled"  , TID_ModHide_Enabled  , JSMIRVSHAREDPROP, ModHide_Enabled_get, ModHide_Enabled_set},
  {NULL,0,0,NULL,NULL}
};

static JSClass ModHide_class = {
    "ModHide", 0,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};


JSObject * ModHide_S_Register(JSContext * cx, JSObject * obj, char const * name)
{
	JSObject * jo = JS_DefineObject(cx, obj, "hide", &ModHide_class, NULL, JSMIRVPROP|JSPROP_READONLY);

	if(jo && JS_DefineProperties(cx, jo, ModHide_props))
		return jo;

	return 0;
}