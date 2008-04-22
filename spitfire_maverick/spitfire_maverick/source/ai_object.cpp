#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/destructable_object.h"
#include "../header/ai_object.h"

AIObject::AIObject(s32 x,s32 y,u16 width,u16 height,s32 heading,SpriteInfo* si,s16 health,s16 particleCount,s16 destroyedGfxRef,ParticleObject* particleSpriteInstance,bool stationary,HardpointObject* hardpoint,STATUS status, SIDE side):
	DestructableObject(x,y,width,height,heading,0,0,si,health,particleCount,destroyedGfxRef,particleSpriteInstance){

	this->side=side;
	this->status=status;
	this->stationary=stationary;
	this->hardpoint=hardpoint;
}
AIObject::AIObject(const AIObject &object):DestructableObject(object){
	this->side=object.side;
	this->status=object.status;
	this->stationary=object.stationary;
	this->hardpoint=new HardpointObject(*object.hardpoint,*this);
}
AIObject::~AIObject(){

}
AIObject::SIDE AIObject::getSide(){
	return this->side;
}
void AIObject::setStatus(STATUS status){
	this->status=status;
}
AIObject::STATUS AIObject::getStatus(){
	return this->status;
}
bool AIObject::getStationary(){
	return this->stationary;
}
HardpointObject* AIObject::getHardpoint(){
	return this->hardpoint;
}
void AIObject::updateHardpoint(){
	//First try and turn hardpoint towards target
	this->hardpoint->turnTowardsTarget();
	//Now update hardpoints render offset
	this->hardpoint->updateRenderOffset();

}
bool AIObject::hasTarget(){
	return this->hardpoint->hasTarget();
}
void AIObject::setTarget(DestructableObject* target){
	this->hardpoint->setTarget(target);
}
