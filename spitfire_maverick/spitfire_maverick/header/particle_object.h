#ifndef __PARTICLE_OBJECT1__
#define __PARTICLE_OBJECT1__

class ParticleObject : public GameObject{
	public:
		
		void setRotSpeed(s16 rotSpeed);
		s16 getRotSpeed();

		bool isHeavy();
		s16 getTtl();
		void setTtl(s16 ttl);

		ParticleObject(string* name,s32 startx,s32 starty,u16 width,u16 height,s32 heading,s16 rotSpeed,s16 vx,s16 vy,s16 ttl,bool heavy,SpriteInfo* si);
		~ParticleObject();
		ParticleObject(const ParticleObject &object);	//Copy constructor
	
	private:
		s16 ttl;
		s16 rotSpeed;
		bool heavy;
};
#endif
