#include "GameScene.h"
#include "AudioManager/AudioManager.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	

	transform_.scale = Vector3::kIdentity;

	// モデルのロード
	drawerManager_->LoadModel("./Resources/Cube.obj");
	// ロードしたモデルの取得
	model_ = drawerManager_->GetModel("./Resources/Cube.obj");
	color_ = Vector4(0.8f,0.2f,0.2f,1.0f).GetColorRGBA();
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	camera_->Debug("camera");
	camera_->Update();

	transform_.Debug("model");
}

void GameScene::Draw() {
	model_->Draw(
		transform_.GetMatrix(),         // ワールドマトリックス
		camera_->GetViewOthographics(), // カメラのマトリックス
		color_,                         // 色
		BlendType::kNone,               // ブレンドタイプ
		false                           // ライティングあり・なし
	);
}