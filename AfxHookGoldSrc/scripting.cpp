#include "stdafx.h"

// Copyright (c) by advancedfx.org
//
// Last changes:
// 2014-02-12 dominik.matrixstorm.com
//
// First changes
// 2009-11-16 dominik.matrixstorm.com

// TODO: JS_ReportError before return JS_FALSE, so users actually see an
// error and not only the program flow is interrupted.

#include "scripting.h"

#include <Windows.h>
#include <GL/GL.h>

#include <hlsdk.h>

#include "cmdregister.h"
#include "hooks/HookHw.h"
#include "hooks/OpenGl32Hooks.h"
#include "hooks/user32Hooks.h"
#include "hooks/client/cstrike/CrossHairFix.h"
#include "hooks/hw/R_DrawEntitiesOnList.h"
#include "hooks/hw/R_DrawParticles.h"
#include "hooks/hw/R_DrawViewModel.h"
#include "hooks/hw/R_RenderView.h"
#include "hooks/hw/UnkDrawHud.h"
#include "mirv_glext.h"
#include "supportrender.h"
#include "AfxGlImage.h"
#include "AfxImageUtils.h"
#include "film_sound.h"
#include "filming.h"
#include "AfxGoldSrcComClient.h"
#include "hlaeFolder.h"

#include <shared/FileTools.h>
#include <shared/StringTools.h>

#include <string>
#include <sstream>

#include <script.h>

#define SCRIPT_FOLDER "scripts\\"

std::string g_ScriptFolder("");

struct {
	bool Has;
	std::string String;
} g_LastFirstError = { false, "" };

JSRuntime * g_JsRt = NULL;
JSContext * g_JsCx = NULL;
JSObject * g_JsGlobal = NULL;
bool g_JsRunning = false;
bool g_Script_CanConsolePrint = false;


void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
	if(g_LastFirstError.Has)
		return;

	g_LastFirstError.Has = true;

	std::ostringstream oss;
	
	oss << "AFX ERROR: "
		<< (report->filename ? report->filename : "<no filename>") << ":"
		<< (unsigned int) report->lineno << ":"
		<< message
	;

	g_LastFirstError.String = oss.str();
}

// JsTestObject ////////////////////////////////////////////////////////////////

class JsTestObject
{
public:
	JsTestObject() {
		if(g_Script_CanConsolePrint) pEngfuncs->Con_Printf("JsTestObject(0x%08x)::JsTestObject\n", this);
	}

	~JsTestObject() {
		if(g_Script_CanConsolePrint) pEngfuncs->Con_Printf("JsTestObject(0x%08x)::~JsTestObject\n", this);
	}

	void SetValue(int value)
	{
		if(g_Script_CanConsolePrint) pEngfuncs->Con_Printf("JsTestObject(0x%08x)::SetValue(%i)\n", this, value);
		m_Value = value;
	}

	int GetValue()
	{
		if(g_Script_CanConsolePrint) pEngfuncs->Con_Printf("JsTestObject(0x%08x)::GetValue = (%i)\n", this, m_Value);
		return m_Value;
	}
private:
	int m_Value;
};


void JsTestObject_finalize(JSFreeOp *fop, JSObject *obj)
{
	JsTestObject *p = (JsTestObject *)JS_GetPrivate(obj);
	delete p;
}

