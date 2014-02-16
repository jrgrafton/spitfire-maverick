#ifndef __PROJECTILE_OBJECT1__
#define __PROJECTILE_OBJECT1__

#include "../header/particle_object.h"

class ProjectileObject : public GameObject{
	public:
		s16 getTtl();
		void setTtl(s16 ttl);
		bool isPlayerProjectile();
		u16 getExplosionRadius();
		u16 getProjectileStrength();
		void explode();
		bool isExploded();
		bool isExplosive();
		bool hasExplosionAnimation();
		ParticleObject* getExplosionAnimation();

		ProjectileObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx,s16 vy,SpriteInfo* si,s16 ttl,u16 explosionRadius,u16 projectileStrength,bool playerProjectile);
		ProjectileObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx,s16 vy,SpriteInfo* si,s16 ttl,u16 explosionRadius,u16 projectileStrength,bool playerProjectile,ParticleObject* explosionAnimation);
		ProjectileObject(const ProjectileObject &object);
		~ProjectileObject();

	private:
		s16 ttl;
		u16 explosionRadius;
		u16 projectileStrength;
		bool playerProjectile;
		bool exploded;
		bool explosive;
		ParticleObject* explosionAnimation;
};
#endif
