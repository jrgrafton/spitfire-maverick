#include "../header/main.h"
#include "../header/sprite_info.h"
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

s32 GameObject::getHeading(){
	return heading;
}

void GameObject::setHeading(s32 heading){
	this->heading=heading;
}
GameObject::GameObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx, s16 vy,SpriteInfo* si){
	this->x=startx;
	this->y=starty;
	this->width=width;
	this->height=height;
	this->name=name;
	this->vx = vx;
	this->vy = vy;
	this->heading =heading;
	this->si = si;
}

GameObject::~GameObject(){
}

SpriteInfo* GameObject::getSpriteInfo(){
	return si;
}

u16 GameObject::getObjectHeight(){
	return (height)/(si->getZoom()/256);
}
u16 GameObject::getObjectWidth(){
	return (width)/(si->getZoom()/256);
}

