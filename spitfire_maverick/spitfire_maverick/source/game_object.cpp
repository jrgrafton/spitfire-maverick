#include "../header/main.h"
#include "../header/game_object.h"

s16 GameObject::getX(){
	return x>>8;
}

s16 GameObject::getY(){
	return y>>8;
}

u16 GameObject::getSpeed(){
	return speed>>8;
}

s32 GameObject::getAngle(){
	return angle>>8;
}

GameObject::GameObject(s32 startx,s32 starty,u16 width,u16 height,s16 spriteIndex){
	this->x=startx;
	this->y=starty;
	this->width=width;
	this->height=height;
	this->spriteIndex=spriteIndex;

	this->vx =this->vy =this->speed=0;
	this->angle =0;//Same as 90 therefore heading -->
}

GameObject::~GameObject(){

}
