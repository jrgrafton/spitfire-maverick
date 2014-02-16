#ifndef __GAME_OBJECT1__
#define __GAME_OBJECT1__
#include "../header/sprite_info.h" //Needed since its a field in class

/**
This is the base class for all in game objects. Have tried to keep it as simple as possible
and create extensions where extra functionality is needed. (E.G SpriteInfo class aninmation info class)
**/

class GameObject{
	public:
		//Functions
		s32 getX() const;
		s32 getY() const;
		void setX(s32 x);
		void setY(s32 y);
		s16 getVx() const;
		s16 getVy() const;
		void setVx(s16);
		void setVy(s16);

		s32 getHeading() const;
		void setHeading(s32 heading);
		s32 getSpeed() const;
		void setSpeed(s32 speed);
		void setObjectWidth(u16 width);
		u16 getObjectWidth() const;
		u16 getObjectHeight() const;
		SpriteInfo* getSpriteInfo() const;
		void setLocation(s32 x,s32 y);
		string* getName() const;
		
		GameObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx, s16 vy,SpriteInfo* si);
		GameObject(const GameObject &object);	//Copy constructor
		virtual ~GameObject();//Make sure right destructor gets called when base object is destroyed	

	private:
		//Fields
		s32 x, y; //s32 position >>8 allows for fp emulation
		s16 vx, vy; // xy trajectory
		s32 speed;
		u16 width, height; //Dimensions in pixels
		string* name;
		SpriteInfo* si;
		s32 heading;

};
#endif
