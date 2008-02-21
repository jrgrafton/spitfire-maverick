//internal headers
#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/plane_object.h"
#include "../header/level.h"
#include "../header/state.h"
#include "../header/in_game.h"

//Sound headers
#include "fiftycal1_sfx.h"
#include "fiftycal2_sfx.h"
#include "fiftycal3_sfx.h"
#include "fiftycal4_sfx.h"
#include "crash_sfx.h"
#include "plane_engine_sfx.h"
#include "player_bomb_sfx.h"
#include "player_bombhit_sfx.h"

//Sound stuff
void* player_gun_sfx[4];
s32 player_gun_sfx_size[4];
u16 engine_channel=0;

//Need to think of a way to map collisions between gameobjects + gameobjects + scenery to sounds


//Are we in game?
u16 inGame = 0;

//Level stuff
Level* currentLevel;

//Game objects representing active bullets
vector<GameObject*> projectiles;

//Sprite pool of available indexes
vector<u16> spritePool;

//vector of indexes taken by landscape
vector<u16> landscapeIndexs;

//View port
s32 viewportx =0;
s32 viewporty =0;

//Grass tiles..13 different possible height differences (0--+2-->24)
//Each tile can be up to 3 deep plus one extra value needed for length of sub array
u16 grassTiles[13][4];
u16 blackTile;

//Track background scrolling
s32 bgscroll=0;

//Represents bottom middle of the plane
PlaneObject* plane;
u16 sqrtLUT[LUTSIZE];	//Need LUT for calc speed from vx and vy

/**
**InGame constructor
**/
InGame::InGame() : State(){
	inGame=1;
	init();
	myName = INGAME;
}

/**
**InGame constructor
**/
InGame::~InGame(){
	delete currentLevel;
	delete plane;
}

inline s16 InGame::getViewPortX(){
	return viewportx>>8;
}
inline s16 InGame::getViewPortY(){
	return viewporty>>8;
}
/**
**InGame init allocate resources
**/
void InGame::init(){
	myName = INGAME;
	
	//Init lookup table
	for(u32 i =0;i<LUTSIZE;i++){
		sqrtLUT[i] = (u16)sqrt(i);
	}
	
	//Load game objects
	//Startx starty width height spriteindex startangle speed fire delay
	//x and y are always top left of sprite
	plane = new PlaneObject(128<<8,50<<8,32,8,0,0,1<<8,9);

	//Load palattes
	PA_LoadSpritePal(0, 0, (void*)plane_image_Pal);
	PA_LoadSpritePal(0,1,(void*)grass_image_Pal);

	//Load plane sprite
	PA_CreateSprite(0, plane->spriteIndex, (void*)plane_image_Sprite,OBJ_SIZE_32X32, 1, 0, plane->getX()-16, plane->getY()-32);

	//Enable rotation for plane
	PA_SetSpriteRotEnable(0,0,0);

	//Populate sprite index pool
	//0 reserved for plane
	for(u16 i=1;i<128;i++){
		spritePool.push_back(i);
	}
	
	//Init graphics
	initGraphics();

	//Init level
	initLevel();

	//Init sound
	initSound();

	//Reset everything
	reset();
}

void InGame::initGraphics(){
	#ifdef DEBUG
		PA_InitText(1,0); // On the top screen
		PA_SetTextCol(1,31,31,31);
	#endif
	PA_SetBgPalCol(0, 2, PA_RGB(31, 0, 0));
	PA_SetBgPalCol(0, 3, PA_RGB(0, 31, 0));
	PA_SetBgPalCol(0, 4, PA_RGB(31, 31, 31));
	PA_SetBgPalCol(0, 5, PA_RGB(19, 19, 5));

	//Init background
	PA_EasyBgLoad(0, 2, background_image);
}

