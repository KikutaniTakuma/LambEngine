#include "FixedCamera.h"

#include <numbers>

#include "Utils/Camera/Camera.h"

FixedCamera::FixedCamera() {}

void FixedCamera::Initialize() {}

void FixedCamera::Update() {
	camera_->pos = goalPos_ * 0.5f;
	camera_->pos.y = 220.0f;
	camera_->rotate = { 89.0f * std::numbers::pi_v<float> / 180.0f, 0.0f, 0.0f };
	camera_->Update();
}
