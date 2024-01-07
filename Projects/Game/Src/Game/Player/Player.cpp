#include "Player.h"
#include "Utils/Camera/Camera.h"
#include "Input/Input.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "imgui.h"
#include "../Enemy/Enemy.h"

#include <algorithm>
#include <numbers>

void Player::Initialize()
{
	model_.reset(new Model{"./Resources/Cube.obj"});

	attack_ = 2.0f;

	maxSpeed_ = 0.628f;
	speed_ = 0.0f;
	addSpeed_ = maxSpeed_ * 0.3f;
	speedDecay_ = -maxSpeed_ * 0.5f;

	float maxOffsetZ = 50.0f;
	float minOffsetZ = 20.0f;

	maxOffset_ = -Vector3::kZIndentity * minOffsetZ;
	minOffset_ = -Vector3::kZIndentity * maxOffsetZ;
	offset_ = -(maxOffset_ + minOffset_) * 0.5f;
	offsetSpeed_ = 5.0f;

	basisSpeedScale_ = 1.0f / minOffsetZ;
	speedScale_ = 1.0f / (offset_.Length() * basisSpeedScale_);

	rotate_ = 0.0f;

	radius_ = 0.5f;
	isCollisioned_ = false;
	invincibleTime_ = 1.5f;
	isCollisionedTime_ = 0.0f;


	hp_ = 100.0f;
}

void Player::Move()
{
	static KeyInput* const key = Input::GetInstance()->GetKey();
	static Gamepad* const gamepad = Input::GetInstance()->GetGamepad();

	Vector3 move;

	const Vector2 stick = {
		gamepad->GetStick(Gamepad::Stick::LEFT_X),
		gamepad->GetStick(Gamepad::Stick::LEFT_Y) 
	};


	// オフセット移動
	if (key->GetKey(DIK_W) || key->GetKey(DIK_UP) ||
		gamepad->GetButton(Gamepad::Button::UP) || 0.0f < stick.y
		) 
	{
		offset_.z += offsetSpeed_ * Lamb::DeltaTime();
	}
	if (key->GetKey(DIK_S) || key->GetKey(DIK_DOWN) ||
		gamepad->GetButton(Gamepad::Button::DOWN) || stick.y < 0.0f
		)
	{
		offset_.z -= offsetSpeed_ * Lamb::DeltaTime();
	}

	offset_.z = std::clamp(offset_.z, minOffset_.z, maxOffset_.z);

	speedScale_ = 1.0f / (offset_.Length() * basisSpeedScale_);

	bool isMove = true;
	// 右へ
	if (key->GetKey(DIK_D) || key->GetKey(DIK_RIGHT) || 
		gamepad->GetButton(Gamepad::Button::RIGHT) || 0.0f < stick.x) 
	{
		speed_ -= addSpeed_ * Lamb::DeltaTime() * speedScale_;
	}
	// 左へ
	else if (key->GetKey(DIK_A) || key->GetKey(DIK_LEFT) || 
		gamepad->GetButton(Gamepad::Button::LEFT) || stick.x < 0.0f) 
	{
		speed_ += addSpeed_ * Lamb::DeltaTime() * speedScale_;
	}
	else {
		isMove = false;
	}

	// 速度のクランプ
	speed_ = std::clamp(speed_, -maxSpeed_ * speedScale_, maxSpeed_ * speedScale_);

	// 動いてなかったら
	if (!isMove) {
		// 速度が0未満
		if (speed_ < 0.0f) {
			speed_ -= speedDecay_ * Lamb::DeltaTime() * speedScale_;
			// 0以上になったら0を代入
			if (0.0f <= speed_) {
				speed_ = 0.0f;
			}
		}
		// 速度が0より大きいい
		else if(0.0f < speed_){
			speed_ += speedDecay_ * Lamb::DeltaTime() * speedScale_;
			// 0以下になったら0を代入
			if (speed_ <= 0.0f) {
				speed_ = 0.0f;
			}
		}
	}
}

void Player::Update() {
	rotate_ += speed_ * Lamb::DeltaTime() * speedScale_;

	model_->pos = offset_ * Quaternion::MakeRotateYAxis(rotate_);
	model_->rotate.y = rotate_;

	model_->Update();

	if (isCollisioned_) {
		isCollisionedTime_ += Lamb::DeltaTime();
		if (invincibleTime_ < isCollisionedTime_) {
			isCollisionedTime_ = 0.0f;
			isCollisioned_ = false;
		}
	}
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(camera.GetViewProjection(), camera.GetPos());
}

void Player::Debug([[maybe_unused]]const std::string& guiName)
{
#ifdef _DEBUG
	ImGui::Begin(guiName.c_str());
	model_->Debug(guiName.c_str());

	if (ImGui::TreeNode("ステータス")) {
		ImGui::DragFloat("最大速度", &maxSpeed_, 0.01f, 0.0f, std::numbers::pi_v<float> *2.0f);
		ImGui::DragFloat("速度加算", &addSpeed_, 0.01f, 0.0f, std::numbers::pi_v<float> * 2.0f);
		ImGui::DragFloat("速度減衰", &speedDecay_);
		ImGui::DragFloat("速度", &speed_);
		ImGui::DragFloat("攻撃", &attack_);
		ImGui::DragFloat3("オフセット", &offset_.x);
		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG
}

void Player::Collision(const Enemy& enemy)
{
	const auto& enemyBullets = enemy.GetBullets();

	// もし無敵だったら
	if (isCollisioned_) {
		return;
	}

	bool isCollision = false;

	for (const auto& i : enemyBullets) {
		if (i->GetIsActive()) {
			if (i->CollisionBullet(model_->pos, radius_)) {
				isCollision = true;
				isCollisioned_ = true;

				hp_ -= i->GetAttack();
				break;
			}
		}
	}
}
