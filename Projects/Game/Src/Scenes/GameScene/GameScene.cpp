#include "GameScene.h"
#include "Engine/Graphics/MeshManager/MeshManager.h"
#include "Engine/Graphics/TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Utils/EngineInfo/EngineInfo.h"
#include "Math/Quaternion.h"
#include "Utils/ScreenOut/ScreenOut.h"
#include <numbers>
#include <format>
//#include "Game/Water/Water.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	//camera_->offset.z = -10.0f;

	model_ = std::make_unique<AnimationModel>();
	model_->Load("./Resources/gltf_test_model/AnimatedCube.gltf");
	model_->animator.Start();
	model_->animator.SetLoopAnimation(true);


	tex2D_ = std::make_unique<Texture2D>();
	tex2D_->Load("./Resources/uvChecker.png");
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");
	camera_->Update();

	model_->Debug("model");
	model_->Update();
	tex2D_->Debug("tex2D_");
}

void GameScene::Draw() {
	//model_->Draw(camera_->GetViewProjection(), BlendType::kNone, true);

	tex2D_->Draw(camera_->GetViewProjection(), BlendType::kNone);
}