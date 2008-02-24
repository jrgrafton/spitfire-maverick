#ifndef __PLANE_OBJECT1__
#define __PLANE_OBJECT1__

class PlaneObject : public GameObject{
	public:
		u16 fireDelay;					//Can fire three times a second
		u16 timeSinceFired;
		u16 throttleOn;
		u16 onRunway;

		PlaneObject(s32 startx,s32 starty,u16 width,u16 height,s16 spriteIndex,u16 gfxref,s32 angle,s16 vx,s16 vy,u16 fireDelay);
		~PlaneObject();
};
#endif
