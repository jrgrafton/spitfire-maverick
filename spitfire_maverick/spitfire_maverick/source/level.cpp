#include "../header/main.h"
#include "../header/level.h"

Level::Level(u16 levelWidth,u16 levelHeight,string *levelTitle,vector<u16>* heightMap){
	this->levelWidth = levelWidth;
	this->levelHeight = levelHeight;
	this->levelTitle=levelTitle;
	this->heightMap=heightMap;
}

Level::~Level(){
	delete heightMap;
	delete levelTitle;
}
