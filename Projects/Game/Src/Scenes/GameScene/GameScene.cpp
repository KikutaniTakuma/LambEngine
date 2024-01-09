#include "GameScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>
#include "Engine/Graphics/PipelineObject/WaterPipeline/WaterPipeline.h"
#include "Game/Water/Water.h"
#include "Game/Player/Player.h"
#include "Game/Enemy/Enemy.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	camera_->rotate.x = 0.04f;
	camera_->offset.z = -60.0f;
	camera_->offset.y = 8.0f;

	water_.reset(new Water{});
	water_->Init();

	player_.reset(new Player{});
	player_->Initialize();

	enemy_.reset(new Enemy{});
	enemy_->Initialize();

	camera_->Update(player_->GetPos());
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");

	water_->Debug("water");
	water_->Update();


	player_->Move();
	player_->Debug("player");
	player_->Update(*camera_);


	enemy_->Debug("Boss");
	enemy_->Update(*player_, *camera_);


	player_->Attack(*enemy_);

	player_->Collision(*enemy_);
	enemy_->Collision(*player_);
}

void GameScene::Draw() {
	meshManager_->ResetDrawCount();

	camera_->rotate.y = player_->GetRotate();
	camera_->Update(player_->GetPos());

	water_->Draw(camera_->GetViewProjection(), camera_->GetPos());

	player_->Draw(*camera_);

	enemy_->Draw(*camera_);

	meshManager_->Draw();

	player_->ParticleDraw();
	//enemy_->ParticleDraw();
}