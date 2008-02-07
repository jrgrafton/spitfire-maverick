//Internal headers
#include "../header/main.h"
#include "../header/game_object.h"
#include "../header/state.h"
#include "../header/in_game.h"

//Sound headers

//Fields of game entitys

//Are we in game?
u16 inGame = 0;

string* levelTitle;

vector<u16> heightMap;

u32 viewportx =0;
u32 viewporty =0;

//Represents bottom middle of the plane
GameObject* plane;

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
/**
**InGame init allocate resources
**/
void InGame::init(){
	myName = INGAME;
	
	//Set the virtual distance in screen to SCREENHOLE
	PA_SetScreenSpace(SCREENHOLE);
	
	//Load game objects
	plane = new GameObject(128<<8,80<<8,32,32,0);

	//Load pallates
	PA_LoadSpritePal (0, 0, (void*)plane_Pal);

	//Load sprite
	PA_CreateSprite (0, plane->spriteIndex, (void*)plane_Sprite,OBJ_SIZE_32X32, 1, 0, plane->getX()-16, plane->getY()-32);

	//Enable rotation
	PA_SetSpriteRotEnable(0,0,0);
	
	initGraphics();
	
}

void InGame::initGraphics(){
	#ifdef DEBUG
		PA_InitText(1,0); // On the top screen
		PA_SetTextCol(1,31,31,31);
	#endif
	PA_SetBgPalCol(0, 2, PA_RGB(31, 31, 31));
	
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
	#endif
	

	//Reset everything
	reset();
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
	if(Pad.Held.Left){
		if(plane->speed>0){plane->speed-=(20);}
	}
	else if(Pad.Held.Right){
		if(plane->getSpeed()<10){plane->speed+=(20);}
	}
	
	else if(Pad.Newpress.Up){
		//viewporty--;
		plane->angle = (plane->angle==0)? 256:0;
	}
	//else if(Pad.Held.Up){
	//	viewporty++;
	//}
}

/**
** Update plane location and viewport
**/
void InGame::doUpdates(){
	u16 angle = plane->angle;

	plane->vx = PA_Cos(angle);
	plane->vy = -PA_Sin(angle);

	plane->x += (plane->vx*plane->speed)>>8;
	//TODO plane->y
	
	s16 flipped = (plane->angle==0)? 1:-1;
	//Viewport calculations
	viewportx = (plane->getX()-SWIDTH/2)+((squared(plane->getSpeed())*2)*flipped);
	if(viewportx<0)viewportx=0;

	//Rotate plane
	//PA_SetRotsetNoZoom(0, 0, plane->angle);
}
/**
Do drawing function
**/
void InGame::doDrawing(void){
	PA_Clear8bitBg(0);

	//Draw plane
	s32 planeoffsetx = (-viewportx)-16;
	s32 planeoffsety = -32;

	PA_SetSpriteXY(0,plane->spriteIndex, plane->getX()+planeoffsetx, plane->getY()+planeoffsety);

	vector<u16>::iterator it;
	
	it = heightMap.begin();
	s16 linex =1-viewportx;
	s16 lasty = 0;
	u16 started=0;
	while( it != heightMap.end() ) {
		u16 value = *it;
		value+=viewporty;
		if(linex>0){
			//Horizontal
			PA_Draw8bitLineEx(0,(linex-16)*started,lasty*started,linex*started,value*started,2,2*started);
			//Vertical
			if(linex<254)PA_Draw8bitLineEx(0,linex,SHEIGHT,linex,value,2,2);
		}
		linex+=16;
		lasty=value;
		started=1;
		it++;
	}


}	

/**
Do collisions function
**/
void InGame::doCollisions(void){

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
	PA_OutputText(1,0, 1, "Viewport x:%d y:%d", viewportx,viewporty);
	PA_OutputText(1,0, 2, "Plane x:%d y:%d", plane->getX(),plane->getY());
	PA_OutputText(1,0, 3, "Plane vx:%d vy:%d", plane->vx,plane->vy);
	PA_OutputText(1,0, 4, "Plane speed:%d", plane->speed);
	PA_OutputText(1,0, 5, "Plane angle:%d", plane->angle);
	//Put your debug print statements here.... make sure to print to screen 1
}
