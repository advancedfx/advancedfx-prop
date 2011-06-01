#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2010-03-05 dominik.matrixstorm.com
//
// First changes
// 2010-03-05 dominik.matrixstorm.com

#include "bits.h"
#include "bits_Js.h"

#include <list>


// Nop /////////////////////////////////////////////////////////////////////////

class Nop :
	public IBit
{
public:
	virtual void Execute() {
	}
};


// Seq /////////////////////////////////////////////////////////////////////////

class Seq :
	public IBit
{
public:
	Seq() {
	}

	virtual void Execute() override {
		for(std::list<IBit *>::iterator it = m_Bits.begin(); it != m_Bits.end(); it++)
			(*it)->Execute();
	}

	int Count_get() {
		return m_Bits.size();
	}

	IBit * Items_get(int index) {

	}

private:
	std::list<IBit *> m_Bits;
};


void Seq_Js_Finalize(JSContext *cx, JSObject *obj)
{
	delete (Seq *)JS_GetPrivate(cx, obj);
}

static JSClass Seq_Js = {
    "ModInfo", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, Seq_Js_Finalize,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

JSBool Seq_Js_New(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_FALSE;
}

JSObject * Seq_Js_Register(JSContext * cx, JSObject * obj, char const * name)
{
/*	JS_DefineFunction(cx, obj, name, Seq_Js_New, 0, 0

	JSObject * jo = JS_DefineObject(cx, obj, name, &Seq_Js, NULL, JSMIRVPROP|JSPROP_READONLY);

	if(jo
		&& JS_SetPrivate(cx, obj, (void *)new Seq());
		&& JS_DefineProperties(cx, obj, ModInfo_props)
		&& JS_DefineFunctions(cx, obj, ModInfo_functions)
	)
		return jo;
*/	
	return 0;
}



////////////////////////////////////////////////////////////////////////////////

JSObject * Bits_Js_Register(JSContext * cx, JSObject * obj, char const * name)
{
	return 0;
}
