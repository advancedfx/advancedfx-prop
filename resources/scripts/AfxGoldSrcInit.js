// OpenGL constants from GL.h //////////////////////////////////////////////////

/* AttribMask */
GL_CURRENT_BIT         = 0x00000001;
GL_POINT_BIT           = 0x00000002;
GL_LINE_BIT            = 0x00000004;
GL_POLYGON_BIT         = 0x00000008;
GL_POLYGON_STIPPLE_BIT = 0x00000010;
GL_PIXEL_MODE_BIT      = 0x00000020;
GL_LIGHTING_BIT        = 0x00000040;
GL_FOG_BIT             = 0x00000080;
GL_DEPTH_BUFFER_BIT    = 0x00000100;
GL_ACCUM_BUFFER_BIT    = 0x00000200;
GL_STENCIL_BUFFER_BIT  = 0x00000400;
GL_VIEWPORT_BIT        = 0x00000800;
GL_TRANSFORM_BIT       = 0x00001000;
GL_ENABLE_BIT          = 0x00002000;
GL_COLOR_BUFFER_BIT    = 0x00004000;
GL_HINT_BIT            = 0x00008000;
GL_EVAL_BIT            = 0x00010000;
GL_LIST_BIT            = 0x00020000;
GL_TEXTURE_BIT         = 0x00040000;
GL_SCISSOR_BIT         = 0x00080000;
GL_ALL_ATTRIB_BITS     = 0x000fffff;

/* BeginMode */
GL_POINTS         = 0x0000;
GL_LINES          = 0x0001;
GL_LINE_LOOP      = 0x0002;
GL_LINE_STRIP     = 0x0003;
GL_TRIANGLES      = 0x0004;
GL_TRIANGLE_STRIP = 0x0005;
GL_TRIANGLE_FAN   = 0x0006;
GL_QUADS          = 0x0007;
GL_QUAD_STRIP     = 0x0008;
GL_POLYGON        = 0x0009;

/* BlendingFactorDest */
GL_ZERO                = 0
GL_ONE                 = 1
GL_SRC_COLOR           = 0x0300
GL_ONE_MINUS_SRC_COLOR = 0x0301
GL_SRC_ALPHA           = 0x0302
GL_ONE_MINUS_SRC_ALPHA = 0x0303
GL_DST_ALPHA           = 0x0304
GL_ONE_MINUS_DST_ALPHA = 0x0305

/* BlendingFactorSrc */
/* GL_ZERO */
/* GL_ONE */
GL_DST_COLOR           = 0x0306
GL_ONE_MINUS_DST_COLOR = 0x0307
GL_SRC_ALPHA_SATURATE  = 0x0308
/* GL_SRC_ALPHA */
/* GL_ONE_MINUS_SRC_ALPHA */
/* GL_DST_ALPHA */
/* GL_ONE_MINUS_DST_ALPHA */

/* Boolean */
GL_TRUE  = 1;
GL_FALSE = 0;

/* DataType */
GL_BYTE           = 0x1400;
GL_UNSIGNED_BYTE  = 0x1401;
GL_SHORT          = 0x1402;
GL_UNSIGNED_SHORT = 0x1403;
GL_INT            = 0x1404;
GL_UNSIGNED_INT   = 0x1405;
GL_FLOAT          = 0x1406;
GL_2_BYTES        = 0x1407;
GL_3_BYTES        = 0x1408;
GL_4_BYTES        = 0x1409;
GL_DOUBLE         = 0x140A;

/* PixelFormat */
GL_COLOR_INDEX     = 0x1900;
GL_STENCIL_INDEX   = 0x1901;
GL_DEPTH_COMPONENT = 0x1902;
GL_RED             = 0x1903;
GL_GREEN           = 0x1904;
GL_BLUE            = 0x1905;
GL_ALPHA           = 0x1906;
GL_RGB             = 0x1907;
GL_RGBA            = 0x1908;
GL_LUMINANCE       = 0x1909;
GL_LUMINANCE_ALPHA = 0x190A;


////////////////////////////////////////////////////////////////////////////////

var GlModChannelMode = {
	NoChange: 0,
	Enabled: 1,
	Disabled: 2,
	toBoolean: function(value, oldGlMode) {
		return
			NoChange == value ? oldGlMode : (
				Enabled == value ? true : false
			)
	}
};

// reuse or define afxGlImage:
if(undefined == afxGlImage)
{
	afxGlImage = newAfxGlImage();
}


// GlMod ///////////////////////////////////////////////////////////////////////

function GlMod()
{
}

