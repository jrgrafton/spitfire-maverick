#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/plane_object.h"
						
PlaneObject::PlaneObject(s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx,s16 vy,u16 fireDelay,u16 bombDelay,SpriteInfo* si):
GameObject(new string("player_plane"),startx,starty,width,height,heading,vx, vy,si){
	this->timeSinceFired=0;
	this->timeSinceBombed=0;
	this->speed=0;
	this->fireDelay=fireDelay;
	this->bombDelay=bombDelay;
	this->throttleOn=0;
	this->onRunway=1;
	this->takingOff=0;
	this->crashed=false;
	this->totalBombs=4;
	this->totalAmmo=500;
	this->totalFuel=1200;
	this->totalHealth=100;
}
bool PlaneObject::canShoot(){
	return (totalAmmo>0&&timeSinceFired>fireDelay);
}

bool PlaneObject::canBomb(){
	return (totalBombs>0&&timeSinceBombed>bombDelay);
}

void PlaneObject::restock(){
	this->totalBombs=4;
	this->totalAmmo=500;
	this->totalFuel=18000;
	this->totalHealth=100;
}
PlaneObject::~PlaneObject(){
}
