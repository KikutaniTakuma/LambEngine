#include "RockOn.h"
#include "Utils/Camera/Camera.h"
#include "Input/Input.h"
#include <numbers>
#include <vector>

void RockOn::Initialize(class Camera* camera) {
	staticCamera_ = std::make_unique<Camera>();
	staticCamera_->Update();

	rockOnRange_.first = 0.01f;
	rockOnRange_.second = 256.0f;

	rockOnRotateRange_.first = std::numbers::pi_v<float> * 0.25f;
	rockOnRotateRange_.second = std::numbers::pi_v<float> * 0.75f;

	camera_ = camera;

	rockOnMark_ = std::make_unique<Texture2D>();
	rockOnMark_->isSameTexSize_ = true;
	rockOnMark_->texScalar_ = 0.5f;
	rockOnMark_->LoadTexture("./Resources/ball.png");

	isAutoRock_ = false;
}

void RockOn::Update() {
	static Input* const input = Input::GetInstance();
	rockOnMark_->Debug("RockOn");
	
	if (isRockOn_) {
		if (!positions_.empty()) {
			if (input->GetGamepad()->Pushed(Gamepad::Button::LEFT)) {
				currentRockOn_--;
			}
			else if (input->GetGamepad()->Pushed(Gamepad::Button::RIGHT)) {
				currentRockOn_++;
			}

			currentRockOn_ = std::clamp(currentRockOn_, 0, int32_t(positions_.size()-1llu));

			targetPos_ = positions_[currentRockOn_];
		}

		Vector3 positionScreen = targetPos_ * camera_->GetViewProjectionVp();

		Vector3 posWorld = positionScreen * MakeMatrixInverse(staticCamera_->GetViewOthographicsVp());

		rockOnMark_->pos_ = posWorld;
		rockOnMark_->pos_.z = 1.0f;
	}
	rockOnMark_->Update();

	if (input->GetGamepad()->Pushed(Gamepad::Button::LEFT_SHOULDER)) {
		isAutoRock_ = !isAutoRock_;
	}
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
	static Input* const input = Input::GetInstance();

	Vector3 viewPos = pos * camera_->GetView();
	float range = viewPos.z;
	if (!(rockOnRange_.first <= range && range <= rockOnRange_.second)) {
		return false;
	}

	viewPos.y = 0.0f;
	float posSin = (viewPos).Normalize().Cross(Vector3::zIdy).Length();
	float rangeSin = std::sin(rockOnRotateRange_.first);
	if (!(posSin < rangeSin)) {
		return false;
	}
	/*Vector3 posTmp = pos;
	posTmp.y = 0.0f;
	if (0.0f > camera_->GetDirection(Vector3::zIdy, posTmp).Dot(pos)) {
		return false;
	}*/
	if (!isAutoRock_ && 0.0f == input->GetGamepad()->GetTriger(Gamepad::Triger::LEFT, 0.0f)) {
		return false;
	}

	return true;
}

Vector3 RockOn::SortRockOn(std::initializer_list<Vector3> positions) {
	std::vector<Vector3> posBuf;
	for (auto& i : positions) {
		if (IsRockOnRange(i)) {
			posBuf.push_back(i);
		}
	}

	if (posBuf.empty()) {
		return Vector3::zero;
	}

	std::vector<Vector3> viewPos;

	for (const auto& i : posBuf) {
		viewPos.push_back(i * camera_->GetView());
	}

	auto minElement = std::min_element(viewPos.begin(), viewPos.end(),
		[](const Vector3& a, const Vector3& b)->bool { 
			return  a.z < b.z;
		}
	);

	size_t index = std::distance(viewPos.begin(), minElement);

	return posBuf[index];
}

void RockOn::SetRockOnPositions(const std::vector<Vector3>& positions) {
	std::vector<Vector3> posBuf;
	for (auto& i : positions) {
		if (IsRockOnRange(i)) {
			posBuf.push_back(i);
		}
	}

	if (posBuf.empty()) {
		return;
	}

	positions_ = posBuf;
}

Vector3 RockOn::NowRockOnPos() const {
	return targetPos_;
}