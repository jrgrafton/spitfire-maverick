#ifndef __AI_OBJECT1__
#define __AI_OBJECT1__

#include "../header/sprite_info.h" //Needed since its a field in class
#include "../header/hardpoint_object.h" //Needed since its a field in class

class AIObject : public GameObject{
	public:
		AIObject(s32 x,s32 y,u16 width,u16 height,s32 heading,SpriteInfo* si);
		AIObject(const AIObject &object);
		~AIObject();
	private:

};
#endif
