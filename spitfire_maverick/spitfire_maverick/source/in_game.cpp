//internal headers
#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/sprite_info.h"
#include "../header/plane_object.h"
#include "../header/particle_object.h"
#include "../header/destructable_object.h"
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

//Game objects representing active projectiles
vector<GameObject*> projectiles;

//Particle objects
vector<ParticleObject*> particles;
u16 planePartgfx;					//Store ref to particle gfx for when we create them

//Sprite pool of available indexes
vector<u16> spritePool;
vector<u16> rotPool; //Pool of available rotsets

//vector of indexes taken by landscape tiles
vector<u16> landscapeIndexs;

//Landscape Object stuff
DestructableObject* landscapeReferenceObjects[6];
vector<DestructableObject*> landscapeObjects;

//View port
s32 viewportx =0;
s32 viewporty =0;

//Grass tiles..13 different possible height differences (0--+2-->24)
//Each tile can be up to 3 deep plus one extra value needed for length of sub array
u16 grassTiles[13][4];
u16 blackTile;

//Track background scrolling
s32 bgscroll=0;

//Represents player plane
PlaneObject* player;

//Represents runway (needs to be part of level object)
u16 runwayStart;
u16 runwayEnd;
u16 runwayHeight;
vector<GameObject*> runwayObjects;

//Particle debug
s16 overlapTest =0;
s16 normalTest =0;
s16 reflectTest = 0;

//Do lookups if final game runs too slow!
#ifndef DEBUG
	u16 sqrtLUT[LUTSIZE];	//Need LUT for calc speed from vx and vy
#endif

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
	delete player;
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
	#ifndef DEBUG
	for(u32 i =0;i<LUTSIZE;i++){
		sqrtLUT[i] = (u16)sqrt(i);
	}
	#endif

	//Populate sprite index pool
	//0 reserved for plane
	for(u16 i=1;i<128;i++){
		spritePool.push_back(i);
	}
	//Populate rot index pool
	//0 reserved for plane
	for(u16 i=1;i<32;i++){
		rotPool.push_back(i);
	}
	
	//Init graphics
	initGraphics();

	//Init level
	initLevel();

	//Init plane
	initPlane();

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

	//Load palattes
	PA_LoadSpritePal(0,0,(void*)spitfire_image_Pal);
	PA_LoadSpritePal(0,1,(void*)grass_image_Pal);
	PA_LoadSpritePal(0,2,(void*)runway_image_Pal);
	PA_LoadSpritePal(0,3,(void*)particles_image_Pal);
	PA_LoadSpritePal(0,4,(void*)landscape_allobjects_Pal);

	//Init gfx ref for particles
	planePartgfx = PA_CreateGfx(0, (void*)plane_piece_particles_image_Sprite, OBJ_SIZE_8X8, 1);

	//Init background
	PA_SetBgPrio(0,0,0);
	PA_EasyBgLoad(0, 2, background_image);
	PA_SetBgPrio(0,2,3);
	PA_InitSpriteExtPrio(1); // Enable extended priorities

	//Init landscape lookup
	initLanscapeLookup();
}
void InGame::addLandscapeObject(s32 x,u16 ref){
	DestructableObject* object = new DestructableObject(*landscapeReferenceObjects[ref]);
	SpriteInfo* si = object->getSpriteInfo();
	s32 y = getHeightAtPoint(x+si->getSpriteWidth()/2);
	s32 offset = object->getObjectHeight();
	object->setLocation(x<<8,(y-offset+4)<<8);
	landscapeObjects.push_back(object);
}

