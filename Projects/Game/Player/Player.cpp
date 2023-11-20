#include "Player.h"
#include "Input/Input.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>
#include <cmath>
#include "externals/nlohmann/json.hpp"
#include "GlobalVariables/GlobalVariables.h"
#include "externals/imgui/imgui.h"

Player::Player():
	tex2D_{},
	speed_{100.0f},
	pos_{},
	rotate_{},
	scale_{Vector2::identity}
{
	tex2D_.isSameTexSize_ = true;
	tex2D_.texScalar_ *= 50.0f;
}

void Player::ResetMoveVec() {
	moveVec_ = Vector3::zero;
}

void Player::Update() {
	pos_ += moveVec_;
	tex2D_.pos_ = pos_;
	tex2D_.scale_ = scale_;
	tex2D_.rotate_ = rotate_;

	tex2D_.Update();
}

void Player::Draw(const Mat4x4& cameraMat) {
	tex2D_.Draw(cameraMat);
}

void Player::Debug() {
	tex2D_.Debug("player");
}

void Player::MoveRight() {
	moveVec_.x += speed_ * FrameInfo::GetInstance()->GetDelta();
}
void Player::MoveLeft() {
	moveVec_.x -= speed_ * FrameInfo::GetInstance()->GetDelta();
}