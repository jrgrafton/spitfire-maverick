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

//Destructable objects and lookup
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

inline s16 InGame::getViewPortX() const{
	return viewportx>>8;
}
inline s16 InGame::getViewPortY() const{
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
	
	//Init backgrounds
	PA_Init8bitBg(0,0); //Init backgrounds
	PA_Init8bitBg(1,0);

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
	SpriteInfo* aaTurretSprite = new SpriteInfo(64,32,0,aaTurretSpriteGfx,-1,-1,7,OBJ_SIZE_64X32,400,1,6,true,true,false); //Prio set to 6 to draw it behind landscape
	ProjectileObject* aaTurrentProjectile = projectileReferenceObjects[4];
	HardpointObject* aaTurret = new HardpointObject(0,0,64,32,128<<8,aaTurretSprite,0,4,60,100,256,0,256,aaTurrentProjectile);
	
	//Now lets lookup base references
	u16 aaBaseSpriteGfx =  PA_CreateGfx(0, (void*)nazi1_base_image_Sprite, OBJ_SIZE_32X32,1);
	u16 aaBaseSpriteDestGfx =  PA_CreateGfx(0, (void*)nazi1_base_dest_image_Sprite, OBJ_SIZE_32X32,1);
	SpriteInfo* aaBaseSprite = new SpriteInfo(32,32,0,aaBaseSpriteGfx,-1,-1,7,OBJ_SIZE_32X32,400,0,3,true,true,false);
	AIObject* aaBase = new AIObject(0,0,32,32,0,aaBaseSprite,1000,5,aaBaseSpriteDestGfx,new ParticleObject(*particleReferenceObjects[1]),true,aaTurret,AIObject::STAYINGSTILL,AIObject::NAZI);
	
	AIReferenceObjects[0] = aaBase;
}

void InGame::addLandscapeObject(s32 x,u16 ref){
	DestructableObject* object = new DestructableObject(*landscapeReferenceObjects[ref]);
	SpriteInfo* si = object->getSpriteInfo();
	s32 y = getHeightAtPoint(x+si->getSpriteWidth()/2);
	s32 offset = si->getSpriteHeight();
	object->setLocation(x<<8,(y-offset+4)<<8);
	landscapeObjects.push_back(object);
}

void InGame::addAIObject(s32 x,u16 ref){
	AIObject* object = new AIObject(*AIReferenceObjects[ref]);
	SpriteInfo* si = object->getSpriteInfo();
	s32 y = getHeightAtPoint(x+si->getSpriteWidth()/2);
	s32 offset = (si->getSpriteHeight()/2)+(object->getObjectHeight()/2);
	object->setLocation(x<<8,(y-offset)<<8);
	AIObjects.push_back(object);
}

void InGame::initProjectileLookup(){
	SpriteInfo* bulletSpriteInfo = new SpriteInfo(-1,-1,0,-1,-1,-1,5,0,0,256,0,2,false,false,false);
	ProjectileObject* playerBulletObject = new ProjectileObject(new string("bullet"),0,0,1,6,0,0,0,bulletSpriteInfo,-1,1,10,true);

	u16 bombGfx = PA_CreateGfx(0, (void*)bomb_image_Sprite, OBJ_SIZE_16X8, 1);
	SpriteInfo* bombSpriteInfo = new SpriteInfo(16,8,0,bombGfx,-1,-1,5,OBJ_SIZE_16X8,540,1,2,true,true,false);
	ProjectileObject* playerBombObject = new ProjectileObject(new string("bomb"),0,0,16,8,0,0,0,bombSpriteInfo,-1,16,1000,true,new ParticleObject(*particleReferenceObjects[6]));
	
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
	

	//For testing purposes atm havnt bothered doing any file reading and parsing properly yet,
	//Wont bother doing it until game is complete.
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

		u16 getLevelWidth = (heightMap->size()-2)*16;
		u16 getLevelHeight = (u16)(SHEIGHT*2);
	
		//Init level with full heightmap
		currentLevel = new Level(getLevelWidth,getLevelHeight,levelTitle,heightMap);

		//Init runway
		initRunway();

		//Test landscape algorithm
		addLandscapeObject(750,0);
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

		////Test AI algorithm
		addAIObject(680,0);
		addAIObject(640,0);
		addAIObject(600,0);
		addAIObject(560,0);
		addAIObject(520,0);
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

	PA_PlaySoundEx2(0,plane_engine_sfx,(s32)plane_engine_sfx_size,player->getSpeed()/1109,44100,0,true,0);
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
	if(Pad.Held.R&&!player->isOnRunway()){
		if(player->canShoot()){player->setTimeSinceFired(0);addPlayerBullet();}
	}
	if(Pad.Newpress.L&&!player->isOnRunway()){
		if(player->canBomb()){player->setTimeSinceBombed(0);addPlayerBomb();}
	}
	if(Pad.Held.B){
		player->setThrottle(true);
	}
	else{
		player->setThrottle(false);
	}
	if(Pad.Held.Left&&!player->isOnRunway()){
		player->setHeading(player->getHeading()+PLANETURNSPEED);
		if(player->getHeading()>(511<<8)){player->setHeading(0);}
	}
	if(Pad.Held.Right&&!player->isOnRunway()){
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
	
	player->setTotalAmmo(player->getTotalAmmo()-1);

	s16 componentx = PA_Cos(player->getHeading()>>8);
	s16 componenty = -PA_Sin(player->getHeading()>>8);
	
	s32 cx = player->getX()+((planeSo->getSpriteWidth()/2)<<8);
	s32 cy = player->getY()+((planeSo->getSpriteHeight()/2)<<8);

	s32 startx = cx+(componentx*(player->getObjectWidth()/2));
	s32 starty = cy+(componenty*(player->getObjectWidth()/2));
	
	s32 heading = player->getHeading()>>8;

	s16 vx = (PA_Cos(heading)*(player->getSpeed()+1000)>>8);
	s16 vy = (-PA_Sin(heading)*(player->getSpeed()+1000)>>8)+PA_RandMax(64);
	
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
	player->setTotalBombs(player->getTotalBombs()-1);


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
	bomb->setVx(player->getVx());
	bomb->setVy(player->getVy());
	bomb->setHeading(heading);

	projectiles.push_back(bomb);
}

/**
** Update plane location and viewport
**/
void InGame::doUpdates(){

	//Update Plane
	if(!player->getDestroyed()){updatePlayer();}

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
		//This will make hardpoint turn towards target (if applicable) and update render offset
		ai->updateHardpoint();
		
		//If AI does not have a current target try and aquire one
		if(!ai->hasTarget()&&!player->getDestroyed()){
			ai->setTarget(player);
			//vector<AIObject*>::iterator targetit;
			//targetit = AIObjects.begin();
			//while( targetit != AIObjects.end()) {
			//	AIObject* targetai = *targetit;
			//}
		}
		it++;
	}
}

