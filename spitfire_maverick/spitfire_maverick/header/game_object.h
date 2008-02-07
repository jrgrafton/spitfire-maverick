#ifndef __GAME_OBJECT1__
#define __GAME_OBJECT1__

class GameObject{
	public:
		//Fields
		s32 x, y; //s32 position >>8 allows for fp emulation
		s16 vx, vy; // xy trajectory
		u32 speed;
		u16 angle;
		s16 spriteIndex;
		u16 width, height; //Dimensions in pixels

		//Functions
		s16 getX();
		s16 getY();

		u16 getSpeed();

		//Image for this sprite
		//void* myImage;

		GameObject(s32 startx,s32 starty,u16 width,u16 height,s16 spriteIndex);
		~GameObject();
};
#endif
