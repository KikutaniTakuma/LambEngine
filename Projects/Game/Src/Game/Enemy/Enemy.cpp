#include "Enemy.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include <numbers>
#include <cmath>
#include "Game/Player/Player.h"
#include "imgui.h"

Enemy::Enemy() :
	spd(3.0f),
	moveVec(),
	camera(nullptr),
	model(0),
	freqSpd(std::numbers::pi_v<float>),
	freq(0.0f),
	radius(5.0f)
{
	model.push_back(std::make_unique<Model>());
	model[0]->LoadObj("AL_Resouce/Enemy/Enemy.obj");
	model.push_back(std::make_unique<Model>());
	model[1]->LoadObj("./Resources/Cube.obj");
	model[1]->SetParent(model[0].get());
	model[1]->scale_ *= 0.5f;

	easeDuration.first.y = 2.0f;
	easeDuration.second.y = 3.0f;

	ease.Start(true, 2.0f, Easeing::InOutSine);

	distanceLimit = 7.0f;
	isPlayerCollsion = false;

	colliser_.scale_ = { 2.0f, 1.5f, 2.0f };

	pos_.y = 8.0f;
}

void Enemy::Move() {
	colliser_.UpdateCollision();
	moveVec = {};
	float deltaTime = FrameInfo::GetInstance()->GetDelta();

	freq += freqSpd * deltaTime;
	if (freq > 2.0f * std::numbers::pi_v<float>) {
		freq = 0.0f;
	}

	if (player_) {
		if ((player_->pos_ - pos_).Length() < distanceLimit) {
			moveVec = (player_->pos_ - pos_).Normalize() * spd;
			moveVec.y = -15.0f;
			Vector2 rotate = { moveVec.z, moveVec.x };

			model[0]->rotate_.y = rotate.GetRad() + (std::numbers::pi_v<float> *1.5f);
		}
		else {
			model[0]->rotate_.y = freq + (std::numbers::pi_v<float> *0.5f);
			moveVec.y = -15.0f;
		}
	}
}

void Enemy::Update() {
	float deltaTime = FrameInfo::GetInstance()->GetDelta();

	pos_ += moveVec * deltaTime;
	colliser_.collisionPos_ = pos_;
	model[0]->pos_ = pos_;

	model[1]->pos_ = ease.Get(easeDuration.first, easeDuration.second);

	ease.Update();
}

void Enemy::Draw() {
	for (auto& i : model) {
		i->Draw(camera->GetViewProjection(), camera->pos);
	}

	colliser_.DebugDraw(camera->GetViewProjection());
}