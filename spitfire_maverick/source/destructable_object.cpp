#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/destructable_object.h"

DestructableObject::DestructableObject(s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx, s16 vy,SpriteInfo* si,s32 health,s16 particleCount,s16 destroyedGfxRef,ParticleObject* particleSpriteInstance):
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
	this->particleSpriteInstance=new ParticleObject(*object.particleSpriteInstance);
	this->objectDestroyed=false;
}
DestructableObject::~DestructableObject(){

}

void DestructableObject::destructObject(){
	this->getSpriteInfo()->setGfxRef(destroyedGfxRef);
	this->objectDestroyed=true;
}

s32 DestructableObject::getHealth() const{
	return health;
}
void DestructableObject::setHealth(s32 health){
	this->health=health;
}
u16 DestructableObject::getParticleCount() const{
	return particleCount;
}
ParticleObject* DestructableObject::getParticleSpriteInstance() const{
	return particleSpriteInstance;
}
bool DestructableObject::getDestroyed() const{
	return objectDestroyed;
}
