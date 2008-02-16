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
		void initGround();
		void doCollisions();
		void processInput();
		void doDrawing();
		void renderLandscape();
		void resetLandscape();
		inline u16 taller(u16 a,u16 b);
		inline u16 smaller(u16 a,u16 b);
		inline u32 squared(u32 a);
		void processHeightMap(char* heightMap);
		int getNextHeight(string* line);
		void doUpdates();
		void print_debug(void);
		inline s16 getViewPortX();
		inline s16 getViewPortY();
		void planeCrash();

};
#endif
