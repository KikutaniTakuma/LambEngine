#include "ChargeCamera.h"

#include <cmath>
#include <numbers>

#include "Camera/Camera.h"
#include "Game/FollowCamera/FollowCamera.h"
#include "Game/CameraManager/CameraManager.h"
#include "Game/Player/Player.h"

ChargeCamera::ChargeCamera() {}

void ChargeCamera::Initialize() {
	EasingData data{};
	data.start = { 5.7f,0.15f,-3.0f };
	data.end = { 0.0f,10.977f,-38.464f };
	data.time = 1;
	easingTranslateData_.emplace_back(data);

	// カスタマイズ画面からチャージのカメラへの
	customizeToChargeTranslate_.start = {};
	customizeToChargeTranslate_.end = data.start;
	customizeToChargeTranslate_.time = 1;

	data.start = { 0.0f, -90.0f*std::numbers::pi_v<float> / 180.0f ,0.0f };
	data.end = { 0.278f,0.0f,0.0f };
	data.time = 1;
	easingRotateData_.emplace_back(data);

	// カスタマイズ画面からチャージのカメラへの
	customizeToChargeRotate_.start = {};
	customizeToChargeRotate_.end = data.start;
	customizeToChargeRotate_.time = 1;

	if (!easingTranslateData_.empty()) {
		easingTranslateData_.front().easing.Start(false, easingTranslateData_.front().time, Easeing::Type::kInSine);
	}
	if (!easingRotateData_.empty()) {
		easingRotateData_.front().easing.Start(false, easingRotateData_.front().time, Easeing::Type::kInSine);
	}
	isSceneChange_ = true;

	camera_->pos = easingTranslateData_.back().start;
	camera_->rotate = easingRotateData_.back().start;
	camera_->Update();
}

void ChargeCamera::Update() {
	if (customizeToChargeTranslate_.easing.GetIsActive()) {
		camera_->pos = customizeToChargeTranslate_.easing.Get(customizeToChargeTranslate_.start, customizeToChargeTranslate_.end);

	}
	else {
		isSceneChange_ = false;
	}
	customizeToChargeTranslate_.easing.Update();
	if (customizeToChargeRotate_.easing.GetIsActive()) {
		camera_->rotate = customizeToChargeRotate_.easing.Get(customizeToChargeRotate_.start, customizeToChargeRotate_.end);
	}
	customizeToChargeRotate_.easing.Update();
	if (!player_->GetIsCharge() &&
		!customizeToChargeTranslate_.easing.GetIsActive() &&
		!customizeToChargeRotate_.easing.GetIsActive()) {
		for (size_t i = 0; auto & translate : easingTranslateData_) {
			i++;
			translate.easing.Update();
			if (!translate.isUsed) {
				camera_->pos = translate.easing.Get(translate.start, translate.end);
				if (translate.easing.ActiveExit()) {
					if (i < easingTranslateData_.size()) {
						easingTranslateData_.at(i).easing.Start(false, easingTranslateData_.at(i).time);
					}
					translate.isUsed = true;
				}
				break;
			}
		}
		for (size_t i = 0; auto & rotate : easingRotateData_) {
			i++;
			rotate.easing.Update();
			if (!rotate.isUsed) {
				camera_->rotate = rotate.easing.Get(rotate.start, rotate.end);
				if (rotate.easing.ActiveExit()) {
					if (i < easingRotateData_.size()) {
						easingRotateData_.at(i).easing.Start(false, easingRotateData_.at(i).time);
					}
					rotate.isUsed = true;
				}
				break;
			}
		}
		// 最後までカメラのイージングが終了したら
		if (easingTranslateData_.back().isUsed) {
			isSceneChange_ = true;
			CameraManager::GetInstance()->SetType(CameraManager::Type::kFollow);
			player_->SetIsStart(true);
			followCamera_->SetDestinationAngle(easingRotateData_.back().end);
		}
	}
	else {
		isSceneChange_ = false;
	}
	camera_->Update();
}

void ChargeCamera::SetStartCamera(const Vector3& pos, const Vector3& rotate) {
	customizeToChargeTranslate_.start = pos;
	customizeToChargeRotate_.start = rotate;
	customizeToChargeRotate_.end = LerpShortAngle(rotate, customizeToChargeRotate_.end);

	customizeToChargeTranslate_.easing.Start(false, customizeToChargeTranslate_.time, Easeing::Type::kInSine);
	customizeToChargeRotate_.easing.Start(false, customizeToChargeRotate_.time, Easeing::Type::kInSine);
	isSceneChange_ = true;
	Reset();
}

Vector3 ChargeCamera::LerpShortAngle(const Vector3& a, const Vector3& b) {
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

void ChargeCamera::Reset() {
	for (auto& easingTranslateData : easingTranslateData_) {
		easingTranslateData.isUsed = false;
	}
	for (auto& easingRotateData : easingRotateData_) {
		easingRotateData.isUsed = false;
	}
	if (!easingTranslateData_.empty()) {
		easingTranslateData_.front().easing.Start(false, easingTranslateData_.front().time, Easeing::Type::kInSine);
	}
	if (!easingRotateData_.empty()) {
		easingRotateData_.front().easing.Start(false, easingRotateData_.front().time, Easeing::Type::kInSine);
	}
	customizeToChargeTranslate_.isUsed = false;
	customizeToChargeRotate_.isUsed = false;
}
