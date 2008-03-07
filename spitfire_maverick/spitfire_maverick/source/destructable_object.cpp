#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/destructable_object.h"

DestructableObject::DestructableObject(s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx, s16 vy,SpriteInfo* si,s16 health,s16 particleCount,s16 destroyedGfxRef,SpriteInfo* particleInstance):
GameObject(new string("landscape_object"),startx,starty,width,height,heading,vx, vy,si){
	
	this->health=health;
	this->particleCount=particleCount;
	this->destroyedGfxRef=destroyedGfxRef;
	this->particleInstance=particleInstance;
	this->destroyed=false;
}

//Call base class copy constructor too!
DestructableObject::DestructableObject(const DestructableObject &object):GameObject(object){
	this->health=object.health;
	this->particleCount=object.particleCount;
	this->destroyedGfxRef=object.destroyedGfxRef;
	this->particleInstance=new SpriteInfo(*particleInstance);
	this->destroyed=false;
}
DestructableObject::~DestructableObject(){

}

void DestructableObject::destroy(){
	destroyed=false;
	this->getSpriteInfo()->setGfxRef(destroyedGfxRef);
}

s16 DestructableObject::getHealth(){
	return health;
}
void DestructableObject::setHealth(s16 health){
	this->health=health;
}
u16 DestructableObject::getParticleCount(){
	return particleCount;
}
SpriteInfo* DestructableObject::getParticleInstance(){
	return particleInstance;
}
bool DestructableObject::getDestroyed(){
	return destroyed;
}
