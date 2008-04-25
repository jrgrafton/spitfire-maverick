#ifndef __DESTRUCTABLE1__
#define __DESTRUCTABLE1__

#include "../header/particle_object.h"

/**
A destructable object is an extension of a game object, allows for it to be destroyed
and contains particle information which can be used to generate particle effects
**/

class DestructableObject: public GameObject{
	public:
		void destructObject();
		s32 getHealth() const;
		void setHealth(s32 health);
		u16 getParticleCount() const;
		ParticleObject* getParticleSpriteInstance() const;
		bool getDestroyed() const;

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
