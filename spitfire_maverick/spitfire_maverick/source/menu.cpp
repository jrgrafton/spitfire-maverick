//Internal headers
#include "../header/main.h"
#include "../header/state.h"
#include "../header/menu.h"
#include "../header/in_game.h"
#include "menu_music.h"  // Include the sound (found in the data folder in .raw format)

/**
**Splash constructor
**/
Menu::Menu() : State(){
	init();
	myName = MAINMENU;
}

/**
**Splash destructor
**/
Menu::~Menu(){

}

/**
**Splash run function
**/
void Menu::run(){
	//PA_PlaySoundRepeat(PA_GetFreeSoundChannel(), menu_music);
	PA_PlaySoundEx2(8, menu_music, (s32)menu_music_size, 120, 44100,0, true,0);
	PA_EasyBgLoad(0, 2, menu_background_image);

	while(!Stylus.Newpress){ // Display until keys pressed
		PA_WaitForVBL();
	}
	
	PA_SetSoundChannelVol(8,0);	//Kill engine sound
	PA_ResetBgSys();
	PA_WaitForVBL();

	//Switch to level splash
	delete mainState;
	mainState = new InGame();
}
/**
**Splash init function
**/
void Menu::init(){
	PA_Init8bitBg(0,0); //Init backgrounds
}

/**
**Splash reset function
**/
void Menu::reset(){}

