#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/particle_object.h"
						
ParticleObject::ParticleObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 rotSpeed,s16 vx,s16 vy,s16 ttl,bool heavy,SpriteInfo* si):
	GameObject(name,startx,starty,width,height,heading,vx,vy,si){

	this->ttl=ttl;
	this->rotSpeed=rotSpeed;
	this->heavy=heavy;
	this->zoom=zoom;
}

ParticleObject::~ParticleObject(){
}
