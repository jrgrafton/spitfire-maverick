#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/particle_object.h"
						
ParticleObject::ParticleObject(string* name,s32 startx,s32 starty,u16 width,u16 height,u16* objsize,s16 spriteIndex,u16 palette,u16 gfxref,s32 angle,s16 rotSpeed,s16 rotIndex,s16 vx,s16 vy,s16 ttl,bool heavy):
	GameObject(name,startx,starty,width,height,objsize,spriteIndex,palette,gfxref,angle,rotIndex,vx,vy){

	this->ttl=ttl;
	this->rotSpeed=rotSpeed;
	this->heavy=heavy;
}

ParticleObject::~ParticleObject(){

}
