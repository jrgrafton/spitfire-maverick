#ifndef __GAME_OBJECT1__
#define __GAME_OBJECT1__
#include "../header/sprite_info.h" //Needed since its a field in class

class GameObject{
	public:
		
		s16 vx, vy; // xy trajectory
		s32 speed;

		//Functions
		s32 getX();
		s32 getY();
		void setX(s32 x);
		void setY(s32 y);
		void setVx(s16);
		void setVy(s16);

		s32 getHeading();
		void setHeading(s32 heading);
		s16 getSpeed();
		void setObjectWidth(u16 width);
		u16 getObjectWidth();
		u16 getObjectHeight();
		SpriteInfo* getSpriteInfo();
		void setLocation(s32 x,s32 y);
		string* getName();
		
		GameObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx, s16 vy,SpriteInfo* si);
		GameObject(const GameObject &object);	//Copy constructor
		virtual ~GameObject();	

	private:
		//Fields
		s32 x, y; //s32 position >>8 allows for fp emulation
		u16 width, height; //Dimensions in pixels
		string* name;
		SpriteInfo* si;
		s32 heading;

};
#endif
