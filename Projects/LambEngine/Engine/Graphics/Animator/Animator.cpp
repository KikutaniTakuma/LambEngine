#include "Animator.h"
#include "../AnimationManager/AnimationManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include <algorithm>


Animator::Animator():
	animationTime_(0.0f),
	animations_(nullptr),
	currentAnimationIndex_(0llu),
	animationMatrix_(Mat4x4::kIdentity),
	isFullAnimation_(false),
	isLoop_(false)
{}

// 警告の誤検知
#pragma warning(push)
#pragma warning(disable:4702)
void Animator::Load(const std::string& fileName) {
	AnimationManager* const animationManager = AnimationManager::GetInstance();
	animationManager->LoadAniamtions(fileName);
	animations_ = animationManager->GetAnimations(fileName);
}
#pragma warning(pop)

void Animator::Update() {
	Animation& currentAnimation = animations_->data[currentAnimationIndex_];



	animationTime_ += Lamb::DeltaTime();
	if (isFullAnimation_) {
		if (animations_->data.size() <= currentAnimationIndex_) {
			if (isLoop_) {
				currentAnimationIndex_ = 0;
			}
			else {
				Stop();
			}
		}
	}
	else {
		if (currentAnimation.duration < animationTime_) {
			if (isLoop_) {
				animationTime_ = 0.0f;
			}
			else {
				Stop();
			}
		}
	}
}

void Animator::Start() {
	Reset();
	Restart();
}

void Animator::Stop() {
	Pause();
	Reset();
}

void Animator::Reset() {
	currentAnimationIndex_ = 0;
	animationTime_ = 0.0f;
}

void Animator::Restart(){
	isActive_ = true;
}

void Animator::Pause(){
	isActive_ = false;
}

const Mat4x4& Animator::GetMat4x4() const
{
	return animationMatrix_;
}

void Animator::SetIsFullAnimation(bool isFullAnimation) {
	isFullAnimation_ = isFullAnimation;
}

void Animator::SetAnimationIndex(size_t index) {
	currentAnimationIndex_ = std::clamp(index, 0llu, animations_->data.size() - 1llu);
}

void Animator::SetLoopAnimation(bool isLoop) {
	isLoop_ = isLoop;
}
