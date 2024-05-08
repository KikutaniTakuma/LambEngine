#include "GameScene.h"
#include "AudioManager/AudioManager.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	currentCamera_->farClip = 3000.0f;
	currentCamera_->pos.z = -10.0f;
	
	drawerManager_->LoadModel("./Resources/Common/human/walk.gltf");
	model_ = std::make_unique<AnimationModel>();
	model_->Load("./Resources/Common/human/walk.gltf");
	model_->GetAnimator().SetLoopAnimation(true);
	model_->GetAnimator().Start();
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	currentCamera_->Debug("camera");
	currentCamera_->Update();

	model_->Update();

	transform_.Debug("model");
}

void GameScene::Draw() {
	model_->Draw(
		transform_.GetMatrix(),
		currentCamera_->GetViewProjection(),
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNone,
		true
	);
}