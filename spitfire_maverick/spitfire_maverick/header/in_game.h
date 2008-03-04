#ifndef __INGAME1__
#define __INGAME1__

#include "../header/game_object.h"
#include "../header/particle_object.h"

//Main State constants
const u16 SCREENHOLE =48;	//How big percieved hole between screens is

//Defines for width and height of screen
const u16 SHEIGHT = 191;
const u16 SWIDTH = 255;

//Defines friction to apply multiply by FRICTION then >>16 (65536)
const u32 FRICTION = 65520;

//Defines gravity (number subtracted from gameobject yvel every update)
const u16 GRAVITY = 8;

//LUT size needs to be 300*300 since taking into account max vx and vy plus gravity
const u32 LUTSIZE = 700*700;

//Plane defines
const u16 PLANEPOWER=2;
const u16 PLANETURNSPEED=600;
const u16 MAXPLANESPEED = 550;
const u16 MINPLANESPEED = 250;

//Runway lenth is always constant (has to be multiple of 64)
const u16 RUNWAYLENGTH = 172;

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
		//Init functions
		void initGraphics();
		void initSound();
		void initLevel();
		void initRunway();
		void initPlane();
		void processHeightMap(char* fileLine,vector<u16>* heightMap);
		int addNextHeight(string* line,vector<u16>* heightMap);
		
		//Collision based functions (these are called from the relative update routines!)
		int landscapeCollision(s16 x,s16 y);
		u16 planeLandscapeCollision();
		u16 getHeightAtPoint(u16 x);
		void planeCrash();
		void planeCrashParticles();
		bool particleLandscapeCollision(ParticleObject* pa);
		u16 getNormalAtPoint(u16 x);
		u16 reflectOverNormal(u16 angle,u16 normal);

		//Input driven functions
		void processInput();
		void addPlayerBullet();
		void addPlayerBomb();
		
		//Drawing functions
		void doDrawing();
		void drawLandscape();
		void resetLandscape();
		void drawProjectiles();
		void drawParticles();
		void drawPlane();
		void drawRunway();
		void drawObject(GameObject* go,u16 priority);
		
		//Utility functions
		inline u16 taller(u16 a,u16 b);
		inline u16 smaller(u16 a,u16 b);
		inline u32 squared(s32 a);
		inline u16 wrapAngle(s16 angle);
		inline u16 wrapAngleShifted(s32 angle);
		inline u16 flipAngle(s16 angle);
		inline s16 wrapAngleDistance(u16 angle1,u16 angle2);
		inline s16 getViewPortX();
		inline s16 getViewPortY();
		
		//Update functions
		void doUpdates();
		void updatePlane();
		u16 getSpeedFromVelocity(s16 vx,s16 vy);
		void updateViewport();
		void updateProjectiles();
		void updateParticles();
		
		//Other functions
		void scrollBackToRunway();
		void print_debug(void);
};
#endif