void InGame::initLevel(){
	blackTile = PA_CreateGfx(0, (void*)black_image_Sprite, OBJ_SIZE_16X32, 1);
	//0 diff
	grassTiles[0][0] = 2;
	grassTiles[0][1] = PA_CreateGfx(0, (void*)grass0_image_Sprite, OBJ_SIZE_16X32, 1);
	//2diff
	grassTiles[1][0] = 2;
	grassTiles[1][1] = PA_CreateGfx(0, (void*)grass2a_image_Sprite, OBJ_SIZE_16X32, 1);
	//4diff
	grassTiles[2][0] = 2;
	grassTiles[2][1] = PA_CreateGfx(0, (void*)grass4a_image_Sprite, OBJ_SIZE_16X32, 1);
	//6diff
	grassTiles[3][0] = 2;
	grassTiles[3][1] = PA_CreateGfx(0, (void*)grass6a_image_Sprite, OBJ_SIZE_16X32, 1);
	//8diff
	grassTiles[4][0] = 2;
	grassTiles[4][1] = PA_CreateGfx(0, (void*)grass8a_image_Sprite, OBJ_SIZE_16X32, 1);
	//10diff
	grassTiles[5][0] = 2;
	grassTiles[5][1] = PA_CreateGfx(0, (void*)grass10a_image_Sprite, OBJ_SIZE_16X32, 1);
	//12diff
	grassTiles[6][0] = 2;
	grassTiles[6][1] = PA_CreateGfx(0, (void*)grass12a_image_Sprite, OBJ_SIZE_16X32, 1);
	//14diff
	grassTiles[7][0] = 2;
	grassTiles[7][1] = PA_CreateGfx(0, (void*)grass14a_image_Sprite, OBJ_SIZE_16X32, 1);
	//16diff
	grassTiles[8][0] = 2;
	grassTiles[8][1] = PA_CreateGfx(0, (void*)grass16a_image_Sprite, OBJ_SIZE_16X32, 1);
	//18diff
	grassTiles[9][0] = 3;
	grassTiles[9][1] = PA_CreateGfx(0, (void*)grass18a_image_Sprite, OBJ_SIZE_16X32, 1);
	grassTiles[9][2] = PA_CreateGfx(0, (void*)grass18b_image_Sprite, OBJ_SIZE_16X32, 1);
	//20diff
	grassTiles[10][0] = 3;
	grassTiles[10][1] = PA_CreateGfx(0, (void*)grass20a_image_Sprite, OBJ_SIZE_16X32, 1);
	grassTiles[10][2] = PA_CreateGfx(0, (void*)grass20b_image_Sprite, OBJ_SIZE_16X32, 1);
	//22diff
	grassTiles[11][0] = 3;
	grassTiles[11][1] = PA_CreateGfx(0, (void*)grass22a_image_Sprite, OBJ_SIZE_16X32, 1);
	grassTiles[11][2] = PA_CreateGfx(0, (void*)grass22b_image_Sprite, OBJ_SIZE_16X32, 1);
	//24diff
	grassTiles[12][0] = 3;
	grassTiles[12][1] = PA_CreateGfx(0, (void*)grass24a_image_Sprite, OBJ_SIZE_16X32, 1);
	grassTiles[12][2] = PA_CreateGfx(0, (void*)grass24b_image_Sprite, OBJ_SIZE_16X32, 1);

	#ifndef EMULATOR 
		FILE* testRead = fopen ("/development/spitfire_maverick/test.txt", "rb"); //rb = read
		
		char fileLine[100]; 
		fgets(fileLine, 100,testRead);
		string* levelTitle = new string(fileLine);
		vector<u16>* heightMap = new vector<u16>();

		while(!feof(testRead)){
			fgets(fileLine, 100,testRead);
			processHeightMap(fileLine);
		}
		fclose(testRead);
	#else
		string* levelTitle = new string("test level");
		vector<u16>* heightMap = new vector<u16>();
		heightMap->push_back(SHEIGHT-20);
		heightMap->push_back(SHEIGHT-24);
		heightMap->push_back(SHEIGHT-30);
		heightMap->push_back(SHEIGHT-34);
		heightMap->push_back(SHEIGHT-56);
		heightMap->push_back(SHEIGHT-78);
		heightMap->push_back(SHEIGHT-90);
		heightMap->push_back(SHEIGHT-100);
		heightMap->push_back(SHEIGHT-96);
		heightMap->push_back(SHEIGHT-80);
		heightMap->push_back(SHEIGHT-78);
		heightMap->push_back(SHEIGHT-60);
		heightMap->push_back(SHEIGHT-54);
		heightMap->push_back(SHEIGHT-56);
		heightMap->push_back(SHEIGHT-58);
		heightMap->push_back(SHEIGHT-50);
		heightMap->push_back(SHEIGHT-44);
		heightMap->push_back(SHEIGHT-42);
		heightMap->push_back(SHEIGHT-40);
		heightMap->push_back(SHEIGHT-20);
		heightMap->push_back(SHEIGHT-20);
		heightMap->push_back(SHEIGHT-24);
		heightMap->push_back(SHEIGHT-30);
		heightMap->push_back(SHEIGHT-34);
		heightMap->push_back(SHEIGHT-56);
		heightMap->push_back(SHEIGHT-78);
		heightMap->push_back(SHEIGHT-90);
		heightMap->push_back(SHEIGHT-100);
		heightMap->push_back(SHEIGHT-96);
		heightMap->push_back(SHEIGHT-80);
		heightMap->push_back(SHEIGHT-78);
		heightMap->push_back(SHEIGHT-60);
		heightMap->push_back(SHEIGHT-54);
		heightMap->push_back(SHEIGHT-56);
		heightMap->push_back(SHEIGHT-58);
		heightMap->push_back(SHEIGHT-50);
		heightMap->push_back(SHEIGHT-44);
		heightMap->push_back(SHEIGHT-42);
		heightMap->push_back(SHEIGHT-40);
		heightMap->push_back(SHEIGHT-20);
		heightMap->push_back(SHEIGHT-20);
		heightMap->push_back(SHEIGHT-24);
		heightMap->push_back(SHEIGHT-30);
		heightMap->push_back(SHEIGHT-34);
		heightMap->push_back(SHEIGHT-56);
		heightMap->push_back(SHEIGHT-78);
		heightMap->push_back(SHEIGHT-90);
		heightMap->push_back(SHEIGHT-100);
		heightMap->push_back(SHEIGHT-96);
		heightMap->push_back(SHEIGHT-80);
		heightMap->push_back(SHEIGHT-78);
		heightMap->push_back(SHEIGHT-60);
		heightMap->push_back(SHEIGHT-54);
		heightMap->push_back(SHEIGHT-56);
		heightMap->push_back(SHEIGHT-58);
		heightMap->push_back(SHEIGHT-50);
		heightMap->push_back(SHEIGHT-44);
		heightMap->push_back(SHEIGHT-42);
		heightMap->push_back(SHEIGHT-40);
		heightMap->push_back(SHEIGHT-20);
		heightMap->push_back(SHEIGHT-20);
		heightMap->push_back(SHEIGHT-24);
		heightMap->push_back(SHEIGHT-30);
		heightMap->push_back(SHEIGHT-34);
		heightMap->push_back(SHEIGHT-56);
		heightMap->push_back(SHEIGHT-78);
		heightMap->push_back(SHEIGHT-90);
		heightMap->push_back(SHEIGHT-100);
		heightMap->push_back(SHEIGHT-96);
		heightMap->push_back(SHEIGHT-80);
		heightMap->push_back(SHEIGHT-78);
		heightMap->push_back(SHEIGHT-60);
		heightMap->push_back(SHEIGHT-54);
		heightMap->push_back(SHEIGHT-56);
		heightMap->push_back(SHEIGHT-58);
		heightMap->push_back(SHEIGHT-50);
		heightMap->push_back(SHEIGHT-44);
		heightMap->push_back(SHEIGHT-42);
		heightMap->push_back(SHEIGHT-40);
		heightMap->push_back(SHEIGHT-20);
		heightMap->push_back(SHEIGHT-20);
		heightMap->push_back(SHEIGHT-24);
		heightMap->push_back(SHEIGHT-30);
		heightMap->push_back(SHEIGHT-34);
		heightMap->push_back(SHEIGHT-56);
		heightMap->push_back(SHEIGHT-78);
		heightMap->push_back(SHEIGHT-90);
		heightMap->push_back(SHEIGHT-100);
		heightMap->push_back(SHEIGHT-96);
		heightMap->push_back(SHEIGHT-80);
		heightMap->push_back(SHEIGHT-78);
		heightMap->push_back(SHEIGHT-60);
		heightMap->push_back(SHEIGHT-54);
		heightMap->push_back(SHEIGHT-56);
		heightMap->push_back(SHEIGHT-58);
		heightMap->push_back(SHEIGHT-50);
		heightMap->push_back(SHEIGHT-44);
		heightMap->push_back(SHEIGHT-42);
		heightMap->push_back(SHEIGHT-40);
		heightMap->push_back(SHEIGHT-20);
		heightMap->push_back(SHEIGHT-44);
		heightMap->push_back(SHEIGHT-10);
	#endif

	u16 levelWidth = (heightMap->size()-2)*16;
	u16 levelHeight = (u16)(SHEIGHT*1.5);
	
	//Init level with full heightmap
	currentLevel = new Level(levelWidth,levelHeight,levelTitle,heightMap);

}

