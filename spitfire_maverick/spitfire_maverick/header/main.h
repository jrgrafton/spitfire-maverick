#ifndef __MAIN1__
#define __MAIN1__
// Includes
#include <PA9.h>
#include <math.h>
#include <fat.h>
#include <string>
#include <vector>

using namespace::std;

//GFX Includes
#include "../source/gfx/all_gfx.h"

//Internal include
#include "../header/state.h"

//Const definitions
#define DEBUG				//Comment out to turn off debug mode
//#define EMULATOR			//Are we compiling for emulator?
void init(void);
void reset();

//Extern main state so other objects can modify it
extern State * mainState;


#endif

