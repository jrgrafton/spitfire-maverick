#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/player_object.h"
						
PlayerObject::PlayerObject(s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx,s16 vy,u16 fireDelay,u16 bombDelay,SpriteInfo* si,ParticleObject* particleSpriteInstance):
DestructableObject(startx,starty,width,height,heading,vx,vy,si,100,5,-1, particleSpriteInstance){
	this->timeSinceFired=0;
	this->timeSinceBombed=0;
	this->speed=0;
	this->fireDelay=fireDelay;
	this->bombDelay=bombDelay;
	this->throttleOn=0;
	this->onRunway=true;
	this->takingOff=0;
	this->totalBombs=4;
	this->totalAmmo=500;
	this->totalFuel=5000;
	this->smokingInterval=6;
	this->timeSinceLastSmoked=0;
}
bool PlayerObject::canShoot(){
	return (totalAmmo>0&&timeSinceFired>fireDelay);
}

bool PlayerObject::canBomb(){
	return (totalBombs>0&&timeSinceBombed>bombDelay);
}

void PlayerObject::restock(){
	this->totalBombs=4;
	this->totalAmmo=500;
	this->totalFuel=18000;
	this->setHealth(100);
}
PlayerObject::~PlayerObject(){
}

u16 PlayerObject::getTimeSinceLastSmoked(){
	return this->timeSinceLastSmoked;
}
void PlayerObject::setTimeSinceLastSmoked(s16 time){
	this->timeSinceLastSmoked = time;
}
u16 PlayerObject::getSmokingInterval(){
	return this->smokingInterval;
}
bool PlayerObject::isOnRunway(){
	return this->onRunway;
}
