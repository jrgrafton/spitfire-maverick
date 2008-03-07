#ifndef __SPRITEINFO1__
#define __SPRITEINFO1__

class SpriteInfo{
	public:
		u16 getSpriteWidth();
		u16 getSpriteHeight();
		bool getUsesRot();
		u16 getZoom();
		s32 getAngle();
		void setAngle(s32 angle);
		u16 getGfxRef();
		void setGfxRef(u16 gfxref);
		s16 getRotIndex();
		void setRotIndex(s16 rotIndex);
		s16 getSpriteIndex();
		void setSpriteIndex(s16 spriteIndex);
		s16 getPaletteIndex();
		u16* getObjSize();
		bool getUsesSprite();
		u16 getDoubleSize();

		SpriteInfo(u16 width,u16 height,s32 angle,s16 gfxref,s16 spriteIndex,s16 rotIndex,s16 palette,u16* objsize,u16 zoom,u16 doubleSize,bool usesRot);
		SpriteInfo(const SpriteInfo &object);//copy constructor
		~SpriteInfo();
	private:
		u16 width, height;
		s32 angle;
		s16 gfxref;
		s16 spriteIndex;
		s16 rotIndex;
		s16 paletteIndex;
		u16* objsize;
		u16 zoom;
		u16 doubleSize;
		bool usesRot;
		bool usesSprite;

};
#endif
