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
		void processHeightMap(char* fileLine,vector<u16>* heightMap);
		int addNextHeight(string* line,vector<u16>* heightMap);
		
		void doCollisions();
		int landscapeCollision(s16 x,s16 y);
		void planeLandscapeCollision();

		void processInput();
		void addPlayerBullet();
		
		void doDrawing();
		void drawLandscape();
		void resetLandscape();
		void drawProjectiles();
		void drawPlane();
		
		inline u16 taller(u16 a,u16 b);
		inline u16 smaller(u16 a,u16 b);
		inline u32 squared(u32 a);
		inline s16 getViewPortX();
		inline s16 getViewPortY();

		void doUpdates();
		void updatePlane();
		void updateViewport();
		void updateProjectiles();

		void planeCrash();

		void print_debug(void);
};
#endif
