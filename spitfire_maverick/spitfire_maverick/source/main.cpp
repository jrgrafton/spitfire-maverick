//Custom header includes
#include "../header/main.h"
#include "../header/state.h"
#include "../header/game_object.h"
#include "../header/in_game.h"
#include "../header/splash.h"
#include "../source/gfx/all_gfx.c"

using namespace std;

State * mainState;

int main(void){
	//Initialise everything
	init();
	
	while(1)
	{
		mainState->run();
	}
	return 0;
}

/**
Initialise everything
**/
void init(void){
	//Init libs
	PA_Init();
	PA_InitVBL();
	PA_InitSound();  // Init the sound system
	PA_InitRand();	//Init rand
	fatInitDefault(); //Initialise fat library
	PA_Init8bitBg(0,0); //Init backgrounds
	PA_Init8bitBg(1,0);
	
	mainState = new Splash();
}

/**
**Call this to reset entities
**/
void reset(){
	
}


