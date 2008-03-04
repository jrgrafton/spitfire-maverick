#ifndef __PLANE_OBJECT1__
#define __PLANE_OBJECT1__

class PlaneObject : public GameObject{
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
		u16 totalHealth;
		u16 totalFuel;

		//functions
		bool canBomb();
		bool canShoot();
		void restock();

		PlaneObject(s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx,s16 vy,u16 fireDelay,u16 bombDelay,SpriteInfo* si);
		~PlaneObject();
};
#endif
