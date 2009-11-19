// example script
//
// To use this script:
// 1. load a demo
// 2. mirv_movie_simulate 1; mirv_recordmovie_start
// 3. "afx load('mask_example.js');"

function myGlBegin(mode) {
	if(info.recording) {
		fx.rgbMask.opRed = info.in_R_DrawEntitiesOnList ? 1 : -1;
		fx.rgbMask.opGreen = info.in_R_DrawParticles ? 1 : -1;
		fx.rgbMask.opBlue = info.in_R_DrawViewModel ? 1 : -1;
		fx.rgbMask.enabled = info.in_R_DrawEntitiesOnList || info.in_R_DrawParticles || info.in_R_DrawViewModel;
		if(!fx.rgbMask.enabled && 0 != info.getCurrentEntityIndex()) {
			fx.rgbMask.opRed = 1;
			fx.rgbMask.opGreen = -1;
			fx.rgbMask.opBlue = 1;
			fx.rgbMask.enabled = true;
		}
	}
	else fx.rgbMask.enabled = false;
}

events.on_glBegin = myGlBegin;
//events.on_glBegin = undefined;
