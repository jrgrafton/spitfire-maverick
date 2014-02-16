#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/particle_object.h"
						
ParticleObject::ParticleObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 rotSpeed,s16 vx,s16 vy,s16 ttl,bool heavy,SpriteInfo* si):
	GameObject(name,startx,starty,width,height,heading,vx,vy,si){

	this->ttl=ttl;
	this->rotSpeed=rotSpeed;
	this->heavy=heavy;
}

ParticleObject::~ParticleObject(){
}

ParticleObject::ParticleObject(const ParticleObject &object):GameObject(object){
	this->ttl =object.ttl;
	this->rotSpeed=object.rotSpeed;
	this->heavy=object.heavy;
}

bool ParticleObject::isHeavy(){
	return this->heavy;
}

void ParticleObject::setRotSpeed(s16 rotSpeed){
	this->rotSpeed=rotSpeed;
}

s16 ParticleObject::getRotSpeed(){
	return this->rotSpeed;
}

s16 ParticleObject::getTtl(){
	return this->ttl;
}

void ParticleObject::setTtl(s16 ttl){
	this->ttl=ttl;
}


