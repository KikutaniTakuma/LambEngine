#include "GameScene.h"
#include "AudioManager/AudioManager.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	currentCamera_->farClip = 3000.0f;
	currentCamera_->pos.z = -10.0f;
	
	/*drawerManager_->LoadModel("./Resources/Common/Hololive/Watame/Watame.obj");
	model2_ = drawerManager_->GetModel("./Resources/Common/Hololive/Watame/Watame.obj");*/
	model_ = std::make_unique<AnimationModel>();
	model_->Load("./Resources/Common/human/sneakWalk.gltf");
	model_->GetAnimator().SetLoopAnimation(true);
	model_->GetAnimator().Start();

	animationManager_->LoadAnimations("./Resources/Common/human/walk.gltf");
	animations_ = animationManager_->GetAnimations("./Resources/Common/human/walk.gltf");
	animations2_ = animationManager_->GetAnimations("./Resources/Common/human/sneakWalk.gltf");
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	currentCamera_->Debug("camera");
	currentCamera_->Update();

	if (input_->GetKey()->Pushed(DIK_1)) {
		model_->GetAnimator().SetAnimations(animations_);
	}
	else if (input_->GetKey()->Pushed(DIK_2)) {
		model_->GetAnimator().SetAnimations(animations2_);
	}

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