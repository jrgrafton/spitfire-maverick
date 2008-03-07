#ifndef __DESTRUCTABLE1__
#define __DESTRUCTABLE1__

class DestructableObject: public GameObject{
	public:
		void destroy();
		s16 getHealth();
		void setHealth(s16 health);
		u16 getParticleCount();
		SpriteInfo* getParticleInstance();
		bool getDestroyed();

		DestructableObject(s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx, s16 vy,SpriteInfo* si,s16 health,s16 particleCount,s16 destroyedGfxRef,SpriteInfo* particleInstance);
		DestructableObject(const DestructableObject &object);	//Copy constructor
		~DestructableObject();
	private:
		//Fields
		s16 health;
		s16 particleCount;
		s16 destroyedGfxRef;
		SpriteInfo* particleInstance;
		bool destroyed;

};
#endif
