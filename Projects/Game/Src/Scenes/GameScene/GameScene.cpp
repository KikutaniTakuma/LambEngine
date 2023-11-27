#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include "Game/RockOn/RockOn.h"
#include <numbers>
#include <format>

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_.farClip = 3000.0f;
	camera_.farClip = 3000.0f;

	Player::Initialize(&globalVariables_);
	globalVariables_.LoadFile();


	player_ = std::make_unique<Player>(&globalVariables_);
	player_->SetCamera(&camera_);

	goal_ = std::make_unique<Goal>();

	skyDome_ = std::make_unique<Model>();
	skyDome_->LoadObj("./Resources/skydome/skydome.obj");
	skyDome_->scale_ *= 1000.0f;

	float moveFloorDuration = 32.0f;

	enemy_ = std::make_unique<Enemy>();
	enemy_->SetCamera(&camera_);
	enemy_->SetPlayer(player_.get());
	enemy_->pos_.z = moveFloorDuration;


	floor_.push_back(MoveFloor());
	floor_[0].moveDuration_.first.x = -4.0f;
	floor_[0].moveDuration_.first.z = moveFloorDuration;
	floor_[0].moveDuration_.second.x = 4.0f;
	floor_[0].moveDuration_.second.z = moveFloorDuration;
	floor_[0].pos_.z = moveFloorDuration;
	floor_.push_back(MoveFloor());
	floor_.push_back(MoveFloor());
	floor_[2].moveDuration_.first.z = moveFloorDuration * 2.0f;
	floor_[2].moveDuration_.second.z = moveFloorDuration * 2.0f;

	goal_ = std::make_unique<Goal>();
	goal_->collisionPos_.z = moveFloorDuration * 2.0f;

	rockOn_ = std::make_unique<RockOn>();
	rockOn_->Initialize(&camera_);
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	globalVariables_.Update();

	for (auto& model : models_) {
		model.Update();
	}

	for (auto& tex : texs_) {
		tex.Update();
	}

	skyDome_->Update();

	for (auto& floor : floor_) {
		floor.Update();
	}

	player_->Move();
	if (enemy_) {
		enemy_->Move();
	}

	for (auto& floor : floor_) {
		floor.IsCollision(player_.get());
		if ((floor.OnStay() || floor.OnEnter()) && player_->pos_.y > floor.pos_.y) {
			if (player_->moveVec_.y <= 0.0f) {
				player_->moveVec_.y = 0.0f;
			}
			player_->collisionPos_.y = player_->pos_.y;
			player_->pos_ += floor.moveVec_;
		}

		if (enemy_) {
			floor.IsCollision(enemy_.get());
			if ((floor.OnStay() || floor.OnEnter()) && enemy_->pos_.y > floor.pos_.y) {
				enemy_->moveVec.y = 0.0f;
				enemy_->collisionPos_.y = enemy_->pos_.y;
				enemy_->pos_ += floor.moveVec_;
			}
		}
	}

	if (enemy_) { enemy_->Update(); }
	player_->Update();
	goal_->Update();

	goal_->IsCollision(player_.get());
	if (enemy_) {
		enemy_->IsCollision(player_.get());
		if (enemy_->OnEnter() || (player_->pos_ - enemy_->pos_).Length() < enemy_->scale_.x) {
			player_.reset();
			player_ = std::make_unique<Player>(&globalVariables_);
			player_->SetCamera(&camera_);

			enemy_.reset();
			enemy_ = std::make_unique<Enemy>();
			enemy_->SetCamera(&camera_);
			enemy_->SetPlayer(player_.get());
			enemy_->pos_.z = 32.0f;
		}
	}
	if (player_->pos_.y < -10.0f || goal_->OnEnter()) {
		player_.reset();
		player_ = std::make_unique<Player>(&globalVariables_);
		player_->SetCamera(&camera_);

		enemy_.reset();
		enemy_ = std::make_unique<Enemy>();
		enemy_->SetCamera(&camera_);
		enemy_->SetPlayer(player_.get());
		enemy_->pos_.z = 32.0f;
	}
	if (enemy_ && player_->GetBehavior() == Player::Behavior::Attack) {
		if (player_->GetWeaponCollider().IsCollision(enemy_.get())) {
			enemy_.reset();
		}
	}

	camera_.Debug("camera");
	if (enemy_) {
		rockOn_->SetRockOnTarget(enemy_->GetPos());
	}
	else {
		rockOn_->isRockOn_ = false;
	}
	rockOn_->Update();
}

void GameScene::Draw() {
	for (auto& model : models_) {
		model.Draw(camera_.GetViewProjection(), camera_.GetPos());
	}

	for (auto& tex : texs_) {
		tex.Draw(camera_.GetViewProjection());
	}

	for (auto& floor : floor_) {
		floor.Draw(camera_.GetViewProjection(), camera_.GetPos());
	}

	skyDome_->Draw(camera_.GetViewProjection(), camera_.GetPos());

	if (enemy_) {
		enemy_->Draw();
	}

	player_->Draw();

	goal_->Draw(camera_.GetViewProjection(), camera_.GetPos());

	rockOn_->Draw();
}