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

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	camera_->rotate.x = 0.29f;

	waterPipelineObject_ = new WaterPipeline{};
	waterPipelineObject_->Init();

	//pera_.Initialize("./Resources/Shaders/PostShader/PostNone.PS.hlsl");
	pera_.Initialize(waterPipelineObject_);
	//pera_.scale = Lamb::ClientSize();
	pera_.scale.x = 5.0f;
	pera_.scale.y = 5.0f;
	pera_.rotate.x = 1.57f;

	model_.reset(new Model{});

	model_->LoadObj("./Resources/Ball.obj");
	staticCamera_.pos.z = -0.01f;
	staticCamera_.Update();

	tex_.reset(new Texture2D{});

	tex_->scale = Lamb::ClientSize();
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");


	pera_.Debug("pera");
	pera_.Update();
	
	model_->Debug("model");
	model_->Update();

	tex_->Debug("tex_");

	tex_->Update();
}

void GameScene::Draw() {
	camera_->Update(Vector3::kZero);

	waterPipelineObject_->SetCameraPos(camera_->GetPos());
	waterPipelineObject_->SetRotate(pera_.rotate);
	
	pera_.PreDraw();
	model_->Draw(camera_->GetViewProjection(), camera_->GetPos());
	//tex_->Draw(staticCamera_.GetViewOthographics());
	pera_.Draw(camera_->GetViewProjection(), Pipeline::Add);
}