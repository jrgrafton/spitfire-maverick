#include "../header/main.h"
#include "../header/sprite_info.h"
#include "../header/game_object.h"

//Constructor
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
//Copy constructor
GameObject::GameObject(const GameObject &object){
	this->x=object.x;
	this->y=object.y;
	this->width=object.width;
	this->height=object.height;
	this->name= new string(object.name->c_str());
	this->vx = object.vx;
	this->vy = object.vy;
	this->heading =object.heading;
	this->si = new SpriteInfo(*object.si);	//Invoke copy constructor for sprite info
}

//Destructor
GameObject::~GameObject(){
}


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
SpriteInfo* GameObject::getSpriteInfo(){
	return si;
}

u16 GameObject::getObjectHeight(){
	return (height<<8)/si->getZoom();
}
u16 GameObject::getObjectWidth(){
	return (width<<8)/si->getZoom();
}
void GameObject::setLocation(s32 x,s32 y){
	this->x=x;
	this->y=y;
}

