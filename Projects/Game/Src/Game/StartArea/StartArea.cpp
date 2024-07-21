#include "StartArea.h"

#include <numbers>

#include "imgui.h"

#include "Utils/Camera/Camera.h"	
#include "Game/StageManager/StageManager.h"

void StartArea::Initialize() {
	start_ = std::make_unique<Model>();
	start_->LoadObj("./Resources/DebugArrow/DebugGoal.obj");
	leftBeach_ = std::make_unique<Model>();
	leftBeach_->LoadObj("./Resources/InGame/Models/beach.obj");
	rightBeach_ = std::make_unique<Model>();
	rightBeach_->LoadObj("./Resources/InGame/Models/beach.obj");
	start_->scale = { 10.0f ,10.0f ,10.0f };
	start_->rotate.y = 180.0f * std::numbers::pi_v<float> / 180.0f;
	start_->pos;
	leftBeach_->pos.x += start_->scale.x * 0.5f;
	rightBeach_->pos.x -= start_->scale.x * 0.5f;
	leftBeach_->SetParent(start_.get());
	rightBeach_->SetParent(start_.get());
}

void StartArea::Update() {
#ifdef _DEBUG
	ImGui::Begin("Start");
	ImGui::DragFloat3("scale", &start_->scale.x, 0.1f);
	ImGui::DragFloat3("rotate", &start_->rotate.x, 0.1f);
	ImGui::DragFloat3("pos", &start_->pos.x, 0.1f);
	ImGui::End();
#endif // _DEBUG

	ResourceUpdate();
}

void StartArea::ResourceUpdate() {
	start_->Update();
	leftBeach_->Update();
	rightBeach_->Update();
}

void StartArea::Draw(const Camera& camera) {
	if (StageManager::GetStage() != 9) {
		start_->Draw(camera.GetViewProjection(), camera.GetPos());
		leftBeach_->Draw(camera.GetViewProjection(), camera.GetPos());
		rightBeach_->Draw(camera.GetViewProjection(), camera.GetPos());
	}
}
