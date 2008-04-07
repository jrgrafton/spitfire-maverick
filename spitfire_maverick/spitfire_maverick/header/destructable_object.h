#ifndef __DESTRUCTABLE1__
#define __DESTRUCTABLE1__

#include "../header/particle_object.h"

class DestructableObject: public GameObject{
	public:
		void destructObject();
		s16 getHealth();
		void setHealth(s16 health);
		u16 getParticleCount();
		ParticleObject* getParticleSpriteInstance();
		bool getDestroyed();

		DestructableObject(s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx, s16 vy,SpriteInfo* si,s16 health,s16 particleCount,s16 destroyedGfxRef,ParticleObject* particleSpriteInstance);
		DestructableObject(const DestructableObject &object);	//Copy constructor
		~DestructableObject();
	private:
		//Fields
		s16 health;
		s16 particleCount;
		s16 destroyedGfxRef;
		ParticleObject* particleSpriteInstance;
		bool objectDestroyed;

};
#endif
