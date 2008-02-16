//internal headers
#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/state.h"
#include "../header/in_game.h"

//Sound headers

//Fields of game entitys

//Are we in game?
u16 inGame = 0;

//Level stuff
string* levelTitle;
vector<u16> heightMap;
u16 levelWidth;
u16 levelHeight;

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
GameObject* plane;

s16 tipx=0;
s16 tipy=0;

u16 actualHeight =0;
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

}

inline s16 InGame::getViewPortX(){
	return viewportx/255;
}
inline s16 InGame::getViewPortY(){
	return viewporty/255;
}
/**
**InGame init allocate resources
**/
void InGame::init(){
	myName = INGAME;
	
	//Set the virtual distance in screen to SCREENHOLE
	//PA_SetScreenSpace(SCREENHOLE);
	
	//Load game objects
	//Startx starty width height spriteindex
	//Index is after last grasstile
	//Plane position is stored as bottom middle of plane
	plane = new GameObject(128<<8,90<<8,32,32,0);
	plane->speed=1<<8;

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
	
	initGraphics();
}

void InGame::initGraphics(){
	#ifdef DEBUG
		PA_InitText(1,0); // On the top screen
		PA_SetTextCol(1,31,31,31);
	#endif
	PA_SetBgPalCol(0, 2, PA_RGB(31, 0, 0));
	PA_SetBgPalCol(0, 3, PA_RGB(0, 31, 0));
	PA_SetBgPalCol(0, 4, PA_RGB(31, 31, 31));
	
	#ifndef EMULATOR 
		FILE* testRead = fopen ("/development/spitfire_maverick/test.txt", "rb"); //rb = read
		
		char filetext[100]; 
		fgets(filetext, 100,testRead);
		levelTitle = new string(filetext);

		while(!feof(testRead)){
			fgets(filetext, 100,testRead);
			processHeightMap(filetext);
		}
		fclose(testRead);
	#else
		levelTitle = new string("test level");
		heightMap.push_back(SHEIGHT-20);
		heightMap.push_back(SHEIGHT-24);
		heightMap.push_back(SHEIGHT-30);
		heightMap.push_back(SHEIGHT-34);
		heightMap.push_back(SHEIGHT-56);
		heightMap.push_back(SHEIGHT-78);
		heightMap.push_back(SHEIGHT-90);
		heightMap.push_back(SHEIGHT-100);
		heightMap.push_back(SHEIGHT-96);
		heightMap.push_back(SHEIGHT-80);
		heightMap.push_back(SHEIGHT-78);
		heightMap.push_back(SHEIGHT-60);
		heightMap.push_back(SHEIGHT-54);
		heightMap.push_back(SHEIGHT-56);
		heightMap.push_back(SHEIGHT-58);
		heightMap.push_back(SHEIGHT-50);
		heightMap.push_back(SHEIGHT-44);
		heightMap.push_back(SHEIGHT-42);
		heightMap.push_back(SHEIGHT-40);
		heightMap.push_back(SHEIGHT-20);
		heightMap.push_back(SHEIGHT-20);
		heightMap.push_back(SHEIGHT-24);
		heightMap.push_back(SHEIGHT-30);
		heightMap.push_back(SHEIGHT-34);
		heightMap.push_back(SHEIGHT-56);
		heightMap.push_back(SHEIGHT-78);
		heightMap.push_back(SHEIGHT-90);
		heightMap.push_back(SHEIGHT-100);
		heightMap.push_back(SHEIGHT-96);
		heightMap.push_back(SHEIGHT-80);
		heightMap.push_back(SHEIGHT-78);
		heightMap.push_back(SHEIGHT-60);
		heightMap.push_back(SHEIGHT-54);
		heightMap.push_back(SHEIGHT-56);
		heightMap.push_back(SHEIGHT-58);
		heightMap.push_back(SHEIGHT-50);
		heightMap.push_back(SHEIGHT-44);
		heightMap.push_back(SHEIGHT-42);
		heightMap.push_back(SHEIGHT-40);
		heightMap.push_back(SHEIGHT-20);
		heightMap.push_back(SHEIGHT-20);
		heightMap.push_back(SHEIGHT-24);
		heightMap.push_back(SHEIGHT-30);
		heightMap.push_back(SHEIGHT-34);
		heightMap.push_back(SHEIGHT-56);
		heightMap.push_back(SHEIGHT-78);
		heightMap.push_back(SHEIGHT-90);
		heightMap.push_back(SHEIGHT-100);
		heightMap.push_back(SHEIGHT-96);
		heightMap.push_back(SHEIGHT-80);
		heightMap.push_back(SHEIGHT-78);
		heightMap.push_back(SHEIGHT-60);
		heightMap.push_back(SHEIGHT-54);
		heightMap.push_back(SHEIGHT-56);
		heightMap.push_back(SHEIGHT-58);
		heightMap.push_back(SHEIGHT-50);
		heightMap.push_back(SHEIGHT-44);
		heightMap.push_back(SHEIGHT-42);
		heightMap.push_back(SHEIGHT-40);
		heightMap.push_back(SHEIGHT-20);
		heightMap.push_back(SHEIGHT-20);
		heightMap.push_back(SHEIGHT-24);
		heightMap.push_back(SHEIGHT-30);
		heightMap.push_back(SHEIGHT-34);
		heightMap.push_back(SHEIGHT-56);
		heightMap.push_back(SHEIGHT-78);
		heightMap.push_back(SHEIGHT-90);
		heightMap.push_back(SHEIGHT-100);
		heightMap.push_back(SHEIGHT-96);
		heightMap.push_back(SHEIGHT-80);
		heightMap.push_back(SHEIGHT-78);
		heightMap.push_back(SHEIGHT-60);
		heightMap.push_back(SHEIGHT-54);
		heightMap.push_back(SHEIGHT-56);
		heightMap.push_back(SHEIGHT-58);
		heightMap.push_back(SHEIGHT-50);
		heightMap.push_back(SHEIGHT-44);
		heightMap.push_back(SHEIGHT-42);
		heightMap.push_back(SHEIGHT-40);
		heightMap.push_back(SHEIGHT-20);
		heightMap.push_back(SHEIGHT-20);
		heightMap.push_back(SHEIGHT-24);
		heightMap.push_back(SHEIGHT-30);
		heightMap.push_back(SHEIGHT-34);
		heightMap.push_back(SHEIGHT-56);
		heightMap.push_back(SHEIGHT-78);
		heightMap.push_back(SHEIGHT-90);
		heightMap.push_back(SHEIGHT-100);
		heightMap.push_back(SHEIGHT-96);
		heightMap.push_back(SHEIGHT-80);
		heightMap.push_back(SHEIGHT-78);
		heightMap.push_back(SHEIGHT-60);
		heightMap.push_back(SHEIGHT-54);
		heightMap.push_back(SHEIGHT-56);
		heightMap.push_back(SHEIGHT-58);
		heightMap.push_back(SHEIGHT-50);
		heightMap.push_back(SHEIGHT-44);
		heightMap.push_back(SHEIGHT-42);
		heightMap.push_back(SHEIGHT-40);
		heightMap.push_back(SHEIGHT-20);
		heightMap.push_back(SHEIGHT-44);
	#endif
	
	levelWidth = (heightMap.size()-2)*16;
	levelHeight = (u16)(SHEIGHT*1.5);

	//Init background
	PA_EasyBgLoad(0, 0, background_image);
	
	//Init ground
	initGround();

	//Reset everything
	reset();
}

