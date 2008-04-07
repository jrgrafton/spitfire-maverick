#ifndef __PLANE_OBJECT1__
#define __PLANE_OBJECT1__

#include "../header/destructable_object.h"

class PlayerObject : public DestructableObject{
	public:
		u16 fireDelay;
		u16 bombDelay;
		u16 timeSinceFired;
		u16 timeSinceBombed;
		u16 throttleOn;
		u16 onRunway;
		s16 takingOff;
		bool crashed;
		u16 totalAmmo;
		u16 totalBombs;
		u16 totalFuel;
		u16 smokingInterval;
		u16 timeSinceLastSmoked;

		//functions
		bool canBomb();
		bool canShoot();
		void restock();

		u16 getSmokingInterval();
		u16 getTimeSinceLastSmoked();
		void setTimeSinceLastSmoked(s16 time);

		PlayerObject(s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx,s16 vy,u16 fireDelay,u16 bombDelay,SpriteInfo* si,ParticleObject* particleSpriteInstance);
		~PlayerObject();
};
#endif
