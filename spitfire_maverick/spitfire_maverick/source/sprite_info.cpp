#include "../header/main.h"
#include "../header/sprite_info.h"

SpriteInfo::SpriteInfo(u16 width,u16 height,s32 angle,s16 gfxref,s16 spriteIndex,s16 rotIndex,s16 paletteIndex,u16* objsize,u16 zoom,u16 doubleSize,bool usesRot){
	this->width=width;
	this->height=height;
	this->angle=angle;
	this->gfxref=gfxref;
	this->spriteIndex=spriteIndex;
	this->rotIndex=rotIndex;
	this->paletteIndex=paletteIndex;
	this->objsize=objsize;
	this->zoom=zoom;
	this->doubleSize=doubleSize;
	this->usesRot=usesRot;
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
	
	//need to make a copy of the array
	this->objsize = new u16[2];
	this->objsize[0] = object.objsize[0];
	this->objsize[1] = object.objsize[1];
}

SpriteInfo::~SpriteInfo(){
	delete[] objsize;
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
u16* SpriteInfo::getObjSize(){
	return objsize;
}

bool SpriteInfo::getUsesSprite(){
	return gfxref>=0;
}

u16 SpriteInfo::getDoubleSize(){
	return doubleSize;
}