void InGame::initSound(){
	player_gun_sfx[0]=(void*)fiftycal1_sfx;
	player_gun_sfx[1]=(void*)fiftycal2_sfx;
	player_gun_sfx[2]=(void*)fiftycal3_sfx;
	player_gun_sfx[3]=(void*)fiftycal4_sfx;

	player_gun_sfx_size[0]=(s32)fiftycal1_sfx_size;
	player_gun_sfx_size[1]=(s32)fiftycal2_sfx_size;
	player_gun_sfx_size[2]=(s32)fiftycal3_sfx_size;
	player_gun_sfx_size[3]=(s32)fiftycal4_sfx_size;

	PA_PlaySoundEx2 (0,plane_engine_sfx,(s32)plane_engine_sfx_size,plane->speed/4-49,44100,0,true,0);
}
/**
**Process height map
*/
void InGame::processHeightMap(char* fileLine,vector<u16>* heightMap){
	string* line = new string(fileLine);
	while(addNextHeight(line,heightMap));
	delete line;
}
int InGame::addNextHeight(string* line,vector<u16>* heightMap){
	u32 index = line->find_first_of(",");
	if(index==string::npos){
		heightMap->push_back(SHEIGHT-atoi(line->substr(0).c_str()));
		return 0;
	}
	else{
		heightMap->push_back(SHEIGHT-atoi(line->substr(0,index+1).c_str()));
		line->erase(0,index+1);
		return 1;
	}
}

