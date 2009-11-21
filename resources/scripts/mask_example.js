// example script
//
// To use this script:
// 1. load a demo
// 2. mirv_movie_simulate 1; mirv_recordmovie_start
// 3. "afx load('mask_example.js');"

function myGlBegin(mode) {
	if(info.recording) {
		fx.replace.red = info.in_R_DrawEntitiesOnList;
		fx.replace.green = info.in_R_DrawParticles;
		fx.replace.blue = info.in_R_DrawViewModel;
		fx.replace.enabled = info.in_R_DrawEntitiesOnList || info.in_R_DrawParticles || info.in_R_DrawViewModel;
		if(!fx.replace.enabled && 0 != info.getCurrentEntityIndex()) {
			fx.replace.red = true;
			fx.replace.green = false;
			fx.replace.blue = true;
			fx.replace.enabled = true;
		}
	}
	else fx.replace.enabled = false;
}

events.on_glBegin = myGlBegin;
//events.on_glBegin = undefined;
