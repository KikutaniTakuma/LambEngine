#include "GameScene.h"
#include "MeshManager/MeshManager.h"
#include "TextureManager/TextureManager.h"
#include "AudioManager/AudioManager.h"
#include "Engine/FrameInfo/FrameInfo.h"
#include <numbers>

GameScene::GameScene() :
	BaseScene(BaseScene::ID::Game),
	model_(),
	tex2D_(),
	texture_(nullptr)
{}

void GameScene::Initialize() {
	camera_.farClip = 3000.0f;
	camera_.pos.z = -5.0f;
	camera_.pos.y = 1.1f;

	// objファイル読み込み
	model_.ThreadLoadObj("./Resources/Watame/Watame.obj");
	
	model2_.scale_ *= 1000.0f;
	//model_.rotate.y = std::numbers::pi_v<float>;

	// テクスチャ読み込み
	tex2D_.ThreadLoadTexture("./Resources/uvChecker.png");
	tex2D_.pos_ = Vector2{ 380.0f,  -80.0f };
	tex2D_.isSameTexSize_ = true;
	tex2D_.texScalar_ = 0.5f;

	
	tex2D2_.pos_ = Vector2{ -380.0f,  -80.0f };
	tex2D2_.scale_ *= 256.0f;

	// テクスチャ単体でも読み込み出来る
	texture_ =
		textureManager_->LoadTexture("./Resources/Rabbit/Rabbit_face.png");
}

void GameScene::Finalize() {

}

void GameScene::Update() {
	meshManager_->ResetDrawCount();

	camera_.Debug("camera_");

	model_.Debug("model");
	model_.Update();
	model2_.Update();

	if (input_->GetKey()->Pushed(DIK_1)) {
		model2_.LoadObj("./Resources/Skydome/skydome.obj");
	}
	if (input_->GetKey()->Pushed(DIK_2)) {
		tex2D2_.LoadTexture("./Resources/watame.png");
	}

	tex2D_.Debug("tex");
	tex2D_.Update();
	tex2D2_.Update();
}

void GameScene::Draw() {
	camera_.Update(Vector3::zero);

	// 投資投影で描画
	model_.Draw(camera_.GetViewProjection(), camera_.GetPos());
	model2_.Draw(camera_.GetViewProjection(), camera_.GetPos());

	// 平行投影で描画
	tex2D_.Draw(camera_.GetOthographics(), Pipeline::Normal, true);
	tex2D2_.Draw(camera_.GetOthographics(), Pipeline::Normal, true);

	meshManager_->Draw();
}