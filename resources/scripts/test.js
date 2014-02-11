this.finishFunc = null;

var filming = new Filming('testRec');

{
	addStream(filming, easyRgbStream('rgb', null));
	
	hookFilming(filming);
		
	finishFunc = function() {
		finishFilming(filming);
	}
	
	filming.startRecording();
}

// to stop filming and clean-up, call:
// if(this.finishFunc) this.finishFunc();