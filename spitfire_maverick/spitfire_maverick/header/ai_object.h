#ifndef __AI_OBJECT1__
#define __AI_OBJECT1__

#include "../header/particle_object.h" //Needed since its a field in class
#include "../header/sprite_info.h" //Needed since its a field in class
#include "../header/hardpoint_object.h" //Needed since its a field in class

/**
This class represents an AI object. An AI object inherits from destructable
object so that it can have particles and can be destroyed. It also contains
hard_point objects which represent its weapons.
**/

class AIObject : public DestructableObject{
	public:
		enum SIDE {NAZI,ALLIES};
		enum STATUS {STAYINGSTILL,ROAMING,ATTACKING};

		SIDE getSide() const;
		void setStatus(STATUS status);
		STATUS getStatus() const;
		bool getStationary() const;
		HardpointObject* getHardpoint() const;
		void updateHardpoint();			//ATM just support for one hardpoint
		bool hasTarget() const;
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