/**
**InGame reset whole state
**/
void InGame::reset(){
	
}
/**
**InGame run
**/
void InGame::run(){
	while(inGame)
	{
		//Process input
		processInput();
		
		//Do updates
		doUpdates();

		//Do collisions
		doCollisions();

		//Do drawing
		doDrawing();
		
		//Print debug stuff (if in debug mode)
		#ifdef DEBUG
			print_debug();
		#endif

		//Wait for vsync
		PA_WaitForVBL();
	}
}
/**
Process input function
**/
void InGame::processInput(void){
	if(Pad.Held.A){
		if(plane->timeSinceFired>plane->fireDelay){plane->timeSinceFired=0;addPlayerBullet();}
	}
	if(Pad.Newpress.B){
		addPlayerBomb();
	}
	if(Pad.Held.Up){
		plane->speed+=4;
	}
	if(Pad.Held.Left){
		plane->angle+=(s32)(plane->speed*1.1);
		if(plane->getAngle()>511){plane->angle=0;}
	}
	if(Pad.Held.Right){
		plane->angle-=(s32)(plane->speed*1.1);
		if(plane->getAngle()<0){plane->angle=511<<8;}
	}
}

void InGame::addPlayerBullet(){
	u16 soundIndex = PA_RandMax(3);
	void* sound = player_gun_sfx[soundIndex];
	s32 sound_size = player_gun_sfx_size[soundIndex];
	PA_PlaySound(PA_GetFreeSoundChannel(),sound,sound_size,127,44100);

	u16 planeRadius = plane->width/2;

	s32 startx = plane->x+((planeRadius)<<8)+(plane->vx*(planeRadius+4));
	s32 starty = plane->y+((planeRadius)<<8)+(plane->vy*(planeRadius+4));
	
	u16 width =2;
	u16 height =1;
	s16 spriteIndex =-1;
	s32 angle = plane->angle;
	u32 speed = plane->speed+1000;
	
	GameObject* bullet = new GameObject(new string("player_bullet"),startx,starty,width,height,spriteIndex,angle,speed);
	bullet->vy +=PA_RandMax(60);
	projectiles.push_back(bullet);
}

