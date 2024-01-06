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

	// Dキーかアローキー(右)かスティックを右に倒したら
	if (key->GetKey(DIK_D) || key->GetKey(DIK_RIGHT) || 
		gamepad->GetButton(Gamepad::Button::RIGHT) || 0.0f < stick.x) 
	{
		speed_ += addSpeed_ * Lamb::DeltaTime();
	}
	else if (key->GetKey(DIK_A) || key->GetKey(DIK_LEFT) || 
		gamepad->GetButton(Gamepad::Button::LEFT) || stick.x < 0.0f) 
	{
		speed_ -= addSpeed_ * Lamb::DeltaTime();
	}
	else {
		isMove = false;
	}

	speed_ = std::clamp(speed_, -kMaxSpeed_, kMaxSpeed_);

	if (!isMove) {
		if (speed_ < 0.0f) {
			speed_ -= speedDecay_ * Lamb::DeltaTime();
			if (0.0f <= speed_) {
				speed_ = 0.0f;
			}
		}
		else if(0.0f < speed_){
			speed_ += speedDecay_ * Lamb::DeltaTime();
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
