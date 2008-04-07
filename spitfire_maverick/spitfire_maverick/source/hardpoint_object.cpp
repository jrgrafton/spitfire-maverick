#include "../header/main.h"
#include "../header/in_game.h"		//Needed for wrap angle functions
#include "../header/game_object.h"
#include "../header/hardpoint_object.h"

HardpointObject::HardpointObject(s32 x,s32 y,u16 width,u16 height,s32 heading,SpriteInfo* si,s16 xOffset,s16 yOffset,u16 fireDelay,u16 turningSpeed,u16 maxInclination,s16 minInclination,u16 firingVelocity,ProjectileObject* po):
	GameObject(new string("hardpoint"),x,y,width,height,heading,0,0,si){
	this->xOffset=xOffset;
	this->yOffset=yOffset;
	this->fireDelay=fireDelay;
	this->turningSpeed=turningSpeed;
	this->maxInclination=maxInclination;
	this->minInclination=minInclination;
	this->po=po;
	this->timeSinceLastFired=0;
	this->firingVelocity=firingVelocity;
	target=NULL;
}

/**
Special copy constructor with ref to parent since when copying another
hardpoint object (when say looking up and copying an AI object) we need
to explicitly set the parent when the parent is copied.
**/
HardpointObject::HardpointObject(const HardpointObject &object,GameObject* parent):GameObject(object){
	this->fireDelay=object.fireDelay;
	this->turningSpeed=object.turningSpeed;
	this->maxInclination=object.maxInclination;
	this->minInclination=object.minInclination;
	this->po=new ProjectileObject(*po);
	this->timeSinceLastFired=0;
	this->xOffset=object.xOffset;
	this->yOffset=object.yOffset;
	target=NULL;
	this->parent = parent;
}

HardpointObject::~HardpointObject(){}

bool HardpointObject::canFire(){
	timeSinceLastFired++;
	if(timeSinceLastFired>fireDelay){
		return true;
	}
	return false;
} 

void HardpointObject::fire(){this->timeSinceLastFired=0;}

/**
Turn this hard point towards its target
**/
void HardpointObject::turnTowardsTarget(){
	if(target==NULL)return;
	
	u16 cx = (this->getX()>>8) + this->getSpriteInfo()->getSpriteWidth()/2;
	u16 cy = (this->getY()>>8) + this->getSpriteInfo()->getSpriteHeight()/2;
	u16 baseObjectAngle = parent->getHeading()>>8;
	u16 angleToTarget = PA_GetAngle(cx,cy,this->target->getX()>>8,this->target->getY()>>8);

	//We are at our full turning extent so return
	s32 necessaryInclination = InGame::wrapAngleDistance(baseObjectAngle,angleToTarget);
	if(necessaryInclination>maxInclination||necessaryInclination<minInclination){return;}

	//We are close to target so aim str8 at it
	s16 distance = InGame::wrapAngleDistance(this->getHeading()>>8,angleToTarget);
	if(abs(distance)<turningSpeed){this->setHeading(angleToTarget<<8);return;}
	
	//Else turn towards it
	s32 heading = (distance<0)? InGame::wrapBigAngle(this->getHeading()-turningSpeed):(distance==0)?this->getHeading():InGame::wrapBigAngle(this->getHeading()+turningSpeed);
	this->setHeading(heading);
	this->getSpriteInfo()->setAngle(heading);
}
void HardpointObject::update(){
	//First try and turn towards target
	turnTowardsTarget();

	//Now calculate the position we should be in based of center of base object
	s32 finalx = 0;
	s32 finaly = 0;
	s32 cx = this->parent->getX()+((this->parent->getSpriteInfo()->getSpriteWidth()/2)<<8);
	s32 cy = this->parent->getY()+((this->parent->getSpriteInfo()->getSpriteHeight()/2)<<8);

	u32 xOffsetAngle = this->parent->getSpriteInfo()->getAngle();
	u32 yOffsetAngle = InGame::wrapBigAngle((this->parent->getSpriteInfo()->getAngle())-(128<<8));

	//Calculate xOffset translation
	s16 xComponant = PA_Cos(xOffsetAngle);
	s16 yComponant = -PA_Sin(xOffsetAngle);
	finalx += cx+xComponant*this->xOffset;
	finaly += cy+yComponant*this->xOffset;

	//Calculate yOffset translation
	xComponant = PA_Cos(yOffsetAngle);
	yComponant = -PA_Sin(yOffsetAngle);
	finalx += xComponant*this->xOffset;
	finaly += yComponant*this->yOffset;

	//Set the position of this hard point
	this->setX(finalx - ((this->getSpriteInfo()->getSpriteWidth()/2)<<8));
	this->setY(finaly - ((this->getSpriteInfo()->getSpriteHeight()/2)<<8));
}

ProjectileObject* HardpointObject::getProjectileObject(){
	return this->po;
}

u16 HardpointObject::getFiringVelocity(){
	return this->firingVelocity;
}