void InGame::addPlayerBomb(){
	PA_PlaySound(PA_GetFreeSoundChannel(),player_bomb_sfx,(u32)player_bomb_sfx_size,127,44100);

	//First get the bottom middle
	s16 currentAngle = plane->getAngle();
	s16 normalAdjust = (currentAngle<128||currentAngle>384)? -128:128;

	s16 normalAngle = currentAngle+normalAdjust;

	if(normalAngle<0)normalAngle+=512;
	if(normalAngle>511)normalAngle &= 512;

	s16 startx = ((PA_Cos(normalAngle) * (plane->height/2))>>8)+plane->getX()+16;
	s16 starty = ((-PA_Sin(normalAngle) * (plane->height/2))>>8)+plane->getY()+16;
	
	u16 width =4;
	u16 height =1;
	s16 spriteIndex =-1;
	s32 angle = plane->angle;
	u32 speed = plane->speed;
	
	GameObject* bomb = new GameObject(new string("player_bomb"),startx<<8,starty<<8,width,height,spriteIndex,angle,speed);
	projectiles.push_back(bomb);
}

/**
** Update plane location and viewport
**/
void InGame::doUpdates(){

	//Update Plane
	updatePlane();
	
	//Update projectiles
	updateProjectiles();

	//Update viewport
	updateViewport();
}

void InGame::updateViewport(){
	u16 angle = plane->getAngle();
	s16 xflipped = (angle>128&&angle<384)? -1:1;
	
	//Viewport calculations using simulated float accuracy
	viewportx = plane->x+(16<<8)-((SWIDTH/2)<<8)+((((plane->speed-(MINSPEED-50))*abs(plane->vx))/5)*xflipped);
	viewporty = plane->y-((SHEIGHT/2)<<8);

	if(getViewPortX()<0){viewportx=0;}
	if(getViewPortX()+SWIDTH>currentLevel->levelWidth){viewportx=((currentLevel->levelWidth-SWIDTH)<<8);}

	if(getViewPortY()<SHEIGHT-currentLevel->levelHeight){viewporty=((SHEIGHT-currentLevel->levelHeight)<<8);}
	if(getViewPortY()>0){viewporty=0;}
	
	//Scroll background
	if(getViewPortX()>0&&getViewPortX()+SWIDTH<currentLevel->levelWidth){
		bgscroll+=(((abs(plane->vx)*(plane->speed)>>8))/7*xflipped);
		PA_BGScrollX(0,2,bgscroll>>8);
	}
}
void InGame::updatePlane(){
	//Update engine sound
	PA_SetSoundChannelVol(0,plane->speed/4-49);

	u16 angle = plane->getAngle();

	plane->vx = PA_Cos(angle);
	plane->vy = -PA_Sin(angle);

	s16 xflipped = (angle>128&&angle<384)? -1:1;
	s16 yflipped = (angle<256)? -1:1;
	
	s32 pSpeed = plane->speed;
	//Apply gravity to plane ...OMG LUT so slow on emulator
	s16 diff = getSpeedFromVelocity(plane->vx,plane->vy) - getSpeedFromVelocity(plane->vx,(plane->vy+(GRAVITY*8)));
	pSpeed-=diff/3;

	//Subtract friction from plane speed
	pSpeed -= FRICTION;
	pSpeed =(pSpeed>MAXSPEED)? MAXSPEED:(pSpeed<MINSPEED)? MINSPEED:pSpeed;
	plane->speed=pSpeed;		//After applying gravity and friction reload the var back into plane object

	plane->x += ((abs(plane->vx)*plane->speed)>>8)*xflipped;
	plane->y += ((abs(plane->vy)*plane->speed)>>8)*yflipped;

	//If the plane is going to be completely rendered above the y threshold reflect its current angle
	if(plane->getY()<(SHEIGHT-currentLevel->levelHeight)-50){
		u16 currentAngle = plane->getAngle();
		s16 diff = 128 - currentAngle;		//Get diff from Y axis
		u16 reflected = 384+diff;			//And reflect :)
		plane->angle=reflected<<8;
	}

	//If the plane is going to be completely off the x axis the reflect its current angle
	if(plane->getX()+32<-5){
		u16 currentAngle = plane->getAngle();
		s16 diff = 256 - currentAngle;		//Get diff from X axis
		s16 reflected = 0+diff;			//And reflect :)
		if(reflected<0)reflected+=512;
		plane->angle=reflected<<8;
	}

	else if(plane->getX()>currentLevel->levelWidth+5){
		u16 currentAngle = plane->getAngle();
		s16 diff = 0 - currentAngle;		//Get diff from Y axis
		s16 reflected = 256+diff;			//And reflect :)
		if(reflected<0)reflected+=512;
		plane->angle=reflected<<8;
	}

	//Rotate plane
	PA_SetRotset(0, 0, angle,256,256);
}

