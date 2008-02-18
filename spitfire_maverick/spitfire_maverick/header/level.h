#ifndef __LEVEL1__
#define __LEVEL1__

class Level{
	public:
		//Fields
		string* levelTitle;
		vector<u16>* heightMap;
		u16 levelWidth;
		u16 levelHeight;

		Level(u16 levelWidth,u16 levelHeight,string *levelTitle,vector<u16>* heightMap);
		~Level();
};
#endif
