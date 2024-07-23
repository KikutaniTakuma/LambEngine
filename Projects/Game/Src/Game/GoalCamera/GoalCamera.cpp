#include "GoalCamera.h"

#include <numbers>

#include "imgui.h"

#include "Camera/Camera.h"

void GoalCamera::Initialize() {
	Mat4x4 goalRotateY = Mat4x4::MakeRotateY(goal_.rotate.y);
	offset_ = Vector3(0.0f, 36.0f, -60.0f) * goalRotateY;
	rotateOffset_ = Vector3(0.5f, goal_.rotate.y, 0.0f);
	endCamera_.pos = goal_.pos + offset_;
	endCamera_.rotate = rotateOffset_;
	easingCount_ = 4.0f;
	rotateEasing_.Start(false, easingCount_, Easeing::InSine);
	translateEasing_.Start(false, easingCount_, Easeing::InSine);
}

void GoalCamera::Update() {
	camera_->pos = translateEasing_.Get(startCamera_.pos, endCamera_.pos);
	camera_->rotate = rotateEasing_.Get(startCamera_.rotate, LerpShortAngle(startCamera_.rotate, endCamera_.rotate));
	translateEasing_.Update();
	rotateEasing_.Update();
	/*ImGui::Begin("GoalCamera");
	ImGui::DragFloat3("pos",&offset_.x,0.1f);
	ImGui::DragFloat3("rotateOffset",&rotateOffset_.x,0.1f);
	ImGui::End();*/
	camera_->Update();
}

Vector3 GoalCamera::LerpShortAngle(const Vector3& a, const Vector3& b) {
	// 角度差分を求める
	Vector3 diff = b - a;
	// 角度[-PI,+PI]に補正
	if (diff.x > 180.0f * std::numbers::pi_v<float> / 180.0f) {
		diff.x -= 360.0f * std::numbers::pi_v<float> / 180.0f;
	}
	else if (diff.x < -180.0f * std::numbers::pi_v<float> / 180.0f) {
		diff.x += 360.0f * std::numbers::pi_v<float> / 180.0f;
	}

	if (diff.y > 180.0f * std::numbers::pi_v<float> / 180.0f) {
		diff.y -= 360.0f * std::numbers::pi_v<float> / 180.0f;
	}
	else if (diff.y < -180.0f * std::numbers::pi_v<float> / 180.0f) {
		diff.y += 360.0f * std::numbers::pi_v<float> / 180.0f;
	}

	if (diff.z > 180.0f * std::numbers::pi_v<float> / 180.0f) {
		diff.z -= 360.0f * std::numbers::pi_v<float> / 180.0f;
	}
	else if (diff.z < -180.0f * std::numbers::pi_v<float> / 180.0f) {
		diff.z += 360.0f * std::numbers::pi_v<float> / 180.0f;
	}

	return a + diff;
}
