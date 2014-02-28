this.finishFunc = null;

var filming = new Filming('testRec');

{
	addStream(filming, easyStream(
		easyClearFnAll(0.0, 0.0, 0.0),
		[easyCaptureRgb('rgb')],
		null
	));
	
	hookFilming(filming);
		
	finishFunc = function() {
		finishFilming(filming);
	}
	
	filming.startRecording();
}

// to stop filming and clean-up, call:
// if(this.finishFunc) this.finishFunc();