GlMod.prototype.onGlBegin = function(mode)
{
};

GlMod.prototype.onGlEnd = function()
{
};


// GlModCond ///////////////////////////////////////////////////////////////////

/**
 * Execute a GlMod based on a condition: true = executed, false = skip
 * The condition is evaluated upon onGlBeding.
 *
 * @param {GlMod} glMod - a GlMod
 * @param  condF - a function that evaluates to boolean
 */
function GlModCond(glMod,condF)
{
	GlMod.call(this);
	
	this.glMod = glMod;
	this.condF = condF;
}

GlModCond.prototype = new GlMod();
GlModCond.prototype.constructor = GlModCond;

GlModCond.prototype.onGlBegin = function(mode)
{
	this.condition = this.condF();
	
	if(this.condition) this.glMod.onGlBegin(mode);
};

GlModCond.prototype.onGlEnd = function()
{
	if(this.condition) this.glMod.onGlEnd();
};


// GlModArray //////////////////////////////////////////////////////////////////

function GlModArray()
{
	GlMod.call(this);

	this.effects = [];
}

GlModArray.prototype = new GlMod();
GlModArray.prototype.constructor = GlModArray;

/**
 * Adds an GlMod to the internal array
 */
GlModArray.prototype.push = function(glMod)
{
	this.effects.push(glMod);
}

GlModArray.prototype.onGlBegin = function(mode)
{
	for (var i = 0, len = this.effects.length; i < len; i++)
	{
		this.effects[i].onGlBegin(mode);
	}
};

GlModArray.prototype.onGlEnd = function()
{
	for (var i = this.effects.length -1; i >= 0; i--)
	{
		this.effects[i].onGlEnd();
	}
};


// GlModColor //////////////////////////////////////////////////////////////////

function GlModColor()
{
	GlMod.call(this);

	this.red = -1;
	this.green = -1;
	this.blue = -1;
	this.alpha = -1;
	this.oldColor = [0, 0, 0, 0];
}

GlModColor.prototype = new GlMod();
GlModColor.prototype.constructor = GlModColor;

GlModColor.prototype.onGlBegin = function(mode)
{
	this.oldColor = glGetCurrentColor();
	
	glColor4d(
		(0 <= this.red ? this.red : this.oldColor[0]),
		(0 <= this.green ? this.green : this.oldColor[1]),
		(0 <= this.blue ? this.blue : this.oldColor[2]),
		(0 <= this.alpha ? this.alpha : this.oldColor[3])
	);
};

GlModColor.prototype.onGlEnd = function()
{
	glColor4d(
		this.oldColor[0],
		this.oldColor[1],
		this.oldColor[2],
		this.oldColor[3]
	);
};

/**
  * value < 0 indicates NoChange
  */
GlModColor.prototype.setRgba = function(red, green, blue, alpha)
{
	this.red = red;
	this.green = green;
	this.blue = blue;
	this.alpha = alpha;
};


// GlModColorMask //////////////////////////////////////////////////////////////

function GlModColorMask()
{
	GlMod.call(this);

	this.red = GlModChannelMode.NoChange;
	this.green = GlModChannelMode.NoChange;
	this.blue = GlModChannelMode.NoChange;
	this.alpha = GlModChannelMode.NoChange;
	this.oldColorMask = [false, false, false, false];
}

GlModColorMask.prototype = new GlMod();
GlModColorMask.prototype.constructor = GlModColorMask;

GlModColorMask.prototype.onGlBegin = function(mode)
{
	this.oldColorMask = glGetColorWriteMask();
	
	glColorMask(
		GlModChannelMode.toBoolean(this.red, this.oldColorMask[0]), 
		GlModChannelMode.toBoolean(this.green, this.oldColorMask[1]),
		GlModChannelMode.toBoolean(this.blue, this.oldColorMask[2]),
		GlModChannelMode.toBoolean(this.alpha, this.oldColorMask[3])
	);
};

GlModColorMask.prototype.onGlEnd = function()
{
	glColorMask(this.oldColorMask[0], this.oldColorMask[1], this.oldColorMask[2], this.oldColorMask[3]);
};

/**
 * values must be of GlModChannelMode
 */
GlModColorMask.prototype.setRgba = function(red, green, blue, alpha)
{
	this.red = red;
	this.green = green;
	this.blue = blue;
	this.alpha = alpha;
};


// GlModDepthMask //////////////////////////////////////////////////////////////

function GlModDepthMask()
{
	GlMod.call(this);

	this.depth = GlModChannelMode.NoChange;
	this.oldDepth = false;
}

