#ifndef __INGAME1__
#define __INGAME1__

#include "../header/game_object.h"

//Main State constants
const u16 SCREENHOLE =48;	//How big percieved hole between screens is

//Defines for width and height of screen
const u16 SHEIGHT = 191;
const u16 SWIDTH = 255;

class InGame : public State{
	public:
		InGame();
		~InGame();
		void run();
		void reset();
		NAME getMyName(); //Not implemented yet
	protected:
		void init();
	private:
		void initGraphics();
		void doCollisions();
		void processInput();
		void doDrawing();
		inline u32 squared(u32 a);
		void processHeightMap(char* heightMap);
		int getNextHeight(string* line);
		void doUpdates();
		void print_debug(void);

};
#endif
