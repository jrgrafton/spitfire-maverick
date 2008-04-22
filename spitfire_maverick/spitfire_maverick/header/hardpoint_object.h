#ifndef __HARDPOINT_OBJECT1__
#define __HARDPOINT_OBJECT1__

#include "../header/sprite_info.h" //Needed since its a field in class
#include "../header/projectile_object.h" //Needed since its a field in class

class AIObject;								//Forward declaraton for AIObject to avoid circular inclusion
class HardpointObject : public GameObject{
	public:
		s32 angleToTarget;

		ProjectileObject* getProjectileObject();
		bool canFire();	//Call this every update so that time since last fired can be incremented
		void fire();	//This simply resets timeSinceLastFired

		//This is called by AI object that its attached to, it will make this hardpoint turn to
		//face a target if one is available
		//It also updates the turrets position relative to offsets and its parent object
		void updateRenderOffset();	
		void turnTowardsTarget();	//Turns hardpoint towards current target
		u16 getFiringVelocity();
		bool hasTarget();
		void setTarget(DestructableObject* target);

		HardpointObject(s32 x,s32 y,u16 width,u16 height,s32 heading,SpriteInfo* si,s16 xOffset,s16 yOffset,u16 fireDelay,u16 turningSpeed,u16 maxInclination,s16 minInclination,u16 firingVelocity,ProjectileObject* po);
		HardpointObject(const HardpointObject &object,GameObject &parent);
		~HardpointObject();

	private:
		//Fields
		u16 timeSinceLastFired;
		u16 fireDelay;
		u16 turningSpeed;
		u16 maxInclination;
		s16 minInclination;
		ProjectileObject* po;
		s16 xOffset;
		s16 yOffset;
		GameObject* parent;
		DestructableObject* target;
		u16 firingVelocity;
};
#endif
