#include "GameScene.h"
#include "AudioManager/AudioManager.h"

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game)
{}

void GameScene::Initialize() {
	currentCamera_->farClip = 3000.0f;
	currentCamera_->pos.z = -5.0f;
	

	transform_.scale = Vector3::kIdentity;

	// モデルのロード
	drawerManager_->LoadModel("./Resources/Common/Hololive/Watame/Watame.obj");
	// ロードしたモデルの取得
	model_ = drawerManager_->GetModel("./Resources/Common/Hololive/Watame/Watame.obj");
	color_ = Vector4(1.f,1.f,1.f,1.0f).GetColorRGBA();
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	currentCamera_->Debug("camera");
	currentCamera_->Update();

	transform_.Debug("model");
}

void GameScene::Draw() {
	model_->Draw(
		transform_.GetMatrix(),         // ワールドマトリックス
		currentCamera_->GetViewProjection(), // カメラのマトリックス
		color_,                         // 色
		BlendType::kNone,               // ブレンドタイプ
		true                           // ライティングあり・なし
	);
}