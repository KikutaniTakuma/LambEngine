#include "Animator.h"
#include "../AnimationManager/AnimationManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include <algorithm>


Animator::Animator():
	animationTime_(0.0f),
	animations_(nullptr),
	currentAnimationIndex_(0llu),
	animationMatrix_(Mat4x4::kIdentity),
	isActive_(false),
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

void Animator::Update(const Mesh* const mesh) {
	Animation& currentAnimation = animations_->data[currentAnimationIndex_];
	NodeAnimation& rootNodeAnimation = currentAnimation.nodeAnimations[mesh->node.name];
	Vector3&& translate = CalaclateValue(rootNodeAnimation.translation, animationTime_);
	Quaternion&& rotate = CalaclateValue(rootNodeAnimation.rotate, animationTime_);
	Vector3&& scale = CalaclateValue(rootNodeAnimation.sacle, animationTime_);

	animationMatrix_ = Mat4x4::MakeAffin(scale, rotate, translate);

	if (isActive_) {
		animationTime_ += Lamb::DeltaTime();
	}

	if (isFullAnimation_ and animations_->data.size() <= currentAnimationIndex_) {
		if (isLoop_) {
			currentAnimationIndex_ = 0;
		}
		else {
			Stop();
		}
	}
	
	if (currentAnimation.duration < animationTime_) {
		if (isLoop_) {
			animationTime_ = 0.0f;
		}
		else if (isFullAnimation_) {
			currentAnimationIndex_++;
		}
		else {
			Stop();
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

const Mat4x4& Animator::GetLocalMat4x4() const
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

Vector3 Animator::CalaclateValue(const AnimationCurve<Vector3>& animationCurve, float time)
{
	if (animationCurve.keyFrames.empty()) {
		throw Lamb::Error::Code<Animator>("keyFrams is empty", __func__);
	}
	if (animationCurve.keyFrames.size() == 1 or time < animationCurve.keyFrames.front().time) {
		return animationCurve.keyFrames.front().value;
	}

	for (size_t index = 0; index < animationCurve.keyFrames.size() - 1; index++) {
		size_t nextIndex = index + 1;
		if (animationCurve.keyFrames[index].time <= time and time <= animationCurve.keyFrames[nextIndex].time) {
			float t = (time - animationCurve.keyFrames[index].time) / (animationCurve.keyFrames[nextIndex].time - animationCurve.keyFrames[index].time);
			return Vector3::Lerp(animationCurve.keyFrames[index].value, animationCurve.keyFrames[nextIndex].value, t);
		}
	}

	return animationCurve.keyFrames.back().value;
}

Quaternion Animator::CalaclateValue(const AnimationCurve<Quaternion>& animationCurve, float time) {
	if (animationCurve.keyFrames.empty()) {
		throw Lamb::Error::Code<Animator>("keyFrams is empty", __func__);
	}
	if (animationCurve.keyFrames.size() == 1 or time < animationCurve.keyFrames.front().time) {
		return animationCurve.keyFrames.front().value;
	}
	for (size_t index = 0; index < animationCurve.keyFrames.size() - 1; index++) {
		size_t nextIndex = index + 1;
		if (animationCurve.keyFrames[index].time <= time and time <= animationCurve.keyFrames[nextIndex].time) {
			float t = (time - animationCurve.keyFrames[index].time) / (animationCurve.keyFrames[nextIndex].time - animationCurve.keyFrames[index].time);
			return Quaternion::Slerp(animationCurve.keyFrames[index].value, animationCurve.keyFrames[nextIndex].value, t);
		}
	}

	return animationCurve.keyFrames.back().value;
}