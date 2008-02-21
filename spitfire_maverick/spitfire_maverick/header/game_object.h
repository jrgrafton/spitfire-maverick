#ifndef __GAME_OBJECT1__
#define __GAME_OBJECT1__

class GameObject{
	public:
		//Fields
		string* name;
		s32 x, y; //s32 position >>8 allows for fp emulation
		s16 spriteIndex;
		u16 width, height; //Dimensions in pixels
		s32 angle;

		//Functions
		s16 getX();
		s16 getY();
		s32 getAngle();

		GameObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s16 spriteIndex,s32 angle,u32 speed);
		~GameObject();
        
		//Stuff thats just for dynamic objects
		s16 vx, vy; // xy trajectory
		s32 speed;
		s16 getSpeed();
};
#endif
