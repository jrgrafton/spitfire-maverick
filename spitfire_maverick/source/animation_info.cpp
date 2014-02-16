#include "../header/main.h"
#include "../header/animation_info.h"

AnimationInfo::AnimationInfo(u16 currentFrame,u16 frameCount,u16 delay,bool isAnimating){
	this->currentFrame=currentFrame;
	this->frameCount=frameCount;
	this->delay=delay;
	this->isAnimating=isAnimating;
	this->timeSinceFrameChange=0;
}

AnimationInfo::AnimationInfo(const AnimationInfo &object){
	this->currentFrame=object.currentFrame;
	this->frameCount=object.frameCount;
	this->delay=object.delay;
	this->isAnimating=object.isAnimating;
	this->timeSinceFrameChange=0;
}

AnimationInfo::~AnimationInfo(){

}

void AnimationInfo::tick(){
	if(!isAnimating){return;}
	this->timeSinceFrameChange++;
	if(this->timeSinceFrameChange>=delay){
		currentFrame=(currentFrame==frameCount-1)?0:currentFrame+1;
		timeSinceFrameChange=0;
	}
}

void AnimationInfo::startAnimation(){isAnimating=true;}
void AnimationInfo::stopAnimation(){isAnimating=false;}

u16 AnimationInfo::getCurrentFrame() const{
	return currentFrame;
}
void AnimationInfo::setCurrentFrame(u16 frame){
	this->currentFrame=frame;
}
u16 AnimationInfo::getFrameCount() const{
	return this->frameCount;
}

