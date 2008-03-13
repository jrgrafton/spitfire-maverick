#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/projectile_object.h"
						
ProjectileObject::ProjectileObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx,s16 vy,SpriteInfo* si,s16 ttl,u16 explosionRadius,u16 projectileStrength,bool playerProjectile):
	GameObject(name,startx,starty,width,height,heading,vx,vy,si){
	this->playerProjectile= playerProjectile;
	this->ttl=ttl;
	this->explosionRadius=explosionRadius;
	this->projectileStrength=projectileStrength;
	this->exploding=false;
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
	this->exploding=true;
}
bool ProjectileObject::isExploded(){
	return this->exploding;
}
