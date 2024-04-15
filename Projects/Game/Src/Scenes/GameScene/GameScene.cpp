#include "GameScene.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	

	transform_.scale = Vector3::kIdentity * 500;
	uvTransform_.scale = Vector3::kIdentity;

	tex2D_ = drawerManager_->GetTexture2D();
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");
	camera_->Update();


}

void GameScene::Draw() {
	tex2D_->Draw(
		transform_.GetMatrix(),
		uvTransform_.GetMatrix(),
		camera_->GetViewOthographics(),
		0u,
		0xffffffff,
		BlendType::kNone
	);
}