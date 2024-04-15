#include "GameScene.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	

	transform_.scale = Vector3::kIdentity/* * 500*/;
	uvTransform_.scale = Vector3::kIdentity;

	tex2D_ = drawerManager_->GetTexture2D();

	drawerManager_->LoadModel("./Resources/Cube.obj");

	model_ = drawerManager_->GetModel("./Resources/Cube.obj");
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");
	camera_->Update();

	transform_.Debug("model");
}

void GameScene::Draw() {
	/*tex2D_->Draw(
		transform_.GetMatrix(),
		uvTransform_.GetMatrix(),
		camera_->GetViewOthographics(),
		0u,
		0xffffffff,
		BlendType::kNone
	);*/

	model_->Draw(
		transform_.GetMatrix(),
		camera_->GetViewProjection(),
		0xffffffff,
		BlendType::kNone,
		false
	);
}