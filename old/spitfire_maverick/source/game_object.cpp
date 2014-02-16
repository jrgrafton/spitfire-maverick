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
	delete si;
	delete name;
}


s32 GameObject::getX() const{
	return x;
}

s32 GameObject::getY() const{
	return y;
}

void GameObject::setX(s32 x){
	this->x=x;
}

void GameObject::setY(s32 y){
	this->y=y;
}


s32 GameObject::getSpeed() const{
	return speed;
}

void GameObject::setSpeed(s32 speed){
	this->speed=speed;
}

s32 GameObject::getHeading() const{
	return heading;
}

void GameObject::setHeading(s32 heading){
	this->heading=heading;
}
SpriteInfo* GameObject::getSpriteInfo() const{
	return si;
}

u16 GameObject::getObjectHeight() const{
	return (height<<8)/si->getZoom();
}
u16 GameObject::getObjectWidth() const{
	return (width<<8)/si->getZoom();
}
void GameObject::setLocation(s32 x,s32 y){
	this->x=x;
	this->y=y;
}
string* GameObject::getName() const{
	return this->name;
}

s16 GameObject::getVx() const{
	return this->vx;
}

s16 GameObject::getVy() const{
	return this->vy;
}

void GameObject::setVx(s16 vx){
	this->vx=vx;
}
void GameObject::setVy(s16 vy){
	this->vy=vy;
}

void GameObject::setObjectWidth(u16 width){
	this->width=width;
}
