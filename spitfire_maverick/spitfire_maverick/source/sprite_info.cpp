#include "../header/main.h"
#include "../header/sprite_info.h"

SpriteInfo::SpriteInfo(u16 width,u16 height,s32 angle,s16 gfxref,s16 spriteIndex,s16 rotIndex,s16 paletteIndex,u8 obj_shape,u8 obj_size,u16 zoom,u16 doubleSize,u16 priority,bool usesRot,bool usesSprite,bool usesAlpha){
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
	this->usesRot=usesRot;
	this->usesSprite=usesSprite;
	this->usesAlpha=usesAlpha;
	this->priority=priority;
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
	this->usesRot=object.usesRot;
	this->usesSprite = object.usesSprite;
	this->obj_shape=object.obj_shape;
	this->obj_size=object.obj_size;
	this->usesAlpha=object.usesAlpha;
	this->priority=object.priority;
}

SpriteInfo::~SpriteInfo(){
}

u16 SpriteInfo::getSpriteHeight(){
	return height*(doubleSize+1);
}
u16 SpriteInfo::getSpriteWidth(){
	return width*(doubleSize+1);
}
bool SpriteInfo::getUsesRot(){
	return usesRot;
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
	return gfxref>=0;
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