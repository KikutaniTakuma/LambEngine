#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/EngineUtils/FrameInfo/FrameInfo.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include "Game/RockOn/RockOn.h"
#include "Utils/Camera/FollowCamera/FollowCamera.h" 
#include <numbers>
#include <format>

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	//camera_.farClip = 3000.0f;

	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->farClip = 3000.0f;
	followCamera_->offset_ = { 0.0f, 5.0f, -30.0f };
	followCamera_->rotate.x = 0.2f;
	followCamera_->rotateSpd_ = std::numbers::pi_v<float>;

	Player::Initialize(&globalVariables_);
	globalVariables_.LoadFile();


	player_ = std::make_unique<Player>(&globalVariables_);
	player_->SetCamera(followCamera_.get());

	goal_ = std::make_unique<Goal>();

	skyDome_ = std::make_unique<Model>();
	skyDome_->LoadObj("./Resources/skydome/skydome.obj");
	skyDome_->scale_ *= 1000.0f;

	float moveFloorDuration = 32.0f;

	InitEnemy();


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
	rockOn_->Initialize(followCamera_.get());

}

void GameScene::Finalize() {

}

void GameScene::InitEnemy() {
	enemys_.clear();
	float x = -12.0f;
	for (int32_t i = 0; i < kEnemyNumber_; i++) {
		enemys_.push_back(std::make_unique<Enemy>());
		float moveFloorDuration = 32.0f;
		enemys_.back()->SetCamera(followCamera_.get());
		enemys_.back()->SetPlayer(player_.get());
		enemys_.back()->pos_.z = moveFloorDuration;
		enemys_.back()->pos_.x = x;
		x += 6.0f;
	}
}

void GameScene::Update() {
	globalVariables_.Update();

	if (player_->IsAtackStart()) {
		for (auto& i : enemys_) {
			if (i) {
				i->EnableDamageAccept();
			}
		}
	}

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

	Mat4x4 cameraRotate;

	if (rockOn_->isRockOn_) {
		Vector3 enemyPos = rockOn_->NowRockOnPos();
		enemyPos.y = 0.0f;
		cameraRotate = followCamera_->GetRotateMatrixY(Vector3::zIdy, enemyPos);
	}
	else {
		cameraRotate = followCamera_->GetRotateMatrixY();
	}
	player_->Move(cameraRotate);
	for (auto& i : enemys_) {
		if (i) {
			i->Move();
		}
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

		for (auto& i : enemys_) {
			if (i) {
				floor.IsCollision(i.get());
				if ((floor.OnStay() || floor.OnEnter()) && i->pos_.y > floor.pos_.y) {
					i->moveVec.y = 0.0f;
					i->collisionPos_.y = i->pos_.y;
					i->pos_ += floor.moveVec_;
				}
			}
		}
	}

	for (auto& i : enemys_) {
		if (i) { i->Update(); }
	}
	player_->Update();
	goal_->Update();

	goal_->IsCollision(player_.get());
	for (auto& i : enemys_) {
		if (i) {
			i->IsCollision(player_.get());
			if (i->OnEnter() || (player_->pos_ - i->pos_).Length() < i->scale_.x) {
				player_.reset();
				player_ = std::make_unique<Player>(&globalVariables_);
				player_->SetCamera(followCamera_.get());

				InitEnemy();
				break;
			}
		}
	}
	
	if (player_->pos_.y < -10.0f || goal_->OnEnter()) {
		player_.reset();
		player_ = std::make_unique<Player>(&globalVariables_);
		player_->SetCamera(followCamera_.get());

		InitEnemy();
	}
	for (auto& i : enemys_) {
		if (i && player_->GetBehavior() == Player::Behavior::Attack && player_->IsWeaopnCollsion()) {
			if (player_->GetWeaponCollider().IsCollision(i.get())) {
				i->Damage();
				break;
			}
		}
	}

	for (auto& i : enemys_) {
		if (i && i->IsDeath()) {
			i.reset();
		}
	}
	followCamera_->Debug("camera");
	followCamera_->Move();

	if (player_) {
		followCamera_->gazePoint_ = player_->pos_;
		if (player_->GetIsDash().OnEnter()) {
			followCamera_->DelayEasingStart(player_->GetPos());
		}
	}
	Vector3 enemyPos;
	if (rockOn_->isRockOn_) {
		enemyPos = rockOn_->NowRockOnPos();
		enemyPos.y = 0.0f;
		followCamera_->Update(Vector3::zIdy, enemyPos);
	}
	else {
		followCamera_->Update();
	}

	float stickMoveValue = input_->GetGamepad()->GetStick(Gamepad::Stick::RIGHT_X);
	if (std::abs(stickMoveValue) == 0.0f) {
		rockOn_->SetRockOnTarget(enemyPos);
	}
	else {
		rockOn_->isRockOn_ = false;
	}
	rockOn_->Update();

	CreateRockOnPosisions();
	rockOn_->SetRockOnPositions(enemyPossiotns_);
}

void GameScene::Draw() {
	for (auto& model : models_) {
		model.Draw(followCamera_->GetViewProjection(), followCamera_->GetPos());
	}

	for (auto& tex : texs_) {
		tex.Draw(followCamera_->GetViewProjection());
	}

	for (auto& floor : floor_) {
		floor.Draw(followCamera_->GetViewProjection(), followCamera_->GetPos());
	}

	skyDome_->Draw(followCamera_->GetViewProjection(), followCamera_->GetPos());

	for(auto& i : enemys_){
		if (i) {
			i->Draw();
		}
	}

	player_->Draw();

	goal_->Draw(followCamera_->GetViewProjection(), followCamera_->GetPos());

	for (auto& i : enemys_) {
		if (i) {
			i->ParticleDraw();
		}
	}

	rockOn_->Draw();
}

void GameScene::CreateRockOnPosisions() {
	enemyPossiotns_.clear();
	for (auto& i : enemys_) {
		if (i && rockOn_->IsRockOnRange(i->GetPos())) {
			enemyPossiotns_.push_back(i->GetPos());
		}
	}
}