void InGame::initGround(){
	blackTile = PA_CreateGfx(0, (void*)black_image_Sprite, OBJ_SIZE_16X16, 1);

	//0 diff
	grassTiles[0][0] = 2;
	grassTiles[0][1] = PA_CreateGfx(0, (void*)grass0_image_Sprite, OBJ_SIZE_16X16, 1);
	//2diff
	grassTiles[1][0] = 3;
	grassTiles[1][1] = PA_CreateGfx(0, (void*)grass2a_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[1][2] = PA_CreateGfx(0, (void*)grass2b_image_Sprite, OBJ_SIZE_16X16, 1);
	//4diff
	grassTiles[2][0] = 3;
	grassTiles[2][1] = PA_CreateGfx(0, (void*)grass4a_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[2][2] = PA_CreateGfx(0, (void*)grass4b_image_Sprite, OBJ_SIZE_16X16, 1);
	//6diff
	grassTiles[3][0] = 3;
	grassTiles[3][1] = PA_CreateGfx(0, (void*)grass6a_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[3][2] = PA_CreateGfx(0, (void*)grass6b_image_Sprite, OBJ_SIZE_16X16, 1);
	//8diff
	grassTiles[4][0] = 3;
	grassTiles[4][1] = PA_CreateGfx(0, (void*)grass8a_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[4][2] = PA_CreateGfx(0, (void*)grass8b_image_Sprite, OBJ_SIZE_16X16, 1);
	//10diff
	grassTiles[5][0] = 3;
	grassTiles[5][1] = PA_CreateGfx(0, (void*)grass10a_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[5][2] = PA_CreateGfx(0, (void*)grass10b_image_Sprite, OBJ_SIZE_16X16, 1);
	//12diff
	grassTiles[6][0] = 3;
	grassTiles[6][1] = PA_CreateGfx(0, (void*)grass12a_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[6][2] = PA_CreateGfx(0, (void*)grass12b_image_Sprite, OBJ_SIZE_16X16, 1);
	//14diff
	grassTiles[7][0] = 3;
	grassTiles[7][1] = PA_CreateGfx(0, (void*)grass14a_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[7][2] = PA_CreateGfx(0, (void*)grass14b_image_Sprite, OBJ_SIZE_16X16, 1);
	//16diff
	grassTiles[8][0] = 3;
	grassTiles[8][1] = PA_CreateGfx(0, (void*)grass16a_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[8][2] = PA_CreateGfx(0, (void*)grass16b_image_Sprite, OBJ_SIZE_16X16, 1);
	//18diff
	grassTiles[9][0] = 4;
	grassTiles[9][1] = PA_CreateGfx(0, (void*)grass18a_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[9][2] = PA_CreateGfx(0, (void*)grass18b_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[9][3] = PA_CreateGfx(0, (void*)grass18c_image_Sprite, OBJ_SIZE_16X16, 1);
	//20diff
	grassTiles[10][0] = 4;
	grassTiles[10][1] = PA_CreateGfx(0, (void*)grass20a_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[10][2] = PA_CreateGfx(0, (void*)grass20b_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[10][3] = PA_CreateGfx(0, (void*)grass20c_image_Sprite, OBJ_SIZE_16X16, 1);
	//22diff
	grassTiles[11][0] = 4;
	grassTiles[11][1] = PA_CreateGfx(0, (void*)grass22a_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[11][2] = PA_CreateGfx(0, (void*)grass22b_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[11][3] = PA_CreateGfx(0, (void*)grass22c_image_Sprite, OBJ_SIZE_16X16, 1);
	//24diff
	grassTiles[12][0] = 4;
	grassTiles[12][1] = PA_CreateGfx(0, (void*)grass24a_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[12][2] = PA_CreateGfx(0, (void*)grass24b_image_Sprite, OBJ_SIZE_16X16, 1);
	grassTiles[12][3] = PA_CreateGfx(0, (void*)grass24c_image_Sprite, OBJ_SIZE_16X16, 1);
}

/**
**Process height map
*/
void InGame::processHeightMap(char* heightMap){
	string* line = new string(heightMap);
	while(getNextHeight(line));
	delete line;
}
int InGame::getNextHeight(string* line){
	u32 index = line->find_first_of(",");
	if(index==string::npos){
		heightMap.push_back(SHEIGHT-atoi(line->substr(0).c_str()));
		return 0;
	}
	else{
		heightMap.push_back(SHEIGHT-atoi(line->substr(0,index+1).c_str()));
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

		//Do collisions
		doCollisions();
		
		//Do updates
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
	if(Pad.Held.Left&&plane->getSpeed()>0){
		plane->speed-=5;
	}
	else if(Pad.Held.Right&&plane->getSpeed()<3){
		plane->speed+=5;
	}
	
	else if(Pad.Held.Up){
		plane->angle+=(s32)(plane->speed*1.2);
		if(plane->getAngle()>511){plane->angle=0;}
	}
	else if(Pad.Held.Down){
		plane->angle-=(s32)(plane->speed*1.2);
		if(plane->getAngle()<0){plane->angle=511<<8;}
	}
}

/**
** Update plane location and viewport
**/
void InGame::doUpdates(){
	u16 angle = plane->getAngle();

	plane->vx = PA_Cos(angle);
	plane->vy = -PA_Sin(angle);

	s16 xflipped = (angle>128&&angle<384)? -1:1;
	s16 yflipped = (angle<256)? -1:1;

	plane->x += ((abs(plane->vx)*plane->speed)>>8)*xflipped;
	plane->y += ((abs(plane->vy)*plane->speed)>>8)*yflipped;
	
	//Viewport calculations using simulated float accuracy
	viewportx = (plane->x-((SWIDTH/2)<<8));//+((squared(plane->speed)*plane->vx)>>14);
	viewporty = (plane->y-((SHEIGHT/2)<<8));
		
	//((squared(plane->speed)/80)*yflipped);

	if(getViewPortX()<0)viewportx=0;
	if(getViewPortX()+SWIDTH>levelWidth)viewportx=(levelWidth-SWIDTH)<<8;

	if(getViewPortY()<SHEIGHT-levelHeight)viewporty=((SHEIGHT-levelHeight)<<8);
	if(getViewPortY()>0)viewporty=0;
	
	//Scroll background
	if(getViewPortX()>0&&getViewPortX()+SWIDTH<levelWidth){
		bgscroll+=((((abs(plane->vx)*plane->speed)>>8))/4*xflipped);
		PA_BGScrollX(0,0,bgscroll>>8);
	}

	//Rotate plane
	PA_SetRotsetNoZoom(0, 0, angle);

	//If the plane is going to be completely rendered above the y threshold reflect its current angle
	if(plane->getY()-16<(SHEIGHT-levelHeight)-50){
		u16 currentAngle = plane->getAngle();
		s16 diff = 128 - currentAngle;		//Get diff from Y axis
		u16 reflected = 384+diff;			//And reflect :)
		plane->angle=reflected<<8;
	}

	//If the plane is going to be completely off the x axis the reflect its current angle
	if(plane->getX()-16<-50){
		u16 currentAngle = plane->getAngle();
		s16 diff = 256 - currentAngle;		//Get diff from X axis
		s16 reflected = 0+diff;			//And reflect :)
		if(reflected<0)reflected+=512;
		plane->angle=reflected<<8;
	}

	else if(plane->getX()-16>levelWidth+18){
		u16 currentAngle = plane->getAngle();
		s16 diff = 0 - currentAngle;		//Get diff from Y axis
		u16 reflected = 256+diff;			//And reflect :)
		plane->angle=reflected<<8;
	}
}
/**
Do drawing function
**/
void InGame::doDrawing(void){
	PA_Clear8bitBg(0);
	//PA_Draw8bitLineEx(0, tipx-getViewPortX(), 0, tipx-getViewPortX(), SHEIGHT,2,2);
	//PA_Draw8bitLineEx(0, 0, tipy-getViewPortY(), SWIDTH,tipy-getViewPortY(),3,2);
	//PA_Draw8bitLineEx(0, tipx-getViewPortX(),SHEIGHT ,tipx-getViewPortX(),actualHeight ,4,2);

	//Draw plane
	s32 planeoffsetx = (-getViewPortX())-16;
	s32 planeoffsety = (-getViewPortY())-18;

	PA_SetSpriteXY(0,plane->spriteIndex, plane->getX()+planeoffsetx, plane->getY()+planeoffsety);

	//Render landscape
	renderLandscape();
}	
void InGame::renderLandscape(){
	
	//Reset old landscape before rendering new one
	resetLandscape();

	vector<u16>::iterator it;
	it = heightMap.begin();


	s16 x =0-getViewPortX();
	while( it != heightMap.end()-1) {
		u16 thisHeight = (*it);
		u16 nextHeight = (*(it+1));
		u8 spriteIndex = abs(thisHeight-nextHeight)/2;
		u16 y = taller(thisHeight,nextHeight);		   //Get height
		y-=getViewPortY();
		
		if(x>-16&&x<256&&y<SHEIGHT){		//Draw if tile on screen
			u16 flipped = (thisHeight>nextHeight)?0:1;	   //Get flipped
			u16 length = grassTiles[spriteIndex][0];
			for(u16 i =1;i<length;i++){
				u16 availableIndex = spritePool.back();
				spritePool.pop_back();					//Get available index
				//Load sprite to screen
				PA_CreateSpriteFromGfx(0,availableIndex,grassTiles[spriteIndex][i],OBJ_SIZE_16X16, 1, 1, x,y);
				PA_SetSpriteHflip(0, availableIndex, flipped); // (screen, sprite, flip(1)/unflip(0)) HFlip -> Horizontal flip
				//Add index to list used by landscape
				landscapeIndexs.push_back(availableIndex);
				//PA_SetSpritePrio(0,availableIndex,1);
				y+=16;
				if(y>SHEIGHT)break;
			}
			while(y<=SHEIGHT){
				u16 availableIndex = spritePool.back();
				spritePool.pop_back();					//Get available index
				PA_CreateSpriteFromGfx(0,availableIndex,blackTile,OBJ_SIZE_16X16, 1, 1, x,y);
				landscapeIndexs.push_back(availableIndex);
				//PA_SetSpritePrio(0,availableIndex,1);
				y+=16;
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
	tipx = plane->getX()+((plane->vx*16)>>8);
	tipy = plane->getY()+((plane->vy*16)>>8);

	s16 landIndex = (tipx/16);

	if((u16)landIndex>heightMap.size()-2||landIndex<0){return;}
	
	u16 a = heightMap.at(landIndex);
	u16 b = heightMap.at(landIndex+1);

	float diff = (tipx &15)/16.0f;

	
	actualHeight = (u16)(a+(diff*(b-a)))-getViewPortY();

	if(tipy>actualHeight){
		planeCrash();
	}
}


/**
**
**/
void InGame::planeCrash(){
	delete plane;
	plane = new GameObject(128<<8,60<<8,32,32,0);
	plane->speed=1<<8;
}
/**
** Inline squared function
**/
inline u32 InGame::squared(u32 a){ return a*a;}

/**
Debug function, put everything here that you
wanna print to screen
**/
void InGame::print_debug(void){
	PA_ClearTextBg(1);
	PA_OutputText(1,0, 0, "Title is: %s", levelTitle->c_str());
	PA_OutputText(1,0, 1, "Viewport x:%d (%d) y:%d (%d)", getViewPortX(),viewportx,getViewPortY(),viewporty);
	PA_OutputText(1,0, 2, "Plane x:%d (%d) y:%d (%d)", plane->getX(),plane->x,plane->getY(),plane->y);
	PA_OutputText(1,0, 3, "Plane vx:%d vy:%d", plane->vx,plane->vy);
	PA_OutputText(1,0, 4, "Plane speed:%d (%d)", plane->getSpeed(),plane->speed);
	PA_OutputText(1,0, 5, "Plane angle:%d (%d)", plane->getAngle(),plane->angle);
	PA_OutputText(1,0, 6, "Landscape sprites used:%d", landscapeIndexs.size());
	PA_OutputText(1,0, 7, "Available sprites:%d", spritePool.size());
	PA_OutputText(1,0, 8, "Background scroll:%d (%d)", bgscroll>>8,bgscroll);
	//Put your debug print statements here.... make sure to print to screen 1
}
