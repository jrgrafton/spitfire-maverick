#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/plane_object.h"

PlaneObject::PlaneObject(s32 startx,s32 starty,u16 width,u16 height,s16 spriteIndex,s32 angle,u32 speed,u16 fireDelay):
	GameObject(new string("player_plane"),startx,starty,width,height,spriteIndex,angle,speed){

	this->timeSinceFired=0;
	this->fireDelay=fireDelay;
}

PlaneObject::~PlaneObject(){

}
