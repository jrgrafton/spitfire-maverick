#ifndef __PARTICLE_OBJECT1__
#define __PARTICLE_OBJECT1__

class ParticleObject : public GameObject{
	public:
		s16 ttl;
		s16 rotSpeed;
		bool heavy;
		u16 zoom;

		ParticleObject(string* name,s32 startx,s32 starty,u16 width,u16 height,u16* objsize,s16 spriteIndex,u16 palette,u16 gfxref,s32 angle,s16 rotSpeed,s16 rotIndex,s16 vx,s16 vy,s16 ttl,bool heavy,u16 zoom);
		~ParticleObject();
};
#endif
