//Internal headers
#include "../header/main.h"
#include "../header/state.h"
#include "../header/splash.h"
#include "splash_top.h" // gif to include
#include "splash_bottom.h" // gif to include

/**
**Splash constructor
**/
Splash::Splash() : State(){
	init();
	myName = SPLASH;
}

/**
**Splash destructor
**/
Splash::~Splash(){

}

/**
**Splash run function
**/
void Splash::run(){
	s16 i;
	// Transition to normal visible background
	for(i = 160; i >= 0; i--){
		PA_SetBrightness(0, i/5); 
		PA_SetBrightness(1, i/5); 
		PA_WaitForVBL();		   
	}   

	s16 time = 180; // 180 frames = 3 seconds
	while(time && (!Pad.Newpress.Anykey) && (!Stylus.Newpress)){ // Display until time over or keys pressed
		time--; // time goes by
		PA_WaitForVBL();
	}		
	
	// Transition to white
	for(i = 0; i < 160; i++){
		PA_SetBrightness(0, i/5); 
		PA_SetBrightness(1, i/5); 
		PA_WaitForVBL();		   
	}  	

	// Now that it's all white, clean all that and you're ready to go !
	PA_ResetBgSys();
	PA_SetBrightness(0, 0); // normal
	PA_SetBrightness(1, 0); // normal
	
	//Switch to main game state 
	delete mainState;
	//mainState = new Menu();
}
/**
**Splash init function
**/
void Splash::init(){

	PA_SetBrightness(0, 31); // all White
	PA_SetBrightness(1, 31); // all White	
	
	// Load splash while hidden
	PA_LoadGif(	1, (void*)splash_top); // Gif File
	PA_LoadGif(	0, (void*)splash_bottom); // Gif File
	
	//Give backgrounds time to load
	for(uint i = -5; i <= 0; i++){
		PA_WaitForVBL();		   
	}   
}

/**
**Splash reset function
**/
void Splash::reset(){}

