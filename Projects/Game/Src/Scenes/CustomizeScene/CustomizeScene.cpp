#include "CustomizeScene.h"

CustomizeScene::CustomizeScene() : 
	BaseScene(BaseScene::ID::Customize) {}

void CustomizeScene::Initialize() {
	customize_ = std::make_unique<Customize>();
	customizeCamera_ = std::make_unique<CustomizeCamera>();

	customize_->Initialize();
	customizeCamera_->SetCamera(camera_.get());
	//customizeCamera_->SetTarget(customize_->GetPosition());
	customizeCamera_->Initialize();

	camera_->farClip = 3000.0f;
	camera_->pos.z = -5.0f;
	camera_->pos.y = 1.1f;
	camera_->rotate.x = 0.3f;

	miniMap_.reset(new Minimap{});
	miniMap_->Initialize(Vector3{}, Vector3{});
}

void CustomizeScene::Finalize() {
	//customize_->Finalize();
	miniMap_->Finalize();
}

void CustomizeScene::Update() {
	camera_->Debug("camera");

	customize_->Update();
	customizeCamera_->Update();

	if (input_->GetInstance()->GetGamepad()->LongPushed(Gamepad::Button::BACK)) {
		SceneManager::GetInstance()->SceneChange(BaseScene::ID::StageSelect);
	}

	if (input_->GetInstance()->GetKey()->LongPush(DIK_SPACE) || input_->GetInstance()->GetGamepad()->LongPushed(Gamepad::Button::START)) {
		SceneManager::GetInstance()->SceneChange(BaseScene::ID::Game);
	}

	miniMap_->Debug();
	miniMap_->Update();
}

void CustomizeScene::Draw() {
	customize_->Draw(*camera_);
	miniMap_->Draw(Vector3{});
	customize_->PostDraw(*camera_);
}