static JSClass JsTestObject_class = {
	"TestObject", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_DeletePropertyStub,
	JS_PropertyStub,
	JS_StrictPropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	JsTestObject_finalize,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool
JsTestObject_value_get(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
	JsTestObject *p = (JsTestObject *)JS_GetPrivate(obj);

	vp.set(JS_NumberValue(p->GetValue()));
    return JS_TRUE;
}

static JSBool
JsTestObject_value_set(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JSBool strict, JS::MutableHandleValue vp)
{
	JsTestObject *p = (JsTestObject *)JS_GetPrivate(obj);

	int32_t value;

	JS::ToInt32(cx, vp.get(), &value);

	p->SetValue(value);

	return JS_TRUE;
}

static JSPropertySpec  JsTestObject_properties[] = {
	{"value", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED, JSOP_WRAPPER(JsTestObject_value_get), JSOP_WRAPPER(JsTestObject_value_set)},
	{0, 0, 0, 0, 0}
};

// AfxGlImage //////////////////////////////////////////////////////////////////

void AfxGlImage_finalize(JSFreeOp *fop, JSObject *obj)
{
	AfxGlImage *p = (AfxGlImage *)JS_GetPrivate(obj);
	delete p;
}

static JSClass AfxGlImage_class = {
	"AfxGlImage", JSCLASS_HAS_PRIVATE,
	JS_PropertyStub,
	JS_DeletePropertyStub,
	JS_PropertyStub,
	JS_StrictPropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	AfxGlImage_finalize,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool
AfxGlImage_debugFloatDepthBuffer(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(0 > args.length())
		return JS_FALSE;

	JSObject * jsObj;
	if(!JS_ValueToObject(cx, args.thisv(), &jsObj))
		return JS_FALSE;

	AfxGlImage *afxGlImage = (AfxGlImage *)JS_GetPrivate(jsObj);

	if(!AfxImageUtils::DebugFloatDepthBuffer(afxGlImage))
		return JS_FALSE;

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlImage_floatDepthBufferToByteBuffer(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(0 > args.length())
		return JS_FALSE;

	JSObject * jsObj;
	if(!JS_ValueToObject(cx, args.thisv(), &jsObj))
		return JS_FALSE;

	AfxGlImage *afxGlImage = (AfxGlImage *)JS_GetPrivate(jsObj);

	if(!AfxImageUtils::FloatDepthBufferToByteBuffer(afxGlImage))
		return JS_FALSE;

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlImage_glReadPixels(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(6 > args.length())
		return JS_FALSE;

	JSObject * jsObj;
	if(!JS_ValueToObject(cx, args.thisv(), &jsObj))
		return JS_FALSE;

	AfxGlImage *afxGlImage = (AfxGlImage *)JS_GetPrivate(jsObj);

	GLint x;
	GLint y;
	GLsizei width;
	GLsizei height;
	GLenum  format;
	GLenum  type;
	
	if(!(
		JS::ToInt32(cx, args[0], &x)
		&& JS::ToInt32(cx, args[1], &y)
		&& JS::ToInt32(cx, args[2], &width)
		&& JS::ToInt32(cx, args[3], &height)
		&& JS::ToUint32(cx, args[4], &format)
		&& JS::ToUint32(cx, args[5], &type)
	))
		return JS_FALSE;

	if(!afxGlImage->GlReadPixels(x, y, width, height, format, type))
		return JS_FALSE;

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlImage_inverseFloatDepthBuffer(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(2 > args.length())
		return JS_FALSE;

	JSObject * jsObj;
	if(!JS_ValueToObject(cx, args.thisv(), &jsObj))
		return JS_FALSE;

	AfxGlImage *afxGlImage = (AfxGlImage *)JS_GetPrivate(jsObj);

	GLdouble zNear;
	GLdouble zFar;
	
	if(!(
		JS::ToNumber(cx, args[0], &zNear)
		&& JS::ToNumber(cx, args[1], &zFar)
	))
		return JS_FALSE;

	if(!AfxImageUtils::InverseFloatDepthBuffer(afxGlImage, zNear, zFar))
		return JS_FALSE;

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlImage_linearizeFloatDepthBuffer(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(2 > args.length())
		return JS_FALSE;

	JSObject * jsObj;
	if(!JS_ValueToObject(cx, args.thisv(), &jsObj))
		return JS_FALSE;

	AfxGlImage *afxGlImage = (AfxGlImage *)JS_GetPrivate(jsObj);

	GLdouble zNear;
	GLdouble zFar;
	
	if(!(
		JS::ToNumber(cx, args[0], &zNear)
		&& JS::ToNumber(cx, args[1], &zFar)
	))
		return JS_FALSE;

	if(!AfxImageUtils::LinearizeFloatDepthBuffer(afxGlImage, zNear, zFar))
		return JS_FALSE;

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlImage_logarithmizeFloatDepthBuffer(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(2 > args.length())
		return JS_FALSE;

	JSObject * jsObj;
	if(!JS_ValueToObject(cx, args.thisv(), &jsObj))
		return JS_FALSE;

	AfxGlImage *afxGlImage = (AfxGlImage *)JS_GetPrivate(jsObj);

	GLdouble zNear;
	GLdouble zFar;
	
	if(!(
		JS::ToNumber(cx, args[0], &zNear)
		&& JS::ToNumber(cx, args[1], &zFar)
	))
		return JS_FALSE;

	if(!AfxImageUtils::LogarithmizeFloatDepthBuffer(afxGlImage, zNear, zFar))
		return JS_FALSE;

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlImage_sliceFloatDepthBuffer(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(2 > args.length())
		return JS_FALSE;

	JSObject * jsObj;
	if(!JS_ValueToObject(cx, args.thisv(), &jsObj))
		return JS_FALSE;

	AfxGlImage *afxGlImage = (AfxGlImage *)JS_GetPrivate(jsObj);

	GLdouble sliceLo;
	GLdouble sliceHi;
	
	if(!(
		JS::ToNumber(cx, args[0], &sliceLo)
		&& JS::ToNumber(cx, args[1], &sliceHi)
	))
		return JS_FALSE;

	if(!AfxImageUtils::SliceFloatDepthBuffer(afxGlImage, sliceLo, sliceHi))
		return JS_FALSE;

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlImage_writeBitmap(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

	JSObject * jsObj;
	if(!JS_ValueToObject(cx, args.thisv(), &jsObj))
		return JS_FALSE;

	AfxGlImage *afxGlImage = (AfxGlImage *)JS_GetPrivate(jsObj);

    JSString *str = JS_ValueToString(cx, args[0]);
    if (!str)
        return JS_FALSE;

    char *c_str = JS_EncodeString(cx, str);
	if(!c_str)
		return JS_FALSE;

	std::wstring wFileName;

	bool bOk = AnsiStringToWideString(c_str, wFileName);

	JS_free(cx, c_str);

	if(!(bOk
		&& AfxImageUtils::WriteBitmap(afxGlImage, wFileName.c_str())))
		return JS_FALSE;

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSFunctionSpec AfxGlImage_functions[] = {
    JS_FS("debugFloatDepthBuffer", AfxGlImage_debugFloatDepthBuffer, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("floatDepthBufferToByteBuffer", AfxGlImage_floatDepthBufferToByteBuffer, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glReadPixels", AfxGlImage_glReadPixels, 6, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("inverseFloatDepthBuffer", AfxGlImage_inverseFloatDepthBuffer, 2, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("linearizeFloatDepthBuffer", AfxGlImage_linearizeFloatDepthBuffer, 2, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("logarithmizeFloatDepthBuffer", AfxGlImage_logarithmizeFloatDepthBuffer, 2, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("sliceFloatDepthBuffer", AfxGlImage_sliceFloatDepthBuffer, 2, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("writeBitmap", AfxGlImage_writeBitmap, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS_END
};


// AfxGlobal ///////////////////////////////////////////////////////////////////

static JSClass AfxGlobal_class = {
	"AfxGlobal", JSCLASS_GLOBAL_FLAGS,
	JS_PropertyStub,
	JS_DeletePropertyStub,
	JS_PropertyStub,
	JS_StrictPropertyStub,
	JS_EnumerateStub,
	JS_ResolveStub,
	JS_ConvertStub,
	NULL,
	JSCLASS_NO_OPTIONAL_MEMBERS
};

static JSBool
AfxGlobal_additionalRRenderView(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallReceiver rec = JS::CallReceiverFromVp(vp);

	Additional_R_RenderView();

	rec.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_additionalUnkDrawHud(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallReceiver rec = JS::CallReceiverFromVp(vp);

	Additional_UnkDrawHud();

	rec.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_afxFilmingStart(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallReceiver rec = JS::CallReceiverFromVp(vp);

	g_AfxGoldSrcComClient.OnRecordStarting();

	if(g_AfxGoldSrcComClient.GetOptimizeCaptureVis())
		UndockGameWindowForCapture();

	if (g_pSupportRender)
		g_pSupportRender->hlaeOnFilmingStart();

	rec.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_afxFilmingStop(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallReceiver rec = JS::CallReceiverFromVp(vp);

	if (g_pSupportRender)
		g_pSupportRender->hlaeOnFilmingStop();

	if(g_AfxGoldSrcComClient.GetOptimizeCaptureVis())
		RedockGameWindow();

	g_AfxGoldSrcComClient.OnRecordEnded();

	rec.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_createPath(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

    JSString *str = JS_ValueToString(cx, args[0]);
    if (!str)
        return JS_FALSE;

    char *c_str = JS_EncodeString(cx, str);
	if(!c_str)
		return JS_FALSE;

	bool bOk = false;

	std::wstring wPath;
	std::string path;

	if(AnsiStringToWideString(c_str, wPath)
		&& CreatePath(wPath.c_str(), wPath)
		&& WideStringToAnsiString(wPath.c_str(), path))
	{
		JSString * str = JS_NewStringCopyZ(cx, path.c_str());

		args.rval().set(STRING_TO_JSVAL(str));
	}
	else
		args.rval().set(JSVAL_NULL);

	JS_free(cx, c_str);

    return JS_TRUE;
}

static JSBool
AfxGlobal_cstrikeCrossHairBlock(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

	g_Cstrike_CrossHair_Block = JS::ToBoolean(args[0]);

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_gc(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallReceiver rec = JS::CallReceiverFromVp(vp);

	JS_GC(g_JsRt);

	rec.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_getZFar(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallReceiver rec = JS::CallReceiverFromVp(vp);

	double value = g_Filming.GetZFar();

	rec.rval().set(JS_NumberValue(value));

	return JS_TRUE;
}

static JSBool
AfxGlobal_getZNear(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallReceiver rec = JS::CallReceiverFromVp(vp);

	double value = g_Filming.GetZNear();

	rec.rval().set(JS_NumberValue(value));

	return JS_TRUE;
}

static JSBool
AfxGlobal_glBlendFunc(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(4 > args.length())
		return JS_FALSE;

	GLenum sfactor, dfactor;
	
	if(!(
		JS::ToUint32(cx, args[0], &sfactor)
		&& JS::ToUint32(cx, args[1], &dfactor)
	))
		return JS_FALSE;

	glBlendFunc(sfactor, dfactor);

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_glClear(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

	GLbitfield mask;

	if(!JS::ToUint32(cx, args[0], &mask))
		return JS_FALSE;
	
	glClear(mask);

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_glClearColor(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(4 > args.length())
		return JS_FALSE;

	double color[4];
	
	if(
		!JS::ToNumber(cx, args[0], &color[0])
		|| !JS::ToNumber(cx, args[1], &color[1])
		|| !JS::ToNumber(cx, args[2], &color[2])
		|| !JS::ToNumber(cx, args[3], &color[3])
	)
		return JS_FALSE;

	glClearColor((GLfloat)color[0], (GLfloat)color[1], (GLfloat)color[2], (GLfloat)color[3]);

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_glColor4d(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(4 > args.length())
		return JS_FALSE;

	double color[4];
	
	if(
		!JS::ToNumber(cx, args[0], &color[0])
		|| !JS::ToNumber(cx, args[1], &color[1])
		|| !JS::ToNumber(cx, args[2], &color[2])
		|| !JS::ToNumber(cx, args[3], &color[3])
	)
		return JS_FALSE;

	glColor4d(color[0], color[1], color[2], color[3]);

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_glColorMask(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(4 > args.length())
		return JS_FALSE;

	glColorMask(JS::ToBoolean(args[0]), JS::ToBoolean(args[1]), JS::ToBoolean(args[2]), JS::ToBoolean(args[3]));

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_glDeleteTextures(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

	JSObject * obj;
	if(!JS_ValueToObject(cx, args[0], &obj))
		return JS_FALSE;

	if(!JS_IsArrayObject(cx, obj))
		return JS_FALSE;

	uint32_t length;
	if(!JS_GetArrayLength(cx, obj, &length))
		return JS_FALSE;

	GLuint * textureIds = new GLuint[length];

	bool bOk = true;
	for(uint32_t i=0; i<length; i++)
	{
		jsval val;
		bOk = bOk && JS_GetElement(cx, obj, i, &val);
		bOk = bOk && JS::ToUint32(cx, val, &textureIds[i]);
	}

	if(!bOk)
	{
		delete textureIds;
		return JS_FALSE;
	}

	glDeleteTextures(length, textureIds);

	delete textureIds;
	
	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_glGenTextures(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

	uint32_t length;
	if(!JS::ToUint32(cx, args[0], &length))
		return JS_FALSE;

	GLuint * textures = new GLuint[length];
	jsval * jsTextures = new jsval[length];

	glGenTextures(length, textures);

	for(uint32_t i=0; i<length; i++)
	{
		jsTextures[i] = JS_NumberValue(textures[i]);
	}

	delete textures;

	JSObject * retObj = JS_NewArrayObject(cx, length, jsTextures);

	delete jsTextures;

	if(!retObj)
		return JS_FALSE;

	args.rval().set(OBJECT_TO_JSVAL(retObj));

	return JS_TRUE;
}

static JSBool
AfxGlobal_glGetBlendFunc(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallReceiver rec = JS::CallReceiverFromVp(vp);

	GLint sfactor, dfactor;
	
	glGetIntegerv(GL_BLEND_SRC, &sfactor);
	glGetIntegerv(GL_BLEND_DST, &dfactor);
	
	jsval jsVals[2] = { JS_NumberValue(sfactor), JS_NumberValue(dfactor) };

	JSObject * retObj = JS_NewArrayObject(cx, 2, jsVals);

	if(!retObj)
		return JS_FALSE;

	rec.rval().set(OBJECT_TO_JSVAL(retObj));

	return JS_TRUE;
}

static JSBool
AfxGlobal_glGetColorWriteMask(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallReceiver rec = JS::CallReceiverFromVp(vp);

	boolean oldColorWriteMask[4];
	
	glGetBooleanv(GL_COLOR_WRITEMASK, oldColorWriteMask);
	
	jsval jsOldColorWriteMask[4] = { BOOLEAN_TO_JSVAL(oldColorWriteMask[0]), BOOLEAN_TO_JSVAL(oldColorWriteMask[1]), BOOLEAN_TO_JSVAL(oldColorWriteMask[2]), BOOLEAN_TO_JSVAL(oldColorWriteMask[3]) };

	JSObject * retObj = JS_NewArrayObject(cx, 4, jsOldColorWriteMask);

	if(!retObj)
		return JS_FALSE;

	rec.rval().set(OBJECT_TO_JSVAL(retObj));

	return JS_TRUE;
}

static JSBool
AfxGlobal_glGetCurrentColor(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallReceiver rec = JS::CallReceiverFromVp(vp);

	double oldColor[4];
	
	glGetDoublev(GL_CURRENT_COLOR, oldColor);
	
	jsval jsOldColor[4] = { JS_NumberValue(oldColor[0]), JS_NumberValue(oldColor[1]), JS_NumberValue(oldColor[2]), JS_NumberValue(oldColor[3]) };

	JSObject * retObj = JS_NewArrayObject(cx, 4, jsOldColor);

	if(!retObj)
		return JS_FALSE;

	rec.rval().set(OBJECT_TO_JSVAL(retObj));

	return JS_TRUE;
}

static JSBool
AfxGlobal_glGetDepthWriteMask(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallReceiver rec = JS::CallReceiverFromVp(vp);

	boolean oldDepthWriteMask;
	
	glGetBooleanv(GL_DEPTH_WRITEMASK, &oldDepthWriteMask);

	rec.rval().set(BOOLEAN_TO_JSVAL(oldDepthWriteMask));

	return JS_TRUE;
}

static JSBool
AfxGlobal_glDepthMask(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

	glDepthMask(JS::ToBoolean(args[0]));

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_load(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

    JSString *str = JS_ValueToString(cx, args[0]);
    if (!str)
        return JS_FALSE;

    char *filename = JS_EncodeString(cx, str);
	if(!filename)
		return JS_FALSE;

	std::string strFile(filename);

	JS_free(cx, filename);

    JS::RootedObject thisobj(cx, JS_THIS_OBJECT(cx, vp));
    if (!thisobj)
        return false;

	JSScript *script = JS::Compile(cx, thisobj, JS::CompileOptions(cx), strFile.c_str());
	if(!script)
		return JS_FALSE;

	jsval result;

	if(!JS_ExecuteScript(cx, thisobj, script, &result))
		return JS_FALSE;

	args.rval().set(result);
	return JS_TRUE;
}


static JSBool
AfxGlobal_hlClientCommand(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

    JSString *str = JS_ValueToString(cx, args[0]);
    if (!str)
        return JS_FALSE;

    char * cmd = JS_EncodeString(cx, str);
	if(!cmd)
		return JS_FALSE;

	pEngfuncs->pfnClientCmd(cmd);

	JS_free(cx, cmd);

	args.rval().set(JSVAL_VOID);
	return JS_TRUE;
}

static JSBool
AfxGlobal_hlCvarSetValue(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(2 > args.length())
		return JS_FALSE;

    JSString *str = JS_ValueToString(cx, args[0]);
    if (!str)
        return JS_FALSE;

    char * cvar = JS_EncodeString(cx, str);
	if(!cvar)
		return JS_FALSE;

	std::string strCvar(cvar);

	JS_free(cx, cvar);

	double value;

	if(!JS::ToNumber(cx, args[1], &value))
		return JS_FALSE;

	pEngfuncs->Cvar_SetValue(const_cast<char *>(strCvar.c_str()), (float)value);

	args.rval().set(JSVAL_VOID);
	return JS_TRUE;
}

static JSBool
AfxGlobal_maybeGc(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::CallReceiver rec = JS::CallReceiverFromVp(vp);

	JS_MaybeGC(cx);

	rec.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_modReplaceOnGlBegin(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

	GLint newTextureBinding2d;

	if(!JS::ToInt32(cx, args[0], &newTextureBinding2d))
		return JS_FALSE;

	GLint oldActiveTextureArb;
	GLboolean oldTexture2d;
	GLint oldTextureBinding2d;
	GLint oldTextureEnvMode;
	
	glGetIntegerv(GL_ACTIVE_TEXTURE_ARB, &oldActiveTextureArb);

	glActiveTextureARB(GL_TEXTURE2_ARB);

	oldTexture2d = glIsEnabled(GL_TEXTURE_2D);
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldTextureBinding2d);
	glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &oldTextureEnvMode);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, newTextureBinding2d);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	jsval jsOldVals[4] = { JS_NumberValue(oldActiveTextureArb), BOOLEAN_TO_JSVAL(oldTexture2d), JS_NumberValue(oldTextureBinding2d), JS_NumberValue(oldTextureEnvMode) };

	JSObject * retObj = JS_NewArrayObject(cx, 4, jsOldVals);

	if(!retObj)
		return JS_FALSE;

	args.rval().set(OBJECT_TO_JSVAL(retObj));

	return JS_TRUE;
}

static JSBool
AfxGlobal_modReplaceOnGlEnd(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(4 > args.length())
		return JS_FALSE;

	GLint oldActiveTextureArb;
	GLboolean oldTexture2d;
	GLint oldTextureBinding2d;
	GLint oldTextureEnvMode;

	if(!JS::ToInt32(cx, args[0], &oldActiveTextureArb))
		return JS_FALSE;

	oldTexture2d = JS::ToBoolean(args[1]);

	if(!JS::ToInt32(cx, args[2], &oldTextureBinding2d))
		return JS_FALSE;
	if(!JS::ToInt32(cx, args[3], &oldTextureEnvMode))
		return JS_FALSE;

	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, oldTextureEnvMode);
	glBindTexture(GL_TEXTURE_2D, oldTextureBinding2d);
	if(!oldTexture2d) glDisable(GL_TEXTURE_2D);

	glActiveTextureARB(oldActiveTextureArb);
	
	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_modReplaceRefreshTexture(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(4 > args.length())
		return JS_FALSE;

	GLuint texture;
	uint32_t red;
	uint32_t green;
	uint32_t blue;

	if(!JS::ToUint32(cx, args[0], &texture))
		return JS_FALSE;
	if(!JS::ToUint32(cx, args[1], &red))
		return JS_FALSE;
	if(!JS::ToUint32(cx, args[2], &green))
		return JS_FALSE;
	if(!JS::ToUint32(cx, args[3], &blue))
		return JS_FALSE;

	GLint oldtex;
	GLubyte texmem[48];

	glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldtex);
		
	for(int i=0; i<16; i++)
	{
		texmem[3*i  ] = (GLubyte)red;
		texmem[3*i+1] = (GLubyte)green;
		texmem[3*i+2] = (GLubyte)blue;
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_UNSIGNED_BYTE, texmem);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);

	glBindTexture(GL_TEXTURE_2D, oldtex);
	
	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_newAfxGlImage(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JSObject *obj = JS_NewObjectWithGivenProto(cx, &AfxGlImage_class, NULL, JS_GetGlobalForScopeChain(cx));
    
	JS_DefineFunctions(cx, obj, AfxGlImage_functions);

	JS_SetPrivate(obj, new AfxGlImage());

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
    return JS_TRUE;
}

static JSBool
AfxGlobal_print(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

    JSString *str = JS_ValueToString(cx, args[0]);
    if (!str)
        return JS_FALSE;

    char *c_str = JS_EncodeString(cx, str);
	if(!c_str)
		return JS_FALSE;

	pEngfuncs->Con_Printf("%s\n", c_str);

	JS_free(cx, c_str);

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_soundBlockChannels(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

	FilmSound_BlockChannels(JS::ToBoolean(args[0]));

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_soundRecAdvance(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

	double targetTime;

	if(!JS::ToNumber(cx, args[0], &targetTime))
		return JS_FALSE;

	g_Filming.GetFilmSound()->AdvanceFrame((float)targetTime);

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_soundRecStart(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(2 > args.length())
		return JS_FALSE;

	double volume;

	if(!JS::ToNumber(cx, args[1], &volume))
		return JS_FALSE;

    JSString *str = JS_ValueToString(cx, args[0]);
    if (!str)
        return JS_FALSE;

    char *c_str = JS_EncodeString(cx, str);
	if(!c_str)
		return JS_FALSE;

	std::wstring wPath;

	if(!AnsiStringToWideString(c_str, wPath))
	{
		JS_free(cx, c_str);
		return JS_FALSE;
	}

	JS_free(cx, c_str);

	bool result = g_Filming.GetFilmSound()->Start(wPath.c_str(), 0, (float)volume);

	args.rval().set(BOOLEAN_TO_JSVAL(result));
    return JS_TRUE;
}

static JSBool
AfxGlobal_soundRecStop(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(0 > args.length())
		return JS_FALSE;

	g_Filming.GetFilmSound()->Stop();

	args.rval().set(JSVAL_VOID);
    return JS_TRUE;
}

static JSBool
AfxGlobal_suggestTakePath(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(2 > args.length())
		return JS_FALSE;

    JSString *str = JS_ValueToString(cx, args[0]);
    if (!str)
        return JS_FALSE;

	int32_t takeDigits;

	if(!JS::ToInt32(cx, args[1], &takeDigits))
		return JS_FALSE;

    char *c_str = JS_EncodeString(cx, str);
	if(!c_str)
		return JS_FALSE;

	std::wstring wTakePath;
	std::string takePath;


	if(AnsiStringToWideString(c_str, wTakePath)
		&& SuggestTakePath(wTakePath.c_str(), takeDigits, wTakePath)
		&& WideStringToAnsiString(wTakePath.c_str(), takePath))
	{
		JSString * str = JS_NewStringCopyZ(cx, takePath.c_str());

		args.rval().set(STRING_TO_JSVAL(str));		
	}
	else
		args.rval().set(JSVAL_NULL);

	JS_free(cx, c_str);

	
    return JS_TRUE;
}

static JSBool
AfxGlobal_swapBuffers(JSContext *cx, unsigned argc, JS::Value *vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
	if(1 > args.length())
		return JS_FALSE;

	HDC hDC;
	if(!JS::ToUint32(cx, args[0], (unsigned int *)&hDC))
		return JS_FALSE;

	BOOL bResWglSwapBuffers;

	if (g_pSupportRender)
		bResWglSwapBuffers = g_pSupportRender->hlaeSwapBuffers(hDC);
	else
		bResWglSwapBuffers = OldWglSwapBuffers(hDC);

	args.rval().set(BOOLEAN_TO_JSVAL(bResWglSwapBuffers));
    return JS_TRUE;
}

static JSBool
AfxGlobal_testObject(JSContext *cx, unsigned argc, JS::Value *vp)
{
	JS::RootedObject thisobj(cx, JS_THIS_OBJECT(cx, vp));
    if (!thisobj)
        return false;

	JSObject *obj = JS_NewObjectWithGivenProto(cx, &JsTestObject_class, NULL, JS_GetGlobalForScopeChain(cx));
    
	JS_DefineProperties(cx, obj, JsTestObject_properties);

	JS_SetPrivate(obj, new JsTestObject());

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));
    return JS_TRUE;
}

static JSFunctionSpec AfxGlobal_functions[] = {
    JS_FS("additionalRRenderView", AfxGlobal_additionalRRenderView, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("additionalUnkDrawHud", AfxGlobal_additionalUnkDrawHud, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("afxFilmingStart", AfxGlobal_afxFilmingStart, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("afxFilmingStop", AfxGlobal_afxFilmingStop, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("createPath", AfxGlobal_createPath, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("cstrikeCrossHairBlock", AfxGlobal_cstrikeCrossHairBlock, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("gc", AfxGlobal_gc, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("getZFar", AfxGlobal_getZFar, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("getZNear", AfxGlobal_getZNear, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glBlendFunc", AfxGlobal_glBlendFunc, 2, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glClear", AfxGlobal_glClear, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glClearColor", AfxGlobal_glClearColor, 4, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glColor4d", AfxGlobal_glColor4d, 4, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glColorMask", AfxGlobal_glColorMask, 4, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glDeleteTextures", AfxGlobal_glDeleteTextures, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glDepthMask", AfxGlobal_glDepthMask, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glGenTextures", AfxGlobal_glGenTextures, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glGetBlendFunc", AfxGlobal_glGetBlendFunc, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glGetColorWriteMask", AfxGlobal_glGetColorWriteMask, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glGetCurrentColor", AfxGlobal_glGetCurrentColor, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("glGetDepthWriteMask", AfxGlobal_glGetDepthWriteMask, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("hlClientCommand", AfxGlobal_hlClientCommand, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("hlCvarSetValue", AfxGlobal_hlCvarSetValue, 2, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("load", AfxGlobal_load, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("maybeGc", AfxGlobal_maybeGc, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("modReplaceOnGlBegin", AfxGlobal_modReplaceOnGlBegin, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("modReplaceOnGlEnd", AfxGlobal_modReplaceOnGlEnd, 4, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("modReplaceRefreshTexture", AfxGlobal_modReplaceRefreshTexture, 4, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("newAfxGlImage", AfxGlobal_newAfxGlImage, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("newTestObject", AfxGlobal_testObject, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("print", AfxGlobal_print, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("soundBlockChannels", AfxGlobal_soundBlockChannels, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("soundRecAdvance", AfxGlobal_soundRecAdvance, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("soundRecStart", AfxGlobal_soundRecStart, 2, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("soundRecStop", AfxGlobal_soundRecStop, 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("suggestTakePath", AfxGlobal_suggestTakePath, 2, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS("swapBuffers", AfxGlobal_swapBuffers, 1, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_READONLY),
    JS_FS_END
};

static JSBool
AfxGlobal_currentEntityIndex_get(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
	cl_entity_t *ce = pEngStudio->GetCurrentEntity();

	int idx = ce ? ce->index : -1;

	vp.set(JS_NumberValue(idx));
    return JS_TRUE;
}

static JSBool
AfxGlobal_hasGlArbMultiTexture_get(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
	vp.set(g_Has_GL_ARB_multitexture ? JSVAL_TRUE : JSVAL_FALSE);
    return JS_TRUE;
}

static JSBool
AfxGlobal_height_get(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
	int value = g_AfxGoldSrcComClient.GetHeight();

	vp.set(JS_NumberValue(value));
    return JS_TRUE;
}

static JSBool
AfxGlobal_inRDrawEntitiesOnList_get(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
	vp.set(g_In_R_DrawEntitiesOnList ? JSVAL_TRUE : JSVAL_FALSE);
    return JS_TRUE;
}

static JSBool
AfxGlobal_inRDrawParticles_get(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
	vp.set(g_In_R_DrawParticles ? JSVAL_TRUE : JSVAL_FALSE);
    return JS_TRUE;
}

static JSBool
AfxGlobal_isCurrentEntityWorldModel_get(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
	cl_entity_t *ce = pEngStudio->GetCurrentEntity();

	bool bIs = ce && ce->model && ce->model->type == mod_brush && 0 != strncmp(ce->model->name, "maps/", 5);

	vp.set(bIs ? JSVAL_TRUE : JSVAL_FALSE);
    return JS_TRUE;
}

static JSBool
AfxGlobal_inRDrawViewModel_get(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
	vp.set(g_In_R_DrawViewModel ? JSVAL_TRUE : JSVAL_FALSE);
    return JS_TRUE;
}

static JSBool
AfxGlobal_lfError_get(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
	if(g_LastFirstError.Has)
	{
		g_LastFirstError.Has = false;

		JSString * str = JS_NewStringCopyZ(cx, g_LastFirstError.String.c_str());

		vp.set(STRING_TO_JSVAL(str));
	}
	else
	{
		vp.set(JSVAL_NULL);
	}
    return true;
}

static JSBool
AfxGlobal_scriptFolder_get(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
	JSString * str = JS_NewStringCopyZ(cx, g_ScriptFolder.c_str());

    vp.set(STRING_TO_JSVAL(str));
    return true;
}

static JSBool
AfxGlobal_width_get(JSContext *cx, JS::HandleObject obj, JS::HandleId id, JS::MutableHandleValue vp)
{
	int value = g_AfxGoldSrcComClient.GetWidth();

	vp.set(JS_NumberValue(value));
    return JS_TRUE;
}


static JSPropertySpec  AfxGlobal_properties[] = {
	{"currentEntityIndex", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED|JSPROP_READONLY, JSOP_WRAPPER(AfxGlobal_currentEntityIndex_get), JSOP_NULLWRAPPER},
	{"hasGlArbMultiTexture", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED|JSPROP_READONLY, JSOP_WRAPPER(AfxGlobal_hasGlArbMultiTexture_get), JSOP_NULLWRAPPER},
	{"height", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED|JSPROP_READONLY, JSOP_WRAPPER(AfxGlobal_height_get), JSOP_NULLWRAPPER},
	{"inRDrawEntitiesOnList", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED|JSPROP_READONLY, JSOP_WRAPPER(AfxGlobal_inRDrawEntitiesOnList_get), JSOP_NULLWRAPPER},
	{"inRDrawParticles", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED|JSPROP_READONLY, JSOP_WRAPPER(AfxGlobal_inRDrawParticles_get), JSOP_NULLWRAPPER},
	{"inRDrawViewModel", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED|JSPROP_READONLY, JSOP_WRAPPER(AfxGlobal_inRDrawViewModel_get), JSOP_NULLWRAPPER},
	{"isCurrentEntityWorldModel", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED|JSPROP_READONLY, JSOP_WRAPPER(AfxGlobal_isCurrentEntityWorldModel_get), JSOP_NULLWRAPPER},
	{"lfError", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED|JSPROP_READONLY, JSOP_WRAPPER(AfxGlobal_lfError_get), JSOP_NULLWRAPPER},
	{"onGlBegin", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER},
	{"onGlEnd", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER},
	{"onHudBegin", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER},
	{"onHudEnd", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER},
	{"onSwapBuffers", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT, JSOP_NULLWRAPPER, JSOP_NULLWRAPPER},
	{"scriptFolder", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED|JSPROP_READONLY, JSOP_WRAPPER(AfxGlobal_scriptFolder_get), JSOP_NULLWRAPPER},
	{"width", 0, JSPROP_ENUMERATE|JSPROP_PERMANENT|JSPROP_SHARED|JSPROP_READONLY, JSOP_WRAPPER(AfxGlobal_width_get), JSOP_NULLWRAPPER},
	{0, 0, 0, 0, 0}
};


static JSObject *
NewGlobalObject(JSContext *cx)
{
    JS::RootedObject glob(cx, JS_NewGlobalObject(cx, &AfxGlobal_class, NULL, JS::CompartmentOptions()));
    if(!glob)
        return NULL;

    {
        JSAutoCompartment ac(cx, glob);

        if(!JS_InitStandardClasses(cx, glob))
            return NULL;

		if(!JS_DefineFunctions(cx, glob, AfxGlobal_functions))
			return NULL;

        if (!JS_DefineProperties(cx, glob, AfxGlobal_properties))
            return NULL;
    }

    return glob;
}

////////////////////////////////////////////////////////////////////////////////

void SetScriptFolder()
{
	g_ScriptFolder.assign(GetHlaeFolder());
	g_ScriptFolder += SCRIPT_FOLDER;
}

bool ScriptEngine_Execute(char const * script) {
	if(!g_JsRunning) return false;

	JSAutoCompartment ac(g_JsCx, g_JsGlobal);
	jsval rval;
	return JS_TRUE == JS_EvaluateScript(g_JsCx, JS_GetGlobalForScopeChain(g_JsCx), script, strlen(script), NULL, 0, &rval);
}

bool ScriptEngine_IsRunning() {
	return g_JsRunning;
}

void ScriptEngine_ShutDown() {
	g_JsRunning = false;
	if(g_JsCx) { JS_DestroyContext(g_JsCx); g_JsCx = 0; }
	if(g_JsRt) { JS_DestroyRuntime(g_JsRt); g_JsRt = 0; }
	JS_ShutDown();
}

bool ScriptEngine_StartUp()
{
	SetScriptFolder();

	bool bOk = true;

	bOk =
		NULL != (g_JsRt = JS_NewRuntime(1L * 1024L * 1024L, JS_USE_HELPER_THREADS))
		&& NULL != (g_JsCx = JS_NewContext(g_JsRt, 8192))
		&& (
			JS_SetOptions(g_JsCx, JSOPTION_VAROBJFIX),
			JS_SetErrorReporter(g_JsCx, reportError),
			true
		)
	
		// . (global):
		&& NULL != (g_JsGlobal = NewGlobalObject(g_JsCx))
	;
	
	if(bOk)
	{
		JSAutoCompartment ac(g_JsCx, g_JsGlobal);
		JS_SetGlobalObject(g_JsCx, g_JsGlobal);

		//char * loadInitScript = "load(scriptFolder+'AfxGoldSrcInit.js');";
		//jsval rval;
		//bOk = bOk && JS_EvaluateScript(g_JsCx, JS_GetGlobalForScopeChain(g_JsCx), loadInitScript, strlen(loadInitScript), NULL, 0, &rval);
	}

	if(!bOk)
	{
		MessageBox(0, "Failed to init scripting engine.", "Error", MB_OK|MB_ICONERROR);
		ScriptEngine_ShutDown();
	}

	g_JsRunning = bOk;

	return bOk;
}

void ScriptEvent_OnGlBegin(unsigned int mode)
{
	JSAutoCompartment ac(g_JsCx, g_JsGlobal);

	jsval f;

	if(!JS_GetProperty(g_JsCx, g_JsGlobal, "onGlBegin", &f) || JSVAL_IS_PRIMITIVE(f))
		return;

	jsval y;
	jsval x[1] = {INT_TO_JSVAL(mode)};

	JS_CallFunctionValue(g_JsCx, g_JsGlobal, f, 1, x, &y);
}

void ScriptEvent_OnGlEnd()
{
	JSAutoCompartment ac(g_JsCx, g_JsGlobal);

	jsval f;

	if(!JS_GetProperty(g_JsCx, g_JsGlobal, "onGlEnd", &f) || JSVAL_IS_PRIMITIVE(f))
		return;

	jsval y;
	JS_CallFunctionValue(g_JsCx, g_JsGlobal, f, 0, NULL, &y);
}


void ScriptEvent_OnHudBegin()
{
	JSAutoCompartment ac(g_JsCx, g_JsGlobal);

	jsval f;

	if(!JS_GetProperty(g_JsCx, g_JsGlobal, "onHudBegin", &f) || JSVAL_IS_PRIMITIVE(f))
		return;

	jsval y;

	JS_CallFunctionValue(g_JsCx, g_JsGlobal, f, 0, NULL, &y);
}

bool ScriptEvent_OnHudEnd()
{
	JSAutoCompartment ac(g_JsCx, g_JsGlobal);

	jsval f;

	if(!JS_GetProperty(g_JsCx, g_JsGlobal, "onHudEnd", &f) || JSVAL_IS_PRIMITIVE(f))
		return false;

	jsval y;
	JS_CallFunctionValue(g_JsCx, g_JsGlobal, f, 0, NULL, &y);

	return JS::ToBoolean(y);
}

bool ScriptEvent_OnSwapBuffers(HDC hDC, BOOL & bSwapRes)
{
	JSAutoCompartment ac(g_JsCx, g_JsGlobal);

	jsval f;

	if(!JS_GetProperty(g_JsCx, g_JsGlobal, "onSwapBuffers", &f) || JSVAL_IS_PRIMITIVE(f))
		return false;

	jsval y;
	jsval x[1] = { JS_NumberValue((unsigned int)hDC) };

	if(!JS_CallFunctionValue(g_JsCx, g_JsGlobal, f, 1, x, &y))
		return false;

	bSwapRes = JS::ToBoolean(y);

	return true;
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
			*ct = ' ';
			ct++;
			len--;
		}

		strcpy_s(ct, len, cur);
		ct += lcur;
		len -= lcur;
	}
	*ct = 0; // Term

	jsval rval;
	JSBool ok;
	JSAutoCompartment ac(g_JsCx, g_JsGlobal);

	pEngfuncs->Con_DPrintf("%s\n", ttt);
	ok = JS_EvaluateScript(g_JsCx, JS_GetGlobalForScopeChain(g_JsCx), ttt, strlen(ttt), NULL, 0, &rval);

	if(!ok)
		pEngfuncs->Con_Printf("Error.\n");
	else if (!JSVAL_IS_VOID(rval)) 
	{
		JSString *str = JS_ValueToSource(g_JsCx, rval);
		if(str)
		{
			char *c_str = JS_EncodeString(g_JsCx, str);
			if(c_str)
			{
				pEngfuncs->Con_Printf("Result: %s\n", c_str);
				JS_free(g_JsCx, c_str);
			}
		}
		else
			pEngfuncs->Con_Printf("Result error.\n");
	}

	free(ttt);
}