void InGame::initLanscapeLookup(){
	//Go through loading in all landscape objects for later lookup
	//Doesnt matter that all objects are pointing to same sources...since they are only being
	//Used for lookup purposes and copy constructor does deep copy!
	u16 temp8X8[] = {OBJ_SIZE_8X8};
	u16* objectsize8X8 = new u16[2];
	objectsize8X8[0] = temp8X8[0];
	objectsize8X8[1] = temp8X8[1];
	
	u16 temp32X32[] = {OBJ_SIZE_32X32};
	u16* objectsize32X32 = new u16[2];
	objectsize32X32[0] = temp32X32[0];
	objectsize32X32[1] = temp32X32[1];

	u16 temp32X64[] = {OBJ_SIZE_32X64};
	u16* objectsize32X64 = new u16[2];
	objectsize32X64[0] = temp32X64[0];
	objectsize32X64[1] = temp32X64[1];
	
	//Now create all gfx ref
	u16 particleGfx = PA_CreateGfx(0, (void*)plane_piece_particles_image_Sprite, OBJ_SIZE_8X8, 1);


	u16 treeOneGfx = PA_CreateGfx(0, (void*)tree1_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 treeTwoGfx = PA_CreateGfx(0, (void*)tree2_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 treeThreeGfx = PA_CreateGfx(0, (void*)tree3_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 treeFourGfx = PA_CreateGfx(0, (void*)tree4_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 towerAlliesGfx = PA_CreateGfx(0, (void*)tower_allies_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 towerGermanGfx = PA_CreateGfx(0, (void*)tower_german_image_Sprite, OBJ_SIZE_32X64, 1);
	
	//Now create all particle sprite info objects
	SpriteInfo* treeParticle = new SpriteInfo(8,8,0,particleGfx,-1,-1,3,objectsize8X8,256,0,false);
	SpriteInfo* towerParticle = new SpriteInfo(8,8,0,particleGfx,-1,-1,3,objectsize8X8,256,0,false);

	u16 destroyedTreeGfxRef=PA_CreateGfx(0, (void*)tree_destroyed_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 destroyedTowerGfxRef=PA_CreateGfx(0, (void*)tower_destroyed_image_Sprite, OBJ_SIZE_32X64, 1);
	
	//Now go onto create all landscape objects with their sprite info objects
	SpriteInfo* treeSpriteOneInfo = new SpriteInfo(32,32,0,treeOneGfx,-1,-1,4,objectsize32X32,256,0,false);
	DestructableObject* treeObjectOne = new DestructableObject(0,0,32,32,0,0,0,treeSpriteOneInfo,100,5,destroyedTreeGfxRef,treeParticle);
	
	SpriteInfo* treeSpriteTwoInfo = new SpriteInfo(32,32,0,treeTwoGfx,-1,-1,4,objectsize32X32,256,0,false);
	DestructableObject* treeObjectTwo = new DestructableObject(0,0,32,32,0,0,0,treeSpriteTwoInfo,100,5,destroyedTreeGfxRef,treeParticle);
	
	SpriteInfo* treeSpriteThreeInfo = new SpriteInfo(32,32,0,treeThreeGfx,-1,-1,4,objectsize32X32,256,0,false);
	DestructableObject* treeObjectThree = new DestructableObject(0,0,32,32,0,0,0,treeSpriteThreeInfo,100,5,destroyedTreeGfxRef,treeParticle);
	
	SpriteInfo* treeSpriteFourInfo = new SpriteInfo(32,32,0,treeFourGfx,-1,-1,4,objectsize32X32,256,0,false);
	DestructableObject* treeObjectFour = new DestructableObject(0,0,32,32,0,0,0,treeSpriteFourInfo,100,5,destroyedTreeGfxRef,treeParticle);
	
	SpriteInfo* towerAlliesSpriteInfo = new SpriteInfo(32,64,0,towerAlliesGfx,-1,-1,4,objectsize32X64,256,0,false);
	DestructableObject* towerAlliesObject = new DestructableObject(0,0,20,64,0,0,0,towerAlliesSpriteInfo,100,5,destroyedTowerGfxRef,towerParticle);
	
	SpriteInfo* towerGermanSpriteInfo = new SpriteInfo(32,64,0,towerGermanGfx,-1,-1,4,objectsize32X64,256,0,false);
	DestructableObject* towerGermanObject = new DestructableObject(0,0,20,64,0,0,0,towerGermanSpriteInfo,100,5,destroyedTowerGfxRef,towerParticle);

	//Now put all objects in an array to be looked up
	landscapeReferenceObjects[5] = treeObjectOne;
	landscapeReferenceObjects[4] = treeObjectTwo;
	landscapeReferenceObjects[3] = treeObjectThree;
	landscapeReferenceObjects[2] = treeObjectFour;
	landscapeReferenceObjects[1] = towerAlliesObject;
	landscapeReferenceObjects[0] = towerGermanObject;
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
		//Title
		string* levelTitle = new string("test level");

		//Height map
		vector<s16>* heightMap = new vector<s16>();
		heightMap->push_back(SHEIGHT-120);
		heightMap->push_back(SHEIGHT-110);
		heightMap->push_back(SHEIGHT-100);
		heightMap->push_back(SHEIGHT-90);
		heightMap->push_back(SHEIGHT-80);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);
		heightMap->push_back(SHEIGHT-70);

		heightMap->push_back(SHEIGHT-82);
		heightMap->push_back(SHEIGHT-88);
		heightMap->push_back(SHEIGHT-98);

		heightMap->push_back(SHEIGHT-108);
		heightMap->push_back(SHEIGHT-110);
		heightMap->push_back(SHEIGHT-110);


		heightMap->push_back(SHEIGHT-112);
		heightMap->push_back(SHEIGHT-122);
		heightMap->push_back(SHEIGHT-132);
		heightMap->push_back(SHEIGHT-144);
		heightMap->push_back(SHEIGHT-134);

		heightMap->push_back(SHEIGHT-124);
		heightMap->push_back(SHEIGHT-114);
		heightMap->push_back(SHEIGHT-104);
		heightMap->push_back(SHEIGHT-94);
		//Tree
		heightMap->push_back(SHEIGHT-92);
		//Tree
		heightMap->push_back(SHEIGHT-92);
		//Tree
		heightMap->push_back(SHEIGHT-98);
		heightMap->push_back(SHEIGHT-86);
		//Tree
		heightMap->push_back(SHEIGHT-100);
		//Tree
		heightMap->push_back(SHEIGHT-104);
		heightMap->push_back(SHEIGHT-106);
		heightMap->push_back(SHEIGHT-110);
		heightMap->push_back(SHEIGHT-110);
		heightMap->push_back(SHEIGHT-110);
		heightMap->push_back(SHEIGHT-110);
		heightMap->push_back(SHEIGHT-110);
		heightMap->push_back(SHEIGHT-110);
		heightMap->push_back(SHEIGHT-110);
		heightMap->push_back(SHEIGHT-110);
		heightMap->push_back(SHEIGHT-110);
		heightMap->push_back(SHEIGHT-124);
		heightMap->push_back(SHEIGHT-126);
		heightMap->push_back(SHEIGHT-136);
		heightMap->push_back(SHEIGHT-146);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);

		u16 levelWidth = (heightMap->size()-2)*16;
		u16 levelHeight = (u16)(SHEIGHT*2);
	
		//Init level with full heightmap
		currentLevel = new Level(levelWidth,levelHeight,levelTitle,heightMap);

		//Init runway
		initRunway();

		//Test landscape algorithm
		addLandscapeObject(850,0);
		addLandscapeObject(900,1);
		addLandscapeObject(950,2);
		addLandscapeObject(1000,3);
		addLandscapeObject(1050,4);
		addLandscapeObject(1100,5);

	#endif
}

void InGame::initRunway(){
	//Runway has to go after heightmap since needs to know heights at specific points
	runwayStart = 208;
	runwayEnd = runwayStart+RUNWAYLENGTH;
	s16 runwaygfx = PA_CreateGfx(0, (void*)runway_image_Sprite, OBJ_SIZE_64X32, 1);
	//Fill up runway vector with gameobects (need to do this to track used sprite indexes)
	for(u16 x = runwayStart;x<runwayEnd;x+=64){
		runwayHeight = getHeightAtPoint(x)-32; //32 is physical height of sprite
		
		u16 temp[] = {OBJ_SIZE_64X32};
		u16* objsize = new u16[2];
		objsize[0] = temp[0];
		objsize[1] = temp[1];
		
		SpriteInfo* so = new SpriteInfo(64,32,0,runwaygfx,-1,-1,2,objsize,256,0,false);
		GameObject * runwayPart = new GameObject(new string("runway"),x<<8,(runwayHeight+10)<<8,64,32,0,0,0,so);
		runwayObjects.push_back(runwayPart);
	}
	//Add the tower
	s16 towergfx = PA_CreateGfx(0, (void*)runway_tower_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 x = (runwayStart+(RUNWAYLENGTH/2)-8);
	u16 y = getHeightAtPoint(x)-64;
	
	u16 temp[] = {OBJ_SIZE_32X64};
	u16* objsize = new u16[2];
	objsize[0] = temp[0];
	objsize[1] = temp[1];
	
	SpriteInfo* so = new SpriteInfo(32,64,0,towergfx,-1,-1,2,objsize,256,0,false);
	GameObject * tower = new GameObject(new string("runway_tower"),x<<8,y<<8,32,64,0,0,0,so);
	runwayObjects.push_back(tower);
}
void InGame::initPlane(){
	//Load game objects
	//Startx starty width height spriteindex gfxindex startangle vx vy firedelay
	//x and y are always top left of sprite

	u16 plane_gfx = PA_CreateGfx(0, (void*)spitfire_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 temp[] = {OBJ_SIZE_32X32};
	u16* objsize = new u16[2];
	objsize[0] = temp[0];
	objsize[1] = temp[1];

	SpriteInfo* so = new SpriteInfo(32,32,0,plane_gfx,0,0,0,objsize,256,1,true);
	player = new PlaneObject(runwayStart<<8,(runwayHeight-4)<<8,16,8,0,0,0,9,60,so);

	//Load plane sprite
	PA_CreateSpriteFromGfx(0, so->getSpriteIndex(),so->getGfxRef(),OBJ_SIZE_32X32, 1, so->getPaletteIndex(), player->getX(), player->getY());

	//Enable rotation for plane
	PA_SetSpriteRotEnable(0,0,0);
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

	PA_PlaySoundEx2 (0,plane_engine_sfx,(s32)plane_engine_sfx_size,(player->speed<<8)/1109,44100,0,true,0);
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
		
		//Do updates...All collisions are handled in respective update functions
		doUpdates();

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
	if(Pad.Held.R&&!player->onRunway){
		if(player->canShoot()){player->timeSinceFired=0;addPlayerBullet();}
	}
	if(Pad.Newpress.L&&!player->onRunway){
		if(player->canBomb()){player->timeSinceBombed=0;addPlayerBomb();}
	}
	if(Pad.Held.B){
		player->throttleOn=1;
	}
	else{
		player->throttleOn=0;
	}
	if(Pad.Held.Left&&!player->onRunway){
		player->heading+=PLANETURNSPEED;
		if(player->getHeading()>(511<<8)){player->setHeading(0);}
	}
	if(Pad.Held.Right&&!player->onRunway){
		player->heading-=PLANETURNSPEED;
		if(player->getHeading()<0){player->setHeading(511<<8);}
	}
}

void InGame::addPlayerBullet(){
	u16 soundIndex = PA_RandMax(3);
	void* sound = player_gun_sfx[soundIndex];
	s32 sound_size = player_gun_sfx_size[soundIndex];
	PA_PlaySound(PA_GetFreeSoundChannel(),sound,sound_size,127,44100);

	SpriteInfo* planeSo = player->getSpriteInfo();
	
	player->totalAmmo--;

	s16 componentx = PA_Cos(player->getHeading()>>8);
	s16 componenty = -PA_Sin(player->getHeading()>>8);
	
	s32 cx = player->x+((planeSo->getSpriteWidth()/2)<<8);
	s32 cy = player->y+((planeSo->getSpriteHeight()/2)<<8);

	s32 startx = cx+(componentx*(player->getObjectWidth()/2));
	s32 starty = cy+(componenty*(player->getObjectWidth()/2));
	
	u16 width =1;
	u16 height =6;
	s32 heading = player->getHeading()>>8;

	s16 vx = (PA_Cos(heading)*(player->speed+1000)>>8);
	s16 vy = (-PA_Sin(heading)*(player->speed+1000)>>8)+PA_RandMax(64);
	
	u16* objsize = new u16[2];
	
	SpriteInfo* so = new SpriteInfo(-1,-1,0,-1,-1,-1,5,objsize,256,0,false);
	GameObject* bullet = new GameObject(new string("player_bullet"),startx,starty,width,height,heading,vx,vy,so);
	projectiles.push_back(bullet);
}

void InGame::addPlayerBomb(){
	PA_PlaySound(PA_GetFreeSoundChannel(),player_bomb_sfx,(u32)player_bomb_sfx_size,127,44100);
	player->totalBombs--;

	SpriteInfo* planeSo = player->getSpriteInfo();

	s32 cx = player->x+((planeSo->getSpriteWidth()/2)<<8);
	s32 cy = player->y+((planeSo->getSpriteHeight()/2)<<8);

	//First get the bottom middle
	s16 currentAngle = player->getHeading()>>8;
	s16 normalAdjust = (currentAngle<128||currentAngle>384)? -128:128;

	s16 normalAngle = currentAngle+normalAdjust;

	if(normalAngle<0)normalAngle+=512;
	if(normalAngle>511)normalAngle &= 512;

	s32 startx = cx+(PA_Cos(normalAngle) * (player->getObjectHeight()/2));
	s32 starty = cy+(-PA_Sin(normalAngle) * (player->getObjectHeight()/2));
	
	u16 width =2;
	u16 height =6;
	s32 heading = player->getHeading();
	
	u16* objsize = new u16[2];
	//Will have need to aquire a rotref when it uses an image...and gfx ref and palette
	
	//u16 width,u16 height,s32 angle,u16 gfxref,s16 rotIndex,s16 palette,u16* objsize,u16 zoom,u16 doubleSize bool usesRot
	SpriteInfo* so = new SpriteInfo(-1,-1,0,-1,-1,-1,5,objsize,256,0,false);
	GameObject* bomb = new GameObject(new string("player_bomb"),startx,starty,width,height,heading,player->vx,player->vy,so);
	projectiles.push_back(bomb);
}

/**
** Update plane location and viewport
**/
void InGame::doUpdates(){

	//Update Plane
	updatePlane();

	//Update viewport
	updateViewport();

	//Update projectiles
	updateProjectiles();

	//Update particles
	updateParticles();
}
void InGame::updatePlane(){
	//Update engine sound
	PA_SetSoundChannelVol(0,(player->speed<<8)/1109);
	
	//Increase time since fired/bombed
	player->timeSinceFired++; //Increase time since last fired
	player->timeSinceBombed++;
	
	//Make sure heading is tied to sprite rotation
	player->getSpriteInfo()->setAngle(player->getHeading());
	s16 heading = player->getHeading()>>8;

	//Check for takeoff
	if(player->onRunway&&((player->getX()+player->width>runwayEnd&&heading==0)||(player->getX()<runwayStart&&heading==256))&&player->takingOff==1){
		if(player->speed<MINPLANESPEED){planeCrash();return;}	//Crash if we have reached the end of runway and we are not fast enough
		player->onRunway=0;
		player->takingOff=0;
		if(heading==256){player->heading=246<<8;}
		else{player->setHeading(10<<8);}
	}

	s32 vy = player->vy;
	s32 vx = player->vx;
	s32 speed =0;
	
	if(!player->onRunway){
		if(player->totalFuel>0)player->totalFuel--;
		//PROPER ALGORITHM
		vy+=GRAVITY;
		speed = getSpeedFromVelocity(vx,vy);//Base speed calculated from vx and vy

		//Increase speed if holding up
		if(player->totalFuel>0)speed+=player->throttleOn*PLANEPOWER;
		
		//Take off friction
		speed*=FRICTION;
		speed=speed>>16;

		//Speed kept in certain range
		if(speed<MINPLANESPEED){speed=MINPLANESPEED;}
		if(speed>MAXPLANESPEED){speed=MAXPLANESPEED;}
	}
	else{
		//If we are on the runway the throttle is on and we are not yet taking off
		//Make sure we are! (taking off is 1 for yes 0 for in air and -1 for landing)
		if(player->onRunway&&player->throttleOn&&player->takingOff==0){
			player->takingOff=1;
		}
		speed = getSpeedFromVelocity(vx,vy);//Base speed calculated from vx and vy
		if(!player->throttleOn&&player->takingOff){player->throttleOn=1;} //Throttle always on when taking off
		speed+=player->throttleOn*PLANEPOWER*player->takingOff;
		
		//Check for end of runway
		if(player->takingOff==-1&&player->speed>0&&(player->getX()+player->width>runwayEnd||player->getX()<runwayStart)){
			planeCrash();
		}

		//Do a refeuling here turning around etc if landing
		if(speed<=0&&player->takingOff==-1){
			speed=0;
			player->takingOff=0;
			player->restock();
			//Flip planes current facing
			if(heading>128&&heading<384){player->setHeading(0);}
			else{player->setHeading(256<<8);}
		}
		//Make sure we dont exceed min speed when taking off
		if(speed>MINPLANESPEED&&player->takingOff==1){speed=MINPLANESPEED;}
	}
	
	//Recalculate xv and yv
	if(player->totalFuel>0){
		player->vx = (PA_Cos(heading)*speed)>>8;
		player->vy = (-PA_Sin(heading)*speed)>>8;
	}
	else{
		if(player->vx>0)player->vx--;
		if(player->vy<MAXPLANESPEED)player->vy+=GRAVITY;
		player->setHeading(PA_GetAngle(player->x,player->y,player->x+player->vx,player->y+player->vy)<<8);
	}	//Hurtle towards earth if no fuel

	player->x+=player->vx;
	player->y+=player->vy;
	player->speed=speed;

	u16 contactingRunway = (player->getX()>runwayStart&&player->getX()<runwayEnd)&&planeLandscapeCollision();

	//If plane is currently in the air and is contacting the runway
	if((player->takingOff==0&&contactingRunway)){
		if((heading>255&&heading<281)||(heading>486)){
			if(player->speed>MINPLANESPEED+100){planeCrash();return;}//Need to be going at slowly to land
			player->takingOff=-1;
			player->onRunway=1;
			player->y=(runwayHeight+12)<<8;
			if(heading>128&&heading<384){player->setHeading(256<<8);}
			else{player->setHeading(0);}
		}
		else{
			planeCrash();return; //Crash since we came at the runway at to steep angle
		}
	}

	//If the plane is going to be completely rendered above the y threshold reflect its current angle
	if(player->getY()<(SHEIGHT-currentLevel->levelHeight)-50){
		player->setHeading(reflectOverNormal(player->getHeading()>>8,384)<<8);
	}

	//If the plane is going to be completely off the x axis the reflect its current angle
	if(player->getX()+32<-5){
		player->setHeading(reflectOverNormal(player->getHeading()>>8,0)<<8);
	}

	else if(player->getX()>currentLevel->levelWidth+5){
		player->setHeading(reflectOverNormal(player->getHeading()>>8,256)<<8);
	}
	if(player->takingOff==0&&!contactingRunway){playerCollisions();}
}

void InGame::playerCollisions(){
	if(planeLandscapeCollision()||playerLandscapeObjectCollison()){
		planeCrash();
	}
}

void InGame::addParticlesFromObject(DestructableObject* destructable){
	SpriteInfo* si = destructable->getSpriteInfo();
	SpriteInfo* particleSi = destructable->getParticleSpriteInstance();

	//get center of destructable
	s16 cx = (destructable->getX()+(si->getSpriteWidth()/2));
	s16 cy = (destructable->getY()+(si->getSpriteHeight()/2));

	u16 width = si->getSpriteWidth();
	u16 height = si->getSpriteHeight();

	u16 halfWidth = destructable->getObjectWidth()/2;
	u16 halfHeight = destructable->getObjectHeight()/2;
	
	u16 ttl = 180;
	bool heavy =true;
	
	//for(u16 i=0;destructable->getParticleCount();i++){
		string* name = new string("landscape_particle");

		s32 startx = cx + (PA_RandMax(destructable->getObjectWidth()))-halfWidth;
		s32 starty = cy + (PA_RandMax(destructable->getObjectHeight()))-halfHeight;

		u16 currentAngle = PA_RandMax(511<<8);
		s16 rotspeed = PA_RandMinMax(1400,2800);
		if(PA_RandMax(1)){rotspeed = 0-rotspeed;}
		
		s16 particlevx = PA_RandMax(256)-256;
		s16 particlevy = PA_RandMax(256)-256;

		particles.push_back(new ParticleObject(name,startx<<8,starty<<8,width,height,currentAngle,rotspeed,particlevx,particlevy,ttl,heavy,particleSi));
	//}
}

bool InGame::playerLandscapeObjectCollison(){
	vector<DestructableObject*>::iterator it;
	it = landscapeObjects.begin();
	
	s16 playerx = 0;
	s16 playery = 0;
	getBottomEndPlane(player,playerx,playery,1);

	while( it != landscapeObjects.end()) {
		DestructableObject* destructable = (*it);
		if(destructable->getDestroyed()){it++;continue;}
		SpriteInfo* si = destructable->getSpriteInfo();
		s16 objectx = (destructable->getX()+(si->getSpriteWidth()/2)) -destructable->getObjectWidth()/2;
		s16 objecty = (destructable->getY()+(si->getSpriteHeight()/2)) -destructable->getObjectHeight()/2;

		if(pointInRectangle(playerx,playery,objectx,objecty,destructable->getObjectWidth(),destructable->getObjectHeight())){
			destructable->destructObject();
			addParticlesFromObject(destructable);
			return true;
		}
		it++;
	}
	return false;
}

bool InGame::pointInRectangle(s16 pointx,s16 pointy,s16 rectanglex,s16 rectangley,u16 width,u16 height){
	if(pointx>rectanglex&&pointx<rectanglex+width){
		if(pointy>rectangley&&pointy<rectangley+height){
			return true;
		}
	}
	return false;
}

void InGame::updateViewport(){
	u16 heading = (player->getHeading()>>8);
	s16 xflipped = (heading>128&&heading<384)? -1:1;
	
	//Viewport calculations using simulated float accuracy
	s16 xComponent = PA_Cos(heading);
	u16 adjust = (player->getSpeed()<MINPLANESPEED)? player->getSpeed():MINPLANESPEED;
	viewportx = player->x+(16<<8)-((SWIDTH/2)<<8)+((player->vx)*40)-((xComponent*((adjust)*40))>>8);
	viewporty = player->y-((SHEIGHT/2)<<8);

	if(getViewPortX()<0){viewportx=0;}
	if(getViewPortX()+SWIDTH>currentLevel->levelWidth){viewportx=((currentLevel->levelWidth-SWIDTH)<<8);}

	if(getViewPortY()<SHEIGHT-currentLevel->levelHeight){viewporty=((SHEIGHT-currentLevel->levelHeight)<<8);}
	if(getViewPortY()>0){viewporty=0;}
	
	//Scroll background
	if(getViewPortX()>0&&getViewPortX()+SWIDTH<currentLevel->levelWidth&&!player->crashed){
		bgscroll+=(((abs(player->vx)*(player->speed)>>8))/7*xflipped);
		PA_BGScrollX(0,2,bgscroll>>8);
	}
}
void InGame::updateProjectiles(){
	vector<GameObject*>::iterator it;
	it = projectiles.begin();

	while( it != projectiles.end()) {
		GameObject* projectile = (GameObject*)(*it);
		
		projectile->vy+=GRAVITY;

		projectile->x += projectile->vx;
		projectile->y += projectile->vy;

		s16 px = projectile->getX();
		s16 py = projectile->getY();
		s16 vx = projectile->vx;
		s16 vy = projectile->vy;

		u16 currentHeading = PA_GetAngle(px<<8,py<<8,(px<<8)+vx,(py<<8)+vy);
		projectile->setHeading(currentHeading<<8);
		projectile->getSpriteInfo()->setAngle(currentHeading<<8);

		s16 bx = projectile->getX();
		s16 by = projectile->getY();
		
		u16 hitLandscape = landscapeCollision(bx,by);
		if(bx>currentLevel->levelWidth||bx<0||by<0-(currentLevel->levelHeight-SHEIGHT)||hitLandscape){
			
			//This to be replace with hashtable lookup for sound (char*,char* --> void*)
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

u16 InGame::getSpeedFromVelocity(s16 vx,s16 vy){	
	u32 index = squared(vx)+squared(vy);
	#ifndef DEBUG
		return sqrtLUT[index];
	#else
		return (u16)sqrt(index);
	#endif
}

void InGame::updateParticles(){
	if(particles.empty())return;

	vector<ParticleObject*>::iterator it;
	it = particles.begin();

	while( it != particles.end()) {
		ParticleObject* po = (*it);
		//Update ttl and current position and current angle
		po->ttl--;
		SpriteInfo* si = po->getSpriteInfo();
		si->setAngle(wrapAngleShifted(si->getAngle()+po->rotSpeed));
		po->x+=po->vx;
		po->y+=po->vy;
		if(po->getX()>0&&po->getX()<currentLevel->levelWidth){
			if(!particleLandscapeCollision(po)&&po->heavy){
				po->vy+=GRAVITY;
			}
		}
		if(po->ttl==0){
			//Delete particle and recover rot index and sprie index
			s16 spriteIndex = si->getSpriteIndex();
			s16 rotIndex = si->getRotIndex();

			if(spriteIndex!=-1){
				spritePool.push_back(spriteIndex);
				PA_SetSpriteY(0, spriteIndex, 193);	//Hide sprite until its used again
			}
			if(rotIndex!=-1){
				PA_SetSpriteRotDisable(0,spriteIndex);
				rotPool.push_back(rotIndex);
			}
			it=particles.erase(it);
		}
		else{it++;}
	}
}
/**
Particle landscape collision, will work by finding all four corners of square
and then checks landscape collision for each of them....and then reflects off of
appropriate normals...and /2 vx and vy.
**/

bool InGame::particleLandscapeCollision(ParticleObject* po){
	//Ok first check height of ground at this point and reject trivial cases
	if(po->getY()<getHeightAtPoint(po->getX()+8)-20){
		return false;
	}
	
	SpriteInfo* si = po->getSpriteInfo();

	//Ok so we know we are close enough so start with getting the center of the particle as reference
	s32 cx = po->x+((si->getSpriteWidth()/2)<<8);
	s32 cy = po->y+((si->getSpriteHeight()/2)<<8);
	
	//What angle is particle currently facing towards
	s16 facingAngle = si->getAngle();
	s16 facingComponentx = PA_Cos(facingAngle);
	s16 facingComponenty = -PA_Sin(facingAngle);
	s16 facingLeft = (facingAngle>128&&facingAngle<384)? 1:-1;
	
	//What angle is particle currently heading towards
	u16 headingAngle =  PA_GetAngle(po->x,po->y, po->x+po->vx,po->y+po->vy);
	
	//Get the top and bottom middle of the particle
	s16 topNormal = wrapAngle(facingAngle+(-128*facingLeft));
	s16 bottomNormal = wrapAngle(facingAngle+(128*facingLeft));
	u16 radius = po->getObjectWidth()/2;

	s16 componentxTop = PA_Cos(topNormal); 
	s16 componentyTop = -PA_Sin(topNormal);

	s32 topx = cx+radius*componentxTop;
	s32 topy = cy+radius*componentyTop;

	s16 componentxBottom = PA_Cos(bottomNormal); 
	s16 componentyBottom = -PA_Sin(bottomNormal);

	s32 bottomx = cx+radius*componentxBottom;
	s32 bottomy = cy+radius*componentyBottom;

	//Find all the corners of the particles
	s32 topLeftx = topx-facingComponentx*radius;
	s32 topLefty = topy-facingComponenty*radius;

	bool collision = false;
	u16 highestOverlap=0;
	s32 collisionx=0;

	if(landscapeCollision(topLeftx>>8,topLefty>>8)){
		collision=true;
		s16 overlap = topLefty-(getHeightAtPoint(topLeftx>>8)<<8);
		if(overlap>highestOverlap){highestOverlap=overlap;collisionx=topLeftx;}
		
	}
	s32 topRightx =topx+facingComponentx*radius; 
	s32 topRighty =topy+facingComponenty*radius;

	if(landscapeCollision(topRightx>>8,topRighty>>8)){
		collision=true;
		s16 overlap =topRighty-(getHeightAtPoint(topRightx>>8)<<8);
		if(overlap>highestOverlap){highestOverlap=overlap;collisionx=topRightx;}
	}

	s32 bottomLeftx = bottomx-facingComponentx*radius;
	s32 bottomLefty = bottomy-facingComponenty*radius;

	if(landscapeCollision(bottomLeftx>>8,bottomLefty>>8)){
		collision=true;
		s16 overlap = bottomLefty-(getHeightAtPoint(bottomLeftx>>8)<<8);
		if(overlap>highestOverlap){highestOverlap=overlap;collisionx=bottomLeftx;}
	}

	s32 bottomRightx =bottomx+facingComponentx*radius;
	s32 bottomRighty =bottomy+facingComponenty*radius;

	if(landscapeCollision(bottomRightx>>8,bottomRighty>>8)){
		collision=true;
		s16 overlap = bottomRighty-(getHeightAtPoint(bottomRightx>>8)<<8);
		if(overlap>highestOverlap){highestOverlap=overlap;collisionx=bottomRightx;}
	}

	if(collision){
		//Make sure object is moved out of scenery
		po->y-=highestOverlap-256;

		//Figure out reflection angle and apply it to current velocity
		u16 reflectAngle = reflectTest= reflectOverNormal(headingAngle,getNormalAtPoint(collisionx>>8));
		u16 currentSpeed=getSpeedFromVelocity(po->vx,po->vy);
		//if(currentSpeed<=1000){currentSpeed=0;}
		po->vx=((currentSpeed*PA_Cos(reflectAngle))>>8);
		po->vy=((currentSpeed*-PA_Sin(reflectAngle))>>8);
		
		//For each collision slow down veclocity and rotate speed
		po->rotSpeed/=2;
		po->vx=(po->vx*180)>>8;
		po->vy=(po->vy*180)>>8;
	}
	return collision;
}

inline u16 InGame::wrapAngle(s16 angle){
	return (angle>511)? angle&511:(angle<0)? 512+angle:angle;
}

inline u16 InGame::wrapAngleShifted(s32 angle){
	return (angle>(511<<8))? angle&((512<<8)-1):(angle<0)? (512<<8)+angle:angle;
}

/**
Do drawing function
**/
void InGame::doDrawing(void){	
	//Clear bg so we can draw on screen!
	PA_Clear8bitBg(0);

	//Draw plane
	drawPlane();

	//Draw runway
	drawRunway();

	//Draw landscape tiles
	drawLandscapeTiles();

	//Draw landscape objects
	drawLandscapeObjects();

	//Draw projectiles
	drawProjectiles();

	//Draw particles
	drawParticles();
}
/**
This does not use drawObject function since it has to do quirky
animations based on its current heading!
**/
void InGame::drawPlane(){
	if(player->crashed)return;
	//Rotate plane and set animation frame
	SpriteInfo* si = player->getSpriteInfo();
	u16 heading = (player->getHeading()>>8);
	PA_SetRotset(0, si->getRotIndex(), heading,si->getZoom(),si->getZoom());
	PA_SetSpriteDblsize(0,si->getSpriteIndex(),si->getDoubleSize());
	if(heading<256){
		if(heading<=68){
			PA_SetSpriteAnim(0, 0, 0);
		}
		else if(heading>=188){
			PA_SetSpriteAnim(0, 0, 12);
		}
		else{
			u16 frame = ((heading-68)/10)+1;
			PA_SetSpriteAnim(0, 0, frame);
		}
	}
	else{
		if(heading>=444){
			PA_SetSpriteAnim(0, 0, 0);
		}
		else if(heading<=324){
			PA_SetSpriteAnim(0, 0, 12);
		}
		else{
			u16 frame = 12-(((heading-324)/10)+1);
			PA_SetSpriteAnim(0, 0, frame);
		}
	}

	s32 planex =((player->x-viewportx)>>8);
	s32 planey =((player->y-viewporty)>>8);
	
	//Dont draw it if its offscreen
	if(planex+32<0||planex>SWIDTH||planey+32<0||planey>SHEIGHT){return;}
	PA_SetSpriteXY(0,si->getSpriteIndex(), planex, planey);
}
void InGame::drawRunway(){

	vector<GameObject*>::iterator it;
	it = runwayObjects.begin();

	while( it != runwayObjects.end()){
		GameObject* runwayPiece = (GameObject*)(*it);
		drawObject(runwayPiece,2);
		it++;
	}

}
void InGame::drawProjectiles(){

	vector<GameObject*>::iterator it;
	it = projectiles.begin();

	while( it != projectiles.end()) {
		GameObject* projectile = (GameObject*)(*it);
		drawObject(projectile,1);
		it++;
	}
}
void InGame::drawParticles(){
	if(particles.empty())return;

	vector<ParticleObject*>::iterator it;
	it = particles.begin();

	while( it != particles.end()) {
		ParticleObject* po = (*it);
		drawObject(po,1);
		it++;
	}
}

void InGame::drawLandscapeObjects(){
	vector<DestructableObject*>::iterator it;
	it = landscapeObjects.begin();

	while( it != landscapeObjects.end()) {
		DestructableObject* lo = (*it);
		drawObject(lo,2);
		it++;
	}
}

void InGame::drawObject(GameObject* go,u16 priority){
		//Do dynamic allocation/deallocation of sprite indexes and rot indexes
		s16 finalx = go->getX()-getViewPortX();
		s16 finaly = go->getY()-getViewPortY();

		SpriteInfo* si = go->getSpriteInfo();

		s16 spriteIndex = si->getSpriteIndex();
		s16 rotIndex = si->getRotIndex();
		u16 width = si->getSpriteWidth();
		u16 height = si->getSpriteHeight();

		//Check if particle is offscreen...if so make sure it doesnt have a sprite index
		if(finalx+width<0||finalx>SWIDTH||finaly>SHEIGHT||finaly+height<0){
			//If piece has a spite index put it back into sprite pool
			if(spriteIndex!=-1){
				si->setSpriteIndex(-1);
				spritePool.push_back(spriteIndex);
				PA_SetSpriteY(0, spriteIndex, 193);	//Hide sprite until its used again
			}
			if(si->getRotIndex()!=-1&&si->getUsesRot()){
				si->setRotIndex(-1);
				rotPool.push_back(rotIndex);
				PA_SetSpriteRotDisable(spriteIndex,spriteIndex);
			}
		}
		//If its on screen test if it has a sprite index yet
		else{
			//It needs a sprite index assign one
			if(spriteIndex==-1&&si->getUsesSprite()){
				spriteIndex = spritePool.back();
				spritePool.pop_back();
				si->setSpriteIndex(spriteIndex);
				PA_CreateSpriteFromGfx(0, spriteIndex,si->getGfxRef(),si->getObjSize()[0],si->getObjSize()[1], 1,si->getPaletteIndex(),finalx,finaly);
			}
			//It already has a sprite index to move it
			else if(si->getUsesSprite()){
				PA_SetSpriteExtPrio(0,spriteIndex,priority);
				PA_SetSpriteDblsize(0,spriteIndex,si->getDoubleSize());
				PA_SetSpriteXY(0,spriteIndex,finalx,finaly);
			}
			//Draw object as line
			else{
				u16 currentHeading = go->getHeading()>>8;
				s16 componentx = PA_Cos(currentHeading);
				s16 componenty = -PA_Sin(currentHeading);

				s16 beginx = finalx - ((componentx*go->height)>>8);
				s16 beginy = finaly - ((componenty*go->height)>>8);

				if(beginx<0){beginx=0;}
				if(beginy<0){beginy=0;}
				
				PA_Draw8bitLineEx (0,beginx,beginy,finalx,finaly,si->getPaletteIndex(),go->getObjectWidth());	
			
			}
			if(rotIndex==-1&&si->getUsesRot()){	//Make sure it has a rot index and enable rotation for that index
				rotIndex = rotPool.back();
				rotPool.pop_back();
				si->setRotIndex(rotIndex);
				PA_SetSpriteRotEnable(0, spriteIndex,rotIndex);
				PA_SetRotset(0, rotIndex, si->getAngle()>>8,si->getZoom(),si->getZoom());
			}
			else if(si->getUsesRot()){
				PA_SetRotset(0, si->getRotIndex(), si->getAngle()>>8,si->getZoom(),si->getZoom());
			}
		}
}

void InGame::drawLandscapeTiles(){
	
	//Reset old landscape before rendering new one
	resetLandscape();

	vector<s16>::iterator it;
	it = currentLevel->heightMap->begin();


	s16 x =0-getViewPortX();
	while( it != currentLevel->heightMap->end()-1) {
		u16 thisHeight = (*it);
		u16 nextHeight = (*(it+1));
		u8 spriteIndex = abs(thisHeight-nextHeight)/2;
		s16 y = taller(thisHeight,nextHeight);		   //Get height
		y-=getViewPortY();
		
		if(x>-16&&x<256&&y<SHEIGHT){		//Draw if tile on screen
			u16 flipped = (thisHeight>nextHeight)?0:1;	   //Get flipped
			u16 length = grassTiles[spriteIndex][0];
			//Paste rest of sprite
			for(u16 i =1;i<length;i++){
				if(y<0){y+=32;break;}
				u16 availableIndex = spritePool.back();
				spritePool.pop_back();					//Get available index
				//Load sprite to screen
				PA_CreateSpriteFromGfx(0,availableIndex,grassTiles[spriteIndex][i],OBJ_SIZE_16X32, 1, 1, x,y);
				PA_SetSpriteHflip(0, availableIndex, flipped); // (screen, sprite, flip(1)/unflip(0)) HFlip -> Horizontal flip
				//Add index to list used by landscape
				landscapeIndexs.push_back(availableIndex);
				PA_SetSpriteExtPrio(0,availableIndex,3);
				y+=32;
				if(y>SHEIGHT)break;
			}
			//Paste black sprite
			while(y<=SHEIGHT){
				if(y<0){y+=32;break;}
				u16 availableIndex = spritePool.back();
				spritePool.pop_back();					//Get available index
				PA_CreateSpriteFromGfx(0,availableIndex,blackTile,OBJ_SIZE_16X32, 1, 1, x,y);
				landscapeIndexs.push_back(availableIndex);
				PA_SetSpriteExtPrio(0,availableIndex,3);
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

u16 InGame::planeLandscapeCollision(){
	if(player->onRunway){return 0;}

	s16 tipxFront = 0;
	s16 tipyFront = 0;
	s16 tipxBack = 0;
	s16 tipyBack = 0;
	getBottomEndPlane(player,tipxFront,tipyFront,1);
	getBottomEndPlane(player,tipxBack,tipyBack,-1);
	
	//Now collision tests
	return(landscapeCollision(tipxFront,tipyFront)||landscapeCollision(tipxBack,tipyBack));
}

/**
Utility function, bottom front of planes are used for collisions
**/
void InGame::getBottomEndPlane(GameObject* go,s16 &x,s16 &y,s16 direction){
	SpriteInfo* si = player->getSpriteInfo();
	u16 planeRadius = player->getObjectWidth()/2;

	s16 currentHeading = (go->getHeading()>>8);
	s16 normalAdjust = (currentHeading<128||currentHeading>384)? -128:128;

	s16 normalAngle = currentHeading+normalAdjust;

	if(normalAngle<0)normalAngle+=512;
	if(normalAngle>511)normalAngle &= 512;

	u16 cx = player->getX()+(si->getSpriteWidth()/2);
	u16 cy = player->getY()+(si->getSpriteHeight()/2);

	s16 bottomx = cx+((PA_Cos(normalAngle) * (go->getObjectHeight()/2))>>8);
	s16 bottomy = cy+((-PA_Sin(normalAngle) * (go->getObjectHeight()/2))>>8);
	
	s16 xComponent =PA_Cos(go->getHeading()>>8);
	s16 yComponent =-PA_Sin(go->getHeading()>>8);

	x = bottomx+(((xComponent*planeRadius)>>8)*direction);
	y = bottomy+(((yComponent*planeRadius)>>8)*direction);
}

int InGame::landscapeCollision(s16 x, s16 y){
	return(y>getHeightAtPoint(x));
}

/**
Note this uses shifted position for y 
**/
u16 InGame::getHeightAtPoint(u16 x){
	s16 landIndex = (x/16);
	if((u16)landIndex>currentLevel->heightMap->size()-2||landIndex<0){return SHEIGHT;}
	u32 a = currentLevel->heightMap->at(landIndex)<<8;
	u32 b = currentLevel->heightMap->at(landIndex+1)<<8;
	
	u32 diff = ((x &15)<<8)/16;
	u16 actualHeight = (a+(diff*((b-a)>>8)))>>8;	
	
	return actualHeight;
}

u16 InGame::getNormalAtPoint(u16 x){
	s16 landIndex = (x/16);
	u32 a = currentLevel->heightMap->at(landIndex);
	u32 b = currentLevel->heightMap->at(landIndex+1);

	u16 angle = PA_GetAngle(x,a,x+16,b);
	
	//a<b must mean that we are going downhill
	return (a>b)? wrapAngle(angle+128):wrapAngle(angle-128);
}

u16 InGame::reflectOverNormal(u16 angle,u16 normal){
	//First get the reflected angle
	u16 reflected = flipAngle(angle);
	s16 normalDiff = wrapAngleDistance(normal,reflected);
	return wrapAngle(normal+normalDiff);
}

inline u16 InGame::flipAngle(s16 angle){
	return wrapAngle(angle-256);
}

inline s16 InGame::wrapAngleDistance(u16 angle1,u16 angle2){
	s16 diff = angle1-angle2;
	s16 flipped = (diff<0)? -1:1;
	return (abs(diff)>256)?(256-(abs(diff)-256)*(0-flipped)):diff;
}
/**
**
**/
void InGame::planeCrash(){
	PA_SetSoundChannelVol(0,0);	//Kill engine sound
	PA_PlaySound(PA_GetFreeSoundChannel(),crash_sfx,(u32)crash_sfx_size,127,44100);
	player->crashed=true;
	PA_SetSpriteY(0, 0, 193);	// Hide sprite
	planeCrashParticles();
	u16 plane_gfx = player->getSpriteInfo()->getGfxRef();
	delete player;
	u16 temp[] = {OBJ_SIZE_32X32};
	u16* objsize = new u16[2];
	objsize[0] = temp[0];
	objsize[1] = temp[1];
	SpriteInfo* so = new SpriteInfo(32,32,0,plane_gfx,0,0,0,objsize,256,0,true);
	player = new PlaneObject(runwayStart<<8,(runwayHeight+12)<<8,16,8,0,0,0,9,60,so);
	scrollBackToRunway();
}
void InGame::planeCrashParticles(){
	s16 planevx = player->vx;
	s16 planevy = player->vy;
	
	string* name = new string("plane_part");

	u16 width=8;
	u16 height=8;

	u16 temp[] = {OBJ_SIZE_8X8};
	u16* objsize = new u16[2];
	objsize[0] = temp[0];
	objsize[1] = temp[1];
	
	//Dont assign spriteIndex yet since drawParticles will do that for us
	s16 spriteIndex = -1;

	u16 palette = 3;
	u16 gfxref = planePartgfx;
	u16 ttl = 240;
	bool heavy = true;
	u16 particleCount = 8;

	for(u16 i=0;i<particleCount;i++){
		s32 startx = (player->getX()+PA_RandMax(16))<<8;
		if(i==7){startx=player->x;}						//Last particle is created at planes current x for tracking reasons
		s32 starty = (player->getY()+(player->getObjectHeight()/2)+PA_RandMax(16))<<8;
		s16 rotspeed = PA_RandMinMax(1400,2800);
		if(PA_RandMax(1)){rotspeed = 0-rotspeed;}
		s16 currentAngle = PA_RandMax(511<<8);
		s16 particlevx = planevx;
		s16 particlevy = planevy;
		s16 rotIndex = -1;	//Rot index also dynamically assigned
		u16 zoom=512;
		
		//Yey new particle
		SpriteInfo* so = new SpriteInfo(width,height,currentAngle,gfxref,spriteIndex,rotIndex,palette,objsize,zoom,0,true);
		
		//Last particle is 
		if(i+1==particleCount){
			startx =player->x;
			starty =player->y;
		}
		particles.push_back(new ParticleObject(name,startx,starty,width,height,currentAngle,rotspeed,particlevx,particlevy,ttl,heavy,so));
	}

	
	//Make sure viewport is kept on particles
	for(u16 i=0;i<ttl-60;i++){
		drawParticles();
		updateParticles();
		ParticleObject* po = particles.back();	//Particle at end will always been bit of plane since this function is called immediately after crash
		player->x = po->x;
		player->y = po->y;
		updateViewport();
		doDrawing();

		print_debug();
		PA_WaitForVBL();
	}
}

void InGame::scrollBackToRunway(){
	s16 xComponent = PA_Cos(player->getHeading()>>8);
	u16 adjust = (player->getSpeed()<MINPLANESPEED)? player->getSpeed():MINPLANESPEED;

	s32 targetviewportx = player->x+(16<<8)-((SWIDTH/2)<<8)+((player->vx)*40)-((xComponent*((adjust)*40))>>8);
	s32 targetviewporty = player->y-((SHEIGHT/2)<<8);
	if(targetviewporty>0){targetviewporty=0;}
	if(targetviewportx<0){targetviewportx=0;}

	s16 goingDown = viewporty<targetviewporty;

	//Do slow transition back to runway
	while(viewportx!=targetviewportx||viewporty!=targetviewporty){
		if(viewportx>targetviewportx)viewportx-=800;
		if(goingDown)viewporty+=800;
		if(!goingDown)viewporty-=800;
		if(viewportx<targetviewportx)viewportx=targetviewportx;
		if(goingDown&&viewporty>targetviewporty)viewporty=targetviewporty;
		if(!goingDown&&viewporty<targetviewporty)viewporty=targetviewporty;
		doDrawing();
		//Wait for vsync
		PA_WaitForVBL();
	}
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
	PA_OutputText(1,0, 2, "Plane x:%d (%d) y:%d (%d)", player->getX(),player->x,player->getY(),player->y);
	PA_OutputText(1,0, 3, "Plane vx:%d vy:%d", player->vx,player->vy);
	PA_OutputText(1,0, 4, "Landscape sprites used:%d", landscapeIndexs.size());
	PA_OutputText(1,0, 5, "Available sprites:%d", spritePool.size());
	PA_OutputText(1,0, 6, "Plane taking off:%d", player->takingOff);
	PA_OutputText(1,0, 7, "Particle count:%d", particles.size());
	PA_OutputText(1,0, 8,"Rot pool size:%d", rotPool.size());
	PA_OutputText(1,0, 9,"Bombs: %d Ammo: %d Fuel: %d Health %d",player->totalBombs,player->totalAmmo,player->totalFuel,player->totalHealth);
	PA_OutputText(1,0, 11,"Landscape test: x:%d",landscapeReferenceObjects[0]->getSpriteInfo()->getObjSize()[1]);
	if(!particles.empty()){
		ParticleObject* po = particles.at(0);
		PA_OutputText(1,0, 13, "Particle test:%d %d", po->getX(),po->getY());
	}
}