GlModDepthMask.prototype = new GlMod();
GlModDepthMask.prototype.constructor = GlModDepthMask;

GlModDepthMask.prototype.onGlBegin = function(mode)
{
	this.oldDepth = glGetDepthWriteMask();
	
	glDepthMask(
		GlModChannelMode.toBoolean(this.depth, this.oldDepth)
	);
};

GlModDepthMask.prototype.onGlEnd = function()
{
	glDepthMask(this.oldDepth);
};

/**
 * values must be of GlModChannelMode
 */
GlModDepthMask.prototype.setDepth = function(depth)
{
	this.depth = depth;
};


// GlModHudAlpha ///////////////////////////////////////////////////////////////

function GlModHudAlpha()
{
	GlMod.call(this);

	this.oldBlendFunc = [0, 0];
	this.oldColorMask = [false, false, false, false];
}

GlModHudAlpha.prototype = new GlMod();
GlModHudAlpha.prototype.constructor = GlModHudAlpha;

GlModHudAlpha.prototype.onGlBegin = function(mode)
{
	this.oldBlendFunc = glGetBlendFunc();
	this.oldColorMask = glGetColorWriteMask();
	
	var sfactor = this.oldBlendFunc[0];
	var dfactor = this.oldBlendFunc[1];
	
	if (dfactor == GL_ONE)//(sfactor == dfactor == GL_ONE)
	{
		// block the Alpha chan of Additive sprites
		glColorMask(true, true, true, false); // block alpha for additive HUD sprites
		glBlendFunc(sfactor,dfactor);
	}
	else
	{
		// don't let sprites lower alpha value:
		glColorMask(true, true, true, true); // allow alpha
		if (sfactor==GL_SRC_ALPHA) sfactor=GL_SRC_ALPHA_SATURATE;
		if (dfactor==GL_SRC_ALPHA) dfactor=GL_SRC_ALPHA_SATURATE;
		glBlendFunc(sfactor,dfactor);
	}
};

GlModHudAlpha.prototype.onGlEnd = function()
{
	glBlendFunc(this.oldBlendFunc[0], this.oldBlendFunc[1]);
	glColorMask(this.oldColorMask[0], this.oldColorMask[1], this.oldColorMask[2], this.oldColorMask[3]);
};

// GlModMask ///////////////////////////////////////////////////////////////////

function GlModMask()
{
	GlMod.call(this);

	this.colorMask = new GlModColorMask();
	this.depthMask = new GlModDepthMask();
}

GlModMask.prototype = new GlMod();
GlModMask.prototype.constructor = GlModMask;

GlModMask.prototype.onGlBegin = function(mode)
{
	this.colorMask.onGlBegin(mode);
	this.depthMask.onGlBegin(mode);
};

GlModMask.prototype.onGlEnd = function()
{
	this.depthMask.onGlEnd();
	this.colorMask.onGlEnd();
};

/**
 * values must be of GlModChannelMode
 */
GlModMask.prototype.setMask = function(mask)
{
	this.colorMask.setRgba(mask, mask, mask, mask);
	this.depthMask.setDepth(mask);
};


// GlModReplace ////////////////////////////////////////////////////////////////

/**
 * Attention: GlModReplace.finish() must be called when the mod is not needed
 * anymore in order to free resources.
 */
function GlModReplace()
{
	GlMod.call(this);
	
	this.red = 0;
	this.green = 0;
	this.blue = 0;
	this.hasTexture = false;
	this.refreshTexture = false;
}

GlModReplace.prototype = new GlMod();
GlModReplace.prototype.constructor = GlModReplace;

/**
 * Frees used resources
 */
GlModReplace.prototype.finish = function(mode)
{
	if(this.hasTexture)
	{
		glDeleteTextures([this.texture]);
	}
};

GlModReplace.prototype.onGlBegin = function(mode)
{
	this.ensureTexture();
	
	if(!this.hasTexture)
		return;
		
	result = modReplaceOnGlBegin(this.texture);
	
	this.oldActiveTextureArb = result[0];
	this.oldTexture2d = result[1];
	this.oldTextureBinding2d = result[2];
	this.oldTextureEnvMode = result[3];
};

GlModReplace.prototype.onGlEnd = function()
{
	if(!this.hasTexture)
		return;

	modReplaceOnGlEnd(this.oldActiveTextureArb, this.oldTexture2d, this.oldTextureBinding2d, this.oldTextureEnvMode);
};

