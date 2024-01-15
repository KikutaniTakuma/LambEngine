#include "GameScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>
#include "Game/Water/Water.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	camera_->rotate.x = 0.04f;
	camera_->offset.z = -10.0f;
	camera_->offset.y = 8.0f;

	//water_ = Water::GetInstance();
	

	model_.reset(new Model{});
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");

	//water_->Debug("water");
	//water_->Update();

	model_->Debug("model");
	model_->Update();
}

void GameScene::Draw() {
	meshManager_->ResetDrawCount();

	camera_->Update(Vector3::kZero);

	model_->Draw(camera_->GetViewProjection(), camera_->GetPos());

	//water_->Draw(camera_->GetViewProjection(), camera_->GetPos());
}