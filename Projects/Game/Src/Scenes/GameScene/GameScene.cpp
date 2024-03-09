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
#include "Game/Cloud/Cloud.h"
#include "AudioManager/AudioManager.h"
#include "Utils/ScreenOut/ScreenOut.h"

#include "Math/Matrix.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	//camera_->pos.y = 15.0f;
	camera_->pos.z = -5.0f;
	//camera_->rotate.x = 0.25f;
	//camera_->offset.z = -60.0f;
	//camera_->offset.y = 8.0f;

	skydome_.reset(new SkyDome());

	skydome_->Initialize();
}

void GameScene::Finalize() {
	skydome_->Finalize();
}

void GameScene::Update() {
	camera_->Debug("カメラ");
	camera_->Update();

	skydome_->Debug("skyDome");
	skydome_->Upadate();
}

void GameScene::Draw() {
	skydome_->Draw(*camera_);
}