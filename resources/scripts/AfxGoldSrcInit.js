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

/* EXT_bgra */
GL_BGR_EXT  = 0x80E0;
GL_BGRA_EXT = 0x80E1;


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

/**
 * Converts a number to a string, with at least the given width b filling with '0' or the given z character
 * @param n - number to convert
 * @param width - minimum width
 * @param z - zero character, optional
 */
function pad(n, width, z)
{
	z = z || '0';
	n = n + '';
	return n.length >= width ? n : new Array(width - n.length + 1).join(z) + n;
}

// reuse or define afxGlImage:
if(!this.afxGlImage)
{
	this.afxGlImage = newAfxGlImage();
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
		this.array[i].process();
	}
}

function ProcCapture(x, y, width, height, format, type)
{
	this.x = x;
	this.y = y;
	this.width = width;
	this.height = height;
	this.format = format;
	this.type = type;
}

ProcCapture.prototype.process = function()
{
	afxGlImage.glReadPixels(this.x, this.y, this.width, this.height, this.format, this.type);
}

function newProcCaptureAlpha(x, y, width, height)
{
	return new ProcCapture(x, y, width, height, GL_ALPHA, GL_UNSIGNED_BYTE);
}

function newProcCaptureDepth(x, y, width, height)
{
	return new ProcCapture(x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
}

function newProcCaptureRgb(x, y, width, height)
{
	return new ProcCapture(x, y, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE);
}

/**
 * @param nameFn - function that evaluates to string that determines the name (without file extension)
 */
function ProcWriteBitmap(nameFn)
{
	this.nameFn = nameFn;
}

ProcWriteBitmap.prototype.process = function()
{
	afxGlImage.writeBitmap(this.nameFn()+'.bmp');
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
	afxGlImage.linearizeFloatDepthBuffer(this.zNearFn(), this.zFarFn());
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
	afxGlImage.linearizeFloatDepthBuffer(this.zNearFn(), this.zFarFn());
	afxGlImage.logarithmizeFloatDepthBuffer(this.zNearFn(), this.zFarFn());
}

function ProcTransDepthByte()
{
}

ProcTransDepthByte.prototype.process = function()
{
	afgGlImage.floatDepthBufferToByteBuffer();
}

// CapturePoint ////////////////////////////////////////////////////////////////

function CapturePoint(name)
{
	this.capturePath = null;
	this.frame = 0;
	this.name = name;
	this.proc = null;
}

CapturePoint.prototype.capture = function()
{
	if(this.proc) this.proc.process();
	
	this.frame++;
}

CapturePoint.prototype.nameFn = function()
{
	return this.capturePath+'\\'+pad(this.frame, 5);
}

CapturePoint.prototype.startRecording = function(takePath)
{
	var wantPath = takePath+'\\'+this.name;
	this.capturePath = createPath(wantPath);
	
	if(null == this.capturePath)
	{
		print('CapturePoint.startRecording error: createPath(\''+wantPath+'\') failed.');
	}
}

CapturePoint.prototype.stopRecording = function()
{
}

// Stream //////////////////////////////////////////////////////////////////////

/**
 * @param clearnFn - function to call for clearing
 * @param capturePoints - array of CapturePoints to use.
 * @param glMod - GlMod object to use, optional.
 */
function Stream(clearFn, capturePoints, glMod)
{
	this.capturePoints = capturePoints;
	this.clearFn = clearFn;
	this.glMod = glMod;
}

Stream.prototype.clear = function()
{
	this.clearFn();
}

Stream.prototype.capture = function()
{
	for(var i in this.capturePoints) this.capturePoints[i].capture();
}

Stream.prototype.hasCaptureName = function(name)
{
	var found = false;
	
	for(var i in this.capturePoints) found = found || this.capturePoints[i].name == name;
	
	return found;
}

Stream.prototype.onGlBegin = function(mode)
{
	if(this.glMod) glMod.onGlBegin(mode);
};

Stream.prototype.onGlEnd = function()
{
	if(this.glMod) glMod.onGlEnd();
};

Stream.prototype.startRecording = function(takePath)
{
	for(var i in this.capturePoints) this.capturePoints[i].startRecording(takePath);
}

Stream.prototype.stopRecording = function()
{
	for(var i in this.capturePoints) this.capturePoints[i].stopRecording();
}



// Filming /////////////////////////////////////////////////////////////////////

function Filming(path)
{
	this.mainStream = null;
	this.streams = [];
	this.hudStreams = [];
	this.activeStream = null;
	this.frameTime = 1.0/90.0;
	this.recording = false;
	this.recordSound = true;
	this.recordingSound = false;
	this.soundVolume = 0.4;
	this.path = path;
}

/**
 * Checks if a stream with give name already exists
 */
Filming.prototype.hasCaptureName = function(name)
{
	var bFound = false;
		
	if(this.mainStream) bFound = bFound || this.mainStream.hasCaptureName(name);
	for(var i in this.streams) bFound = bFound || this.streams[i].hasCaptureName(name);
	for(var i in this.hudStreams) bFound = bFound || this.hudStreams[i].hasCaptureName(name);
	
	return bFound;
}

Filming.prototype.startRecording = function()
{
	this.stopRecording();
	
	var takePath = suggestTakePath(this.path+'\\take', 4);
	
	if(null == takePath)
	{
		print('Filming.startRecording error: suggestTakePath(\''+takePath+'\') failed.');
		return;
	}
	
	takePath = createPath(takePath);
	
	if(null == takePath)
	{
		print('Filming.startRecording error: createPath(\''+takePath+'\') failed.');
		return;
	}
	
	afxFilmingStart();
	
	hlCvarSetValue('host_framerate', this.frameTime);
	
	if(this.mainStream) mainStream.startRecording(takePath);
	for(var i in this.streams) this.streams[i].startRecording(takePath);
	for(var i in this.hudStreams) this.hudStreams[i].StartRecording(takePath);
	
	print('takePath = '+takePath);
	
	if(this.recordSound)
	{
		this.recordingSound = soundRecStart(takePath+'\\sound.wav', this.soundVolume);
		if(!this.recordingSound) print('Filming.startRecording error: soundRecStart failed.');
	}
	
	this.activeStream = this.mainStream;
	this.recTime = 0.0;
	this.recording = true;
};

Filming.prototype.stopRecording = function()
{
	if(!this.recording)
		return;
		
	if(this.recordingSound)
	{
		soundRecStop();
		this.recordingSound = false;
	}
	
	if(this.mainStream) this.mainStream.stopRecording();
	for(var i in this.streams) this.streams[i].stopRecording();
	for(var i in this.hudStreams) this.hudStreams[i].stopRecording();

	this.activeStream = null;
	
	hlCvarSetValue('host_framerate', 0);
	
	afxFilmingStop();
	
	this.recording = false;
};

Filming.prototype.finish = function()
{
	this.stopRecording();
};

Filming.prototype.onGlBegin = function(mode)
{
	if(this.activeStream) this.activeStream.onGlBegin(mode);
};

Filming.prototype.onGlEnd = function()
{
	if(this.activeStream) this.activeStream.onGlEnd();
};

Filming.prototype.onHudBegin = function()
{
	//print('Filming.onHudBegin');
};

Filming.prototype.onHudEnd = function()
{
	//print('Filming.onHudEnd');

	return false;
};

Filming.prototype.onSwapBuffers = function(hDc)
{
	var result = swapBuffers(hDc);
	
	if(this.recording)
	{
		this.recTime += this.frameTime;
		
		if(this.recordingSound) soundRecAdvance(this.recTime);
	
		cstrikeCrossHairBlock(true);
		soundBlockChannels(true);
	
		for(var i=0, len=this.streams.length; i<len; i++)
		{
			this.activeStream = this.streams[i];
			
			this.activeStream.clear();
			
			additionalRRenderView();
			
			this.activeStream.capture();
		}

		for(var i=0, len=this.hudStreams.length; i<len; i++)
		{
			this.activeStream = this.hudStreams[i];
			
			this.activeStream.clear();
			
			additionalUnkDrawHud();
			
			this.activeStream.capture();
		}
		
		this.activeStream = this.mainStream;
		
		soundBlockChannels(false);
		cstrikeCrossHairBlock(false);
		
		hlCvarSetValue('host_framerate', this.frameTime);
	}
	
	return result;
};

// easy* ///////////////////////////////////////////////////////////////////////

function easyCaptureAlpha(name)
{
	var cap = new CapturePoint(name);
	var nameFn = function() {
		return cap.nameFn();
	};
	var proc = new ProcArray([
		newProcCaptureAlpha(0, 0, width, height),
		new ProcWriteBitmap(nameFn)
	]);
	cap.proc = proc;
	
	return cap;
}

function easyCaptureDepthInverse(name)
{
	var cap = new CapturePoint(name);
	var nameFn = function() {
		return cap.nameFn();
	};
	var proc = new ProcArray([
		newProcCaptureDepth(0, 0, width, height),
		new ProcTransDepthByte(),
		new ProcWriteBitmap(nameFn)
	]);
	cap.proc = proc;
	
	return cap;
}

function easyCaptureDepthLinear(name)
{
	var cap = new CapturePoint(name);
	var nameFn = function() {
		return cap.nameFn();
	};
	var proc = new ProcArray([
		newProcCaptureDepth(0, 0, width, height),
		new ProcTransDepthLinear(getZNear, getZFar),
		new ProcTransDepthByte(),
		new ProcWriteBitmap(nameFn)
	]);
	cap.proc = proc;
	
	return cap;
}

function easyCaptureDepthLog(name)
{
	var cap = new CapturePoint(name);
	var nameFn = function() {
		return cap.nameFn();
	};
	var proc = new ProcArray([
		newProcCaptureDepth(0, 0, width, height),
		new ProcTransDepthLog(getZNear, getZFar),
		new ProcTransDepthByte(),
		new ProcWriteBitmap(nameFn)
	]);
	cap.proc = proc;
	
	return cap;
}

function easyCaptureRgb(name)
{
	var cap = new CapturePoint(name);
	var nameFn = function() {
		return cap.nameFn();
	};
	var proc = new ProcArray([
		newProcCaptureRgb(0, 0, width, height),
		new ProcWriteBitmap(nameFn)
	]);
	cap.proc = proc;
	
	return cap;
}

function easyClearFnAll(red, green, blue)
{
	return function() {
		glClearColor(red, green, blue, 0.0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	};
}

function easyClearFnColor(red, green, blue)
{
	return function() {
		glClearColor(red, green, blue, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
	};
}

function easyClearFnDepth()
{
	return function() {
		glClear(GL_DEPTH_BUFFER_BIT);
	};
}

function easyClearFnNone()
{
	return function() {
	};
}

/**
 * @param clearFn - clear function to use.
 * @param capArray - array of CapturePoints.
 * @param glMod - optional
 */
function easyStream(clearFn, capArray, glMod)
{
	return new Stream(clearFn, capArray, glMod);
}

function easyStreamHudColor(name)
{
	return easyStream(easyClearFnAll(), [easyCaptureRgb(name)], null);
}

function easyStreamHudAlpha(name)
{
	return easyStream(easyClearFnAll(), [easyCaptureAlpha(name)], new GlModHudAlpha());
	
	return stream;
}

////////////////////////////////////////////////////////////////////////////////

function checkStreamName(filming, stream)
{
	for(var i in stream.capturePoints)
	{
		if(filming.hasCaptureName(stream.capturePoints[i].name))
			throw new Error('Filming object already has a stream with a CapturePoint named \''+i.name+'\'.');
	}
}

function addStream(filming, stream)
{
	checkStreamName(filming, stream);
	
	filming.streams[filming.streams.length] = stream;
}

function addHudStream(filming, stream)
{
	checkStreamName(filming, stream);
	
	filming.hudStreams[filming.hudStreams.length] = stream;
}

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

Some notes to myself:

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

*/

// TODO: check for circular references, there are probably some:
// Especially check nameFn referencing capture objects, being referenced by procs, which are inderectly referenced by the capture object
