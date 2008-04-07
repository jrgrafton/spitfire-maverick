#ifndef __ANIMATION_INFO1__
#define __ANIMATION_INFO1__

class AnimationInfo{
	public:
		void tick();
		void startAnimation();
		void stopAnimation();
		u16 getCurrentFrame();
		void setCurrentFrame(u16 frame);
		u16 getFrameCount();

		AnimationInfo(u16 currentFrame,u16 frameCount,u16 delay,bool isAnimating);
		AnimationInfo(const AnimationInfo &object);
		~AnimationInfo();
	private:
		//Fields
		u16 currentFrame;
		u16 delay;
		u16 timeSinceFrameChange;
		u16 frameCount;
		bool isAnimating;

};
#endif
