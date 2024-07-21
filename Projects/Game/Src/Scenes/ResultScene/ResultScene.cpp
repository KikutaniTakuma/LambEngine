#include "ResultScene.h"

ResultScene::ResultScene() : BaseScene(BaseScene::ID::Result) {}

void ResultScene::Initialize() {
	result_ = std::make_unique<Texture2D>();
	result_->LoadTexture("./Resources/OutGame/result.png");
	result_->scale = { 1280.0f, 720.0f };
}

void ResultScene::Finalize() {}

void ResultScene::Update() {
	camera_->Update();

	result_->Update();

	if (input_->GetInstance()->GetKey()->LongPush(DIK_SPACE) || input_->GetInstance()->GetGamepad()->LongPushed(Gamepad::Button::A)) {
		SceneManager::GetInstance()->SceneChange(BaseScene::ID::StageSelect);
	}
}

void ResultScene::Draw() {
	result_->Draw(camera_->GetViewOthographics(), Pipeline::Normal, false, false);
}
