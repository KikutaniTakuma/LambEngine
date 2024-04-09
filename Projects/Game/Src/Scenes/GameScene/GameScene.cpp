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
	model_->animator.SetLoopAnimation(false);
	model_->animator.SetIsFullAnimation(false);


	//for (auto& i : tex2D_) {
	//	i = std::make_unique<Texture2D>();
	//	i->Load("./Resources/uvChecker.png");
	//}
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");
	camera_->Update();

	model_->Debug("model");
	model_->Update();
	//for (size_t index = 0; auto & i : tex2D_) {
	//	i->Debug("tex2D_" + std::to_string(index));
	//	index++;
	//}
}

void GameScene::Draw() {
	model_->Draw(camera_.get(), BlendType::kNone, true);

	//for (auto& i : tex2D_) {
	//	i->Draw(camera_.get(), BlendType::kNone);
	//}
}