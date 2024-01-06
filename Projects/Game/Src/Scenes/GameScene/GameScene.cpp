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

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	camera_->rotate.x = 0.29f;

	water_.reset(new Water{});
	water_->Init();

	player_.reset(new Player{});
	player_->Initialize();
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");

	water_->Debug("water");
	water_->Update();


	player_->Move();
	player_->Debug("player");
	player_->Update();
}

void GameScene::Draw() {
	camera_->Update(Vector3::kZero);

	water_->Draw(camera_->GetViewProjection(), camera_->GetPos());

	player_->Draw(*camera_);
}