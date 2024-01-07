#include "Player.h"
#include "Utils/Camera/Camera.h"
#include "Input/Input.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "imgui.h"

#include <algorithm>

void Player::Initialize()
{
	model_.reset(new Model{"./Resources/Cube.obj"});

	addSpeed_ = 1.0f;

	speed_ = 0.0f;
	attack_ = 2.0f;

	speedDecay_ = -5.0f;
}

void Player::Move()
{
	static KeyInput* const key = Input::GetInstance()->GetKey();
	static Gamepad* const gamepad = Input::GetInstance()->GetGamepad();

	Vector3 move;

	Vector2 stick = {
		gamepad->GetStick(Gamepad::Stick::LEFT_X),
		gamepad->GetStick(Gamepad::Stick::LEFT_Y) 
	};

	bool isMove = true;

	// 速度アップ
	if (key->GetKey(DIK_D) || key->GetKey(DIK_RIGHT) || 
		gamepad->GetButton(Gamepad::Button::RIGHT) || 0.0f < stick.x) 
	{
		speed_ += addSpeed_ * Lamb::DeltaTime();
	}
	// 上とは反対方向に移動
	else if (key->GetKey(DIK_A) || key->GetKey(DIK_LEFT) || 
		gamepad->GetButton(Gamepad::Button::LEFT) || stick.x < 0.0f) 
	{
		speed_ -= addSpeed_ * Lamb::DeltaTime();
	}
	else {
		isMove = false;
	}

	// 速度のクランプ
	speed_ = std::clamp(speed_, -kMaxSpeed_, kMaxSpeed_);

	// 動いてなかったら
	if (!isMove) {
		// 速度が0未満
		if (speed_ < 0.0f) {
			speed_ -= speedDecay_ * Lamb::DeltaTime();
			// 0以上になったら0を代入
			if (0.0f <= speed_) {
				speed_ = 0.0f;
			}
		}
		// 速度が0より大きいい
		else if(0.0f < speed_){
			speed_ += speedDecay_ * Lamb::DeltaTime();
			// 0以下になったら0を代入
			if (speed_ <= 0.0f) {
				speed_ = 0.0f;
			}
		}
	}
}

void Player::Update() {
	model_->pos.x += speed_ * Lamb::DeltaTime();
	model_->Update();
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
		ImGui::DragFloat("速度", &speed_);
		ImGui::DragFloat("攻撃", &attack_);
		ImGui::TreePop();
	}
	ImGui::End();
#endif // _DEBUG
}
