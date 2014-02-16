#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/projectile_object.h"
						
ProjectileObject::ProjectileObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx,s16 vy,SpriteInfo* si,s16 ttl,u16 explosionRadius,u16 projectileStrength,bool playerProjectile):
	GameObject(name,startx,starty,width,height,heading,vx,vy,si){
	this->playerProjectile= playerProjectile;
	this->ttl=ttl;
	this->explosionRadius=explosionRadius;
	this->projectileStrength=projectileStrength;
	this->exploded=false;
	this->explosionAnimation=NULL;
	explosive=false;
}

ProjectileObject::ProjectileObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx,s16 vy,SpriteInfo* si,s16 ttl,u16 explosionRadius,u16 projectileStrength,bool playerProjectile,ParticleObject* explosionAnimation): 
	GameObject(name,startx,starty,width,height,heading,vx,vy,si){
	this->playerProjectile= playerProjectile;
	this->ttl=ttl;
	this->explosionRadius=explosionRadius;
	this->projectileStrength=projectileStrength;
	this->exploded=false;
	this->explosionAnimation=explosionAnimation;
	explosive=true;
}

ProjectileObject::ProjectileObject(const ProjectileObject &object):GameObject(object){
	this->playerProjectile=object.playerProjectile;
	this->ttl=object.ttl;
	this->explosionRadius=object.explosionRadius;
	this->projectileStrength=object.projectileStrength;
	this->exploded=object.exploded;

	//WTF please someone tell me why you cant assign a pointer a null value before initialising it in
	//a copy constructor!!!!
	this->explosionAnimation=new ParticleObject(*object.explosionAnimation);
	if(object.explosionAnimation==NULL)this->explosionAnimation=NULL;
	this->explosive=(this->explosionAnimation==NULL)?false:true;
}

ProjectileObject::~ProjectileObject(){
}

s16 ProjectileObject::getTtl(){
	return this->ttl;
}
void ProjectileObject::setTtl(s16 ttl){
	this->ttl=ttl;
}
u16 ProjectileObject::getExplosionRadius(){
	return this->explosionRadius;
}
u16 ProjectileObject::getProjectileStrength(){
	return this->projectileStrength;
}
bool ProjectileObject::isPlayerProjectile(){
	return this->playerProjectile;
}
void ProjectileObject::explode(){
	this->exploded=true;
}
bool ProjectileObject::isExploded(){
	return this->exploded;
}
bool ProjectileObject::hasExplosionAnimation(){
	return this->explosionAnimation!=NULL;
}

ParticleObject* ProjectileObject::getExplosionAnimation(){
	return this->explosionAnimation;
}

bool ProjectileObject::isExplosive(){
	return this->explosive;
}
