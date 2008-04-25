#ifndef __PLANE_OBJECT1__
#define __PLANE_OBJECT1__

#include "../header/destructable_object.h"

class PlayerObject : public DestructableObject{
	public:
		//functions
		bool canBomb();
		bool canShoot();
		void restock();

		u16 getSmokingInterval();
		u16 getTimeSinceLastSmoked();
		void setTimeSinceLastSmoked(s16 time);

		u16 getTimeSinceFired();
		void setTimeSinceFired(u16 time);
		u16 getTimeSinceBombed();
		void setTimeSinceBombed(u16 time);
		s16 getTotalAmmo();
		void setTotalAmmo(s16 totalAmmo);
		s16 getTotalBombs();
		void setTotalBombs(s16 totalBombs);
		bool isThrottleOn();
		void setThrottle(bool throttleOn);
		bool isOnRunway();
		void setOnRunway(bool onRunway);
		s16 getTakingOff();
		void setTakingOff(s16 takingOff);
		s16 getTotalFuel();
		void setTotalFuel(s16 totalFuel);

		PlayerObject(s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx,s16 vy,u16 fireDelay,u16 bombDelay,SpriteInfo* si,ParticleObject* particleSpriteInstance);
		~PlayerObject();

	private:
		u16 fireDelay;
		u16 bombDelay;
		u16 timeSinceFired;
		u16 timeSinceBombed;
		bool throttleOn;
		bool onRunway;
		s16 takingOff;
		s16 totalAmmo;
		s16 totalBombs;
		s16 totalFuel;
		u16 smokingInterval;
		u16 timeSinceLastSmoked;
};
#endif