u16 InGame::getSpeedFromVelocity(s16 vx,s16 vy){	
	u32 index = squared(vx)+squared(vy);
	return sqrtLUT[index];
}

void InGame::updateProjectiles(){
	//Update bullets first
	plane->timeSinceFired++; //Increase time since last fired

	vector<GameObject*>::iterator it;
	it = projectiles.begin();

	while( it != projectiles.end()) {
		GameObject* projectile = (GameObject*)(*it);
		s16 angle = projectile->getAngle();
		
		projectile->vy+=GRAVITY;

		//All this flipped stuff is done since >> negative numbers does not work the same as >> positive ones
		s16 xflipped = (angle>128&&angle<384)? -1:1;
		s16 yflipped = (projectile->vy<0)? -1:1;

		projectile->x += ((abs(projectile->vx)*projectile->speed)>>8)*xflipped;
		projectile->y += ((abs(projectile->vy)*projectile->speed)>>8)*yflipped;

		s16 bx = projectile->getX();
		s16 by = projectile->getY();
		
		u16 hitLandscape = landscapeCollision(bx,by);
		if(bx>currentLevel->levelWidth||bx<0||by<0-(currentLevel->levelHeight-SHEIGHT)||hitLandscape){
			//This to be replace with hashtable lookup for sound (char* --> void*)
			if(hitLandscape&&strcmp(projectile->name->c_str(),"player_bomb")==0){
				PA_PlaySound(PA_GetFreeSoundChannel(),player_bombhit_sfx,(u32)player_bombhit_sfx_size,127,44100);
			}
			it=projectiles.erase(it);
		}
		else{
			it++;
		}
	}
	
}
/**
Do drawing function
**/
void InGame::doDrawing(void){	
	//Linear interpolation debug
	//PA_Draw8bitLineEx(0, tipxFront-getViewPortX(), 0, tipxFront-getViewPortX(), SHEIGHT,2,1);
	//PA_Draw8bitLineEx(0, 0, tipyFront-getViewPortY(), SWIDTH,tipyFront-getViewPortY(),3,1);
	//PA_Draw8bitLineEx(0, tipxBack-getViewPortX(), 0, tipxBack-getViewPortX(), SHEIGHT,2,1);
	//PA_Draw8bitLineEx(0, 0, tipyBack-getViewPortY(), SWIDTH,tipyBack-getViewPortY(),3,1);
	//PA_Draw8bitLineEx(0, tipxFront-getViewPortX(), SHEIGHT, tipxFront-getViewPortX(), heightFront-getViewPortY(),4,1);
	//PA_Draw8bitLineEx(0, tipxBack-getViewPortX(), SHEIGHT, tipxBack-getViewPortX(), heightBack-getViewPortY(),4,1);

	//Draw plane
	drawPlane();

	//Render landscape
	drawLandscape();

	//Render projectiles
	drawProjectiles();
}
void InGame::drawPlane(){
	s32 planex =((plane->x-viewportx)>>8);
	s32 planey =((plane->y-viewporty)>>8);
	PA_SetSpriteXY(0,plane->spriteIndex, planex, planey);
}
void InGame::drawProjectiles(){
	PA_Clear8bitBg(0);

	//Just drawing bullets atm
	vector<GameObject*>::iterator it;
	it = projectiles.begin();
	
	s32 offsetx = (-getViewPortX());
	s32 offsety = (-getViewPortY());

	while( it != projectiles.end()) {
		GameObject* projectile = (GameObject*)(*it);
		s32 bx = projectile->getX();
		s32 by = projectile->getY();
		s16 vx = projectile->vx;
		s16 vy = projectile->vy;

		s32 screenxEnd =  bx+offsetx;
		s32 screenyEnd =  by+offsety;
		if(screenxEnd<0||screenxEnd>SWIDTH){it++;continue;}
		if(screenyEnd<0||screenyEnd>SHEIGHT){it++;continue;}
		
		s16 screenxStart = bx + (((-vx)*3)>>8)+offsetx;
		s16 screenyStart = by + (((-vy)*3)>>8)+offsety;
		
		if(projectile->spriteIndex==-1){
			PA_Draw8bitLineEx (0,screenxStart,screenyStart,screenxEnd,screenyEnd,5,projectile->width);		
		}
		it++;
	}
}
void InGame::drawLandscape(){
	
	//Reset old landscape before rendering new one
	resetLandscape();

	vector<u16>::iterator it;
	it = currentLevel->heightMap->begin();


	s16 x =0-getViewPortX();
	while( it != currentLevel->heightMap->end()-1) {
		u16 thisHeight = (*it);
		u16 nextHeight = (*(it+1));
		u8 spriteIndex = abs(thisHeight-nextHeight)/2;
		u16 y = taller(thisHeight,nextHeight);		   //Get height
		y-=getViewPortY();
		
		if(x>-16&&x<256&&y<SHEIGHT){		//Draw if tile on screen
			u16 flipped = (thisHeight>nextHeight)?0:1;	   //Get flipped
			u16 length = grassTiles[spriteIndex][0];
			//Paste rest of sprite
			for(u16 i =1;i<length;i++){
				u16 availableIndex = spritePool.back();
				spritePool.pop_back();					//Get available index
				//Load sprite to screen
				PA_CreateSpriteFromGfx(0,availableIndex,grassTiles[spriteIndex][i],OBJ_SIZE_16X32, 1, 1, x,y);
				PA_SetSpriteHflip(0, availableIndex, flipped); // (screen, sprite, flip(1)/unflip(0)) HFlip -> Horizontal flip
				//Add index to list used by landscape
				landscapeIndexs.push_back(availableIndex);
				//PA_SetSpritePrio(0,availableIndex,1);
				y+=32;
				if(y>SHEIGHT)break;
			}
			//Paste black sprite
			while(y<=SHEIGHT){
				u16 availableIndex = spritePool.back();
				spritePool.pop_back();					//Get available index
				PA_CreateSpriteFromGfx(0,availableIndex,blackTile,OBJ_SIZE_16X32, 1, 1, x,y);
				landscapeIndexs.push_back(availableIndex);
				//PA_SetSpritePrio(0,availableIndex,1);
				y+=32;
			}
		}
		x+=16;
		it++;
	}
}