void InGame::updatePlayer(){
	//Update engine sound
	PA_SetSoundChannelVol(0,(player->getSpeed()<<8)/1109);
	
	//Increase time since fired/bombed
	player->setTimeSinceFired(player->getTimeSinceFired()+1);
	player->setTimeSinceBombed(player->getTimeSinceBombed()+1);
	
	//Make sure heading is tied to sprite rotation
	player->getSpriteInfo()->setAngle(player->getHeading());
	s16 heading = player->getHeading()>>8;

	//Check for takeoff
	if(player->isOnRunway()&&(((player->getX()>>8)+player->getObjectWidth()>runwayEnd&&heading==0)||((player->getX()>>8)<runwayStart&&heading==256))&&player->getTakingOff()==1){
		if(player->getSpeed()<MINPLANESPEED){playerCrash();return;}	//Crash if we have reached the end of runway and we are not fast enough
		player->setOnRunway(0);
		player->setTakingOff(0);
		if(heading==256){player->setHeading(246<<8);}
		else{player->setHeading(10<<8);}
	}

	s32 vy = player->getVy();
	s32 vx = player->getVx();
	s32 speed =0;
	
	if(!player->isOnRunway()){
		if(player->getTotalFuel()>0)player->setTotalFuel(player->getTotalFuel()-1);
		//PROPER ALGORITHM
		vy+=GRAVITY;
		speed = getSpeedFromVelocity(vx,vy);//Base speed calculated from vx and vy

		//Increase speed if holding up
		if(player->getTotalFuel()>0)speed+=player->isThrottleOn()*PLANEPOWER;
		
		//Take off friction
		speed*=FRICTION;
		speed=speed>>16;

		//Speed kept in certain range
		if(speed<MINPLANESPEED){speed=MINPLANESPEED;}
		if(speed>MAXPLANESPEED){speed=MAXPLANESPEED;}
	}
	else{
		//If we are on the runway the throttle is on and we are not yet taking off
		//Make sure we are! (taking off is 1,0 for in air and -1 for landing)
		if(player->isOnRunway()&&player->isThrottleOn()&&player->getTakingOff()==0){
			player->setTakingOff(1);
		}
		speed = getSpeedFromVelocity(vx,vy);//Base speed calculated from vx and vy
		if(!player->isThrottleOn()&&player->getTakingOff()==1){player->setThrottle(1);} //Throttle always on when taking off
		speed+=player->isThrottleOn()*PLANEPOWER*player->getTakingOff();
		
		//Check for end of runway
		if(player->getTakingOff()==-1&&player->getSpeed()>0&&((player->getX()>>8)+player->getObjectWidth()>runwayEnd||(player->getX()>>8)<runwayStart)){
			playerCrash();
		}

		//Do a refeuling here turning around etc if landing
		if(speed<=0&&player->getTakingOff()==-1){
			speed=0;
			player->setTakingOff(0);
			player->restock();
			//Flip planes current facing
			if(heading>128&&heading<384){player->setHeading(0);}
			else{player->setHeading(256<<8);}
		}
		//Make sure we dont exceed min speed when taking off
		if(speed>MINPLANESPEED&&player->getTakingOff()==1){speed=MINPLANESPEED;}
	}
	
	//Possibly add some smoke
	player->setTimeSinceLastSmoked(player->getTimeSinceLastSmoked()+1);
	if(player->getTimeSinceLastSmoked()>player->getSmokingInterval()&&player->getHealth()<50){
		ParticleObject* po = new ParticleObject(*(particleReferenceObjects[1]));
		SpriteInfo* si = po->getSpriteInfo();
		po->setVy(15+PA_RandMax(15)); //WTF HEAVY SMOKE?
		po->setVx(player->getVx()/7);
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
	if(player->getTotalFuel()>0){
		player->setVx((PA_Cos(heading)*speed)>>8);
		player->setVy((-PA_Sin(heading)*speed)>>8);
	}
	else{
		if(player->getVx()>0)player->setVx(player->getVx()-1);
		if(player->getVy()<MAXPLANESPEED)player->setVy(player->getVy()+GRAVITY);
		player->setHeading(PA_GetAngle(player->getX(),player->getY(),player->getX()+player->getVx(),player->getY()+player->getVy())<<8);
	}	//Hurtle towards earth if no fuel
	
	player->setX(player->getX()+player->getVx());
	player->setY(player->getY()+player->getVy());
	player->setSpeed(speed);

	u16 contactingRunway = ((player->getX()>>8)>runwayStart&&(player->getX()>>8)<runwayEnd)&&collideObject(player,&InGame::rotatedRectangleCollision,true,false,false,false,false);

	//If plane is currently in the air and is contacting the runway
	if((player->getTakingOff()==0&&contactingRunway)){
		if((heading>255&&heading<281)||(heading>486)){
			if(player->getSpeed()>MINPLANESPEED+100){playerCrash();return;}//Need to be going at slowly to land
			player->setTakingOff(-1);
			player->setOnRunway(1);
			player->setY((runwayHeight+12)<<8);
			if(heading>128&&heading<384){player->setHeading(256<<8);}
			else{player->setHeading(0);}
		}
		else{
			playerCrash();return; //Crash since we came at the runway at to steep angle
		}
	}

	//If the plane is going to be completely rendered above the y threshold reflect its current angle
	if((player->getY()>>8)<(SHEIGHT-currentLevel->getLevelHeight())-50){
		player->setHeading(reflectOverNormal(player->getHeading()>>8,384)<<8);
	}

	//If the plane is going to be completely off the x axis the reflect its current angle
	if((player->getX()>>8)+32<-5){
		player->setHeading(reflectOverNormal(player->getHeading()>>8,0)<<8);
	}

	else if((player->getX()>>8)>currentLevel->getLevelWidth()+5){
		player->setHeading(reflectOverNormal(player->getHeading()>>8,256)<<8);
	}
	if(player->getTakingOff()==0&&!contactingRunway){playerCollisions();}
}

void InGame::playerCollisions(){
	if(collideObject(player,&InGame::rotatedRectangleCollision,true,true,false,true,false)){
		playerCrash();
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
	
		s16 particleVx = (destructable->getVx()==0)?PA_RandMax(256)-256:destructable->getVx();
		s16 particleVy = (destructable->getVy()==0)?PA_RandMax(256)-256:destructable->getVy();
		
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

/**
Assumes that go1 represents single point
and go2 represents rect bounding box

@go1 => Represents single point
@go2 => Represents bounding box

**/
bool InGame::pointInRectangleCollision(GameObject* go1,GameObject* go2) const{
	s16 pointx = go1->getX()>>8;
	s16 pointy = go1->getY()>>8;

	s16 cx = (go2->getX()>>8)+go2->getSpriteInfo()->getSpriteWidth()/2;
	s16 cy = (go2->getY()>>8)+go2->getSpriteInfo()->getSpriteHeight()/2;

	s16 rectanglex = cx - go2->getObjectWidth();
	s16 rectangley = cy - go2->getObjectHeight();
	u16 width =go2->getObjectWidth();
	u16 height =go2->getObjectHeight();
	
	if(pointx>rectanglex&&pointx<rectanglex+width){
		if(pointy>rectangley&&pointy<rectangley+height){
			return true;
		}
	}
	return false;
}
/**
Reasonably fast circle collision, overlap needs to be sqrtd to be accurate
works by getting closest corner and doing squared distance to center of circle.

@go1 => circle object...object width is taken as explosion radius
@go2 => rect object 
**/
bool InGame::circleAndRectangleCollision(GameObject* go1,GameObject* go2) const{
	SpriteInfo* si = go2->getSpriteInfo();
	s16 x0 = ((go2->getX()>>8)+(si->getSpriteWidth()/2)) -go2->getObjectWidth()/2;
	s16 y0 = ((go2->getY()>>8)+(si->getSpriteHeight()/2)) -go2->getObjectHeight()/2;
	s16 w0 = go2->getObjectWidth();
	s16 h0 = go2->getObjectHeight();
	
	s16 cx = (go1->getX()>>8)+go1->getSpriteInfo()->getSpriteWidth()/2;
	s16 cy = (go1->getY()>>8)+go1->getSpriteInfo()->getSpriteHeight()/2;
	u16 radius =  go1->getObjectWidth()/2;
	
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
	u16 adjust = ((player->getSpeed()>>8)<MINPLANESPEED)? (player->getSpeed()>>8):MINPLANESPEED;
	viewportx = player->getX()+(16<<8)-((SWIDTH/2)<<8)+((player->getVx())*40)-((xComponent*((adjust)*40))>>8);
	viewporty = player->getY()-((SHEIGHT/2)<<8);

	if(getViewPortX()<0){viewportx=0;}
	if(getViewPortX()+SWIDTH>currentLevel->getLevelWidth()){viewportx=((currentLevel->getLevelWidth()-SWIDTH)<<8);}

	if(getViewPortY()<SHEIGHT-currentLevel->getLevelHeight()){viewporty=((SHEIGHT-currentLevel->getLevelHeight())<<8);}
	if(getViewPortY()>0){viewporty=0;}
	
	//Scroll background
	if(getViewPortX()>0&&getViewPortX()+SWIDTH<currentLevel->getLevelWidth()&&!player->getDestroyed()){
		bgscroll+=(((abs(player->getVx())*(player->getSpeed())>>8))/7*xflipped);
		PA_BGScrollX(0,2,bgscroll>>8);
	}
}

/**
Idea is to provide a generic function for all objects to collide with all
others. Assuming that all objects are rotated rects....4 vertices of game object are found
and the those points are checked against collisions with objects that they can collide with.

Params:

@object => Object to check for collisions
@collisionRoutine => Collision routine that this object uses...either point in rect rot rect or circle rect
@landscape => Does this object collide with the landscape
@landscapeObj => Does this object collide with landscapeObjects
@player => Does this object collide with the player
@ai => Does this object collide with the AI
**/

bool InGame::collideObject(GameObject* go,bool (InGame::* collisionRoutine) (GameObject* go1,GameObject* go2) const,bool landscape,bool landscapeObj,bool player,bool ai, bool reflect){
	bool collision = false;

	if(landscape){
		if(landscapeCollision(go,reflect))collision=true;
	}
	if(landscapeObj){
		if(landscapeObjectCollision(go,collisionRoutine))collision=true;
	}
	if(ai){
		if(AIObjectCollision(go,collisionRoutine))collision=true;
	}
	return collision;
}

bool InGame::landscapeCollision(GameObject* go,bool reflect){
	
	bool collision = false;
	bool singlePointCollision = (!go->getSpriteInfo()->getUsesSprite());
	u16 highestOverlap=0;
	s32 collisionx=0;		//Used for finding normal of landscape at collision point
	
	//Setup collision vertices
	s32 v0[2];
	s32 v1[2];
	s32 v2[2];
	s32 v3[2];
	getVertices(go,v0,v1,v2,v3);
	
	//If single point collision just collide with one point!
	if(singlePointCollision){
		v0[0]=go->getX();
		v0[1]=go->getY();
	}
	if(landscapePointCollision(v0[0]>>8,v0[1]>>8)){
		collision=true;
		s16 overlap = v0[1]-(getHeightAtPoint(v0[0]>>8)<<8);
		if(overlap>highestOverlap){highestOverlap=overlap;collisionx=v0[0];}
	}
	
	if(!singlePointCollision){
		if(landscapePointCollision(v1[0]>>8,v1[1]>>8)){
			collision=true;
			s16 overlap =v1[1]-(getHeightAtPoint(v1[0]>>8)<<8);
			if(overlap>highestOverlap){highestOverlap=overlap;collisionx=v1[0];}
		}

		if(landscapePointCollision(v2[0]>>8,v2[1]>>8)){
			collision=true;
			s16 overlap = v2[1]-(getHeightAtPoint(v2[0]>>8)<<8);
			if(overlap>highestOverlap){highestOverlap=overlap;collisionx=v2[0];}
		}

		if(landscapePointCollision(v3[0]>>8,v3[1]>>8)){
			collision=true;
			s16 overlap = v3[1]-(getHeightAtPoint(v3[0]>>8)<<8);
			if(overlap>highestOverlap){highestOverlap=overlap;collisionx=v3[0];}
		}
	}
	

	//If object is to be reflected move it out of landscape and reflect.
	if(collision&&reflect){
		u16 headingAngle =  PA_GetAngle(go->getX(),go->getY(), go->getX()+go->getVx(),go->getY()+go->getVy());

		//Make sure object is moved out of scenery...Should really translate back along
		//normal however its faster and easier to translate str8 up.
		go->setY((go->getY()-highestOverlap)+256);

		//Figure out reflection angle and apply it to current velocity
		u16 reflectAngle = reflectOverNormal(headingAngle,getNormalAtPoint(collisionx>>8));
		u16 currentSpeed=getSpeedFromVelocity(go->getVx(),go->getVy());
		go->setVx(((currentSpeed*PA_Cos(reflectAngle))>>8));
		go->setVy(((currentSpeed*-PA_Sin(reflectAngle))>>8));
		
		//For each collision slow down veclocity	
		go->setVx((go->getVx()*180)>>8);
		go->setVy((go->getVy()*180)>>8);

		//If we are dealing with a particle slow down rotation speed
		ParticleObject* po = dynamic_cast<ParticleObject*>(go);
		if(po!=NULL){
			po->setRotSpeed(po->getRotSpeed()/3);
		}
	}
	return collision;
}

bool InGame::landscapePointCollision(s16 x, s16 y) const{
	return(y>getHeightAtPoint(x));
}

bool InGame::landscapeObjectCollision(GameObject * go,bool (InGame::* collisionRoutine) (GameObject* go1,GameObject* go2)const){
	bool collision = false;
	
	u16 objectStrength = numeric_limits<u16>::max();
	//Projectiles are a special case of objectStrength
	ProjectileObject* po = dynamic_cast<ProjectileObject*>(go);
	if(po!=NULL){
		if(po->isExploded()||!po->isExplosive()){
			objectStrength=po->getProjectileStrength();
		}
		//An unexploded explosive projectile has no strength
		else{
			objectStrength=0;
		}
	}
	
	vector<DestructableObject*>::iterator it;
	it = landscapeObjects.begin();

	while( it != landscapeObjects.end()) {
		DestructableObject* destructable = (*it);
		if(destructable->getDestroyed()){it++;continue;}
		if((this->*collisionRoutine)(go,destructable)){
			collision=true;
			destructable->setHealth(destructable->getHealth()-objectStrength);
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

bool InGame::AIObjectCollision(GameObject * go,bool (InGame::* collisionRoutine) (GameObject* go1,GameObject* go2)const){
	bool collision = false;

	u16 objectStrength = numeric_limits<u16>::max();
	//Projectiles are a special case of objectStrength
	ProjectileObject* po = dynamic_cast<ProjectileObject*>(go);
	if(po!=NULL){
		if(po->isExploded()||!po->isExplosive()){
			objectStrength=po->getProjectileStrength();
		}
		//An unexploded explosive projectile has no strength
		else{
			objectStrength=0;
		}
	}
	
	vector<AIObject*>::iterator it;
	it = AIObjects.begin();

	while( it != AIObjects.end()) {
		AIObject* ai = (*it);
		if(ai->getDestroyed()){it++;continue;}
		if((this->*collisionRoutine)(go,ai)){
			collision=true;
			ai->setHealth(ai->getHealth()-objectStrength);
			if(ai->getHealth()<=0){
				ai->destructObject();
				releaseObjectResources(ai);
				releaseObjectResources(ai->getHardpoint());
				addParticlesFromObject(ai);
			}
		}
		it++;
	}
	return collision;
}

/**
Gets the 4 vertices of a game object, sets them using <<8 position
**/
void InGame::getVertices(GameObject* go,s32 *v0,s32 *v1,s32 *v2,s32 *v3) const{
	SpriteInfo* si = go->getSpriteInfo();
	
	s32 cx = go->getX()+((si->getSpriteWidth()/2)<<8);
	s32 cy = go->getY()+((si->getSpriteHeight()/2)<<8);

	//What angle is particle currently facing towards
	s16 facingAngle = si->getAngle()>>8;
	s16 facingComponentx = PA_Cos(facingAngle);
	s16 facingComponenty = -PA_Sin(facingAngle);
	s16 facingLeft = (facingAngle>128&&facingAngle<384)? 1:-1;
	
	//Get the top and bottom middle of the game object
	s16 topNormal = wrapAngle(facingAngle+(-128*facingLeft));
	s16 bottomNormal = wrapAngle(facingAngle+(128*facingLeft));
	
	u16 halfWidth = go->getObjectWidth()/2;		//Half height for finding top and bottom middle
	u16 halfHeight = go->getObjectHeight()/2;	//Half width for translating along side to tip on rect

	s16 componentxTop = PA_Cos(topNormal); 
	s16 componentyTop = -PA_Sin(topNormal);

	s32 topx = cx+halfHeight*componentxTop;
	s32 topy = cy+halfHeight*componentyTop;

	s16 componentxBottom = PA_Cos(bottomNormal); 
	s16 componentyBottom = -PA_Sin(bottomNormal);

	s32 bottomx = cx+halfHeight*componentxBottom;
	s32 bottomy = cy+halfHeight*componentyBottom;

	//Find all the corners of the object
	s32 topLeftx = topx-facingComponentx*halfWidth;
	s32 topLefty = topy-facingComponenty*halfWidth;

	s32 topRightx =topx+facingComponentx*halfWidth; 
	s32 topRighty =topy+facingComponenty*halfWidth;

	s32 bottomLeftx = bottomx-facingComponentx*halfWidth;
	s32 bottomLefty = bottomy-facingComponenty*halfWidth;

	s32 bottomRightx =bottomx+facingComponentx*halfWidth;
	s32 bottomRighty =bottomy+facingComponenty*halfWidth;

	v0[0] = topLeftx;
	v0[1] = topLefty;
	v1[0] = topRightx;
	v1[1] = topRighty;
	v2[0] = bottomLeftx;
	v2[1] = bottomLefty;
	v3[0] = bottomRightx;
	v3[1] = bottomRighty;
}

/**
Since my math is not amazing, i am going to go for a line intersection
algorithm (instead of linear sepparation algorithm). Test if any lines of one 
rect intersect with any lines of the other, as soon as we find a single intersection
there must be a collision!. Optimized by doing a quick circle bounds check first.
**/

bool InGame::rotatedRectangleCollision(GameObject * go1,GameObject* go2) const{

	//First make sure that they r close enuf to collide
	s32 go1cx = (go1->getX()>>8)+go1->getSpriteInfo()->getSpriteWidth()/2;
	s32 go1cy = (go1->getY()>>8)+go1->getSpriteInfo()->getSpriteHeight()/2;

	s32 go2cx = (go2->getX()>>8)+go2->getSpriteInfo()->getSpriteWidth()/2;
	s32 go2cy = (go2->getY()>>8)+go2->getSpriteInfo()->getSpriteHeight()/2;
	
	u32 currentDistSquared = squared(go1cx-go2cx)+squared(go1cy-go2cy);

	//Basically here we are getting the highest dimension (width or height) for each
	//Game object then *1.5 to get circle  of collision each of them. We then get the square
	//of the combined circles radius and compare it to the current dist. If we are closer then this
	//we go on to do full collision check
	u32 minDistSquared = squared((u16)((highest((u16)(go1->getObjectWidth()/2),(u16)(go1->getObjectHeight()/2))*1.5)+(highest((u16)(go2->getObjectWidth()/2),(u16)(go2->getObjectHeight()/2))*1.5)));
	if(currentDistSquared>minDistSquared){return false;}

	//Ok so we know they r quite close so do a full collision check
	Line go1Lines[4];
	Line go2Lines[4];

	//Function to get all lines of rect1
	getLinesForRectangle(go1,go1Lines);

	//Function to get all lines of rect2
	getLinesForRectangle(go2,go2Lines);

	//N^2 loop which tests all lines intersecting all other lines (4v4 so 16 tests)
	PA_ClearTextBg(1);
	for(u16 i =0;i<4;i++){
		for(u16 i2=0;i2<4;i2++){
			if(LineIntersect(go1Lines[i],go2Lines[i2])){
				return true;
			}
		}
	}

	//No intersecting lines found
	return false;
}

void InGame::getLinesForRectangle(GameObject* go,Line* lines) const{
	s32 v0[2];
	s32 v1[2];
	s32 v2[2];
	s32 v3[2];
	
	getVertices(go,v0,v1,v2,v3);
	
	//Top left to top right
	lines[0].o = Vector2D(v0[0]>>8,v0[1]>>8);
	lines[0].p = Vector2D(v1[0]>>8,v1[1]>>8);

	//Top right to bottom right
	lines[1].o = Vector2D(v1[0]>>8,v1[1]>>8);
	lines[1].p = Vector2D(v3[0]>>8,v3[1]>>8);

	//Bottom right to bottom left
	lines[2].o = Vector2D(v3[0]>>8,v3[1]>>8);
	lines[2].p = Vector2D(v2[0]>>8,v2[1]>>8);

	//Bottom left to top left
	lines[3].o = Vector2D(v2[0]>>8,v2[1]>>8);
	lines[3].p = Vector2D(v0[0]>>8,v0[1]>>8);
}

bool InGame::LineIntersect( Line &a, Line &b) const{
	s32 x1 = a.o.x;
	s32 y1 = a.o.y;
	s32 x2 = a.p.x;
	s32 y2 = a.p.y;
	s32 x3 = b.o.x;
	s32 y3 = b.o.y;
	s32 x4 = b.p.x;
	s32 y4 = b.p.y;

    s32 d = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);
    if (d == 0) return false; //No intersect
    
    s32 xi = ((x3-x4)*(x1*y2-y1*x2)-(x1-x2)*(x3*y4-y3*x4))/d;
    s32 yi = ((y3-y4)*(x1*y2-y1*x2)-(y1-y2)*(x3*y4-y3*x4))/d;
	
	//Test if point is on both lines
	if(min(x1,x2)<=xi&&max(x1,x2)>=xi&&min(y1,y2)<=yi&&max(y1,y2)>=yi){
		if(min(x3,x4)<=xi&&max(x3,x4)>=xi&&min(y3,y4)<=yi&&max(y3,y4)>=yi){
			PA_OutputText(1,0, 18, "COLLISION!!!!!!!");
			return true;
		}
	}
	return false;
 }
void InGame::updateProjectiles(){
	vector<ProjectileObject*>::iterator it;
	it = projectiles.begin();

	while( it != projectiles.end()) {
		ProjectileObject* projectile = (ProjectileObject*)(*it);
		
		projectile->setVy(projectile->getVy()+GRAVITY);
		
		projectile->setX(projectile->getX()+projectile->getVx());
		projectile->setY(projectile->getY()+projectile->getVy());
		
		//Need to modify this to find bottom middle of projectile
		s32 px = (projectile->getX()>>8);
		s32 py = (projectile->getY()>>8);
		
		//If projectile uses sprite current position will jst be top left of it..we need to find front middle
		if(projectile->getSpriteInfo()->getUsesSprite()){
			getMiddleEndOfObject((GameObject*)projectile,px,py,1);
		}

		s16 vx = projectile->getVx();
		s16 vy = projectile->getVy();
		s16 ttl = projectile->getTtl();
		
		//Make sure we calculate current heading and turn to face it
		u16 currentHeading = PA_GetAngle(px<<8,py<<8,(px<<8)+vx,(py<<8)+vy);
		projectile->setHeading(currentHeading<<8);
		projectile->getSpriteInfo()->setAngle(currentHeading<<8);
		
		//Simple out of level check
		if(px>currentLevel->getLevelWidth()||px<0||py<0-(currentLevel->getLevelHeight()-SHEIGHT)){
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
		bool collision = false;

		//Projectiles with no sprite use point in rect otherwise they use rot rect routine
		if(projectile->getSpriteInfo()->getUsesSprite()){
			collision = collideObject(projectile,&InGame::rotatedRectangleCollision,true,true,false,true,false);
		}
		else{
			collision = collideObject(projectile,&InGame::pointInRectangleCollision,true,true,false,true,false);
		}

		//If we had a collison explode the projectile
		if(collision){projectile->explode();}

		//If somehow it exploded do another collision checl with explosion then erase
		if(projectile->isExploded()){
			//Collide explosion from projectile (if it has one)
			if(projectile->isExplosive()){
				projectile->setObjectWidth(projectile->getExplosionRadius()*2);
				collideObject(projectile,&InGame::circleAndRectangleCollision,false,true,false,true,false);
			}

			//Ideally would like to do sounds with LUT
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
	if(landscapePointCollision(projectile->getX()>>8,projectile->getY()>>8)){
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
	releaseObjectResources(projectile);
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
		po->setTtl(po->getTtl()-1);
		SpriteInfo* si = po->getSpriteInfo();
		si->setAngle(wrapBigAngle(si->getAngle()+po->getRotSpeed()));
		po->setX(po->getX()+po->getVx());
		po->setY(po->getY()+po->getVy());
		if((po->getX()>>8)>0&&(po->getX()>>8)<currentLevel->getLevelWidth()){
			if(po->isHeavy()){
				if(!collideObject((GameObject*)po,&InGame::rotatedRectangleCollision,true,false,false,false,true)){
					po->setVy(po->getVy()+GRAVITY);
				}
			}
		}
		if(po->getTtl()==0){
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
	if(player->getDestroyed())return;
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
		drawObject(ai);
		if(!ai->getDestroyed())drawObject(ai->getHardpoint());
		it++;
	}
}

/**
Draw object function which allows automatic resource aquisition
and release for game objects via 2D culling
**/
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
	it = currentLevel->getHeightMap()->begin();


	s16 x =0-getViewPortX();
	while( it != currentLevel->getHeightMap()->end()-1) {
		u16 thisHeight = (*it);
		u16 nextHeight = (*(it+1));
		u8 spriteIndex = abs(thisHeight-nextHeight)/2;
		s16 y = lowest(thisHeight,nextHeight);	//Lowest value is actually tallest
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
** inline lower function
**/
u16 InGame::lowest(u16 a,u16 b) const{
	return(a<b)? a:b;
}

/**
** inline smaller function
**/
u16 InGame::highest(u16 a,u16 b) const{
	return(a>b)? a:b;
}

/**
Utility function, bottom front of planes are used for collisions
**/
void InGame::getBottomEndOfObject(GameObject* go,s32 &x,s32 &y,s16 direction) const{
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
void InGame::getMiddleEndOfObject(GameObject* go,s32 &x,s32 &y,s16 direction) const{
	SpriteInfo* si = go->getSpriteInfo();
	u16 radius = go->getObjectWidth()/2;

	u16 cx = (go->getX()>>8)+(si->getSpriteWidth()/2);
	u16 cy = (go->getY()>>8)+(si->getSpriteHeight()/2);

	s16 xComponent =PA_Cos(go->getHeading()>>8);
	s16 yComponent =-PA_Sin(go->getHeading()>>8);

	x = cx+(((xComponent*radius)>>8)*direction);
	y = cy+(((yComponent*radius)>>8)*direction);
}

/**
Note this uses shifted position for y 
**/
u16 InGame::getHeightAtPoint(u16 x) const{
	s16 landIndex = (x/16);
	if((u16)landIndex>currentLevel->getHeightMap()->size()-2||landIndex<0){return SHEIGHT;}
	u32 a = currentLevel->getHeightMap()->at(landIndex)<<8;
	u32 b = currentLevel->getHeightMap()->at(landIndex+1)<<8;
	
	u32 diff = ((x &15)<<8)/16;
	u16 actualHeight = (a+(diff*((b-a)>>8)))>>8;	
	
	return actualHeight;
}

u16 InGame::getNormalAtPoint(u16 x) const{
	s16 landIndex = (x/16);
	u32 a = currentLevel->getHeightMap()->at(landIndex);
	u32 b = currentLevel->getHeightMap()->at(landIndex+1);

	u16 angle = PA_GetAngle(x,a,x+16,b);
	
	//a<b must mean that we are going downhill
	return (a>b)? wrapAngle(angle+128):wrapAngle(angle-128);
}

u16 InGame::reflectOverNormal(u16 angle,u16 normal) const{
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
void InGame::playerCrash(){
	PA_SetSoundChannelVol(0,0);	//Kill engine sound
	PA_PlaySound(PA_GetFreeSoundChannel(),crash_sfx,(u32)crash_sfx_size,127,44100);
	u16 plane_gfx = player->getSpriteInfo()->getGfxRef();
	player->destructObject();
	PA_SetSpriteY(0, 0, 193);	// Hide sprite
	playerCrashParticles();
	delete player;
	SpriteInfo* si = new SpriteInfo(32,32,0,plane_gfx,0,0,0,OBJ_SIZE_32X32,256,0,2,true,true,false);
	player = new PlayerObject(runwayStart<<8,(runwayHeight+12)<<8,16,8,0,0,0,9,60,si,new ParticleObject(*particleReferenceObjects[4]));
	scrollBackToRunway();
}
void InGame::playerCrashParticles(){
	//Add particles from object 
	addParticlesFromObject(player);

	//Little hack to make sure that viewport does not jump when tracking last particle
	ParticleObject* po = particles.back();
	po->setX(player->getX());
	po->setY(player->getY());

	//Make sure viewport is kept on last particle
	for(u16 i=0;i<player->getParticleSpriteInstance()->getTtl()-60;i++){
		doUpdates();
		doDrawing();

		player->setX(po->getX());
		player->setY(po->getY());

		print_debug();
		PA_WaitForVBL();
	}
}

void InGame::scrollBackToRunway(){
	s16 xComponent = PA_Cos(player->getHeading()>>8);
	u16 adjust = ((player->getSpeed()>>8)<MINPLANESPEED)? (player->getSpeed()>>8):MINPLANESPEED;

	s32 targetviewportx = player->getX()+(16<<8)-((SWIDTH/2)<<8)+((player->getVx())*40)-((xComponent*((adjust)*40))>>8);
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
inline u32 InGame::squared(s32 a) const{ return a*a;}

/**
Debug function, put everything here that you
wanna print to screen
**/
void InGame::print_debug(void) const{
	//Put your debug print statements here.... make sure to print to screen 1	
	PA_ClearTextBg(1);
	PA_OutputText(1,0, 0, "Title is: %s", currentLevel->getTitle()->c_str());
	PA_OutputText(1,0, 1, "Landscape sprites used:%d", landscapeIndexs.size());
	PA_OutputText(1,0, 2, "Available sprites:%d", spritePool.size());
	PA_OutputText(1,0, 3, "Particle count:%d", particles.size());
	PA_OutputText(1,0, 4, "Projectile count:%d", projectiles.size());
	PA_OutputText(1,0, 5,"Rot pool size:%d", rotPool.size());
	PA_OutputText(1,0, 6,"Bombs: %d Ammo: %d Fuel: %d Health %d",player->getTotalBombs(),player->getTotalAmmo(),player->getTotalFuel(),player->getHealth());
}
