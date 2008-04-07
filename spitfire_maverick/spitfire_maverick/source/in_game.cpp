//internal header
#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/sprite_info.h"
#include "../header/player_object.h"
#include "../header/particle_object.h"
#include "../header/destructable_object.h"
#include "../header/projectile_object.h"
#include "../header/ai_object.h"
#include "../header/hardpoint_object.h"
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
void* player_gun_sfx[4]; //4 sfx for varying gun sounds
s32 player_gun_sfx_size[4];
u16 engine_channel=0;

//Need to think of a way to map sound to collisions between all gameobjects....string,string -> soundObject
//would be ideal........

//Are we in game?
u16 inGame = 0;

//Level stuff
Level* currentLevel;

//Projectile objects and lookup
vector<ProjectileObject*> projectiles;
ProjectileObject* projectileReferenceObjects[5];

//Particle objects and lookup
vector<ParticleObject*> particles;
ParticleObject* particleReferenceObjects[10];

//Landscape objects and lookup
vector<DestructableObject*> landscapeObjects;
DestructableObject* landscapeReferenceObjects[20];

//AI objects lookup
vector<AIObject*> AIObjects;
AIObject* AIReferenceObjects[6];

//Sprite pool of available indexes
vector<u16> spritePool;
vector<u16> rotPool; //Pool of available rotsets

//vector of indexes taken by landscape tiles
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

//Represents player plane
PlayerObject* player;
u16 bombRefGfx;

//Represents runway (needs to be part of level object)
u16 runwayStart;
u16 runwayEnd;
u16 runwayHeight;
vector<GameObject*> runwayObjects;

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
	initPlayer();

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

	//Palettes for primitive drawing
	PA_SetBgPalCol(0, 2, PA_RGB(31, 0, 0));
	PA_SetBgPalCol(0, 3, PA_RGB(0, 31, 0));
	PA_SetBgPalCol(0, 4, PA_RGB(31, 31, 31));
	PA_SetBgPalCol(0, 5, PA_RGB(19, 19, 5));
	PA_SetBgPalCol(0, 6, PA_RGB(28,28,28));

	//Load palattes
	PA_LoadSpritePal(0,0,(void*)spitfire_image_Pal);
	PA_LoadSpritePal(0,1,(void*)grass_image_Pal);
	PA_LoadSpritePal(0,2,(void*)runway_image_Pal);	
	PA_LoadSpritePal(0,3,(void*)landscape_collection1_Pal);
	PA_LoadSpritePal(0,4,(void*)landscape_collestion2_Pal);
	PA_LoadSpritePal(0,5,(void*)trees_particles_bomb_Pal);
	PA_LoadSpritePal(0,6,(void*)bomb_explosion_Pal);
	PA_LoadSpritePal(0,7,(void*)ai_Pal);

	//Init alpha
	PA_EnableSpecialFx(0,SFX_ALPHA,0,SFX_BG0 | SFX_BG1 | SFX_BG2 | SFX_BG3 | SFX_BD);

	//Init background
	PA_SetBgPrio(0,0,0);
	PA_EasyBgLoad(0, 2, background_image);
	PA_SetBgPrio(0,2,3);
	PA_InitSpriteExtPrio(1); // Enable extended priorities

	//Init particle lookup needs to be before landscape lookup!!
	initParticleLookup();

	//Init landscape lookup
	initLanscapeLookup();

	//Init projectile lookup
	initProjectileLookup();

	//Init AI lookup
	initAILookup();
}
void InGame::initAILookup(){
	//First lets lookup hardpoint refences
	u16 aaTurretSpriteGfx =  PA_CreateGfx(0, (void*)nazi1_turret_image_Sprite, OBJ_SIZE_64X32,1);
	SpriteInfo* aaTurretSprite = new SpriteInfo(64,32,0,aaTurretSpriteGfx,-1,-1,7,OBJ_SIZE_64X32,256,1,4,true,true,false);
	ProjectileObject* aaTurrentProjectile = projectileReferenceObjects[4];
	HardpointObject* aaTurret = new HardpointObject(0,0,64,32,128,aaTurretSprite,0,4,60,3,256,0,256,aaTurrentProjectile);

}

void InGame::addLandscapeObject(s32 x,u16 ref){
	DestructableObject* object = new DestructableObject(*landscapeReferenceObjects[ref]);
	SpriteInfo* si = object->getSpriteInfo();
	s32 y = getHeightAtPoint(x+si->getSpriteWidth()/2);
	s32 offset = si->getSpriteHeight();
	object->setLocation(x<<8,(y-offset+4)<<8);
	landscapeObjects.push_back(object);
}

