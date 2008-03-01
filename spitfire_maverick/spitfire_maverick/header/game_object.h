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
		u16 gfxref;
		s16 rotIndex;
		s16 palette;
		u16* objsize;

		//Functions
		s16 getX();
		s16 getY();
		s32 getAngle();
		
		GameObject(string* name,s32 startx,s32 starty,u16 width,u16 height,u16* objsize,s16 spriteIndex,s16 palette,u16 gfxref,s32 angle,s16 rotIndex,s16 vx, s16 vy);
		~GameObject();
        
		//Stuff thats just for dynamic objects
		s16 vx, vy; // xy trajectory
		s32 speed;
		s16 getSpeed();
};
#endif
