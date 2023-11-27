#include "RockOn.h"
#include "Utils/Camera/Camera.h"
#include <numbers>

void RockOn::Initialize(class Camera* camera) {
	staticCamera_ = std::make_unique<Camera>();
	staticCamera_->Update();

	rockOnRange_.first = 0.01f;
	rockOnRange_.second = 256.0f;

	rockOnRotateRange_.first = std::numbers::pi_v<float> / 2.5f;
	rockOnRotateRange_.second = std::numbers::pi_v<float> * 0.75f;

	camera_ = camera;

	rockOnMark_ = std::make_unique<Texture2D>();
	rockOnMark_->isSameTexSize_ = true;
	rockOnMark_->texScalar_ = 0.5f;
	rockOnMark_->LoadTexture("./Resources/ball.png");
}

void RockOn::Update() {
	rockOnMark_->Debug("RockOn");
	
	if (isRockOn_) {
		Vector3 positionScreen = targetPos_ * camera_->GetViewProjectionVp();

		Vector3 posWorld = positionScreen * MakeMatrixInverse(staticCamera_->GetViewOthographicsVp());

		rockOnMark_->pos_ = posWorld;
		rockOnMark_->pos_.z = 1.0f;
	}
	rockOnMark_->Update();
}

void RockOn::Draw() {
	if (isRockOn_) {
		rockOnMark_->Draw(staticCamera_->GetViewOthographics(), Pipeline::Normal, false);
	}
}

void RockOn::SetRockOnTarget(const Vector3& pos) {
	if (IsRockOnRange(pos)) {
		targetPos_ = pos;
		isRockOn_ = true;
	}
	else {
		isRockOn_ = false;
	}
}

bool RockOn::IsRockOnRange(const Vector3& pos) const {
	Vector3 viewPos = pos * camera_->GetView();
	viewPos = { viewPos.x,0.0f,viewPos.y };
	float range = viewPos.Length();
	if (!(rockOnRange_.first <= range && range <= rockOnRange_.second)) {
		return false;
	}

	float posSin = (viewPos).Normalize().Cross(Vector3::zIdy).Length();
	float rangeSin = std::sin(rockOnRotateRange_.first);
	if (!(posSin <= rangeSin)) {
		return false;
	}

	return true;
}