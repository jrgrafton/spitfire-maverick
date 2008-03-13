#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/destructable_object.h"

DestructableObject::DestructableObject(s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx, s16 vy,SpriteInfo* si,s16 health,s16 particleCount,s16 destroyedGfxRef,SpriteInfo* particleSpriteInstance):
GameObject(new string("landscape_object"),startx,starty,width,height,heading,vx, vy,si){
	this->health=health;
	this->particleCount=particleCount;
	this->destroyedGfxRef=destroyedGfxRef;
	this->particleSpriteInstance=particleSpriteInstance;
	this->objectDestroyed=false;
}

//Call base class copy constructor too!
DestructableObject::DestructableObject(const DestructableObject &object):GameObject(object){
	this->health=object.health;
	this->particleCount=object.particleCount;
	this->destroyedGfxRef=object.destroyedGfxRef;
	this->particleSpriteInstance=new SpriteInfo(*object.particleSpriteInstance);
	this->objectDestroyed=false;
}
DestructableObject::~DestructableObject(){

}

void DestructableObject::destructObject(){
	this->getSpriteInfo()->setGfxRef(destroyedGfxRef);
	PA_SetSpriteY(0,this->getSpriteInfo()->getSpriteIndex(),193);
	this->getSpriteInfo()->setSpriteIndex(-1);
	this->objectDestroyed=true;
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
SpriteInfo* DestructableObject::getParticleSpriteInstance(){
	return particleSpriteInstance;
}
bool DestructableObject::getDestroyed(){
	return objectDestroyed;
}
