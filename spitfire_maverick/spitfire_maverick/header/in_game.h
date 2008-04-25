#ifndef __INGAME1__
#define __INGAME1__

#include "../header/game_object.h"
#include "../header/particle_object.h"
#include "../header/destructable_object.h"
#include "../header/projectile_object.h"

/**
Class representing main game state. Ideally some more modularization is required which
would allow for the creation of physics and rendering classes.
**/

//Just used for rotated rectangle collision...didnt wanna bother with two extra
//files just for this function
class Vector2D {
	public:
		Vector2D(){}
		Vector2D(s32 x,s32 y){this->x=x;this->y=y;}
		s32 x, y;
};

class Line {
	public:
		Vector2D o;	//Start
		Vector2D p; //End
		Line(){}
		Line(Vector2D o,Vector2D p){this->o=o;this->p=p;}
};

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

		//Static utility functions that can be used outside of class (used in turret for turret rotation)
		static u16 wrapAngle(s16 angle);
		static u32 wrapBigAngle(s32 angle);
		static s16 wrapAngleDistance(u16 angle1,u16 angle2);	//Needed by turret class for rotation
		static u16 flipAngle(s16 angle);

	protected:
		void init();
	private:
		/**--------------------------------Init functions--------------------------------**/
		void initGraphics();
		void initSound();
		void initLanscapeLookup();
		void initParticleLookup();
		void initProjectileLookup();
		void initAILookup();
		void initLevel();
		void initRunway();
		void initPlayer();
		void processHeightMap(char* fileLine,vector<u16>* heightMap);
		int addNextHeight(string* line,vector<u16>* heightMap);
		void addLandscapeObject(s32 x,u16 ref);
		void addAIObject(s32 x,u16 ref);

		/**--------------------------------Drawing functions--------------------------------**/
		void doDrawing();
		void drawLandscapeTiles();
		void drawLandscapeObjects();
		void resetLandscape();
		void drawProjectiles();
		void drawParticles();
		void drawPlayer();
		void drawRunway();
		void drawAI();
		void drawObject(GameObject* go);

		/**--------------------------------Update functions--------------------------------**/
		void doUpdates();
		void updatePlayer();
		u16 getSpeedFromVelocity(s16 vx,s16 vy);
		void updateViewport();
		void updateProjectiles();
		void updateParticles();
		void updateAI();
		
		/**--------------------------------Collision functions--------------------------------**/
		bool collideObject(GameObject* go,bool (InGame::* collisionRoutine) (GameObject* go1,GameObject* go2) const,bool landscape,bool landscapeObj,bool player,bool ai,bool reflect);
		bool landscapeCollision(GameObject * go,bool reflect);
		bool landscapePointCollision(s16 x,s16 y) const;
		bool landscapeObjectCollision(GameObject * go,bool (InGame::* collisionRoutine) (GameObject* go1,GameObject* go2)const);
		bool AIObjectCollision(GameObject * go,bool (InGame::* collisionRoutine) (GameObject* go1,GameObject* go2)const);
		
		bool pointInRectangleCollision(GameObject* go1,GameObject* go2) const;
		bool circleAndRectangleCollision(GameObject* go1,GameObject* go2) const;
		bool rotatedRectangleCollision(GameObject* go1,GameObject* go2) const;
		bool LineIntersect( Line &a, Line &b) const;
		
		void getLinesForRectangle(GameObject* go,Line* lines) const;
		void getVertices(GameObject* go,s32 *v0,s32 *v1,s32 *v2,s32 *v3) const;
		void getBottomEndOfObject(GameObject* go,s32 &frontx,s32 &fronty,s16 direction) const;
		void getMiddleEndOfObject(GameObject* go,s32 &frontx,s32 &fronty,s16 direction) const;
		void releaseObjectResources(GameObject* go);		//Whenever we delete a gameObject make sure it releases its rot and sprite index
		
		//Player has special crash functions for camera tracking particles and scrolling back to runway
		void playerCollisions();
		void playerCrash();
		void playerCrashParticles();
		void scrollBackToRunway();
		
		//Called when destructable object explodes
		void addParticlesFromObject(DestructableObject* destructable);

		//Called when projectile explodes
		void addExplosionAnimationFromProjectile(ProjectileObject* projctile);
		
		/**--------------------------------Util functions--------------------------------**/
		inline u16 lowest(u16 a,u16 b) const;
		inline u16 highest(u16 a,u16 b) const;
		inline u32 squared(s32 a) const;
		u16 getHeightAtPoint(u16 x) const;
		u16 getNormalAtPoint(u16 x) const;
		u16 reflectOverNormal(u16 angle,u16 normal) const;
		inline s16 getViewPortX() const;
		inline s16 getViewPortY() const;

		//Input driven functions
		void processInput();
		void addPlayerBullet();
		void addPlayerBomb();
		
		//Other functions
		void print_debug(void) const;
};
#endif
