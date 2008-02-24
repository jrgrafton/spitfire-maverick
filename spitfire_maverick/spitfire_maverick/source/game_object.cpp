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

GameObject::GameObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s16 spriteIndex,u16 gfxref,s32 angle,s16 vx,s16 vy){
	this->x=startx;
	this->y=starty;
	this->width=width;
	this->height=height;
	this->spriteIndex=spriteIndex;
	this->name=name;

	this->vx = vx;
	this->vy = vy;
	this->angle =angle;//Same as 90 therefore heading -->
	this->gfxref=gfxref;
}

GameObject::~GameObject(){

}
