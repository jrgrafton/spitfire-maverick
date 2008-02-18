#ifndef __PLANE_OBJECT1__
#define __PLANE_OBJECT1__

class PlaneObject : public GameObject{
	public:
		u16 fireDelay;					//Can fire three times a second
		u16 timeSinceFired;

		PlaneObject(s32 startx,s32 starty,u16 width,u16 height,s16 spriteIndex,s32 angle,u32 speed,u16 fireDelay);
		~PlaneObject();
};
#endif