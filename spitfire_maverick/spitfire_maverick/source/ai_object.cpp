#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/ai_object.h"

AIObject::AIObject(s32 x,s32 y,u16 width,u16 height,s32 heading,SpriteInfo* si):
	GameObject(new string("Ai"),x,y,width,height,heading,0,0,si){
}
AIObject::AIObject(const AIObject &object):GameObject(object){

}
AIObject::~AIObject(){

}
