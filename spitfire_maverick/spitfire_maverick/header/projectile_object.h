#ifndef __PROJECTILE_OBJECT1__
#define __PROJECTILE_OBJECT1__

class ProjectileObject : public GameObject{
	public:
		s16 getTtl();
		void setTtl(s16 ttl);
		bool isPlayerProjectile();
		u16 getExplosionRadius();
		u16 getProjectileStrength();
		void explode();
		bool isExploded();

		ProjectileObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 vx,s16 vy,SpriteInfo* si,s16 ttl,u16 explosionRadius,u16 projectileStrength,bool playerProjectile);
		~ProjectileObject();

	private:
		s16 ttl;
		u16 explosionRadius;
		u16 projectileStrength;
		bool playerProjectile;
		bool exploding;
};
#endif
