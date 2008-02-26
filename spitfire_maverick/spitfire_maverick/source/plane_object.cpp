#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/plane_object.h"
						
PlaneObject::PlaneObject(s32 startx,s32 starty,u16 width,u16 height,s16 spriteIndex,u16 gfxref,s32 angle,s16 vx,s16 vy,u16 fireDelay):
	GameObject(new string("player_plane"),startx,starty,width,height,spriteIndex,gfxref,angle,vx,vy){

	this->timeSinceFired=0;
	this->speed=0;
	this->fireDelay=fireDelay;
	this->throttleOn=0;
	this->onRunway=1;
	this->takingOff=0;
}

PlaneObject::~PlaneObject(){

}
