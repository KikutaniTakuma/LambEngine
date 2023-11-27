#pragma once
#include "../Camera.h"
#include "Utils/Easing/Easing.h"

class FollowCamera final : public Camera{
public:
	FollowCamera() = default;
	FollowCamera(const FollowCamera&) = default;
	FollowCamera(FollowCamera&&) = default;
	~FollowCamera() = default;

	FollowCamera& operator=(const FollowCamera&) = default;
	FollowCamera& operator=(FollowCamera&&) = default;

public:
	void Move();

	void Update() override;

	void Update(const Vector3& basis, const Vector3& target);

	void DelayEasingStart(const Vector3& endPos);

public:
	Vector3 offset_;
	Vector3 gazePoint_;
	float rotateSpd_;
	float delayCaemraSpeed_;

private:
	Easing delayEasing_;
	Vector3 endPos_;
	Vector3 easingStartPos_;
};