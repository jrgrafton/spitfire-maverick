#include "../header/main.h"
#include "../header/game_object.h"

s16 GameObject::getX(){
	return x>>8;
}

s16 GameObject::getY(){
	return y>>8;
}

s16 GameObject::getSpeed(){
	return speed>>8;
}

s32 GameObject::getAngle(){
	return angle>>8;
}

GameObject::GameObject(string* name,s32 startx,s32 starty,u16 width,u16 height,u16* objsize,s16 spriteIndex,s16 palette,u16 gfxref,s32 angle,s16 rotIndex,s16 vx, s16 vy){
	this->x=startx;
	this->y=starty;
	this->width=width;
	this->height=height;
	this->objsize=objsize;
	this->spriteIndex=spriteIndex;
	this->name=name;
	this->palette=palette;
	
	this->vx = vx;
	this->vy = vy;
	this->angle =angle;
	this->rotIndex=rotIndex;
	this->gfxref=gfxref;
}

GameObject::~GameObject(){
	delete[] objsize;
}