/**
** Frees all indexes currently held by landscape and returns them to sprite
** pool
**/ 
void InGame::resetLandscape(){
	//Hide all current sprite indexes and re-add them to the pool
	while(!landscapeIndexs.empty()){
		u16 value = landscapeIndexs.back();
		landscapeIndexs.pop_back();
		PA_SetSpriteY(0, value, 193);	// Hide sprite
		spritePool.push_back(value);	///Re add sprite to pool
	}
}

/**
** inline taller function
**/
u16 InGame::taller(u16 a,u16 b){
	return(a<b)? a:b;
}

/**
** inline smaller function
**/
u16 InGame::smaller(u16 a,u16 b){
	return(a>b)? a:b;
}

/**
Do collisions function
**/
void InGame::doCollisions(){
	//Plane landscape collision
	planeLandscapeCollision();
}

void InGame::planeLandscapeCollision(){
	//First get the bottom middle
	s16 currentAngle = plane->getAngle();
	s16 normalAdjust = (currentAngle<128||currentAngle>384)? -128:128;

	s16 normalAngle = currentAngle+normalAdjust;

	if(normalAngle<0)normalAngle+=512;
	if(normalAngle>511)normalAngle &= 512;

	s16 bottomx = ((PA_Cos(normalAngle) * (plane->height/2))>>8)+plane->getX()+16;
	s16 bottomy = ((-PA_Sin(normalAngle) * (plane->height/2))>>8)+plane->getY()+16;
	
	//Now extend from bottom middle half the length of the plane along the curent vector
	s16 tipxFront = bottomx+((plane->vx*16)>>8);
	s16 tipyFront = bottomy+((plane->vy*16)>>8);
	s16 tipxBack = bottomx+((plane->vx*-16)>>8);
	s16 tipyBack = bottomy+((plane->vy*-16)>>8);
	
	//Now collision tests
	if(landscapeCollision(tipxFront,tipyFront))planeCrash();
	if(landscapeCollision(tipxBack,tipyBack))planeCrash();
}