/**
 * values are byte (0-255).
 * Changing the color causes the texture to be regenerated, thus using multiple
 * GlModReplace instances instead (and thus textures) is usually prefered.
 */
GlModReplace.prototype.setRgb = function(red, green, blue)
{
	this.red = red;
	this.green = green;
	this.blue = blue;
	this.refreshTexture = true;
};

GlModReplace.prototype.ensureTexture = function()
{
	if(!hasGlArbMultiTexture) return;

	if(!this.hasTexture)
	{
		var textureIds = glGenTextures(1);
		
		this.texture = textureIds[0];

		this.hasTexture = true;
		this.refreshTexture = true;
	}

	if(this.refreshTexture)
	{
		modReplaceRefreshTexture(this.texture, this.red, this.green, this.blue);
		this.refreshTexture = false;
	}
};

// GlModAlphaMatte ////////////////////////////////////////////////////////////

/**
 * Attention: finish() must be called in order to free resources.
 *
 * @param condF - function that evaluates to boolean, true -> white, false -> black
 */
function GlModAlphaMatte(condF)
{
	GlMod.call(this);

	this.condF = condF;
	this.color = new GlModColor();
	this.replaceWhite = new GlModReplace();
	this.replaceBlack = new GlModReplace();
	
	this.replaceWhite.setRgb(255, 255, 255);
	this.replaceBlack.setRgb(0, 0, 0);
}

GlModAlphaMatte.prototype = new GlMod();
GlModAlphaMatte.prototype.constructor = GlModAlphaMatte;

GlModAlphaMatte.prototype.finish = function()
{
	this.replaceBlack.finish();
	this.replaceWhite.finish();
};

GlModAlphaMatte.prototype.onGlBegin = function(mode)
{
	this.condition = this.condF();
	
	if(this.condition)
	{
		this.replaceWhite.onGlBegin(mode);

		this.color.setRgba(1, 1, 1, -1);
		this.color.onGlBegin(mode);	
	}
	else
	{
		this.replaceBlack.onGlBegin(mode);

		this.color.setRgba(0, 0, 0, -1);
		this.color.onGlBegin(mode);	
	}
};

GlModAlphaMatte.prototype.onGlEnd = function()
{
	if(this.condition)
	{
		this.color.onGlEnd();
		this.replaceWhite.onGlEnd();
	}
	else
	{
		this.color.onGlEnd();
		this.replaceBlack.onGlEnd();
	}
};


// Proc* ///////////////////////////////////////////////////////////////////////

/**
 * @param array - array of Proc* objects
 */
function ProcArray(array)
{
	this.array = array;
}

ProcArray.prototype.process = function()
{
	for(var i=0, len=this.array.length; i<len; i++)
	{
		array[i].process();
	}
}

function ProcCapture(x, y, width, height, format, type)
{
	this.x = x;
	this.y = y;
	this.height = heigth;
	this.format = format;
	this.type = type;
}

ProcCapture.prototype.process = function()
{
	afxGlImage.glReadPixels(this.x, this.y, this.height, this.format, this.type);
}

function newProcCaptureAlpha(x, y, width, height)
{
	return new ProcCapture(x, y, width, heihgt, GL_UNSIGNED_BYTE, GL_ALPHA);
}

function newProcCaptureDepth(x, y, width, height)
{
	return new ProcCapture(x, y, width, heihgt, GL_FLOAT, GL_DEPTH_COMPONENT);
}

function newProcCaptureRgb(x, y, width, height)
{
	return new ProcCapture(x, y, width, heihgt, GL_UNSIGNED_BYTE, GL_BGR_EXT);
}

/**
 * @param nameFn - function that evaluates to string that determines the name
 */
function ProcWriteBitmap(nameFn)
{
	this.nameFn = nameFn;
}

ProcWriteBitmap.prototype.process = function()
{
	afxGlImage.writeBitmap(this.nameFn());
}

/**
 * @param zNearFn - function that evaluates to double, representing zNear value
 * @param zFarFn - function that evaluates to double, representing zFar value*/
function ProcTransDepthLinear(zNearFn,zFarFn)
{
	this.zNearFn = zNearFn;
	this.zFarFn = zFarFn;
}

ProcTransDepthLinear.prototype.process = function()
{
	afxGlImage.linearizeFloatDepthBuffer(zNearFn(), zFarFn());
}

/**
 * @param zNearFn - function that evaluates to double, representing zNear value
 * @param zFarFn - function that evaluates to double, representing zFar value*/
function ProcTransDepthLog(zNearFn,zFarFn)
{
	this.zNearFn = zNearFn;
	this.zFarFn = zFarFn;
}