void InGame::initProjectileLookup(){
	SpriteInfo* bulletSpriteInfo = new SpriteInfo(-1,-1,0,-1,-1,-1,5,0,0,256,0,2,false,false,false);
	ProjectileObject* playerBulletObject = new ProjectileObject(new string("bullet"),0,0,1,6,0,0,0,bulletSpriteInfo,-1,1,10,true);

	u16 bombGfx = PA_CreateGfx(0, (void*)bomb_image_Sprite, OBJ_SIZE_16X8, 1);
	SpriteInfo* bombSpriteInfo = new SpriteInfo(16,8,0,bombGfx,-1,-1,5,OBJ_SIZE_16X8,540,1,2,true,true,false);
	ProjectileObject* playerBombObject = new ProjectileObject(new string("bomb"),0,0,16,8,0,0,0,bombSpriteInfo,-1,5,1000,true,new ParticleObject(*particleReferenceObjects[6]));
	
	SpriteInfo* tankShellSpriteInfo = new SpriteInfo(-1,-1,0,-1,-1,-1,5,0,0,256,0,2,false,false,false);
	ProjectileObject* tankShellObject = new ProjectileObject(new string("shell"),0,0,2,4,0,0,0,tankShellSpriteInfo,-1,1,10,true,new ParticleObject(*particleReferenceObjects[7]));

	SpriteInfo* humanRocketSpriteInfo = new SpriteInfo(-1,-1,0,-1,-1,-1,6,0,0,256,0,2,false,false,false);
	ProjectileObject* humanRocketObject = new ProjectileObject(new string("rocket"),0,0,2,4,0,0,0,humanRocketSpriteInfo,-1,1,10,true,new ParticleObject(*particleReferenceObjects[8]));
	
	u16 aaShellGfx = PA_CreateGfx(0, (void*)nazi1_projectile_image_Sprite, OBJ_SIZE_8X8, 1);
	SpriteInfo* aaShellSpriteInfo = new SpriteInfo(8,8,0,aaShellGfx,-1,-1,5,OBJ_SIZE_8X8,256,0,2,false,true,false);
	ProjectileObject* aaShellObject = new ProjectileObject(new string("shell"),0,0,2,4,0,0,0,aaShellSpriteInfo,-1,1,10,true,new ParticleObject(*particleReferenceObjects[9]));

	projectileReferenceObjects[0] = playerBulletObject;
	projectileReferenceObjects[1] = playerBombObject;
	projectileReferenceObjects[2] = tankShellObject;
	projectileReferenceObjects[3] = humanRocketObject;
	projectileReferenceObjects[4] = aaShellObject;
}
void InGame::initLanscapeLookup(){
	//Now create all gfx ref
	u16 treeOneGfx = PA_CreateGfx(0, (void*)tree1_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 treeTwoGfx = PA_CreateGfx(0, (void*)tree2_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 treeThreeGfx = PA_CreateGfx(0, (void*)tree3_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 treeFourGfx = PA_CreateGfx(0, (void*)tree4_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 towerAlliesGfx = PA_CreateGfx(0, (void*)tower1_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 towerGermanGfx = PA_CreateGfx(0, (void*)tower2_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 buildOneGfx = PA_CreateGfx(0, (void*)build1_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 buildTwoGfx = PA_CreateGfx(0, (void*)build2_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 buildThreeGfx = PA_CreateGfx(0, (void*)build3_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 buildFourGfx = PA_CreateGfx(0, (void*)build4_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 buildFive1o3Gfx = PA_CreateGfx(0, (void*)build51o3_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 buildFive2o3Gfx = PA_CreateGfx(0, (void*)build52o3_image_Sprite, OBJ_SIZE_64X64, 1);
	u16 buildFive3o3Gfx = PA_CreateGfx(0, (void*)build53o3_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 buildSix1o3Gfx = PA_CreateGfx(0, (void*)build61o3_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 buildSix2o3Gfx = PA_CreateGfx(0, (void*)build62o3_image_Sprite, OBJ_SIZE_64X64, 1);
	u16 buildSix3o3Gfx = PA_CreateGfx(0, (void*)build63o3_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 buildSevenGfx = PA_CreateGfx(0, (void*)build7_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 buildEightGfx = PA_CreateGfx(0, (void*)build8_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 church1o2Gfx = PA_CreateGfx(0, (void*)church1o2_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 church2o2Gfx = PA_CreateGfx(0, (void*)church2o2_image_Sprite, OBJ_SIZE_32X64, 1);
	
	//Destroyed gfx ref
	u16 treeOneDestGfx = PA_CreateGfx(0, (void*)tree1_dest_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 treeTwoDestGfx = PA_CreateGfx(0, (void*)tree2_dest_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 treeThreeDestGfx = PA_CreateGfx(0, (void*)tree3_dest_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 treeFourDestGfx = PA_CreateGfx(0, (void*)tree4_dest_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 towerAlliesDestGfx = PA_CreateGfx(0, (void*)tower1_dest_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 towerGermanDestGfx = PA_CreateGfx(0, (void*)tower2_dest_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 buildOneDestGfx = PA_CreateGfx(0, (void*)build1_dest_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 buildTwoDestGfx = PA_CreateGfx(0, (void*)build2_dest_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 buildThreeDestGfx = PA_CreateGfx(0, (void*)build3_dest_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 buildFourDestGfx = PA_CreateGfx(0, (void*)build4_dest_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 buildFive1o3DestGfx = PA_CreateGfx(0, (void*)build51o3_dest_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 buildFive2o3DestGfx = PA_CreateGfx(0, (void*)build52o3_dest_image_Sprite, OBJ_SIZE_64X64, 1);
	u16 buildFive3o3DestGfx = PA_CreateGfx(0, (void*)build53o3_dest_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 buildSix1o3DestGfx = PA_CreateGfx(0, (void*)build61o3_dest_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 buildSix2o3DestGfx = PA_CreateGfx(0, (void*)build62o3_dest_image_Sprite, OBJ_SIZE_64X64, 1);
	u16 buildSix3o3DestGfx = PA_CreateGfx(0, (void*)build63o3_dest_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 buildSevenDestGfx = PA_CreateGfx(0, (void*)build7_dest_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 buildEightDestGfx = PA_CreateGfx(0, (void*)build8_dest_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 church1o2DestGfx = PA_CreateGfx(0, (void*)church1o2_dest_image_Sprite, OBJ_SIZE_64X32, 1);
	u16 church2o2DestGfx = PA_CreateGfx(0, (void*)church2o2_dest_image_Sprite, OBJ_SIZE_32X64, 1);


	//Now go onto create all landscape objects with their sprite info objects
	SpriteInfo* treeSpriteOneInfo = new SpriteInfo(32,32,0,treeOneGfx,-1,-1,5,OBJ_SIZE_32X32,256,0,1,false,true,false);
	DestructableObject* treeObjectOne = new DestructableObject(0,0,32,32,0,0,0,treeSpriteOneInfo,100,5,treeOneDestGfx,new ParticleObject(*particleReferenceObjects[3]));
	
	SpriteInfo* treeSpriteTwoInfo = new SpriteInfo(32,32,0,treeTwoGfx,-1,-1,5,OBJ_SIZE_32X32,256,0,1,false,true,false);
	DestructableObject* treeObjectTwo = new DestructableObject(0,0,32,32,0,0,0,treeSpriteTwoInfo,100,5,treeTwoDestGfx,new ParticleObject(*particleReferenceObjects[3]));
	
	SpriteInfo* treeSpriteThreeInfo = new SpriteInfo(32,32,0,treeThreeGfx,-1,-1,5,OBJ_SIZE_32X32,256,0,1,false,true,false);
	DestructableObject* treeObjectThree = new DestructableObject(0,0,32,32,0,0,0,treeSpriteThreeInfo,100,5,treeThreeDestGfx,new ParticleObject(*particleReferenceObjects[3]));
	
	SpriteInfo* treeSpriteFourInfo = new SpriteInfo(32,32,0,treeFourGfx,-1,-1,5,OBJ_SIZE_32X32,256,0,1,false,true,false);
	DestructableObject* treeObjectFour = new DestructableObject(0,0,32,32,0,0,0,treeSpriteFourInfo,100,5,treeFourDestGfx,new ParticleObject(*particleReferenceObjects[3]));

	SpriteInfo* towerAlliesSpriteInfo = new SpriteInfo(32,64,0,towerAlliesGfx,-1,-1,4,OBJ_SIZE_32X64,256,0,1,false,true,false);
	DestructableObject* towerAlliesObject = new DestructableObject(0,0,20,64,0,0,0,towerAlliesSpriteInfo,1000,5,towerAlliesDestGfx,new ParticleObject(*particleReferenceObjects[1]));
	
	SpriteInfo* towerGermanSpriteInfo = new SpriteInfo(32,64,0,towerGermanGfx,-1,-1,4,OBJ_SIZE_32X64,256,0,1,false,true,false);
	DestructableObject* towerGermanObject = new DestructableObject(0,0,20,64,0,0,0,towerGermanSpriteInfo,1000,5,towerGermanDestGfx,new ParticleObject(*particleReferenceObjects[1]));

	SpriteInfo* buildOneSpriteInfo = new SpriteInfo(64,32,0,buildOneGfx,-1,-1,3,OBJ_SIZE_64X32,256,0,1,false,true,false);
	DestructableObject* buildOneObject = new DestructableObject(0,0,64,32,0,0,0,buildOneSpriteInfo,1000,5,buildOneDestGfx,new ParticleObject(*particleReferenceObjects[0]));

	SpriteInfo* buildTwoSpriteInfo = new SpriteInfo(64,32,0,buildTwoGfx,-1,-1,3,OBJ_SIZE_64X32,256,0,1,false,true,false);
	DestructableObject* buildTwoObject = new DestructableObject(0,0,64,32,0,0,0,buildTwoSpriteInfo,1000,5,buildTwoDestGfx,new ParticleObject(*particleReferenceObjects[1]));

	SpriteInfo* buildThreeSpriteInfo = new SpriteInfo(64,32,0,buildThreeGfx,-1,-1,3,OBJ_SIZE_64X32,256,0,1,false,true,false);
	DestructableObject* buildThreeObject = new DestructableObject(0,0,64,32,0,0,0,buildThreeSpriteInfo,1000,5,buildThreeDestGfx,new ParticleObject(*particleReferenceObjects[1]));

	SpriteInfo* buildFourSpriteInfo = new SpriteInfo(64,32,0,buildFourGfx,-1,-1,3,OBJ_SIZE_64X32,256,0,1,false,true,false);
	DestructableObject* buildFourObject = new DestructableObject(0,0,64,32,0,0,0,buildFourSpriteInfo,1000,5,buildFourDestGfx,new ParticleObject(*particleReferenceObjects[1]));

	SpriteInfo* buildFive1o3SpriteInfo = new SpriteInfo(32,64,0,buildFive1o3Gfx,-1,-1,3,OBJ_SIZE_32X64,256,0,1,false,true,false);
	DestructableObject* buildFive1o3Object = new DestructableObject(0,0,32,64,0,0,0,buildFive1o3SpriteInfo,1000,5,buildFive1o3DestGfx,new ParticleObject(*particleReferenceObjects[0]));

	SpriteInfo* buildFive2o3SpriteInfo = new SpriteInfo(64,64,0,buildFive2o3Gfx,-1,-1,3,OBJ_SIZE_64X64,256,0,1,false,true,false);
	DestructableObject* buildFive2o3Object = new DestructableObject(0,0,64,40,0,0,0,buildFive2o3SpriteInfo,1000,5,buildFive2o3DestGfx,new ParticleObject(*particleReferenceObjects[0]));

	SpriteInfo* buildFive3o3SpriteInfo = new SpriteInfo(32,64,0,buildFive3o3Gfx,-1,-1,3,OBJ_SIZE_32X64,256,0,1,false,true,false);
	DestructableObject* buildFive3o3Object = new DestructableObject(0,0,20,64,0,0,0,buildFive3o3SpriteInfo,1000,5,buildFive3o3DestGfx,new ParticleObject(*particleReferenceObjects[0]));

	SpriteInfo* buildSix1o3SpriteInfo = new SpriteInfo(32,64,0,buildSix1o3Gfx,-1,-1,3,OBJ_SIZE_32X64,256,0,1,false,true,false);
	DestructableObject* buildSix1o3Object = new DestructableObject(0,0,32,64,0,0,0,buildSix1o3SpriteInfo,1000,5,buildSix1o3DestGfx,new ParticleObject(*particleReferenceObjects[0]));

	SpriteInfo* buildSix2o3SpriteInfo = new SpriteInfo(64,64,0,buildSix2o3Gfx,-1,-1,3,OBJ_SIZE_64X64,256,0,1,false,true,false);
	DestructableObject* buildSix2o3Object = new DestructableObject(0,0,64,40,0,0,0,buildSix2o3SpriteInfo,1000,5,buildSix2o3DestGfx,new ParticleObject(*particleReferenceObjects[0]));

	SpriteInfo* buildSix3o3SpriteInfo = new SpriteInfo(32,64,0,buildSix3o3Gfx,-1,-1,3,OBJ_SIZE_32X64,256,0,1,false,true,false);
	DestructableObject* buildSix3o3Object = new DestructableObject(0,0,32,64,0,0,0,buildSix3o3SpriteInfo,1000,5,buildSix3o3DestGfx,new ParticleObject(*particleReferenceObjects[0]));
	
	SpriteInfo* buildSevenSpriteInfo = new SpriteInfo(64,32,0,buildSevenGfx,-1,-1,4,OBJ_SIZE_64X32,400,0,1,true,true,false);
	DestructableObject* buildSevenObject = new DestructableObject(0,0,64,32,0,0,0,buildSevenSpriteInfo,1000,5,buildSevenDestGfx,new ParticleObject(*particleReferenceObjects[1]));

	SpriteInfo* buildEightSpriteInfo = new SpriteInfo(64,32,0,buildEightGfx,-1,-1,4,OBJ_SIZE_64X32,400,0,1,true,true,false);
	DestructableObject* buildEightObject = new DestructableObject(0,0,64,32,0,0,0,buildEightSpriteInfo,1000,5,buildEightDestGfx,new ParticleObject(*particleReferenceObjects[2]));

	SpriteInfo* church1o2SpriteInfo = new SpriteInfo(64,32,0,church1o2Gfx,-1,-1,4,OBJ_SIZE_64X32,256,0,1,false,true,false);
	DestructableObject* church1o2Object = new DestructableObject(0,0,64,32,0,0,0,church1o2SpriteInfo,1000,5,church1o2DestGfx,new ParticleObject(*particleReferenceObjects[1]));

	SpriteInfo* church2o2SpriteInfo = new SpriteInfo(32,64,0,church2o2Gfx,-1,-1,4,OBJ_SIZE_32X64,256,0,1,false,true,false);
	DestructableObject* church2o2Object = new DestructableObject(0,0,32,64,0,0,0,church2o2SpriteInfo,1000,5,church2o2DestGfx,new ParticleObject(*particleReferenceObjects[0]));

	//Now put all objects in an array to be looked up
	landscapeReferenceObjects[0] = treeObjectOne;
	landscapeReferenceObjects[1] = treeObjectTwo;
	landscapeReferenceObjects[2] = treeObjectThree;
	landscapeReferenceObjects[3] = treeObjectFour;
	landscapeReferenceObjects[4] = towerAlliesObject;
	landscapeReferenceObjects[5] = towerGermanObject;
	landscapeReferenceObjects[6] = buildOneObject;
	landscapeReferenceObjects[7] = buildTwoObject;
	landscapeReferenceObjects[8] = buildThreeObject;
	landscapeReferenceObjects[9] = buildFourObject;
	landscapeReferenceObjects[10] = buildFive1o3Object;
	landscapeReferenceObjects[11] = buildFive2o3Object;
	landscapeReferenceObjects[12] = buildFive3o3Object;
	landscapeReferenceObjects[13] = buildSix1o3Object;
	landscapeReferenceObjects[14] = buildSix2o3Object;
	landscapeReferenceObjects[15] = buildSix3o3Object;
	landscapeReferenceObjects[16] = buildSevenObject;
	landscapeReferenceObjects[17] = buildEightObject;
	landscapeReferenceObjects[18] = church1o2Object;
	landscapeReferenceObjects[19] = church2o2Object;
} 

void InGame::initParticleLookup(){
	//Now create all gfx ref
	u16 buildPart1Gfx = PA_CreateGfx(0, (void*)b_part1_image_Sprite, OBJ_SIZE_8X8, 1);
	u16 buildPart2Gfx = PA_CreateGfx(0, (void*)b_part2_image_Sprite, OBJ_SIZE_8X8, 1);
	u16 buildPart3Gfx = PA_CreateGfx(0, (void*)b_part3_image_Sprite, OBJ_SIZE_8X8, 1);
	u16 treePartGfx = PA_CreateGfx(0, (void*)t_part_image_Sprite, OBJ_SIZE_8X8, 1);
	u16 planePartGfx = PA_CreateGfx(0, (void*)p_part_image_Sprite, OBJ_SIZE_8X8, 1);
	u16 smokeGfx = PA_CreateGfx(0, (void*)smoke_image_Sprite, OBJ_SIZE_8X8, 1);
	u16 aaSmokeGfx = PA_CreateGfx(0, (void*)nazi1_projectile_particle_image_Sprite, OBJ_SIZE_32X32, 1);
	u16 explosionGfx = PA_CreateGfx(0, (void*)bomb_explosion_image_Sprite, OBJ_SIZE_32X32, 1);

	//Now go onto create all particle ref objects
	SpriteInfo* buildPart1SpriteInfo = new SpriteInfo(8,8,0,buildPart1Gfx,-1,-1,5,OBJ_SIZE_8X8,256,1,0,true,true,false);
	ParticleObject* buildPart1Particle = new ParticleObject(new string("particle"),0,0,8,8,0,0,0,0,180,true,buildPart1SpriteInfo);
	
	SpriteInfo* buildPart2SpriteInfo = new SpriteInfo(8,8,0,buildPart2Gfx,-1,-1,5,OBJ_SIZE_8X8,256,1,0,true,true,false);
	ParticleObject* buildPart2Particle = new ParticleObject(new string("particle"),0,0,8,8,0,0,0,0,180,true,buildPart2SpriteInfo);
	
	SpriteInfo* buildPart3SpriteInfo = new SpriteInfo(8,8,0,buildPart3Gfx,-1,-1,5,OBJ_SIZE_8X8,256,1,0,true,true,false);
	ParticleObject* buildPart3Particle = new ParticleObject(new string("particle"),0,0,8,8,0,0,0,0,180,true,buildPart3SpriteInfo);
	
	AnimationInfo* treePartFrameInfo = new AnimationInfo(0,3,0,false);
	SpriteInfo* treePartSpriteInfo = new SpriteInfo(8,8,0,treePartGfx,-1,-1,5,OBJ_SIZE_8X8,256,1,0,true,true,false,treePartFrameInfo,(void*)t_part_image_Sprite);
	ParticleObject* treePartParticle = new ParticleObject(new string("particle"),0,0,8,8,0,0,0,0,180,true,treePartSpriteInfo);
	
	AnimationInfo* planePartFrameInfo = new AnimationInfo(0,3,0,false);  //Using animation object so that single particle can have multiple images
	SpriteInfo* planePartSpriteInfo = new SpriteInfo(8,8,0,planePartGfx,-1,-1,5,OBJ_SIZE_8X8,300,1,0,true,true,false,planePartFrameInfo,(void*)p_part_image_Sprite);
	ParticleObject* planePartParticle = new ParticleObject(new string("particle"),0,0,8,8,0,0,0,0,240,true,planePartSpriteInfo);

	SpriteInfo* smokeSpriteInfo = new SpriteInfo(8,8,0,smokeGfx,-1,-1,5,OBJ_SIZE_8X8,480,1,0,true,true,true);
	ParticleObject* smokeParticle = new ParticleObject(new string("particle"),0,0,8,8,0,0,0,0,120,false,smokeSpriteInfo);
	
	AnimationInfo* bombExplosionAnimation = new AnimationInfo(0,29,2,true);
	SpriteInfo* bombExplosionSpriteInfo = new SpriteInfo(32,32,0,explosionGfx,-1,-1,6,OBJ_SIZE_32X32,350,0,0,true,true,false,bombExplosionAnimation,(void*)bomb_explosion_image_Sprite);
	ParticleObject* bombExplosionParticle = new ParticleObject(new string("particle"),0,0,32,32,0,0,0,0,58,false,bombExplosionSpriteInfo);

	AnimationInfo* shellExplosionAnimation = new AnimationInfo(0,29,2,true);
	SpriteInfo* shellExplosionSpriteInfo = new SpriteInfo(32,32,0,explosionGfx,-1,-1,6,OBJ_SIZE_32X32,450,0,0,true,true,false,shellExplosionAnimation,(void*)bomb_explosion_image_Sprite);
	ParticleObject* shellExplosionParticle = new ParticleObject(new string("particle"),0,0,32,32,0,0,0,0,58,false,shellExplosionSpriteInfo);

	AnimationInfo* rocketExplosionAnimation = new AnimationInfo(0,29,2,true);
	SpriteInfo* rocketExplosionSpriteInfo = new SpriteInfo(32,32,0,explosionGfx,-1,-1,6,OBJ_SIZE_32X32,600,0,0,true,true,false,rocketExplosionAnimation,(void*)bomb_explosion_image_Sprite);
	ParticleObject* rocketExplosionParticle = new ParticleObject(new string("particle"),0,0,32,32,0,0,0,0,58,false,rocketExplosionSpriteInfo);

	SpriteInfo* aaSmokeSpriteInfo = new SpriteInfo(32,32,0,aaSmokeGfx,-1,-1,5,OBJ_SIZE_32X32,480,1,0,true,true,true);
	ParticleObject* aaSmokeParticle = new ParticleObject(new string("particle"),0,0,32,32,0,0,0,0,300,false,aaSmokeSpriteInfo);

	//Now put all objects in an array to be looked up
	particleReferenceObjects[0] = buildPart1Particle;
	particleReferenceObjects[1] = buildPart2Particle;
	particleReferenceObjects[2] = buildPart3Particle;
	particleReferenceObjects[3] = treePartParticle;
	particleReferenceObjects[4] = planePartParticle;
	particleReferenceObjects[5] = smokeParticle;
	particleReferenceObjects[6] = bombExplosionParticle;
	particleReferenceObjects[7] = shellExplosionParticle;
	particleReferenceObjects[8] = rocketExplosionParticle;
	particleReferenceObjects[9] = aaSmokeParticle;
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
		heightMap->push_back(SHEIGHT-90);
		heightMap->push_back(SHEIGHT-110);
		heightMap->push_back(SHEIGHT-130);
		heightMap->push_back(SHEIGHT-150);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
				heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
				heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
		heightMap->push_back(SHEIGHT-156);
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
		addLandscapeObject(810,0);
		addLandscapeObject(880,1);
		addLandscapeObject(950,2);
		addLandscapeObject(1020,3);
		addLandscapeObject(1090,4);
		addLandscapeObject(1160,5);
		addLandscapeObject(1230,6);
		addLandscapeObject(1300,7);
		addLandscapeObject(1370,8);
		addLandscapeObject(1440,9);
		addLandscapeObject(1510,10);//build51o1
		addLandscapeObject(1542,11);
		addLandscapeObject(1606,12);
		addLandscapeObject(1676,13);//build61o1
		addLandscapeObject(1708,14);
		addLandscapeObject(1772,15);
		addLandscapeObject(1844,16);
		addLandscapeObject(1914,17);
		addLandscapeObject(1984,18);//church1o1
		addLandscapeObject(2048,19);
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
		SpriteInfo* so = new SpriteInfo(64,32,0,runwaygfx,-1,-1,2,OBJ_SIZE_64X32,256,0,3,false,true,false);
		GameObject * runwayPart = new GameObject(new string("runway"),x<<8,(runwayHeight+10)<<8,64,32,0,0,0,so);
		runwayObjects.push_back(runwayPart);
	}
	//Add the tower
	s16 towergfx = PA_CreateGfx(0, (void*)runway_tower_image_Sprite, OBJ_SIZE_32X64, 1);
	u16 x = (runwayStart+(RUNWAYLENGTH/2)-8);
	u16 y = getHeightAtPoint(x)-64;

	SpriteInfo* so = new SpriteInfo(32,64,0,towergfx,-1,-1,2,OBJ_SIZE_32X64,256,0,3,false,true,false);
	GameObject * tower = new GameObject(new string("runway_tower"),x<<8,y<<8,32,64,0,0,0,so);
	runwayObjects.push_back(tower);
}
void InGame::initPlayer(){
	u16 plane_gfx = PA_CreateGfx(0, (void*)spitfire_image_Sprite, OBJ_SIZE_32X32, 1);

	SpriteInfo* so = new SpriteInfo(32,32,0,plane_gfx,0,0,0,OBJ_SIZE_32X32,256,0,0,true,true,false);
	player = new PlayerObject(runwayStart<<8,(runwayHeight+12)<<8,16,8,0,0,0,9,60,so,new ParticleObject(*particleReferenceObjects[4]));

	//Load plane sprite
	PA_CreateSpriteFromGfx(0, so->getSpriteIndex(),so->getGfxRef(),OBJ_SIZE_32X32, 1, so->getPaletteIndex(), (player->getX()>>8), (player->getY()>>8));

	//Enable rotation for player
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

	PA_PlaySoundEx2 (0,plane_engine_sfx,(s32)plane_engine_sfx_size,(player->getSpeed()<<8)/1109,44100,0,true,0);
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
		player->setHeading(player->getHeading()+PLANETURNSPEED);
		if(player->getHeading()>(511<<8)){player->setHeading(0);}
	}
	if(Pad.Held.Right&&!player->onRunway){
		player->setHeading(player->getHeading()-PLANETURNSPEED);
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
	
	s32 cx = player->getX()+((planeSo->getSpriteWidth()/2)<<8);
	s32 cy = player->getY()+((planeSo->getSpriteHeight()/2)<<8);

	s32 startx = cx+(componentx*(player->getObjectWidth()/2));
	s32 starty = cy+(componenty*(player->getObjectWidth()/2));
	
	s32 heading = player->getHeading()>>8;

	s16 vx = (PA_Cos(heading)*(player->speed+1000)>>8);
	s16 vy = (-PA_Sin(heading)*(player->speed+1000)>>8)+PA_RandMax(64);
	
	ProjectileObject* bullet = new ProjectileObject(*projectileReferenceObjects[0]);
	bullet->setX(startx);
	bullet->setY(starty);
	bullet->setVx(vx);
	bullet->setVy(vy);
	bullet->setHeading(heading);

	projectiles.push_back(bullet);
}

void InGame::addPlayerBomb(){
	PA_PlaySound(PA_GetFreeSoundChannel(),player_bomb_sfx,(u32)player_bomb_sfx_size,127,44100);
	player->totalBombs--;

	SpriteInfo* planeSo = player->getSpriteInfo();

	s32 cx = player->getX()+((planeSo->getSpriteWidth()/2)<<8);
	s32 cy = player->getY()+((planeSo->getSpriteHeight()/2)<<8);

	//First get the bottom middle
	s16 currentAngle = player->getHeading()>>8;
	s16 normalAdjust = (currentAngle<128||currentAngle>384)? -128:128;

	s16 normalAngle = currentAngle+normalAdjust;

	if(normalAngle<0)normalAngle+=512;
	if(normalAngle>511)normalAngle &= 512;

	s32 startx = cx+(PA_Cos(normalAngle) * (player->getObjectHeight()/2));
	s32 starty = cy+(-PA_Sin(normalAngle) * (player->getObjectHeight()/2));
	
	s32 heading = player->getHeading();

	ProjectileObject* bomb = new ProjectileObject(*projectileReferenceObjects[1]);
	bomb->setX(startx-((bomb->getSpriteInfo()->getSpriteWidth()/2)<<8));
	bomb->setY(starty-((bomb->getSpriteInfo()->getSpriteHeight()/2)<<8));
	bomb->setVx(player->vx);
	bomb->setVy(player->vy);
	bomb->setHeading(heading);

	projectiles.push_back(bomb);
}

/**
** Update plane location and viewport
**/
void InGame::doUpdates(){

	//Update Plane
	updatePlayer();

	//Update viewport
	updateViewport();

	//Update projectiles
	updateProjectiles();

	//Update particles
	updateParticles();

	//Update AI
	updateAI();
}
void InGame::updateAI(){
	vector<AIObject*>::iterator it;
	it = AIObjects.begin();

	while( it != AIObjects.end()) {
		AIObject* ai = *it;
		//Do something here

		it++;
	}
}

void InGame::updatePlayer(){
	//Update engine sound
	PA_SetSoundChannelVol(0,(player->speed<<8)/1109);
	
	//Increase time since fired/bombed
	player->timeSinceFired++; //Increase time since last fired
	player->timeSinceBombed++;
	
	//Make sure heading is tied to sprite rotation
	player->getSpriteInfo()->setAngle(player->getHeading());
	s16 heading = player->getHeading()>>8;

	//Check for takeoff
	if(player->onRunway&&(((player->getX()>>8)+player->getObjectWidth()>runwayEnd&&heading==0)||((player->getX()>>8)<runwayStart&&heading==256))&&player->takingOff==1){
		if(player->speed<MINPLANESPEED){planeCrash();return;}	//Crash if we have reached the end of runway and we are not fast enough
		player->onRunway=0;
		player->takingOff=0;
		if(heading==256){player->setHeading(246<<8);}
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
		if(player->takingOff==-1&&player->speed>0&&((player->getX()>>8)+player->getObjectWidth()>runwayEnd||(player->getX()>>8)<runwayStart)){
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
	
	//Possibly add some smoke
	player->setTimeSinceLastSmoked(player->getTimeSinceLastSmoked()+1);
	if(player->getTimeSinceLastSmoked()>player->getSmokingInterval()&&player->getHealth()<50){
		ParticleObject* po = new ParticleObject(*(particleReferenceObjects[1]));
		SpriteInfo* si = po->getSpriteInfo();
		po->vy=15+PA_RandMax(15); //WTF HEAVY SMOKE?
		po->vx=player->vx/7;
		s32 cx = player->getX()+((player->getSpriteInfo()->getSpriteWidth()/2)<<8);
		s32 cy = player->getY()+((player->getSpriteInfo()->getSpriteHeight()/2)<<8);
		
		s16 xComponent =PA_Cos(player->getHeading()>>8);
		s16 yComponent =-PA_Sin(player->getHeading()>>8);

		s32 x = cx+((xComponent*(player->getObjectWidth()-2)/2));
		s32 y = cy+((yComponent*(player->getObjectWidth()-2)/2));


		po->setX(x-((si->getSpriteWidth()/2)<<8));
		po->setY(y-((si->getSpriteHeight()/2)<<8));
		player->setTimeSinceLastSmoked(0);
		particles.push_back(po);
	}

	//Recalculate xv and yv
	if(player->totalFuel>0){
		player->vx = (PA_Cos(heading)*speed)>>8;
		player->vy = (-PA_Sin(heading)*speed)>>8;
	}
	else{
		if(player->vx>0)player->vx--;
		if(player->vy<MAXPLANESPEED)player->vy+=GRAVITY;
		player->setHeading(PA_GetAngle(player->getX(),player->getY(),player->getX()+player->vx,player->getY()+player->vy)<<8);
	}	//Hurtle towards earth if no fuel
	
	player->setX(player->getX()+player->vx);
	player->setY(player->getY()+player->vy);
	player->speed=speed;

	u16 contactingRunway = ((player->getX()>>8)>runwayStart&&(player->getX()>>8)<runwayEnd)&&playerLandscapeCollision();

	//If plane is currently in the air and is contacting the runway
	if((player->takingOff==0&&contactingRunway)){
		if((heading>255&&heading<281)||(heading>486)){
			if(player->speed>MINPLANESPEED+100){planeCrash();return;}//Need to be going at slowly to land
			player->takingOff=-1;
			player->onRunway=1;
			player->setY((runwayHeight+12)<<8);
			if(heading>128&&heading<384){player->setHeading(256<<8);}
			else{player->setHeading(0);}
		}
		else{
			planeCrash();return; //Crash since we came at the runway at to steep angle
		}
	}

	//If the plane is going to be completely rendered above the y threshold reflect its current angle
	if((player->getY()>>8)<(SHEIGHT-currentLevel->levelHeight)-50){
		player->setHeading(reflectOverNormal(player->getHeading()>>8,384)<<8);
	}

	//If the plane is going to be completely off the x axis the reflect its current angle
	if((player->getX()>>8)+32<-5){
		player->setHeading(reflectOverNormal(player->getHeading()>>8,0)<<8);
	}

	else if((player->getX()>>8)>currentLevel->levelWidth+5){
		player->setHeading(reflectOverNormal(player->getHeading()>>8,256)<<8);
	}
	if(player->takingOff==0&&!contactingRunway){playerCollisions();}
}

void InGame::playerCollisions(){
	if(playerLandscapeCollision()||playerLandscapeObjectCollison()){
		planeCrash();
	}
}

void InGame::addParticlesFromObject(DestructableObject* destructable){
	SpriteInfo* si = destructable->getSpriteInfo();

	//get center of destructable
	s32 cx = destructable->getX()+((si->getSpriteWidth()/2)<<8);
	s32 cy = destructable->getY()+((si->getSpriteHeight()/2)<<8);

	u16 halfWidth = destructable->getObjectWidth()/2;
	u16 halfHeight = destructable->getObjectHeight()/2;

	for(u16 i=0;i<destructable->getParticleCount();i++){
		ParticleObject* po = new ParticleObject(*(destructable->getParticleSpriteInstance()));

		s32 startx = cx + (PA_RandMax(destructable->getObjectWidth()<<8))-(halfWidth<<8);
		s32 starty = cy + (PA_RandMax(destructable->getObjectHeight()<<8))-(halfHeight<<8);

		u16 currentAngle = PA_RandMax(511<<8);
		s16 rotSpeed = PA_RandMinMax(1400,2800);
		if(PA_RandMax(1)){rotSpeed = 0-rotSpeed;}
	
		s16 particleVx = (destructable->vx==0)?PA_RandMax(256)-256:destructable->vx;
		s16 particleVy = (destructable->vy==0)?PA_RandMax(256)-256:destructable->vy;
		
		po->setX(startx);
		po->setY(starty);
		po->setHeading(currentAngle);
		po->getSpriteInfo()->setAngle(currentAngle);
		po->setRotSpeed(rotSpeed);
		po->setVx(particleVx);
		po->setVy(particleVy);

		//Particle objects can have multiple frames (plane and tree particles) so set random frame
		SpriteInfo* si = po->getSpriteInfo();
		AnimationInfo* ai = si->getAnimationInfo();
		if(ai!=NULL){
			u16 randIndex = PA_RandMax(ai->getFrameCount()-1);
			ai->setCurrentFrame(randIndex);
		}
		particles.push_back(po);
	}
}

/**
Could optimise this by seeing distance between player and object
caching it each time then if its increasing break the loop since gameobjects
are in linear order
**/
bool InGame::playerLandscapeObjectCollison(){
	vector<DestructableObject*>::iterator it;
	it = landscapeObjects.begin();
	
	s32 playerx = 0;
	s32 playery = 0;
	getBottomEndOfObject(player,playerx,playery,1);

	while( it != landscapeObjects.end()) {
		DestructableObject* destructable = (*it);
		if(destructable->getDestroyed()){it++;continue;}
		SpriteInfo* si = destructable->getSpriteInfo();
		s16 destx = ((destructable->getX()>>8)+(si->getSpriteWidth()/2)) -destructable->getObjectWidth()/2;
		s16 desty = ((destructable->getY()>>8)+(si->getSpriteHeight()/2)) -destructable->getObjectHeight()/2;
		s16 destwidth = destructable->getObjectWidth();
		s16 destheight = destructable->getObjectHeight();

		if(pointInRectangleCollision(playerx,playery,destx,desty,destwidth,destheight)){
			destructable->destructObject();
			releaseObjectResources(destructable);
			addParticlesFromObject(destructable);
			return true;
		}
		it++;
	}
	return false;
}

void InGame::releaseObjectResources(GameObject* go){
	SpriteInfo* si = go->getSpriteInfo();
	
	s16 spriteIndex = si->getSpriteIndex();
	s16 rotIndex = si->getRotIndex();

	if(spriteIndex!=-1){
		PA_SetSpriteY(0,spriteIndex,193);
		si->setSpriteIndex(-1);
		spritePool.push_back(spriteIndex);
		if(si->getUsesAlpha()){
			PA_SetSpriteMode(0,spriteIndex,0);
		}

		if(rotIndex!=-1&&si->getUsesRotZoom()){
			PA_SetSpriteRotDisable(0,spriteIndex);
			si->setRotIndex(-1);
			rotPool.push_back(rotIndex);
		}	
	}
	if(si->usesAnimation()){
		PA_SetSpriteAnim(0, spriteIndex, 0);
	}
}

bool InGame::pointInRectangleCollision(s16 pointx,s16 pointy,s16 rectanglex,s16 rectangley,u16 width,u16 height){
	if(pointx>rectanglex&&pointx<rectanglex+width){
		if(pointy>rectangley&&pointy<rectangley+height){
			return true;
		}
	}
	return false;
}
/**
Reasonably fast circle collision, overlap needs to be sqrtd to be accurate
works by getting closest corner and doing squared distance to center of circle
**/
bool InGame::circleAndSquareCollision(s16 x0,s16 y0,u16 w0,u16 h0,s16 cx,s16 cy,u16 radius){
    s16 testX=cx;
    s16 testY=cy;
	if(testX < x0){testX=x0;}
	if(testX >x0+w0){testX=x0+w0;}
	if(testY < y0){testY=y0;}
	if(testY > y0+h0){testY=y0+h0;}
	
	s32 overlap = radius*radius-((cx-testX)*(cx-testX)+(cy-testY)*(cy-testY));

    return overlap>0;
}

void InGame::updateViewport(){
	u16 heading = (player->getHeading()>>8);
	s16 xflipped = (heading>128&&heading<384)? -1:1;
	
	//Viewport calculations using simulated float accuracy
	s16 xComponent = PA_Cos(heading);
	u16 adjust = (player->getSpeed()<MINPLANESPEED)? player->getSpeed():MINPLANESPEED;
	viewportx = player->getX()+(16<<8)-((SWIDTH/2)<<8)+((player->vx)*40)-((xComponent*((adjust)*40))>>8);
	viewporty = player->getY()-((SHEIGHT/2)<<8);

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
bool InGame::projectileLandscapeObjectCollison(ProjectileObject* projectile){
	vector<DestructableObject*>::iterator it;
	it = landscapeObjects.begin();
	
	//Needs to be modified to end middle of projectile
	s32 cx = (projectile->getX()>>8);
	s32 cy = (projectile->getY()>>8);

	//If projectile uses sprite current position will jst be top left of it..we need to find front middle
	if(projectile->getSpriteInfo()->getUsesSprite()){
		getMiddleEndOfObject((GameObject*)projectile,cx,cy,1);
	}

	bool collision = false;
	while( it != landscapeObjects.end()) {
		DestructableObject* destructable = (*it);
		if(destructable->getDestroyed()){it++;continue;}
		u16 radius = (projectile->isExploded())? projectile->getExplosionRadius():1;
		SpriteInfo* si = destructable->getSpriteInfo();
		s16 destx = ((destructable->getX()>>8)+(si->getSpriteWidth()/2)) -destructable->getObjectWidth()/2;
		s16 desty = ((destructable->getY()>>8)+(si->getSpriteHeight()/2)) -destructable->getObjectHeight()/2;
		s16 destwidth = destructable->getObjectWidth();
		s16 destheight = destructable->getObjectHeight();
		if(circleAndSquareCollision(destx,desty,destwidth,destheight,cx,cy,radius)){
			destructable->setHealth(destructable->getHealth()-projectile->getProjectileStrength());
			collision=true;
			projectile->explode();
			if(destructable->getHealth()<=0){
				destructable->destructObject();
				releaseObjectResources(destructable);
				addParticlesFromObject(destructable);
			}
			
		}
		it++;
	}
	return collision;
}
bool InGame::projectileCollision(ProjectileObject* projectile){
	return (projectileLandscapeObjectCollison(projectile));
}
void InGame::updateProjectiles(){
	vector<ProjectileObject*>::iterator it;
	it = projectiles.begin();

	while( it != projectiles.end()) {
		ProjectileObject* projectile = (ProjectileObject*)(*it);
		
		projectile->vy+=GRAVITY;
		
		projectile->setX(projectile->getX()+projectile->vx);
		projectile->setY(projectile->getY()+projectile->vy);
		
		//Need to modify this to find bottom middle of projectile
		s32 px = (projectile->getX()>>8);
		s32 py = (projectile->getY()>>8);
		
		//If projectile uses sprite current position will jst be top left of it..we need to find front middle
		if(projectile->getSpriteInfo()->getUsesSprite()){
			getMiddleEndOfObject((GameObject*)projectile,px,py,1);
		}

		s16 vx = projectile->vx;
		s16 vy = projectile->vy;
		s16 ttl = projectile->getTtl();
		
		//Make sure we calculate current heading and turn to face it
		u16 currentHeading = PA_GetAngle(px<<8,py<<8,(px<<8)+vx,(py<<8)+vy);
		projectile->setHeading(currentHeading<<8);
		projectile->getSpriteInfo()->setAngle(currentHeading<<8);
		
		//Simple out of level check
		if(px>currentLevel->levelWidth||px<0||py<0-(currentLevel->levelHeight-SHEIGHT)){
			it=projectiles.erase(it);
			continue;
		}
		
		//If ttl has run out explode it
		if(ttl>0){
			projectile->setTtl(ttl--);
			if(ttl==0){
				projectile->explode();
			}
		}
		//If it collides with landscape explode it
		if(landscapeCollision(px,py)){
			projectile->explode();
		}
		//Now check collision with actual game objects
		projectileCollision(projectile);
		
		//Check for out of level bounds
		if(px<0||px>currentLevel->levelWidth||py<SHEIGHT-currentLevel->levelHeight){
			it=projectiles.erase(it);
			continue;
		}

		//If somehow it exploded erase it and add explosion animation if it has one!
		if(projectile->isExploded()){
			if(projectile->getName()->compare("bomb") == 0){
				PA_PlaySound(PA_GetFreeSoundChannel(),player_bombhit_sfx,(u32)player_bombhit_sfx_size,127,44100);
			}
			if(projectile->hasExplosionAnimation())addExplosionAnimationFromProjectile(projectile);
			it=projectiles.erase(it);
			continue;
		}
		it++;
	}
	
}
void InGame::addExplosionAnimationFromProjectile(ProjectileObject* projectile){
	if(landscapeCollision(projectile->getX()>>8,projectile->getY()>>8)){
		projectile->setY((getHeightAtPoint(projectile->getX()>>8)<<8)); //Push projectile out of the ground
	}
	ParticleObject* particle = new ParticleObject(*projectile->getExplosionAnimation());
	s32 startx = projectile->getX()-((particle->getSpriteInfo()->getSpriteWidth()/2)<<8);
	s32 starty = projectile->getY()-((particle->getSpriteInfo()->getSpriteHeight()/2)<<8);
	
	//If projectile uses sprite current position will jst be top left of it..we need to find front middle
	if(projectile->getSpriteInfo()->getUsesSprite()){
		getMiddleEndOfObject((GameObject*)projectile,startx,starty,1);
		startx<<=8;
		starty<<=8;
		startx-=((particle->getSpriteInfo()->getSpriteWidth()/2)<<8);
		starty-=((particle->getSpriteInfo()->getSpriteHeight()/2)<<8);
	}
	
	particle->setX(startx);
	particle->setY(starty);
	particles.push_back(particle);

	//If particle had sprite ref add it back to the pool at set it to invisible
	s16 spriteRef = projectile->getSpriteInfo()->getSpriteIndex();
	if(spriteRef!=-1){
		spritePool.push_back(spriteRef);
		PA_SetSpriteY(0,spriteRef,193);
		projectile->getSpriteInfo()->setSpriteIndex(-1);
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
		si->setAngle(wrapBigAngle(si->getAngle()+po->rotSpeed));
		po->setX(po->getX()+po->vx);
		po->setY(po->getY()+po->vy);
		if((po->getX()>>8)>0&&(po->getX()>>8)<currentLevel->levelWidth){
			if(po->isHeavy()){
				if(!particleLandscapeCollision(po)){
					po->vy+=GRAVITY;
				}
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
	if((po->getY()>>8)<getHeightAtPoint((po->getX()>>8)+po->getObjectHeight())-20){
		return false;
	}
	
	SpriteInfo* si = po->getSpriteInfo();

	//Ok so we know we are close enough so start with getting the center of the particle as reference
	s32 cx = po->getX()+((si->getSpriteWidth()/2)<<8);
	s32 cy = po->getY()+((si->getSpriteHeight()/2)<<8);
	
	//What angle is particle currently facing towards
	s16 facingAngle = si->getAngle();
	s16 facingComponentx = PA_Cos(facingAngle);
	s16 facingComponenty = -PA_Sin(facingAngle);
	s16 facingLeft = (facingAngle>128&&facingAngle<384)? 1:-1;
	
	//What angle is particle currently heading towards
	u16 headingAngle =  PA_GetAngle(po->getX(),po->getY(), po->getX()+po->vx,po->getY()+po->vy);
	
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
		po->setY((po->getY()-highestOverlap)+256);

		//Figure out reflection angle and apply it to current velocity
		u16 reflectAngle = reflectOverNormal(headingAngle,getNormalAtPoint(collisionx>>8));
		u16 currentSpeed=getSpeedFromVelocity(po->vx,po->vy);
		//if(currentSpeed<=1000){currentSpeed=0;}
		po->vx=((currentSpeed*PA_Cos(reflectAngle))>>8);
		po->vy=((currentSpeed*-PA_Sin(reflectAngle))>>8);
		
		//For each collision slow down veclocity and rotate speed
		po->rotSpeed/=3;
		po->vx=(po->vx*180)>>8;
		po->vy=(po->vy*180)>>8;
	}
	return collision;
}

u16 InGame::wrapAngle(s16 angle){
	return (angle>511)? angle&511:(angle<0)? 512+angle:angle;
}

u32 InGame::wrapBigAngle(s32 angle){
	return (angle>(511<<8))? angle&((512<<8)-1):(angle<0)? (512<<8)+angle:angle;
}

/**
Do drawing function
**/
void InGame::doDrawing(void){	
	//Clear bg so we can draw on screen!
	PA_Clear8bitBg(0);

	//Set alpha level for bottom screen
	PA_SetSFXAlpha(0,3,15);

	//Draw plane
	drawPlayer();

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

	//Draw AI
	drawAI();
}
/**
This does not use drawObject function since it has to do quirky
animations based on its current heading!
**/
void InGame::drawPlayer(){
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

	s32 planex =((player->getX()-viewportx)>>8);
	s32 planey =((player->getY()-viewporty)>>8);
	
	//Dont draw it if its offscreen
	if(planex+32<0||planex>SWIDTH||planey+32<0||planey>SHEIGHT){return;}
	PA_SetSpriteXY(0,si->getSpriteIndex(), planex, planey);
	PA_SetSpriteExtPrio(0,0,0);
}
void InGame::drawRunway(){

	vector<GameObject*>::iterator it;
	it = runwayObjects.begin();

	while( it != runwayObjects.end()){
		GameObject* runwayPiece = (*it);
		drawObject(runwayPiece);
		it++;
	}

}
void InGame::drawProjectiles(){

	vector<ProjectileObject*>::iterator it;
	it = projectiles.begin();

	while( it != projectiles.end()) {
		ProjectileObject* projectile = (*it);
		drawObject(projectile);
		it++;
	}
}
void InGame::drawParticles(){
	if(particles.empty())return;

	vector<ParticleObject*>::iterator it;
	it = particles.begin();

	while( it != particles.end()) {
		ParticleObject* po = (*it);
		drawObject(po);
		it++;
	}
}

void InGame::drawLandscapeObjects(){
	vector<DestructableObject*>::iterator it;
	it = landscapeObjects.begin();

	while( it != landscapeObjects.end()) {
		DestructableObject* lo = (*it);
		drawObject(lo);
		it++;
	}
}

void InGame::drawAI(){
	vector<AIObject*>::iterator it;
	it = AIObjects.begin();

	while( it != AIObjects.end()) {
		AIObject* ai = (*it);
		//Need logic to draw all hardpoints as well


		it++;
	}
}


void InGame::drawObject(GameObject* go){
		//Do dynamic allocation/deallocation of sprite indexes and rot indexes
		s16 finalx = (go->getX()>>8)-getViewPortX();
		s16 finaly = (go->getY()>>8)-getViewPortY();

		SpriteInfo* si = go->getSpriteInfo();
		//SpriteInfo handles all the particulars like is this object animated....we jst call updateSpriteFrame
		si->updateSpriteFrame();

		s16 spriteIndex = si->getSpriteIndex();
		s16 rotIndex = si->getRotIndex();
		u16 width = si->getSpriteWidth();
		u16 height = si->getSpriteHeight();
		u16 priority = si->getPriority();

		//Check if particle is offscreen...if so make sure it doesnt have a sprite index
		if(finalx+width<0||finalx>SWIDTH||finaly>SHEIGHT||finaly+height<0){
			releaseObjectResources(go);
		}
		//If its on screen test if it has a sprite index yet
		else{
			//It needs a sprite index assign one
			if(spriteIndex==-1&&si->getUsesSprite()){
				spriteIndex = spritePool.back();
				spritePool.pop_back();
				si->setSpriteIndex(spriteIndex);
				//If we are using animations create new physical sprite object rather then reference
				if(si->usesAnimation()){
					PA_CreateSprite(0, spriteIndex,si->getData(),si->getObjShape(),si->getObjSize(), 1,si->getPaletteIndex(),finalx,finaly);
				}
				else{
					PA_CreateSpriteFromGfx(0, spriteIndex,si->getGfxRef(),si->getObjShape(),si->getObjSize(), 1,si->getPaletteIndex(),finalx,finaly);
				}
				
				if(si->getUsesAlpha()){
					PA_SetSpriteMode(0,spriteIndex,1);
				}
			}
			//It already has a sprite index to move it
			else if(si->getUsesSprite()){
				PA_SetSpriteExtPrio(0,spriteIndex,priority);
				PA_SetSpriteDblsize(0,spriteIndex,si->getDoubleSize());
				PA_SetSpriteXY(0,spriteIndex,finalx,finaly);
				if(si->getUsesAlpha()){
					PA_SetSpriteMode(0,spriteIndex,1);
				}
			}
			//Draw object as line
			else{
				u16 currentHeading = go->getHeading()>>8;
				s16 componentx = PA_Cos(currentHeading);
				s16 componenty = -PA_Sin(currentHeading);

				s16 beginx = finalx - ((componentx*go->getObjectHeight())>>8);
				s16 beginy = finaly - ((componenty*go->getObjectHeight())>>8);

				if(beginx<0){beginx=0;}
				if(beginy<0){beginy=0;}

				if(finalx<0){finalx=0;}
				if(finaly<0){finaly=0;}
				
				PA_Draw8bitLineEx (0,beginx,beginy,finalx,finaly,si->getPaletteIndex(),go->getObjectWidth());	
			
			}
			if(rotIndex==-1&&si->getUsesRotZoom()){	//Make sure it has a rot index and enable rotation for that index
				rotIndex = rotPool.back();
				rotPool.pop_back();
				si->setRotIndex(rotIndex);
				PA_SetSpriteRotEnable(0, spriteIndex,rotIndex);
				PA_SetRotset(0, rotIndex, si->getAngle()>>8,si->getZoom(),si->getZoom());
			}
			else if(si->getUsesRotZoom()){
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
				PA_SetSpriteExtPrio(0,availableIndex,5);
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
				PA_SetSpriteExtPrio(0,availableIndex,5);
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

u16 InGame::playerLandscapeCollision(){
	if(player->onRunway){return 0;}

	s32 tipxFront = 0;
	s32 tipyFront = 0;
	s32 tipxBack = 0;
	s32 tipyBack = 0;
	getBottomEndOfObject(player,tipxFront,tipyFront,1);
	getBottomEndOfObject(player,tipxBack,tipyBack,-1);
	
	//Now collision tests
	return(landscapeCollision(tipxFront,tipyFront)||landscapeCollision(tipxBack,tipyBack));
}

/**
Utility function, bottom front of planes are used for collisions
**/
void InGame::getBottomEndOfObject(GameObject* go,s32 &x,s32 &y,s16 direction){
	SpriteInfo* si = go->getSpriteInfo();
	u16 radius = go->getObjectWidth()/2;

	s16 currentHeading = (go->getHeading()>>8);
	s16 normalAdjust = (currentHeading<128||currentHeading>384)? -128:128;

	s16 normalAngle = currentHeading+normalAdjust;

	if(normalAngle<0)normalAngle+=512;
	if(normalAngle>511)normalAngle &= 512;

	u16 cx = (go->getX()>>8)+(si->getSpriteWidth()/2);
	u16 cy = (go->getY()>>8)+(si->getSpriteHeight()/2);

	s16 bottomx = cx+((PA_Cos(normalAngle) * (go->getObjectHeight()/2))>>8);
	s16 bottomy = cy+((-PA_Sin(normalAngle) * (go->getObjectHeight()/2))>>8);
	
	s16 xComponent =PA_Cos(go->getHeading()>>8);
	s16 yComponent =-PA_Sin(go->getHeading()>>8);

	x = bottomx+(((xComponent*radius)>>8)*direction);
	y = bottomy+(((yComponent*radius)>>8)*direction);
}
/**
Utility function, middle front or back of game objects (currently used for projectile bombs)
**/
void InGame::getMiddleEndOfObject(GameObject* go,s32 &x,s32 &y,s16 direction){
	SpriteInfo* si = go->getSpriteInfo();
	u16 radius = go->getObjectWidth()/2;

	u16 cx = (go->getX()>>8)+(si->getSpriteWidth()/2);
	u16 cy = (go->getY()>>8)+(si->getSpriteHeight()/2);

	s16 xComponent =PA_Cos(go->getHeading()>>8);
	s16 yComponent =-PA_Sin(go->getHeading()>>8);

	x = cx+(((xComponent*radius)>>8)*direction);
	y = cy+(((yComponent*radius)>>8)*direction);
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

u16 InGame::flipAngle(s16 angle){
	return wrapAngle(angle-256);
}

s16 InGame::wrapAngleDistance(u16 angle1,u16 angle2){
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
	SpriteInfo* so = new SpriteInfo(32,32,0,plane_gfx,0,0,0,OBJ_SIZE_32X32,256,0,2,true,true,false);
	player = new PlayerObject(runwayStart<<8,(runwayHeight+12)<<8,16,8,0,0,0,9,60,so,new ParticleObject(*particleReferenceObjects[4]));
	scrollBackToRunway();
}
void InGame::planeCrashParticles(){
	//Add particles from object 
	addParticlesFromObject(player);

	//Little hack to make sure that viewport does not jump when tracking last particle
	ParticleObject* po = particles.back();
	po->setX(player->getX());
	po->setY(player->getY());
	
	//Make sure viewport is kept on last particle
	for(u16 i=0;i<player->getParticleSpriteInstance()->getTtl()-60;i++){
		drawParticles();
		updateParticles();
		updateProjectiles();
		ParticleObject* po = particles.back();	//Particle at end will always been bit of plane since this function is called immediately after crash
		player->setX(po->getX());
		player->setY(po->getY());
		updateViewport();
		doDrawing();

		print_debug();
		PA_WaitForVBL();
	}
}

void InGame::scrollBackToRunway(){
	s16 xComponent = PA_Cos(player->getHeading()>>8);
	u16 adjust = (player->getSpeed()<MINPLANESPEED)? player->getSpeed():MINPLANESPEED;

	s32 targetviewportx = player->getX()+(16<<8)-((SWIDTH/2)<<8)+((player->vx)*40)-((xComponent*((adjust)*40))>>8);
	s32 targetviewporty = player->getY()-((SHEIGHT/2)<<8);
	if(targetviewporty>0){targetviewporty=0;}
	if(targetviewportx<0){targetviewportx=0;}

	s16 goingDown = viewporty<targetviewporty;

	//Do transition back to runway
	while(viewportx!=targetviewportx||viewporty!=targetviewporty){
		if(viewportx>targetviewportx)viewportx-=1600;
		if(goingDown)viewporty+=1600;
		if(!goingDown)viewporty-=1600;
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
	PA_OutputText(1,0, 1, "Viewport x:%d y:%d", getViewPortX(),getViewPortY());
	PA_OutputText(1,0, 2, "Plane x:%d y:%d", player->getX()>>8,player->getY()>>8);
	PA_OutputText(1,0, 3, "Plane vx:%d vy:%d", player->vx,player->vy);
	PA_OutputText(1,0, 4, "Landscape sprites used:%d", landscapeIndexs.size());
	PA_OutputText(1,0, 5, "Available sprites:%d", spritePool.size());
	PA_OutputText(1,0, 6, "Plane taking off:%d", player->takingOff);
	PA_OutputText(1,0, 7, "Particle count:%d", particles.size());
	PA_OutputText(1,0, 8, "Projectile count:%d", projectiles.size());
	PA_OutputText(1,0, 9,"Rot pool size:%d", rotPool.size());
	PA_OutputText(1,0, 10,"Bombs: %d Ammo: %d Fuel: %d Health %d",player->totalBombs,player->totalAmmo,player->totalFuel,player->getHealth());
	PA_OutputText(1,0, 14,"Player test :%d", landscapeObjects.at(0)->getSpriteInfo()->usesAnimation());

	if(particles.size()>0){
		ParticleObject* dest = particles.at(0);
		
		PA_OutputText(1,0, 15,"Test: %d %d",dest->getX()>>8,dest->getY()>>8);
	}
}
