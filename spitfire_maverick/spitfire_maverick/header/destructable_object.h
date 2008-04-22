#ifndef __DESTRUCTABLE1__
#define __DESTRUCTABLE1__

#include "../header/particle_object.h"

class DestructableObject: public GameObject{
	public:
		void destructObject();
		s32 getHealth();
		void setHealth(s32 health);
		u16 getParticleCount();
		ParticleObject* getParticleSpriteInstance();
		bool getDestroyed();

		DestructableObject(s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx, s16 vy,SpriteInfo* si,s32 health,s16 particleCount,s16 destroyedGfxRef,ParticleObject* particleSpriteInstance);
		DestructableObject(const DestructableObject &object);	//Copy constructor
		~DestructableObject();
	private:
		//Fields
		s32 health;
		s16 particleCount;
		s16 destroyedGfxRef;
		ParticleObject* particleSpriteInstance;
		bool objectDestroyed;

};
#endif
