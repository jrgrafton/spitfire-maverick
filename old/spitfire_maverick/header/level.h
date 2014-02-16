#ifndef __LEVEL1__
#define __LEVEL1__

class Level{
	public:		
		string* getTitle();
		vector<s16>* getHeightMap();
		u16 getLevelWidth();
		u16 getLevelHeight();


		Level(u16 levelWidth,u16 levelHeight,string *levelTitle,vector<s16>* heightMap);
		~Level();

	private:
		string* levelTitle;
		vector<s16>* heightMap;
		u16 levelWidth;
		u16 levelHeight;
};
#endif
