#include "../header/main.h"
#include "../header/sprite_info.h"

SpriteInfo::SpriteInfo(u16 width,u16 height,s32 angle,s16 gfxref,s16 spriteIndex,s16 rotIndex,s16 paletteIndex,u8 obj_shape,u8 obj_size,u16 zoom,u16 doubleSize,u16 priority,bool usesRotZoom,bool usesSprite,bool usesAlpha){
	this->width=width;
	this->height=height;
	this->angle=angle;
	this->gfxref=gfxref;
	this->spriteIndex=spriteIndex;
	this->rotIndex=rotIndex;
	this->paletteIndex=paletteIndex;
	this->obj_shape=obj_shape;
	this->obj_size=obj_size;
	this->zoom=zoom;
	this->doubleSize=doubleSize;
	this->usesRotZoom=usesRotZoom;
	this->usesSprite=usesSprite;
	this->usesAlpha=usesAlpha;
	this->priority=priority;
	this->animationInfo=NULL;
	this->data=NULL;
}

SpriteInfo::SpriteInfo(u16 width,u16 height,s32 angle,s16 gfxref,s16 spriteIndex,s16 rotIndex,s16 paletteIndex,u8 obj_shape,u8 obj_size,u16 zoom,u16 doubleSize,u16 priority,bool usesRotZoom,bool usesSprite,bool usesAlpha,AnimationInfo* animationInfo,void* data){
	this->width=width;
	this->height=height;
	this->angle=angle;
	this->gfxref=gfxref;
	this->spriteIndex=spriteIndex;
	this->rotIndex=rotIndex;
	this->paletteIndex=paletteIndex;
	this->obj_shape=obj_shape;
	this->obj_size=obj_size;
	this->zoom=zoom;
	this->doubleSize=doubleSize;
	this->usesRotZoom=usesRotZoom;
	this->usesSprite=usesSprite;
	this->usesAlpha=usesAlpha;
	this->priority=priority;
	this->animationInfo=animationInfo;
	this->data = data;
}
//Sprite info copy constructor....needed since this class has an array
SpriteInfo::SpriteInfo(const SpriteInfo &object){
	this->width=object.width;
	this->height=object.height;
	this->angle=object.angle;
	this->gfxref=object.gfxref;
	this->spriteIndex=object.spriteIndex;
	this->rotIndex=object.rotIndex;
	this->paletteIndex=object.paletteIndex;
	this->zoom=object.zoom;
	this->doubleSize=object.doubleSize;
	this->usesRotZoom=object.usesRotZoom;
	this->usesSprite = object.usesSprite;
	this->obj_shape=object.obj_shape;
	this->obj_size=object.obj_size;
	this->usesAlpha=object.usesAlpha;
	this->priority=object.priority;

	//WTF please someone tell me why you cant assign a pointer a null value before initialising it in
	//a copy constructor!!!!
	this->animationInfo=new AnimationInfo(*object.animationInfo);
	if(object.animationInfo==NULL)this->animationInfo=NULL;

	//Hmm dodgy? (is void* being properly copied?)
	this->data=object.data;
}

SpriteInfo::~SpriteInfo(){
	animationInfo=NULL;
	if(this->spriteIndex!=-1){
		PA_SetSpriteAnim(0, spriteIndex, 0);	//Reset this sprite indexes animation
	}
}

u16 SpriteInfo::getSpriteHeight(){
	return height*(doubleSize+1);
}
u16 SpriteInfo::getSpriteWidth(){
	return width*(doubleSize+1);
}
bool SpriteInfo::getUsesRotZoom(){
	return usesRotZoom;
}
u16 SpriteInfo::getZoom(){
	return zoom;
}
s32 SpriteInfo::getAngle(){
	return angle;
}
void SpriteInfo::setAngle(s32 angle){
	this->angle=angle;
}
u16 SpriteInfo::getGfxRef(){
	return gfxref;
}
void SpriteInfo::setGfxRef(u16 gfxref){
	this->gfxref=gfxref;
}
s16 SpriteInfo::getRotIndex(){
	return rotIndex;
}
void SpriteInfo::setRotIndex(s16 rotIndex){
	this->rotIndex=rotIndex;
}

s16 SpriteInfo::getSpriteIndex(){
	return spriteIndex;
}
void SpriteInfo::setSpriteIndex(s16 spriteIndex){
	this->spriteIndex=spriteIndex;
}

s16 SpriteInfo::getPaletteIndex(){
	return paletteIndex;
}

bool SpriteInfo::getUsesSprite(){
	return gfxref!=-1;
}

u16 SpriteInfo::getDoubleSize(){
	return doubleSize;
}

u8 SpriteInfo::getObjShape(){
	return this->obj_shape;
}

u8 SpriteInfo::getObjSize(){
	return this->obj_size;
}
bool SpriteInfo::getUsesAlpha(){
	return this->usesAlpha;
}

void SpriteInfo::setUsesAlpha(bool usesAlpha){
	this->usesAlpha=usesAlpha;
}
u16 SpriteInfo::getPriority(){
	return this->priority;
}

void SpriteInfo::updateSpriteFrame(){
	if(!this->usesAnimation()){return;}
	this->animationInfo->tick();
	if(this->spriteIndex!=-1){
		PA_SetSpriteAnim(0, this->getSpriteIndex(), this->animationInfo->getCurrentFrame());
	}
}

/**
Used so draw Object knows whether to call create SpriteObject or create SpriteObject from gfx
**/
bool SpriteInfo::usesAnimation(){
	return animationInfo!=NULL;
}

void* SpriteInfo::getData(){
	return this->data;
}

AnimationInfo* SpriteInfo::getAnimationInfo(){
	return this->animationInfo;
}
