#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-02-23 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

#include "ModInfo_S.h"

#include "ModInfo.h"

// ModInfo /////////////////////////////////////////////////////////////////////

enum ModInfo_tinyid {
	TID_ModInfo_In_R_DrawEntitiesOnList,
	TID_ModInfo_In_R_DrawParticles,
	TID_ModInfo_In_R_DrawViewModel,
	TID_ModInfo_In_R_Renderview,
	TID_ModInfo_Recording
};

JSBool ModInfo_In_R_DrawEntitiesOnList_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModInfo.In_R_DrawEntitiesOnList_get());
	return JS_TRUE;
}

JSBool ModInfo_In_R_DrawParticles_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModInfo.In_R_DrawParticles_get());
	return JS_TRUE;
}

JSBool ModInfo_In_R_DrawViewModel_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModInfo.In_R_DrawViewModel_get());
	return JS_TRUE;
}

JSBool ModInfo_In_R_Renderview_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModInfo.In_R_Renderview_get());
	return JS_TRUE;
}

JSBool ModInfo_Recording_get(JSContext *cx, JSObject *obj, jsval idval, jsval *vp) {
	*vp = BOOLEAN_TO_JSVAL(g_ModInfo.Recording_get());
	return JS_TRUE;
}

static JSPropertySpec ModInfo_props[] = {
  {"in_R_DrawEntitiesOnList", TID_ModInfo_In_R_DrawEntitiesOnList, JSMIRVSHAREDPROP|JSPROP_READONLY, ModInfo_In_R_DrawEntitiesOnList_get, NULL},
  {"in_R_DrawParticles"     , TID_ModInfo_In_R_DrawParticles     , JSMIRVSHAREDPROP|JSPROP_READONLY, ModInfo_In_R_DrawParticles_get     , NULL},
  {"in_R_DrawViewModel"     , TID_ModInfo_In_R_DrawViewModel     , JSMIRVSHAREDPROP|JSPROP_READONLY, ModInfo_In_R_DrawViewModel_get     , NULL},
  {"in_R_Renderview"        , TID_ModInfo_In_R_Renderview        , JSMIRVSHAREDPROP|JSPROP_READONLY, ModInfo_In_R_Renderview_get        , NULL},
  {"recording"              , TID_ModInfo_Recording              , JSMIRVSHAREDPROP|JSPROP_READONLY, ModInfo_Recording_get              , NULL},
  {NULL,0,0,NULL,NULL}
};

static JSBool
ModInfo_GetCurrentEntityIndex(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = INT_TO_JSVAL(g_ModInfo.GetCurrentEntityIndex());	

    return JS_TRUE;
}

static JSFunctionSpec ModInfo_functions[] = {
    {"getCurrentEntityIndex", ModInfo_GetCurrentEntityIndex, 0, JSMIRVSHAREDPROP|JSPROP_READONLY,0},
	{NULL,NULL,0,0,0}
};

static JSClass ModInfo_class = {
    "ModInfo", 0,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

JSObject * ModInfo_S_Register(JSContext * cx, JSObject * obj, char const * name)
{
	JSObject * jo = JS_DefineObject(cx, obj, name, &ModInfo_class, NULL, JSMIRVPROP|JSPROP_READONLY);

	if(jo
		&& JS_DefineProperties(cx, jo, ModInfo_props)
		&& JS_DefineFunctions(cx, jo, ModInfo_functions)
	)
		return jo;
	
	return 0;
}


