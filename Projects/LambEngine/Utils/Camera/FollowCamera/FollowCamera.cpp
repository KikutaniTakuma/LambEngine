#include "FollowCamera.h"
#include "Input/Input.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"

void FollowCamera::Move() {
	static Input* input = Input::GetInstance();
	float delataTime = FrameInfo::GetInstance()->GetDelta();
	float stickMoveValue = input->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_X);

	if (std::abs(stickMoveValue) > 0.0f) {
		rotate.y += rotateSpd_ * stickMoveValue * delataTime;
	}
}

void FollowCamera::Update() {
	if (delayEasing_.ActiveEnter() || delayEasing_.ActiveStay()) {
		pos = delayEasing_.Get(pos, gazePoint_ + (offset_ * MakeMatrixRotate(rotate)));
	}
	else {
		pos = gazePoint_ + (offset_ * MakeMatrixRotate(rotate));
	}

	Camera::Update();

	delayEasing_.Update();
}

void FollowCamera::Update(const Vector3& basis, const Vector3& target) {
	if (delayEasing_.ActiveEnter() || delayEasing_.ActiveStay()) {
		pos = delayEasing_.Get(easingStartPos_, gazePoint_ + (offset_ * MakeMatrixRotate(rotate)));
	}
	else {
		pos = gazePoint_ + (offset_ * MakeMatrixRotate(rotate));
	}

	Mat4x4 worldMat = MakeMatrixScalar(scale) * DirectionToDirection(basis, target) * MakeMatrixTranslate(pos);

	SetMat4x4Update(worldMat);

	delayEasing_.Update();
}

void FollowCamera::DelayEasingStart(const Vector3& endPos) {
	delayEasing_.Start(
		false,
		delayCaemraSpeed_
	);
	easingStartPos_ = pos;
	endPos_ = endPos;
}