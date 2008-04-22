#ifndef __AI_OBJECT1__
#define __AI_OBJECT1__

#include "../header/particle_object.h" //Needed since its a field in class
#include "../header/sprite_info.h" //Needed since its a field in class
#include "../header/hardpoint_object.h" //Needed since its a field in class

class AIObject : public DestructableObject{
	public:
		enum SIDE {NAZI,ALLIES};
		enum STATUS {STAYINGSTILL,ROAMING,ATTACKING};

		SIDE getSide();
		void setStatus(STATUS status);
		STATUS getStatus();
		bool getStationary();
		HardpointObject* getHardpoint();
		void updateHardpoint();			//ATM just support for one hardpoint
		bool hasTarget();
		void setTarget(DestructableObject* target);

		AIObject(s32 x,s32 y,u16 width,u16 height,s32 heading,SpriteInfo* si,s16 health,s16 particleCount,s16 destroyedGfxRef,ParticleObject* particleSpriteInstance,bool stationary,HardpointObject* hardpoint,STATUS status, SIDE side);
		AIObject(const AIObject &object);
		~AIObject();
	private:
		SIDE side;
		STATUS status;
		bool stationary;
		HardpointObject* hardpoint;
};
#endif