int InGame::landscapeCollision(s16 x, s16 y){
	s16 landIndex = (x/16);
	if((u16)landIndex>currentLevel->heightMap->size()-2||landIndex<0){return 0;}
	u32 a = currentLevel->heightMap->at(landIndex)<<8;
	u32 b = currentLevel->heightMap->at(landIndex+1)<<8;
	
	

	u32 diff = ((x &15)<<8)/16;
	u16 actualHeight = (a+(diff*((b-a)>>8)))>>8;
	return(y>actualHeight);
}
/**
**
**/
void InGame::planeCrash(){
	PA_PlaySound(PA_GetFreeSoundChannel(),crash_sfx,(u32)crash_sfx_size,127,44100);
	PA_SetSpriteY(0, 0, 193);	// Hide sprite
	delete plane;
	plane = new PlaneObject(128<<8,60<<8,32,8,0,0,1<<8,9);
}
/**
** Inline squared function
**/
inline u32 InGame::squared(s32 a){ return a*a;}

/**
Debug function, put everything here that you
wanna print to screen
**/
void InGame::print_debug(void){
	//Put your debug print statements here.... make sure to print to screen 1
	
	PA_ClearTextBg(1);
	PA_OutputText(1,0, 0, "Title is: %s", currentLevel->levelTitle->c_str());
	PA_OutputText(1,0, 1, "Viewport x:%d (%d) y:%d (%d)", getViewPortX(),viewportx,getViewPortY(),viewporty);
	PA_OutputText(1,0, 2, "Plane x:%d (%d) y:%d (%d)", plane->getX(),plane->x,plane->getY(),plane->y);
	PA_OutputText(1,0, 3, "Plane vx:%d vy:%d", plane->vx,plane->vy);
	PA_OutputText(1,0, 4, "Plane speed:%d (%d)", plane->getSpeed(),plane->speed);
	PA_OutputText(1,0, 5, "Plane angle:%d (%d)", plane->getAngle(),plane->angle);
	PA_OutputText(1,0, 6, "Landscape sprites used:%d", landscapeIndexs.size());
	PA_OutputText(1,0, 7, "Available sprites:%d", spritePool.size());
	PA_OutputText(1,0, 8, "Active projectiles:%d", projectiles.size());
	PA_OutputText(1,0, 9, "Time since last fired:%d", plane->timeSinceFired);
	PA_OutputText(1,0, 10, "Speed would be:%d", getSpeedFromVelocity(plane->vx,(plane->vy+(GRAVITY*20))));
}
