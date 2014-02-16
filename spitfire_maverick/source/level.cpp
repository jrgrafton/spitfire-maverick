#include "../header/main.h"
#include "../header/level.h"

Level::Level(u16 levelWidth,u16 levelHeight,string *levelTitle,vector<s16>* heightMap){
	this->levelWidth = levelWidth;
	this->levelHeight = levelHeight;
	this->levelTitle=levelTitle;
	this->heightMap=heightMap;
}

Level::~Level(){
	delete heightMap;
	delete levelTitle;
}
string* Level::getTitle(){
	return this->levelTitle;
}
vector<s16>* Level::getHeightMap(){
	return this->heightMap;
}
u16 Level::getLevelWidth(){
	return this->levelWidth;
}
u16 Level::getLevelHeight(){
	return this->levelHeight;
}