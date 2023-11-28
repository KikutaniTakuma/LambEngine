#include "FollowCamera.h"
#include "Input/Input.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"

void FollowCamera::Move() {
	static Input* const input = Input::GetInstance();
	float delataTime = FrameInfo::GetInstance()->GetDelta();
	float stickMoveValue = input->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_X);

	if (std::abs(stickMoveValue) > 0.0f) {
		rotate.y += rotateSpd_ * stickMoveValue * delataTime;
	}
}

void FollowCamera::Update() {
	rotateMat_ = MakeMatrixRotate(rotate);

	if (delayEasing_.ActiveEnter() || delayEasing_.ActiveStay()) {
		pos = delayEasing_.Get(pos, gazePoint_ + (offset_ * rotateMat_));
	}
	else {
		pos = gazePoint_ + (offset_ * rotateMat_);
	}

	Camera::Update();

	delayEasing_.Update();
}

void FollowCamera::Update(const Vector3& basis, const Vector3& target) {
	Vector3 posTmp = pos;
	posTmp.y = 0.0f;
	rotateMat_ = MakeMatrixRotateX(rotate.x) * DirectionToDirection(basis.Normalize(), (target - posTmp).Normalize());

	if (delayEasing_.ActiveEnter() || delayEasing_.ActiveStay()) {
		pos = delayEasing_.Get(easingStartPos_, gazePoint_ + (offset_ * rotateMat_));
	}
	else {
		pos = gazePoint_ + (offset_ * rotateMat_);
	}

	Mat4x4 worldMat = MakeMatrixScalar(scale) * rotateMat_ * MakeMatrixTranslate(pos);

	SetMat4x4Update(worldMat);

	delayEasing_.Update();
}

Mat4x4 FollowCamera::GetRotateMatrixY() const {
	return MakeMatrixRotateY(rotate.y);
}

Mat4x4 FollowCamera::GetRotateMatrixY(const Vector3& basis, const Vector3& target) const {
	Vector3 posTmp = pos;
	posTmp.y = 0.0f;
	return DirectionToDirection(basis.Normalize(), (target - posTmp).Normalize());
}

void FollowCamera::DelayEasingStart(const Vector3& endPos) {
	delayEasing_.Start(
		false,
		delayCaemraSpeed_
	);
	easingStartPos_ = pos;
	endPos_ = endPos;
}