ProcTransDepthLog.prototype.process = function()
{
	afxGlImage.linearizeFloatDepthBuffer(zNearFn(), zFarFn());
	afxGlImage.logarithmizeFloatDepthBuffer(zNearFn(), zFarFn());
}

function ProcTransDepthByte()
{
}

ProcTransDepthByte.prototype.process = function()
{
	afgGlImage.floatDepthBufferToByteBuffer();
}

function CapturePoint(filming,name,captureFn)
{
	this.captureFn = captureFn;
	this.filming = filming;
	this.frame = 0;
	this.name = name;
}

CapturePoint.prototype.finish = function()
{
	print('FakeCapturePoint('+this.name+').finish');
}

CapturePoint.prototype.capture = function()
{
	print('FakeCapturePoint('+this.name+').capture: '+this.frame);
	
	this.frame++;
}

// Filming /////////////////////////////////////////////////////////////////////

function Filming()
{
	this.mainStream = null;
	this.streams = [];
	this.hudStreams = [];
	this.activeStream = null;
	this.recording = false;
}

Filming.prototype.startRecording = function()
{
	stopRecording();
	
	if(this.mainStream) mainStream.startRecording();
	for(var i in this.streams) i.startRecording();
	for(var i in this.hudStreams) i.StartRecording();
	
	this.recording = true;
};

Filming.prototype.stopRecording = function()
{
	if(!this.recording)
		return;
		
	if(this.mainStream) mainStream.stopRecording();
	for(var i in this.streams) i.stopRecording();
	for(var i in this.hudStreams) i.stopRecording();

	this.recording = false;
};

Filming.prototype.previewStream = function(stream)
{
	
};

Filming.prototype.previewEnd = function()
{
	
};


Filming.prototype.finish = function()
{
	stopRecording();
};

Filming.prototype.onGlBegin = function(mode)
{
	if(null != this.activeStream) this.activeStream.onGlBegin(mode);
};

Filming.prototype.onGlEnd = function()
{
	if(null != this.activeStream) this.activeStream.onGlEnd();
};

Filming.prototype.onHudBegin = function()
{
	print('Filming.onHudBegin');
};

Filming.prototype.onHudEnd = function()
{
	print('Filming.onHudEnd');

	return false;
};

Filming.prototype.onSwapBuffers = function(hDc)
{
	var result = swapBuffers(hDc);
	
	if(this.recording)
	{
		for(var i=0, len=this.streams.length; i<len; i++)
		{
			this.activeStream = streams[i];
			
			additionalRRenderView();
		}
		
		this.activeStream = mainStream;
	}
	
	return result;
};

////////////////////////////////////////////////////////////////////////////////

function hookFilming(filming)
{
	onGlBegin = function(mode) { filming.onGlBegin(mode) };
	onGlEnd = function() { filming.onGlEnd(); };
	onHudBegin = function() { filming.onHudBegin(); };
	onHudEnd = function() { return filming.onHudEnd(); };
	onSwapBuffers = function(hDc) { return filming.onSwapBuffers(hDc); };
}

function unHookFilming()
{
	onGlBegin = undefined;
	onGlEnd = undefined;
	onHudBegin = undefined;
	onHudEnd = undefined;
	onSwapBuffers = undefined;
}

function finishFilming(filming)
{
	unHookFilming();
	filming.finish();
}

/*

- Capture Points: possibly onEvent
- additonal render passes (R_RenderView)
- main render pass (SwapBuffers)
- at least one hudpass, maybe multiple hudpasses using loop condition
- GlMods control rendering between captures
- GlMods can be changed anytime except between glBegin and glEnd
- at a capture point multiple captures can take place (i.e. Color / Alpha / Depth)

--> standard capture points: SwapBuffers, before hud is drawn, after hud is drawn
--> adding R_RenderView causes one additonal RenderPoint, so does looping the Hud do (or maybe Hud do)
- potentially more capture points in the future (those things indicated by is* Properties atm)

-> Stream object: capture properties, name, maybe sampling, file writing (bmp/tga) associated with it,
   also changing image content has to work (i.e. GLfloat depthbuffer to erm dunno byte(s))
   
   Stream object has combination/chain of these:
   - Capture object (buffer (color,depth,alpha), rect(x,y,width,height))
   - Sampling object (float/byte)
   - File writing object (bmp/tga)
   - Transform object (i.e. transform GLfloat buffer into GLbyte buffer)
   
   Maybe for now we will just port FilmingStream (meaning all packed together into a single object to use)

*/