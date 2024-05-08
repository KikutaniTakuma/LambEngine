#include "GameScene.h"
#include "AudioManager/AudioManager.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	currentCamera_->farClip = 3000.0f;
	currentCamera_->pos.z = -10.0f;
	
	drawerManager_->LoadModel("./Resources/Common/human/walk.gltf");
	model_ = drawerManager_->GetModel("./Resources/Common/human/walk.gltf");

	skeleton_ = std::make_unique<Skeleton>(Lamb::CreateSkeleton(model_->GetNode()));

	animator_ = std::make_unique<Animator>();
	animator_->Load("./Resources/Common/human/walk.gltf");

	animator_->SetLoopAnimation(true);
	animator_->Start();

	transform_.scale *= 100.0f;
	transform_.rotate.x = -1.57f;
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	currentCamera_->Debug("camera");
	currentCamera_->Update();

	animator_->Update(*skeleton_);
	skeleton_->Update();

	transform_.Debug("model");
}

void GameScene::Draw() {
	model_->Draw(
		transform_.GetMatrix(),
		currentCamera_->GetViewProjection(),
		std::numeric_limits<uint32_t>::max(),
		BlendType::kNone
	);

	skeleton_->Draw(currentCamera_->GetViewProjection());
}